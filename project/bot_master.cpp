/*
implements virtual functions that handle game events such as "OnStep" and the 
rest of the On... type functions we are going to use 
*/

#include "bot_master.h"
#include <sc2api/sc2_api.h>
#include <iostream>

using namespace sc2;

bot_master::bot_master() {
	// never going to be called
	// initialize atributes on "OnGameStart()"
}

void bot_master::OnGameStart() {
	set_variables();
	load_build_order();

	scout(opp_location[scout_location++]); // select and send scout
	set_builder(); // select and send builder to position
}


void bot_master::OnStep() {
	// tries to research warpgate
	if (core_completed && !research_wrapgate) {
		upgrade(ABILITY_ID::RESEARCH_WARPGATE);
	}

	// tries to construct current building on the build order
	if (index_build < build_order.size() && 
		build(build_order[index_build], build_placement[index_build], nullptr)) {
		// building succesfully placed 
		++index_build; // increase index_build so the next on the building list will be 
		         // tried to be constructed on next step
	}

	// obtains info from the game: current minerals and vespene gas available
	int minerals = observation->GetMinerals();
	int vespene  = observation->GetVespene();

	// tries to build a warp prism 
	if (!warp_prism && robotics_completed) {
		// robotics have no orders 
		if (robotics->orders.size() == 0) {
			Actions()->UnitCommand(robotics, ABILITY_ID::TRAIN_WARPPRISM);
		}
	}

	// tries to warp units 
	if (research_wrapgate && warp_ready) {
		Units units = observation->GetUnits(Unit::Alliance::Self);
		for (const auto &unit : units) {
			if (unit->unit_type == UNIT_TYPEID::PROTOSS_WARPGATE) {
				if (vespene > 150) {
					// if we can afford to make dark templars
					Actions()->UnitCommand(unit, ABILITY_ID::TRAINWARP_DARKTEMPLAR, 
										   warp_position, true);
				} else if (minerals > 350) {
					// else, make zealots if we have plenty of minerals
					Actions()->UnitCommand(unit, ABILITY_ID::TRAINWARP_ZEALOT,
									       warp_position, true);
				}
			}
		}
	}

	// tries to build proxy pylon
	if (minerals > 800 && index_pylon < proxy_pylons.size()) {
		if (builder_unit->orders.empty()) {
			// if doing nothing
			if (build(ABILITY_ID::BUILD_PYLON, proxy_pylons[index_pylon], 
			    builder_unit)) {
				// if built successfully
				++index_pylon;
			}
		}
	}
}

void bot_master::OnUnitEnterVision(const sc2::Unit *unit) {
	// checks for the different units we are interested in
	switch (unit->unit_type.ToType()) {
		// the three possible bases. if found then set our opponent's location
		case UNIT_TYPEID::TERRAN_COMMANDCENTER: {
			if (!opp_base_found) {
				// this set the starting opponent location to the one closest 
				// to where the base was found 
				opp_base = closest(opp_location, scout_unit->pos);
				opp_base_found = true;

				set_opp_position(); // set warp location and expansions to attack
			}
			break;
		}
		case UNIT_TYPEID::ZERG_HATCHERY: {
			if (!opp_base_found) {
				// std::cout << "opponent position: " << unit->pos.x << " " << unit->pos.y  << std::endl; 
				// this set the starting opponent location to the one closest 
				// to where the base was found 
				opp_base = closest(opp_location, scout_unit->pos);
				opp_base_found = true;

				set_opp_position(); // set warp location and expansions to attack 
			}
			break;
		}
		case UNIT_TYPEID::PROTOSS_NEXUS: {
			if (!opp_base_found) {
				// this set the starting opponent location to the one closest 
				// to where the base was found 
				opp_base = closest(opp_location, scout_unit->pos);
				opp_base_found = true;

				set_opp_position(); // set warp location and expansions to attack
			}
			break;
		}
	}

}

void bot_master::OnBuildingConstructionComplete(const sc2::Unit *unit) {
	// std::cout << unit->pos.x << "  " << unit->pos.y << std::endl;
	// set to true the buildings completed so we know the required buildings
	// have been completed
	switch(unit->unit_type.ToType()) {
		case UNIT_TYPEID::PROTOSS_PYLON: {
			pylon_completed = true; 
			// add it to the list of pylons 
			pylons.push_back(unit->pos);
			break;
		}
		case UNIT_TYPEID::PROTOSS_FORGE: {
			forge_completed = true;
			break;
		}
		case UNIT_TYPEID::PROTOSS_ASSIMILATOR: {
			// select two random probes and send them to extract the newly 
			// created assimilator
			for (int i = 0; i < 2; ++i) {
				const Unit *set_worker = random_probe();
				Actions()->UnitCommand(set_worker, ABILITY_ID::SMART, unit);
				gas_workers.insert(set_worker); // add it to the set so it is 
												// not selected for other task
			}
			break;
		}
		case UNIT_TYPEID::PROTOSS_GATEWAY: {
			gateway_completed = true; break;
		}
		case UNIT_TYPEID::PROTOSS_CYBERNETICSCORE: {
			core_completed = true;
			cybernetics = unit;
			upgrade(ABILITY_ID::RESEARCH_WARPGATE);
			break;
		}
		case UNIT_TYPEID::PROTOSS_TWILIGHTCOUNCIL: {
			twilight_completed = true; break;
		}
		case UNIT_TYPEID::PROTOSS_ROBOTICSFACILITY: {
			// if there is no warprism then train one 
			robotics_completed = true;
			robotics = unit;
		}
		default: {
			break;
		}
	}
}

