#include "bot_master.h"
#include <sc2api/sc2_api.h>
#include <iostream>

using namespace sc2;

void bot_master::scout(Point2D location) {
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