#include "bot_master.h"
#include <sc2api/sc2_api.h>
#include <iostream>

using namespace sc2;

void bot_master::set_variables() {
    const GameInfo &game_info = Observation()->GetGameInfo();
	observation = Observation();            // set up observational interface

	base = observation->GetStartLocation(); // gets our location

    // set the building placement according to our start location
	if (base.x == 33.5 && base.y == 158.5) {         // top-left
		load_top_left();
	} else if (base.x == 158.5 && base.y == 158.5) { // top-right
		load_top_right();
	} else if (base.x == 158.5 && base.y == 33.5) {  // bottom-right
		load_bottom_right();
	} else if (base.x == 33.5 && base.y == 33.5) {   // bottom-left
		load_bottom_left();
	}


    // gets the potential opponent's start locations
	opp_location = game_info.enemy_start_locations;

	index = 0;
    scout_location = 0;
	pylon_completed = false;
	forge_completed = false;
    opp_base_found = false;
}

void bot_master::load_build_order() {
	build_order.push_back(ABILITY_ID::BUILD_PYLON);
	build_order.push_back(ABILITY_ID::BUILD_FORGE);
	build_order.push_back(ABILITY_ID::BUILD_PYLON);
	build_order.push_back(ABILITY_ID::BUILD_PHOTONCANNON);
	build_order.push_back(ABILITY_ID::BUILD_PYLON);
	build_order.push_back(ABILITY_ID::BUILD_PHOTONCANNON);
	build_order.push_back(ABILITY_ID::BUILD_PHOTONCANNON);
	build_order.push_back(ABILITY_ID::BUILD_PHOTONCANNON);
	build_order.push_back(ABILITY_ID::BUILD_PHOTONCANNON);
	build_order.push_back(ABILITY_ID::BUILD_ASSIMILATOR);
	build_order.push_back(ABILITY_ID::BUILD_ASSIMILATOR);
}

void bot_master::load_top_left() {
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

void bot_master::load_top_right() {
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

void bot_master::load_bottom_right() {
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

void bot_master::load_bottom_left() {
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