void bot_master::OnUnitIdle(const Unit *unit) {
	switch (unit->unit_type.ToType()) {
		case UNIT_TYPEID::PROTOSS_NEXUS: {
			// build up to 21 workers. three scouts and 18 miners
			if (observation->GetFoodWorkers() < 24) {
				Actions()->UnitCommand(unit, ABILITY_ID::TRAIN_PROBE);
			}
			break;
		}
		case UNIT_TYPEID::PROTOSS_PROBE: {
			if (is_scout(unit)) {
				// std::cout << "keep exploring" << std::endl;
				// if not seen the opponent's start location then check 
				// the next potential location 
				if (scout_location < 3) {
					scout(opp_location[scout_location++]);
				}
				break;
			}

			if (is_builder(unit)) {
				break;
			}

			// sends the probe to the nearest mine from the nexus
			const Unit *mineral_target = FindNearestMineralPatch(base);
			if (!mineral_target) {
				break;
			}
			Actions()->UnitCommand(unit, ABILITY_ID::SMART, mineral_target);
			break;
		}
		case UNIT_TYPEID::PROTOSS_WARPPRISM: {
			// if not already in position to warp units 
			if (unit->pos.x != warp_position.x && 
				unit->pos.y != warp_position.y) {
				// send it to position
				Actions()->UnitCommand(unit, ABILITY_ID::SMART, warp_position);
			} else {
				// change to phasing mode to be ready to warp units
				Actions()->UnitCommand(unit, ABILITY_ID::MORPH_WARPPRISMPHASINGMODE);
				warp_ready = true;
			}
			break;
		}
		case UNIT_TYPEID::PROTOSS_ZEALOT: {
			// same as dark templar
		}
		case UNIT_TYPEID::PROTOSS_DARKTEMPLAR: {
			// std::cout << "attack: " << expansions[index_patroll].x << " " << expansions[index_patroll].y << " " << index_patroll << std::endl;
			// send them to attack the current location in expansions 

			// if done attacking location in "expansions" 
			if (euclidean_dist(unit->pos, expansions[index_patroll]) < 3) {
				// std::cout << "done attacking\n";
				++index_patroll;
			}

			// if our starting base skip attacking it 
			if (index_patroll < expansions.size() && 
				expansions[index_patroll].x == base.x && 
				expansions[index_patroll].y == base.y) {
				++index_patroll;
			}

			// if all have been explored loop back to begining 
			if (index_patroll >= expansions.size()) {
				index_patroll = 0;
			}

			Actions()->UnitCommand(unit, ABILITY_ID::ATTACK_ATTACK, 
								expansions[index_patroll]);
			
			break;
		}
		default: {
			break;
		}
	}
}

void bot_master::OnUnitCreated(const Unit *unit) {
	switch (unit->unit_type.ToType()) {
		case UNIT_TYPEID::PROTOSS_WARPPRISM: {
			warp_prism = true;
			break;
		}
		case UNIT_TYPEID::PROTOSS_DARKTEMPLAR: {
			// std::cout << unit->pos.x << " " << unit->pos.y << std::endl;
		}
		default: {
			break;
		}
	}
}

void bot_master::OnUnitDestroyed(const Unit *unit) {
	switch (unit->unit_type.ToType()) {
		case UNIT_TYPEID::PROTOSS_PROBE: {
			// if scout was killed and opp base has not been determined 
			if (is_scout(unit) && !opp_base_found) {
				// std::cout << "scout was killed\n";
				// set it to the closest base where the probe was killed
				opp_base = closest(opp_location, unit->pos);
				opp_base_found = true;

				set_opp_position(); // set warp location and expansions to attack
			}
			break;
		}
		case UNIT_TYPEID::PROTOSS_WARPPRISM: {
			// std::cout << "warp killed\n";
			// if warp prism was killed then set variables to false to build a new one 
			warp_prism = false;
			warp_ready = false;
			break;
		}
		case UNIT_TYPEID::PROTOSS_WARPPRISMPHASING: {
			// std::cout << "warp killed\n";
			// if warp prism was killed then set variables to false to build a new one 
			warp_prism = false;
			warp_ready = false;
			break;
		}
		default: {
			break;
		}
	}
}

void bot_master::OnUpgradeCompleted(UpgradeID upgrade_id) {
	switch(upgrade_id.ToType()) {
		case UPGRADE_ID::WARPGATERESEARCH: {
			// std::cout << "wrapgate research completed" << std::endl;
			research_wrapgate = true;
			break;
		}
		default: {
			break; // only concerned with warpgate for now
		}
	}
}
