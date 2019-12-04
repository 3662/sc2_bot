#include "bot_master.h"
#include <sc2api/sc2_api.h>
#include <iostream>

using namespace sc2;

void bot_master::set_variables() {
    const GameInfo &game_info = Observation()->GetGameInfo();
	observation = Observation();            // set up observational interface

	base = observation->GetStartLocation(); // gets our location
	expansion = Point2D(98.5, 35.5);        // set exp location

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

	for (const auto &l : opp_location) {
		std::cout << "exp " << l.x << " " << l.y << std::endl;
	}

	std::cout << "exp " << base.x << " " << base.y << std::endl;

	// set warp positions custom to the map
	warp_positions.push_back(Point2D(28, 139));
	warp_positions.push_back(Point2D(139, 165));
	warp_positions.push_back(Point2D(164, 54));
	warp_positions.push_back(Point2D(52, 32));
	// set proxy pylons locations
	load_proxy_pylons();
	// set expansions
	load_expansions();

	index_build = 0;
	index_pylon = 0;
	index_patroll = 0;
    scout_location = 0;
	scout_unit = nullptr;
	pylon_completed = false;
	forge_completed = false;
	gateway_completed = false;
	core_completed = false;
	twilight_completed = false;
	robotics_completed = false;
    opp_base_found = false;
	research_wrapgate = false;
	warp_prism = false;
	warp_ready = false;

	defenders_set = false;
}

void bot_master::load_build_order() {
	build_order.push_back(ABILITY_ID::BUILD_PYLON);
	build_order.push_back(ABILITY_ID::BUILD_FORGE);
	build_order.push_back(ABILITY_ID::BUILD_PYLON);
	build_order.push_back(ABILITY_ID::BUILD_PHOTONCANNON);
	build_order.push_back(ABILITY_ID::BUILD_PYLON);
	// build_order.push_back(ABILITY_ID::BUILD_PHOTONCANNON);
	build_order.push_back(ABILITY_ID::BUILD_PHOTONCANNON);
	// build_order.push_back(ABILITY_ID::BUILD_PHOTONCANNON);
	// build_order.push_back(ABILITY_ID::BUILD_PHOTONCANNON);
	build_order.push_back(ABILITY_ID::BUILD_GATEWAY);
	build_order.push_back(ABILITY_ID::BUILD_ASSIMILATOR);
	build_order.push_back(ABILITY_ID::BUILD_ASSIMILATOR);
	build_order.push_back(ABILITY_ID::BUILD_CYBERNETICSCORE);
	build_order.push_back(ABILITY_ID::BUILD_PYLON);
	build_order.push_back(ABILITY_ID::BUILD_ROBOTICSFACILITY);
	build_order.push_back(ABILITY_ID::BUILD_TWILIGHTCOUNCIL);
	build_order.push_back(ABILITY_ID::BUILD_DARKSHRINE);
	build_order.push_back(ABILITY_ID::BUILD_PYLON);
	build_order.push_back(ABILITY_ID::BUILD_GATEWAY);
	build_order.push_back(ABILITY_ID::BUILD_GATEWAY);
	build_order.push_back(ABILITY_ID::BUILD_GATEWAY);
	build_order.push_back(ABILITY_ID::BUILD_GATEWAY);
	build_order.push_back(ABILITY_ID::BUILD_GATEWAY);
	build_order.push_back(ABILITY_ID::BUILD_GATEWAY);
}

void bot_master::load_top_left() {
	build_placement.push_back(Point2D(51, 160));
	build_placement.push_back(Point2D(51.5, 162.5));
	build_placement.push_back(Point2D(54, 163));
	build_placement.push_back(Point2D(49, 160));
	build_placement.push_back(Point2D(26, 164));
	// build_placement.push_back(Point2D(30, 161));
	build_placement.push_back(Point2D(54, 165));
	// build_placement.push_back(Point2D(25, 160));
	// build_placement.push_back(Point2D(30, 167));
	build_placement.push_back(Point2D(49.5, 157.5));
	build_placement.push_back(Point2D(36.5, 165.5));
	build_placement.push_back(Point2D(26.5, 154.5));
	build_placement.push_back(Point2D(46.5, 157.5));
	build_placement.push_back(Point2D(44, 158));
	build_placement.push_back(Point2D(44.5, 160.5));
	build_placement.push_back(Point2D(41.5, 157.5));
	build_placement.push_back(Point2D(42, 160));
	build_placement.push_back(Point2D(41, 152));
	build_placement.push_back(Point2D(41.5, 154.5));
	build_placement.push_back(Point2D(44.5, 154.5));
	build_placement.push_back(Point2D(38.5, 154.5));
	build_placement.push_back(Point2D(38.5, 151.5));
	build_placement.push_back(Point2D(26.5, 166.5));
	build_placement.push_back(Point2D(23.5, 163.5));
}

