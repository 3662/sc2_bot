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
}


void bot_master::OnStep() {
	// tries to construct current building on the build order
	if (index < build_order.size() && 
		build(build_order[index], build_placement[index])) {
		// building succesfully placed 
		++index; // increase index so the next on the building list will be 
		         // tried to be constructed on next step
	}
}

void bot_master::OnUnitEnterVision(const sc2::Unit *unit) {
	// checks for the different units we are interested in
	switch (unit->unit_type.ToType()) {
		// the three possible bases. if found then set our opponent's location
		case UNIT_TYPEID::TERRAN_COMMANDCENTER: {
			if (!opp_base_found) { opp_base = unit->pos; opp_base_found = true; }
			break;
		}
		case UNIT_TYPEID::ZERG_HATCHERY: {
			if (!opp_base_found) { opp_base = unit->pos; opp_base_found = true; }
			break;
		}
		case UNIT_TYPEID::PROTOSS_NEXUS: {
			if (!opp_base_found) { opp_base = unit->pos; opp_base_found = true; }
			break;
		}
	}

}

void bot_master::OnBuildingConstructionComplete(const sc2::Unit *unit) {
	std::cout << unit->pos.x << "  " << unit->pos.y << std::endl;
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
			forge_completed = true; break;
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
			core_completed = true; break;
		}
		case UNIT_TYPEID::PROTOSS_TWILIGHTCOUNCIL: {
			twilight_completed = true; break;
		}
	}
}

void bot_master::OnUnitIdle(const Unit *unit) {
	switch (unit->unit_type.ToType()) {
		case UNIT_TYPEID::PROTOSS_NEXUS: {
			// build up to 21 workers. three scouts and 18 miners
			if (observation->GetFoodWorkers() < 23) {
				Actions()->UnitCommand(unit, ABILITY_ID::TRAIN_PROBE);
			}
			break;
		}
		case UNIT_TYPEID::PROTOSS_PROBE: {
			if (is_scout(unit)) {
				std::cout << "keep exploring" << std::endl;
				// if not seen the opponent's start location then check 
				// the next potential location 
				if (scout_location < 3) {
					scout(opp_location[scout_location++]);
				}
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
		default: {
			break;
		}
	}
}

const Unit * bot_master::random_probe() {
    const Unit *unit_selected = nullptr;

	Units units = observation->GetUnits(Unit::Alliance::Self);

	// get random probe that has no orther other than mine
	// ability_id for mining and bringing resources is 3666 and 3667
	for (const auto &unit : units) {
		// if probe, not a scout, not a gas worker and no orders then select this unit 
		if (unit->unit_type == UNIT_TYPEID::PROTOSS_PROBE &&
			!is_scout(unit) && gas_workers.find(unit) == gas_workers.end()) {
			for (auto &order : unit->orders) {
				// std::cout << order.ability_id << std::endl;
				if (order.ability_id != 3666 && order.ability_id != 3667) {
					// this means the probe is doing something other than 
					// mining so do not select it
					break;
				} else {
					unit_selected = unit;
				}
			}
		} 
	}

    return unit_selected;
}

size_t bot_master::CountUnitType(UNIT_TYPEID unit_type) {
	return observation->GetUnits(Unit::Alliance::Self, IsUnit(unit_type)).size();
}

const Unit * bot_master::FindNearestMineralPatch(const Point2D &start) {
	Units units = observation->GetUnits(Unit::Alliance::Neutral);
	float distance = std::numeric_limits<float>::max();

	const Unit *target = nullptr;

	for (const auto &u : units) {
		if (u->unit_type == UNIT_TYPEID::NEUTRAL_MINERALFIELD) {
			float d = DistanceSquared2D(u->pos, start);
			if (d < distance) {
				distance = d;
				target = u;
			}
		}
	}

	return target;
}

const bool bot_master::is_scout(const Unit *unit) {
	return scout_unit == unit;
}