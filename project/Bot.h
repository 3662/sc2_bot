#pragma once

#include <sc2api/sc2_api.h>
#include <iostream>
#include <vector>

using namespace sc2;

class Bot : public Agent {
public:
	Bot();

	// called at the start of game
	// initializes the data like observational interface, starting locations etc. 
	// and send scout
	virtual void OnGameStart() final;

	// specifies what to do each step on game
	virtual void OnStep() final;

	// called when enemy unit enters vision 
	virtual void OnUnitEnterVision(const sc2::Unit *unit) final;

	virtual void OnBuildingConstructionComplete(const sc2::Unit *unit) final;

private:

	// manages idle units - "unit" parameter is the unit detected to be iddle
	// send probes back to mining
	// set buildings to produce units (if needed)
	// workers cap is 18
	virtual void OnUnitIdle(const Unit *unit) final;

	// send a worker to scout given location
	void scout(Point2D location);

	// builds the given type of structure in the given coordinates
	// for now building in a random location
	bool build(ABILITY_ID ability_type_for_structure, Point2D location);

	// create specific buildings
	bool build_pylon();
	bool build_forge();

	// copied from tutorial https://blizzard.github.io/s2client-api/
	// return number of units/buildings of given type
	size_t CountUnitType(UNIT_TYPEID unit_type);

	// copied from tutorial https://blizzard.github.io/s2client-api/
	// find closest mineral path from start (start is usually nexus from main base)
	const Unit *FindNearestMineralPatch(const Point2D &start);

	// returns true if probe is a scout 
	const bool is_scout(const Unit *unit);

	// data

	// contains probe scout
	const Unit* scout_unit;

	// counter for number of locations scouted 
	int scout_location;

	// opponent's potential start locations
	std::vector<Point2D> opp_location;

	// opponent's start location
	Point2D opp_base;
	bool opp_base_found;

	// start location (of our nexus)
	Point2D base;

	// observational interface used to 
	const ObservationInterface *observation;

	// index of build order and location
	int index;
	bool pylon_completed;
	bool forge_completed;

	// build order: contains <building-name, <mineral cost, gas cost>>
	std::vector<std::pair<ABILITY_ID, std::pair<int, int>>> build_order;

	// locations of where the buildings are going to be placed
	std::vector<Point2D> build_placement; 

	void load_top_left();
	void load_top_right();
	void load_bottom_right();
	void load_bottom_left();

};