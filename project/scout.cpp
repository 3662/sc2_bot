/*
Set up a scout Probe and sends it to scout a given location on the map.
*/

#include "bot_master.h"
#include <sc2api/sc2_api.h>
#include <iostream>

using namespace sc2;

const bool bot_master::is_scout(const Unit *unit) {
	return scout_unit == unit;
}

void bot_master::scout(Point2D location) {
	if (scout_unit == nullptr) {     // if no scout 
		scout_unit = random_probe(); // set scout
	}
	
	// send scout to given location
	Actions()->UnitCommand(scout_unit, ABILITY_ID::SMART, location);
}
