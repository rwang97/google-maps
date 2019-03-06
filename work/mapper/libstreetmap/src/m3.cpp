/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include <iostream>
#include "m1.h"
#include "m2.h"
#include "StreetsDatabaseAPI.h"
#include "m3.h"
#include <vector>
#include "map_data.h"
#include <math.h> 
#include "UCS.h"

using namespace std;
//=========== Global variable ==========//

// Global variable map which stores all the data structures
extern map_data* my_map;



//=========== Main algorithm implementation ===========//
// Returns the time required to travel along the path specified, in seconds. 
// The path is given as a vector of street segment ids, and this function 
// can assume the vector either forms a legal path or has size == 0.
// The travel time is the sum of the length/speed-limit of each street 
// segment, plus the given turn_penalty (in seconds) per turn implied by the path. 
// A turn occurs when two consecutive street segments have different street IDs.
double compute_path_travel_time(const std::vector<unsigned>& path, 
                                const double turn_penalty){
    
    double travel_time = 0;
    if(!path.empty()){
        // add the segment travel time
        travel_time = find_street_segment_travel_time(path[0]);
        for (unsigned i = 1; i < path.size(); i++){
            // add the turn penalty if two different street IDs
            travel_time = travel_time + find_street_segment_travel_time(path[i]);
            if(getStreetSegmentInfo(path[i - 1]).streetID != getStreetSegmentInfo(path[i]).streetID)  
                travel_time = travel_time + turn_penalty;
        }
    }
    
    return travel_time;
}

// Returns a path (route) between the start intersection and the end 
// intersection, if one exists. This routine should return the shortest path
// between the given intersections when the time penalty to turn (change
// street IDs) is given by turn_penalty (in seconds).
// If no path exists, this routine returns an empty (size == 0) vector. 
// If more than one path exists, the path with the shortest travel time is 
// returned. The path is returned as a vector of street segment ids; traversing 
// these street segments, in the returned order, would take one from the start 
// to the end intersection.
std::vector<unsigned> find_path_between_intersections(const unsigned intersect_id_start, 
                                                      const unsigned intersect_id_end,
                                                      const double turn_penalty) {
    
    
    std::vector<unsigned> path;
    unsigned num_intersections = getNumberOfIntersections();
    // check valid input
    if (intersect_id_start > num_intersections || intersect_id_end > num_intersections)
        return path;
    // find shortest path
    UCS(intersect_id_start, intersect_id_end, turn_penalty, path);
    return path;

}
  
// Returns the shortest travel time path (vector of street segments) from 
// the start intersection to a point of interest with the specified name.
// The path will begin at the specified intersection, and end on the 
// intersection that is closest (in Euclidean distance) to the point of 
// interest.
// If no such path exists, returns an empty (size == 0) vector.
std::vector<unsigned> find_path_to_point_of_interest(const unsigned intersect_id_start, 
                                               const std::string point_of_interest_name,
                                               const double turn_penalty) {

    std::vector<unsigned> path;
    
    // check whether the input is valid
    if (intersect_id_start > getNumberOfIntersections())
        return path;
    
    // Get the point of interest ids
    std::vector<unsigned> poi_ids = my_map -> get_poi_name_id()[point_of_interest_name];   
    if (!poi_ids.empty()) {
        std::vector<unsigned> intersection_end;
        intersection_end.resize(poi_ids.size());
        // get all nearest intersection for each poi
        for (unsigned i = 0; i < poi_ids.size(); i++) {
            intersection_end[i] = find_closest_intersection(getPointOfInterestPosition(poi_ids[i]));           
        }
        UCS_multi_target(intersect_id_start, intersection_end, turn_penalty, path); 
    }
    

    return path;
}
   
