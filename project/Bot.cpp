#include "Bot.h"
#include <sc2api/sc2_api.h>
#include <iostream>

using namespace sc2;

Bot::Bot() {
	// never going to be called
	// initialize atributes on "OnGameStart()"
}

void Bot::OnGameStart() {
	// build order
	build_order.push_back(std::make_pair(ABILITY_ID::BUILD_PYLON, std::make_pair(100, 0)));
	build_order.push_back(std::make_pair(ABILITY_ID::BUILD_FORGE, std::make_pair(150, 0)));
	build_order.push_back(std::make_pair(ABILITY_ID::BUILD_PYLON, std::make_pair(100, 0)));
	build_order.push_back(std::make_pair(ABILITY_ID::BUILD_PHOTONCANNON, std::make_pair(150, 0)));
	build_order.push_back(std::make_pair(ABILITY_ID::BUILD_PYLON, std::make_pair(100, 0)));
	build_order.push_back(std::make_pair(ABILITY_ID::BUILD_PHOTONCANNON, std::make_pair(150, 0)));
	build_order.push_back(std::make_pair(ABILITY_ID::BUILD_PHOTONCANNON, std::make_pair(150, 0)));
	build_order.push_back(std::make_pair(ABILITY_ID::BUILD_PHOTONCANNON, std::make_pair(150, 0)));
	build_order.push_back(std::make_pair(ABILITY_ID::BUILD_PHOTONCANNON, std::make_pair(150, 0)));
	build_order.push_back(std::make_pair(ABILITY_ID::BUILD_ASSIMILATOR, std::make_pair(75, 0)));
	build_order.push_back(std::make_pair(ABILITY_ID::BUILD_ASSIMILATOR, std::make_pair(75, 0)));

	const GameInfo &game_info = Observation()->GetGameInfo();

	// set up observational interface
	observation = Observation();

	// gets the potential opponent's start locations
	opp_location = game_info.enemy_start_locations;

	// gets our location
	base = observation->GetStartLocation();
	index = 0;
	pylon_completed = false;
	forge_completed = false;

	// set the building placement according to our start location
	if (base.x == 33.5 && base.y == 158.5) { // top-left
		load_top_left();
	} else if (base.x == 158.5 && base.y == 158.5) {
		load_top_right();
	} else if (base.x == 158.5 && base.y == 33.5) {
		load_bottom_right();
	} else if (base.x == 33.5 && base.y == 33.5) {
		load_bottom_left();
	}
	
	opp_base_found = false;
	scout_location = 0;

	// select and send scout
	scout(opp_location[scout_location++]);
}

void Bot::load_top_left() {
	build_placement.push_back(Point2D(51, 160));
	build_placement.push_back(Point2D(51.5, 162.5));
	build_placement.push_back(Point2D(54, 163));
	build_placement.push_back(Point2D(49, 160));
	build_placement.push_back(Point2D(26, 164));
	build_placement.push_back(Point2D(30, 161));
	build_placement.push_back(Point2D(54, 165));
	build_placement.push_back(Point2D(25, 160));
	build_placement.push_back(Point2D(30, 167));
	build_placement.push_back(Point2D(36.5, 165.5));
	build_placement.push_back(Point2D(26.5, 154.5));
}

void Bot::load_top_right() {
	build_placement.push_back(Point2D(160, 141));
	build_placement.push_back(Point2D(162.5, 140.5));
	build_placement.push_back(Point2D(163, 138));
	build_placement.push_back(Point2D(160, 143));
	build_placement.push_back(Point2D(166, 164));
	build_placement.push_back(Point2D(162, 161));
	build_placement.push_back(Point2D(165, 138));
	build_placement.push_back(Point2D(162, 166));
	build_placement.push_back(Point2D(167, 161));
	build_placement.push_back(Point2D(165.5, 154.5));
	build_placement.push_back(Point2D(155.5, 165.5));
}

void Bot::load_bottom_right() {
	build_placement.push_back(Point2D(141, 32));
	build_placement.push_back(Point2D(140.5, 29.5));
	build_placement.push_back(Point2D(138, 29));
	build_placement.push_back(Point2D(143, 32));
	build_placement.push_back(Point2D(166, 28));
	build_placement.push_back(Point2D(162, 31));
	build_placement.push_back(Point2D(138, 27));
	build_placement.push_back(Point2D(167, 32));
	build_placement.push_back(Point2D(163, 26));
	build_placement.push_back(Point2D(155.5, 26.5));
	build_placement.push_back(Point2D(165.5, 37.5));
}

