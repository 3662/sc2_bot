#include "bot_master.h"
#include <sc2api/sc2_api.h>
#include <sc2lib/sc2_lib.h>
#include <iostream>

using namespace sc2;

bool bot_master::build(ABILITY_ID ability_type_for_structure, Point2D location) {
	const Unit *unit_to_build = random_probe(); // retrieves a random probe

    if (!unit_to_build) { 
        return false;
    }

    // obtains info from the game: current minerals and vespene gas available
    int minerals = observation->GetMinerals();
    int vespene  = observation->GetVespene();

    // build only if required resources and buildings are available for each 
    // type of building
    switch(ability_type_for_structure) {
        // checks cost and wether it can be build in the given location
        case ABILITY_ID::BUILD_PYLON: {
            // requirements: 100 minerals
            if (minerals < 100 || 
                !Query()->Placement(ABILITY_ID::BUILD_PYLON, location)) {
                return false;
            }
            break;
        }
        case ABILITY_ID::BUILD_FORGE: {
            // requirements: 150 minerals / near a pylon
            if (minerals < 150 || !is_near_pylon(location) ||
                !Query()->Placement(ABILITY_ID::BUILD_FORGE, location)) {
                return false;
            }
            break;
        }
        case ABILITY_ID::BUILD_PHOTONCANNON: {
            // requirements: 150 minerals / forge / near a pylon
            if (minerals < 150 || !forge_completed || !is_near_pylon(location)
                || !Query()->Placement(ABILITY_ID::BUILD_PHOTONCANNON, location)) {
                return false;
            }
            break;
        }
        case ABILITY_ID::BUILD_ASSIMILATOR: {
            // requirements: 75 minerals 
            if (minerals < 75) {
                return false;
            }
            if (build_vespene_geyser(unit_to_build)) {
                gas_workers.insert(unit_to_build); // add it as assimilator worker 
            } else {
                return false;
            }
        }
    }

	Actions()->UnitCommand(unit_to_build, ability_type_for_structure, location);
    for (const auto& order : unit_to_build->orders) {
		std::cout << "probe: " << unit_to_build << " order: " << order.ability_id.to_string() << std::endl;
	}
    std::cout << "end\n";

	return true;
}

bool bot_master::build_vespene_geyser(const Unit *unit_to_build) {
    Units units = observation->GetUnits(Unit::Alliance::Neutral);

    for (const auto &u : units) {
        if (u->unit_type == UNIT_TYPEID::NEUTRAL_VESPENEGEYSER) {
            // checks if near base and can be build
            if (Distance2D(base, u->pos) < 15 && 
                Query()->Placement(ABILITY_ID::BUILD_ASSIMILATOR, u->pos)) {
                Actions()->UnitCommand(unit_to_build, ABILITY_ID::BUILD_ASSIMILATOR, u);
                return true;
            }
        }
    }

    return false; // could not be build
}

bool bot_master::is_near_pylon(Point2D location) {
    // checks the location given is close enough from a pylon
    for (const auto &pylon : pylons) {
        if (DistanceSquared2D(pylon, location) < 41) {
            return true; // it is close enough
        }
    }
    return false; // not close enough to an existing pylon
}