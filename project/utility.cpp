/*
contains a variety of “helper” functions that are generic and independent of 
our bot functionality. Other modules can call its functions to, for example, 
sort a vector of points with respect to the distance to a given point, find the 
closest location of an object and a vector of point, or retrieve a random probe. 
*/

#include "bot_master.h"
#include <sc2api/sc2_api.h>
#include <iostream>
#include <cmath>

using namespace sc2;

// returns the euclidean distance between two points 
double bot_master::euclidean_dist(const Point2D a, const Point2D b) {
    return sqrt(pow(a.x-b.x, 2) + pow(a.y-b.y, 2));
}

// return Point2D in array closest to point p
Point2D bot_master::closest(std::vector<Point2D> a, Point2D p) {
    double dist;
    double min_dist = INFINITY;
    Point2D min_loc;

    for (const auto loc : a) {
        dist = euclidean_dist(loc, p);
        if (dist < min_dist) {
            min_dist = dist;
            min_loc = loc;
        }
    }

    return min_loc;
}

size_t bot_master::CountUnitType(UNIT_TYPEID unit_type) {
	return observation->GetUnits(Unit::Alliance::Self, IsUnit(unit_type)).size();
}

const Unit * bot_master::FindNearestMineralPatch(const Point2D &start) {
	Units units = observation->GetUnits(Unit::Alliance::Neutral);
	float distance = std::numeric_limits<float>::max();

	const Unit *target = nullptr;

	for (const auto &u : units) {
		if (u->unit_type == UNIT_TYPEID::NEUTRAL_MINERALFIELD) {
			float d = DistanceSquared2D(u->pos, start);
			if (d < distance) {
				distance = d;
				target = u;
			}
		}
	}

	return target;
}

void bot_master::selection_sort(std::vector<Point2D> &a, Point2D p) {
    // selection sort

    size_t size = a.size(); // size of vector

    for (int i = 0; i < size; ++i) {
        int index = i;                             // index of closest point
        double min_dist = euclidean_dist(a[i], p); // closest dist
        for (int j = i+1; j < size; ++j) {
            double dist = euclidean_dist(a[j], p);
            if (dist < min_dist) {
                // then update values 
                index = j;
                min_dist = dist;
            }
        }
        std::swap(a[i], a[index]); // swap them
    }

    // std::cout << "sorted:\n";
    // for (const auto e : a) {
    //     std::cout << e.x << " " << e.y << " " << euclidean_dist(e, p) << std::endl;
    // }   
}

const Unit * bot_master::random_probe() {
    const Unit *unit_selected = nullptr;

	Units units = observation->GetUnits(Unit::Alliance::Self);

	// get random probe that has no orther other than mine
	// ability_id for mining and bringing resources is 3666 and 3667
	for (const auto &unit : units) {
		// if probe, not a scout, not a gas worker and no orders then select this unit 
		if (unit->unit_type == UNIT_TYPEID::PROTOSS_PROBE && !is_builder(unit) &&
			!is_scout(unit) && gas_workers.find(unit) == gas_workers.end()) {
			for (auto &order : unit->orders) {
				// std::cout << order.ability_id << std::endl;
                if (unit->orders.size() == 1 && (order.ability_id == 3666 || order.ability_id == 3667)) {
                    // if probe is mining and has no queued orders select it
                    unit_selected = unit;
                }
			}
		} 
	}
    return unit_selected;
}