void Bot::load_bottom_left() {
	build_placement.push_back(Point2D(32, 51));
	build_placement.push_back(Point2D(29.5, 51.5));
	build_placement.push_back(Point2D(29, 54));
	build_placement.push_back(Point2D(32, 49));
	build_placement.push_back(Point2D(27, 27));
	build_placement.push_back(Point2D(30, 31));
	build_placement.push_back(Point2D(27, 54));
	build_placement.push_back(Point2D(32, 25));
	build_placement.push_back(Point2D(24, 32));
	build_placement.push_back(Point2D(26.5, 37.5));
	build_placement.push_back(Point2D(36.5, 26.5));
}

void Bot::OnStep() {
	// if enough minerals and/or vespine build the next building on the build order
	if (index < build_order.size() && 
		observation->GetMinerals() >= build_order[index].second.first &&
		observation->GetVespene() >= build_order[index].second.second) {
		
		// check required buildings 
		if (build_order[index].first == ABILITY_ID::BUILD_FORGE && 
			!pylon_completed) {
			return; // there is no pylon to build the forge
		} 
		if (build_order[index].first == ABILITY_ID::BUILD_PHOTONCANNON && 
			!forge_completed) {
			return; // there is no forge to build the cannons
		} 
		
		build(build_order[index].first, build_placement[index]);
		++index;
	}
}

void Bot::OnUnitEnterVision(const sc2::Unit *unit) {
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

void Bot::OnBuildingConstructionComplete(const sc2::Unit *unit) {
	// std::cout << unit->pos.x << "  " << unit->pos.y << std::endl;
	// set to true the buildings completed so we know the required buildings
	// have been completed
	switch(unit->unit_type.ToType()) {
		case UNIT_TYPEID::PROTOSS_PYLON: {
			pylon_completed = true; break;
		}
		case UNIT_TYPEID::PROTOSS_FORGE: {
			forge_completed = true; break;
		}
	}
}

void Bot::OnUnitIdle(const Unit *unit) {
	switch (unit->unit_type.ToType()) {
		case UNIT_TYPEID::PROTOSS_NEXUS: {
			// build up to 21 workers. three scouts and 18 miners
			if (observation->GetFoodWorkers() < 21) {
				Actions()->UnitCommand(unit, ABILITY_ID::TRAIN_PROBE);
			}
			break;
		}
		case UNIT_TYPEID::PROTOSS_PROBE: {
			if (is_scout(unit)) {
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

bool Bot::build(ABILITY_ID ability_type_for_structure, Point2D location) {
	const Unit *unit_to_build = nullptr;

	Units units = observation->GetUnits(Unit::Alliance::Self);

	// get random probe to build
	for (const auto &unit : units) {
		// if probe and not a scout select this unit to build the structure
		if (unit->unit_type == UNIT_TYPEID::PROTOSS_PROBE &&
			!is_scout(unit)) {
			unit_to_build = unit;
		}

	}

	Actions()->UnitCommand(unit_to_build, ability_type_for_structure,
		location);

	std::cout << "done: " << index << std::endl;

	return true;
}

// bool Bot::build_pylon() {
// 	// only when supply capped (for now)
// 	if (observation->GetFoodUsed() <= observation->GetFoodCap() - 2) {
// 		return false;
// 	}

// 	build(ABILITY_ID::BUILD_PYLON, 3, 3);
// }

// bool Bot::build_forge() {
// 	// build only if there is a pylon
// 	if (CountUnitType(UNIT_TYPEID::PROTOSS_PYLON) < 1) {
// 		return false;
// 	}

// 	// only build if there is no forge
// 	if (CountUnitType(UNIT_TYPEID::PROTOSS_FORGE) > 0) {
// 		return false;
// 	}

// 	return build(ABILITY_ID::BUILD_FORGE, 3, 3);
// }

void Bot::scout(Point2D location) {
	Units units = observation->GetUnits(Unit::Alliance::Self);

	// if no scout 
	if (scout_unit == nullptr) {
		// select random worker that is not a scout
		const Unit *scout_selected = nullptr;

		for (const auto &unit : units) {
			// if probe and not a scout then select this unit 
			if (unit->unit_type == UNIT_TYPEID::PROTOSS_PROBE &&
				!is_scout(unit)) {
				scout_selected = unit;
			}
		}

		// set scout
		scout_unit = scout_selected;
	}
	
	// send scout to given location
	Actions()->UnitCommand(scout_unit, ABILITY_ID::SMART, location);
}

size_t Bot::CountUnitType(UNIT_TYPEID unit_type) {
	return observation->GetUnits(Unit::Alliance::Self, IsUnit(unit_type)).size();
}

const Unit * Bot::FindNearestMineralPatch(const Point2D &start) {
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

const bool Bot::is_scout(const Unit *unit) {
	return scout_unit == unit;
}