void bot_master::load_top_right() {
	build_placement.push_back(Point2D(160, 141));
	build_placement.push_back(Point2D(162.5, 140.5));
	build_placement.push_back(Point2D(163, 138));
	build_placement.push_back(Point2D(160, 143));
	build_placement.push_back(Point2D(166, 164));
	// build_placement.push_back(Point2D(162, 161));
	build_placement.push_back(Point2D(165, 138));
	// build_placement.push_back(Point2D(162, 166));
	// build_placement.push_back(Point2D(167, 161));
	build_placement.push_back(Point2D(157.5, 142.5));
	build_placement.push_back(Point2D(165.5, 154.5));
	build_placement.push_back(Point2D(155.5, 165.5));
	build_placement.push_back(Point2D(160.5, 145.5));
	build_placement.push_back(Point2D(158, 146));
	build_placement.push_back(Point2D(159.5, 148.5));
	build_placement.push_back(Point2D(162.5, 148.5));
	build_placement.push_back(Point2D(160, 151));
	build_placement.push_back(Point2D(157, 151));
	build_placement.push_back(Point2D(156.5, 148.5));
	build_placement.push_back(Point2D(153.5, 154.5));
	build_placement.push_back(Point2D(153.5, 148.5));
	build_placement.push_back(Point2D(153.5, 151.5));
	build_placement.push_back(Point2D(163.5, 145.5));
	build_placement.push_back(Point2D(164.5, 166.5));
}

void bot_master::load_bottom_right() {
	build_placement.push_back(Point2D(141, 32));
	build_placement.push_back(Point2D(140.5, 29.5));
	build_placement.push_back(Point2D(138, 29));
	build_placement.push_back(Point2D(143, 32));
	build_placement.push_back(Point2D(166, 28));
	// build_placement.push_back(Point2D(162, 31));
	build_placement.push_back(Point2D(138, 27));
	// build_placement.push_back(Point2D(167, 32));
	// build_placement.push_back(Point2D(163, 26));
	build_placement.push_back(Point2D(145.5, 32.5));	
	build_placement.push_back(Point2D(155.5, 26.5));
	build_placement.push_back(Point2D(165.5, 37.5));
	build_placement.push_back(Point2D(145.5, 29.5));
	build_placement.push_back(Point2D(148.5, 29.5));
	build_placement.push_back(Point2D(151.5, 34.5));
	build_placement.push_back(Point2D(142.5, 34.5));
	build_placement.push_back(Point2D(151, 32));
	build_placement.push_back(Point2D(152, 37));
	build_placement.push_back(Point2D(148.5, 34.5));
	build_placement.push_back(Point2D(149.5, 37.5));
	build_placement.push_back(Point2D(146.5, 38.5));
	build_placement.push_back(Point2D(154.5, 37.5));
	build_placement.push_back(Point2D(168.5, 29.5));
	build_placement.push_back(Point2D(165.5, 25.5));
}

void bot_master::load_bottom_left() {
	build_placement.push_back(Point2D(32, 51));
	build_placement.push_back(Point2D(29.5, 51.5));
	build_placement.push_back(Point2D(29, 54));
	build_placement.push_back(Point2D(32, 49));
	build_placement.push_back(Point2D(27, 27));
	// build_placement.push_back(Point2D(30, 31));
	build_placement.push_back(Point2D(27, 54));
	// build_placement.push_back(Point2D(32, 25));
	// build_placement.push_back(Point2D(24, 32));
	build_placement.push_back(Point2D(34.5, 49.5));
	build_placement.push_back(Point2D(26.5, 37.5));
	build_placement.push_back(Point2D(36.5, 26.5));
	build_placement.push_back(Point2D(34.5, 46.5));
	build_placement.push_back(Point2D(32, 46));
	build_placement.push_back(Point2D(34.5, 43.5));
	build_placement.push_back(Point2D(31.5, 43.5));
	build_placement.push_back(Point2D(30, 46));
	build_placement.push_back(Point2D(40, 43));
	build_placement.push_back(Point2D(37.5, 43.5));
	build_placement.push_back(Point2D(37.5, 40.5));
	build_placement.push_back(Point2D(40.5, 40.5));
	build_placement.push_back(Point2D(37.5, 46.5));
	build_placement.push_back(Point2D(29.5, 24.5));
	build_placement.push_back(Point2D(24.5, 28.5));
}

void bot_master::load_proxy_pylons() {
	proxy_pylons.push_back(Point2D(76, 70));
	proxy_pylons.push_back(Point2D(76, 121));
}

void bot_master::load_expansions() {
	expansions.push_back(Point2D(33.5, 158.5));
	expansions.push_back(Point2D(66, 163));
	expansions.push_back(Point2D(53, 134));
	expansions.push_back(Point2D(92, 158));
	expansions.push_back(Point2D(134, 139));
	expansions.push_back(Point2D(158.5, 158.5));
	expansions.push_back(Point2D(163, 126));
	expansions.push_back(Point2D(158, 98));
	expansions.push_back(Point2D(138, 58));
	expansions.push_back(Point2D(158.5, 33.5));
	expansions.push_back(Point2D(125, 29));
	expansions.push_back(Point2D(98, 34));
	expansions.push_back(Point2D(59, 53));
	expansions.push_back(Point2D(30, 67));
	expansions.push_back(Point2D(33.5, 33.5));
	expansions.push_back(Point2D(35, 93));
}

void bot_master::set_opp_position() {
    // assumes the opp base has been found 
    if (opp_base_found) {
        // sort expansions vector according to their distance to opp_base 
        selection_sort(expansions, opp_base);

        // now set the warp location based on our opponent's start loc
        warp_position = closest(warp_positions, opp_base);
    }
}