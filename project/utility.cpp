#include "bot_master.h"
#include <sc2api/sc2_api.h>
#include <iostream>
#include <cmath>

using namespace sc2;

// returns the euclidean distance between two points 
double euclidean_dist(Point2D a, Point2D b) {
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