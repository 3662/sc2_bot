#pragma once

#include <sc2api/sc2_api.h>
#include <iostream>
#include <vector>
#include <set>

using namespace sc2;

class bot_master : public Agent {
public:
	bot_master();

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

	// returns a random probe that is not a scout 
	const Unit * random_probe();

	// builds the given type of structure in the given coordinates
	// it checks pre-requisites such as mineral/vespene cost, required buildings
	// and proximity to a pylon depending on the building
	bool build(ABILITY_ID ability_type_for_structure, Point2D location);


	// build helper that determines location of neares vespene geyser and tries 
	// to build an assimilator on it
	bool build_vespene_geyser(const Unit *unit_to_build);

	// build helper that determines if the building to construct is near an 
	// existing pylon (pre-requisite)
	bool is_near_pylon(Point2D location);

	// copied from tutorial https://blizzard.github.io/s2client-api/
	// return number of units/buildings of given type
	size_t CountUnitType(UNIT_TYPEID unit_type);

	// copied from tutorial https://blizzard.github.io/s2client-api/
	// find closest mineral path from start (start is usually nexus from main base)
	const Unit *FindNearestMineralPatch(const Point2D &start);

	// returns true if probe is a scout 
	const bool is_scout(const Unit *unit);

	// set initial variables
	void set_variables();

	// set build order
	void load_build_order();

	// set location of buildings based on starting location
	void load_top_left();
	void load_top_right();
	void load_bottom_right();
	void load_bottom_left();

	// data

	const ObservationInterface *observation; 

	// build order
	std::vector<ABILITY_ID> build_order;

	// locations of where the buildings are going to be placed
	std::vector<Point2D> build_placement; 

	// set containing the probes working on assimilators
	std::set<const Unit*> gas_workers; 

	// list of location of existing pylons (need to know since we have to place 
	// buildings near pylons)
	std::vector<Point2D> pylons;

	Point2D base;                      // start location (of our nexus)
	std::vector<Point2D> opp_location; // opponent's potential start locations
	Point2D opp_base;                  // opponent's start location

	const Unit *scout_unit; // contains probe scout
	const Unit *builder_unit; 

	int index;            // index of build order and location
	int scout_location;   // counter for number of locations scouted 

	bool opp_base_found;
	bool pylon_completed;
	bool forge_completed;
	bool gateway_completed;
	bool core_completed;
	bool twilight_completed;
	const Unit *sybernetiscore;
};