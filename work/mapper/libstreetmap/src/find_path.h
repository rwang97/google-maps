/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   find_path.h
 * Author: donganzh
 *
 * Created on April 2, 2018, 10:29 PM
 */

#ifndef FIND_PATH_H
#define FIND_PATH_H

#include "StreetsDatabaseAPI.h"
#include "map_data.h"
#include "m1.h"
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <math.h>
#include <stack> 
#include <vector>
#include <deque>
#include <queue> 
#include <climits>
#include <list>

#define HI_SPD_LIM 120

struct cell {
    unsigned cell_index;

    float distance_from_start = INT_MAX;

    unsigned this_edge_index;

    std::vector<unsigned> path_intersection_id;

    std::vector<unsigned> path_segment_id;
};

struct intersection {
    int index = -1;

    int from = -1;

    int through = -1;

    float cost = INT_MAX;
};

struct POIcell {
    unsigned POI_id;

    std::string POI_name;

    POI_position position;

    n_point POI_coord;


};


class compare_cell {
public:

    bool const operator()(intersection &cellX, intersection &cellY) {
        return (cellX.cost > cellY.cost);
    }
};

class compare_POI {
public:

    bool const operator()(poi_data &POI1, poi_data &POI2) {
        return (POI1.distance < POI2.distance);
    }
};

class compare_POI2 {
public:

    bool const operator()(poi_data &POI1, poi_data &POI2) {
        return (POI1.distance < POI2.distance);
    }
};

class path_cell {
private:

public:

    std::priority_queue<intersection, std::vector<intersection>, compare_cell> cell_queue;
};


#endif /* FIND_PATH_H */

