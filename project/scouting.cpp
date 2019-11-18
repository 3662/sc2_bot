#include "bot_master.h"
#include <sc2api/sc2_api.h>
#include <iostream>

using namespace sc2;

void bot_master::scout(Point2D location) {
	if (scout_unit == nullptr) {     // if no scout 
		scout_unit = random_probe(); // set scout
	}
	
	// send scout to given location
	Actions()->UnitCommand(scout_unit, ABILITY_ID::SMART, location);
}