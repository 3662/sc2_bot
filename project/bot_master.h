/*
header file that contains documentation of "Bot Master" and all the virtual and
defined functions we are going to use 

in this file can be found detailed description of all functions and variables 
in use
*/

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

private:

	// called when enemy unit enters vision 
	virtual void OnUnitEnterVision(const Unit *unit) final;

	// determines when buildings are completed 
	// useful to set the building completed booleans that are used to know
	// if we have the requirements for more advanced buildings
	virtual void OnBuildingConstructionComplete(const sc2::Unit *unit) final;

	// manages idle units - "unit" parameter is the unit detected to be iddle
	// send probes back to mining
	// set buildings to produce units (if needed)
	// workers cap is 18
	virtual void OnUnitIdle(const Unit *unit) final;

	// used to retrieve units or buildings that have been destoryed
	virtual void OnUnitDestroyed(const Unit *unit) final;

	// register when a unit has been created 
	// used to know when war prism was created so only one is queued
	virtual void OnUnitCreated(const Unit *unit) final;

	// mainly to know when warp gate research is completed
	virtual void OnUpgradeCompleted(UpgradeID upgrade_id);

	// send a worker to scout given location
	void scout(Point2D location);

	// returns true if probe is a scout 
	const bool is_scout(const Unit *unit);

	// set worker responsible to scout and send it to position
	void set_builder();

	// returns true if probe is a builder 
	const bool is_builder(const Unit *unit);

	// returns a random probe that is not a scout 
	const Unit * random_probe();

	// builds the given type of structure in the given coordinates
	// it checks pre-requisites such as mineral/vespene cost, required buildings
	// and proximity to a pylon depending on the building
	bool build(ABILITY_ID ability_type_for_structure, Point2D location, 
			   const Unit *unit);


	// build helper that determines location of neares vespene geyser and tries 
	// to build an assimilator on it
	bool build_vespene_geyser(const Unit *unit_to_build);

	// build helper that determines if the building to construct is near an 
	// existing pylon (pre-requisite)
	bool is_near_pylon(Point2D location);

	// handles upgrades e.g. wrapgrate research
	bool upgrade(ABILITY_ID upgrade_type);

	// copied from tutorial https://blizzard.github.io/s2client-api/
	// return number of units/buildings of given type
	size_t CountUnitType(UNIT_TYPEID unit_type);

	// copied from tutorial https://blizzard.github.io/s2client-api/
	// find closest mineral path from start (start is usually nexus from main base)
	const Unit * FindNearestMineralPatch(const Point2D &start);

	// set initial variables
	void set_variables();

	// set build order
	void load_build_order();

	// set location of buildings based on starting location
	void load_top_left();
	void load_top_right();
	void load_bottom_right();
	void load_bottom_left();

	// set locations of proxy pylons 
	void load_proxy_pylons();

	// set the 16 potential expansions
	void load_expansions();

	// returns the euclidean distance between two points 
	double euclidean_dist(const Point2D a, const Point2D b);

	// given an array of Point2D and a point return the coordinates in array
	// closest to point
	Point2D closest(std::vector<Point2D> a, Point2D p);

	// custom selection sort
	// given an array of points a sort them in non-decreasing order based 
	// on their distance to point p
	void selection_sort(std::vector<Point2D> &a, Point2D p);

	// set warp prism position and sort expansions according to distance to 
	// opponent's location after having found it 
	void set_opp_position();

	// data

	const ObservationInterface *observation; 

	// build order
	std::vector<ABILITY_ID> build_order;

	// locations of where the buildings are going to be placed
	std::vector<Point2D> build_placement; 

	// location of proxy pylons
	std::vector<Point2D> proxy_pylons;

	// set containing the probes working on assimilators
	std::set<const Unit*> gas_workers; 

	// list of location of existing pylons (need to know since we have to place 
	// buildings near pylons)
	std::vector<Point2D> pylons;

	Point2D base;                        // start location (of our nexus)
	Point2D expansion;					 // location of expansion
	std::vector<Point2D> opp_location;   // opponent's potential start locations
	std::vector<Point2D> warp_positions; // all the potential warp positions
	std::vector<Point2D> expansions;     // all 16 potential expansion on the map
	Point2D opp_base;                    // opponent's start location
	Point2D warp_position; 			     // location where the warp prism will warp units

	const Unit *scout_unit;   			 // contains probe scout
	const Unit *builder_unit; 			 // probe responsible for expansion

	int index_build;            		 // index of build order and location
	int index_pylon;		
	int index_patroll;          	     // index of element in "expansions" that is currently 
										 // being atacked by our units	
	int scout_location;         		 // counter for number of locations scouted 

	// bool variables that give information regarding buildings and upgrades 
	// being completed. used mostly to check pre-requisites 
	bool opp_base_found;
	bool pylon_completed;
	bool forge_completed;
	bool gateway_completed;
	bool core_completed;
	bool twilight_completed;
	bool robotics_completed;

	bool research_wrapgate;
	bool warp_ready;
	bool warp_prism;

	// some buildings used to research abilities or create core units
	const Unit *cybernetics;
	const Unit *robotics;
};