/*
Contains functions that handle in-game upgrades (e.g. Warpgate). Given an 
upgrade type start it. It checks constrains such as building requirements 
(e.g. Cybernetics Core for Warpgate) and mineral/gas cost. Easily scalable by 
adding more upgrade types and their respective constrain.
*/

#include "bot_master.h"
#include <sc2api/sc2_api.h>
#include <sc2lib/sc2_lib.h>
#include <iostream>

using namespace sc2;

bool bot_master::upgrade(ABILITY_ID upgrade_type) {
    // obtains info from the game: current minerals and vespene gas available
    int minerals = observation->GetMinerals();
    int vespene  = observation->GetVespene();

    // build only if required resources and buildings are available for each 
    // type of building
    switch(upgrade_type) {
		case ABILITY_ID::RESEARCH_WARPGATE: {
			if (minerals < 50 || vespene < 50 || !forge_completed 
                || !gateway_completed|| !core_completed) {
				return false;
			}
			Actions()->UnitCommand(cybernetics, upgrade_type);
			return true;
		}
		default:
        return false; // no other upgrade is supported at this time
    }

	return true;
}