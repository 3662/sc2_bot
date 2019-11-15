#include "bot_master.h"
#include <sc2api/sc2_api.h>
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
        case ABILITY_ID::BUILD_PYLON: {
            // requirements: 100 minerals
            if (minerals < 100) {
                return false;
            }
            break;
        }
        case ABILITY_ID::BUILD_FORGE: {
            // requirements: 150 minerals / near a pylon
            if (minerals < 150 || !is_near_pylon(location)) {
                return false;
            }
            break;
        }
        case ABILITY_ID::BUILD_PHOTONCANNON: {
            // requirements: 150 minerals / forge / near a pylon
            if (minerals < 150 || !forge_completed || !is_near_pylon(location)) {
                return false;
            }
            break;
        }
        case ABILITY_ID::BUILD_ASSIMILATOR: {
            // requirements: 75 minerals 
            if (minerals < 75) {
                return false;
            }
            break;
        }
    }

	Actions()->UnitCommand(unit_to_build, ability_type_for_structure, location);

	return true;
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

const Unit * bot_master::random_probe() {
    const Unit *unit_selected = nullptr;

	Units units = observation->GetUnits(Unit::Alliance::Self);

	// get random probe
	for (const auto &unit : units) {
		// if probe and not a scout select this unit to build the structure
		if (unit->unit_type == UNIT_TYPEID::PROTOSS_PROBE &&
			!is_scout(unit)) {
			unit_selected = unit;
		}
	}

    return unit_selected;
}