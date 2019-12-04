/*
Unfinished since expanding is not part of the game plan. However, it laid the 
foundation to implement it. Currently selects a Probe scout and send it out to 
the map.
*/

#include "bot_master.h"
#include <sc2api/sc2_api.h>
#include <iostream>
#include <cmath>

using namespace sc2;

const bool bot_master::is_builder(const Unit *unit) {
    return builder_unit == unit;
}

void bot_master::set_builder() {
    // selects expansion builder and send it to position
	builder_unit = random_probe();
    Actions()->UnitCommand(builder_unit, ABILITY_ID::SMART, expansion);
}