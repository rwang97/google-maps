/* 
 * Copyright 2018 University of Toronto
 *
 * Permission is hereby granted, to use this software and associated 
 * documentation files (the "Software") in course work at the University 
 * of Toronto, or for personal use. Other uses are prohibited, in 
 * particular the distribution of the Software either publicly or to third 
 * parties.
 *
 * The above copyright notice and this permission notice shall be included in 
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#include "m1.h"
#include "math.h"
#include "algorithm"
#include "map_data.h"

// Global variable map which stores all the data structures
map_data *my_map;

// helper function to concatenate and sort
void helper_find_intersection_ids_from_street_names(std::string& street_name, std::vector<unsigned>& intersection);

bool load_map(std::string map_path) {
    
    //Indicates whether the map has loaded successfully    
    if(!loadStreetsDatabaseBIN(map_path)) 
        return false;
    
    //Create a new map
    my_map = new map_data;
 //   map->get_osm_info();
    
    return true;
}

void close_map() {
    
    //Clean-up your map related data structures here  
    delete my_map;  
    closeStreetDatabase();

    return; 
}

//Returns street id(s) for the given street name
//If no street with this name exists, returns a 0-length vector.
//Search for the name in the structure using .find function
std::vector<unsigned> find_street_ids_from_name(std::string street_name){
    std::vector<unsigned> str_ids;

    auto it = my_map-> get_street_name_id().find(street_name);
    if (it == my_map-> get_street_name_id().end())
        return str_ids;
    else
        return my_map-> get_id_from_street_name(street_name);
}

//Returns the street segments for the given intersection 
//Return from the data structure
std::vector<unsigned> find_intersection_street_segments(unsigned intersection_id){
    return my_map-> get_segments_inside_intersection(intersection_id); 
}

//Returns the street names at the given intersection (includes duplicate street 
//names in returned vector)
std::vector<std::string> find_intersection_street_names(unsigned intersection_id){
    std::vector<std::string> street_names;
    // loop with number of segments times, find all the street names from segments info
    for (unsigned i = 0; i < getIntersectionStreetSegmentCount(intersection_id); i++) {
        unsigned str_id = getStreetSegmentInfo(my_map-> get_segments_inside_intersection(intersection_id)[i]).streetID;
        street_names.push_back(getStreetName(str_id));
    } 
    return street_names;
}

//Returns true if you can get from intersection1 to intersection2 using a single 
//street segment (hint: check for 1-way streets too)
//corner case: an intersection is considered to be connected to itself
bool are_directly_connected(unsigned intersection_id1, unsigned intersection_id2){
    
    // corner case, two arguments are the same
    if (intersection_id1 == intersection_id2)
        return true;
    
    std::vector<unsigned> street_seg = my_map-> get_segments_inside_intersection(intersection_id1);
    for (unsigned i = 0; i < street_seg.size(); i++) {

        StreetSegmentInfo info = getStreetSegmentInfo(street_seg[i]);
        // if id1 is "from" and id2 is "to"
        if (intersection_id1 == info.from) {
            if (intersection_id2 == info.to)
                return true;
        }
        
        // if id1 is "to", id2 is "from", and its two-way segment
        else if (!info.oneWay && intersection_id2 == info.from)
            return true;
    }
    
    return false;
}

//Returns all intersections reachable by traveling down one street segment 
//from given intersection (hint: you can't travel the wrong way on a 1-way street)
//the returned vector should NOT contain duplicate intersections
std::vector<unsigned> find_adjacent_intersections(unsigned intersection_id){

    
    std::vector<unsigned> adj_intersection;
    std::vector<unsigned> ss_inside = my_map->get_segments_inside_intersection(intersection_id);
    
    for (unsigned i = 0; i < ss_inside.size(); i++) {
        
        StreetSegmentInfo info = getStreetSegmentInfo(ss_inside[i]);
        
        // if the intersection is the "from", all the "to"s are adjacent
        if (info.from == intersection_id) {
            // check if current "to" is already stored
            auto it = std::find(adj_intersection.begin(),adj_intersection.end(), info.to);
            if (it == adj_intersection.end())
                adj_intersection.push_back(info.to);
        }
        
        // if the intersection is the "to", and it's two way segment, then the "from" is adjacent
        else if (!info.oneWay) {
            // check if current "from" is already stored
            auto it = std::find(adj_intersection.begin(),adj_intersection.end(), info.from);
            if (it == adj_intersection.end())
            adj_intersection.push_back(info.from); 
        }
    }
    return adj_intersection;
}

//Returns all street segments for the given street
std::vector<unsigned> find_street_street_segments(unsigned street_id){

    return my_map-> get_segments_inside_street(street_id);

}

//Returns all intersections along the a given street
std::vector<unsigned> find_all_street_intersections(unsigned street_id){
    std::vector<unsigned> intersection(my_map-> get_intersection_inside_street(street_id).begin(), 
                                       my_map-> get_intersection_inside_street(street_id).end());
    return intersection;
}

//Return all intersection ids for two intersecting streets
//This function will typically return one intersection id.
//However street names are not guarenteed to be unique, so more than 1 intersection id
//may exist

// helper function to concatenate and sort
void helper_find_intersection_ids_from_street_names(std::string& street_name, std::vector<unsigned>& intersection) {
    
    std::vector<unsigned> street_id = find_street_ids_from_name(street_name);
    
    std::vector<unsigned> temp;
    // Concatenate all intersections for street name
    for(unsigned i = 0; i < street_id.size(); i++) {
        temp = find_all_street_intersections(street_id[i]);
        intersection.insert(intersection.begin(), temp.begin(), temp.end()); 
    }    
    // Sort the two vectors
    std::sort(intersection.begin(),intersection.end());
}

std::vector<unsigned> find_intersection_ids_from_street_names(std::string street_name1,
        std::string street_name2){
    
    std::vector<unsigned> empty_ids;
    if (street_name1.empty() || street_name2.empty())
        return empty_ids;
//Concatenate vectors and use set_intersection from std algorithm    
//set_intersection: Reference: http://www.cplusplus.com/reference/algorithm/set_intersection/
//Concatenating two vectors: https://stackoverflow.com/questions/201718/concatenating-two-stdvectors
    std::vector<unsigned> temp, intersection1, intersection2; 

    // call helper functions to concatenate and sort
    helper_find_intersection_ids_from_street_names(street_name1, intersection1);
    
    helper_find_intersection_ids_from_street_names(street_name2, intersection2);
    
    // create intersection_id vector
    std::vector<unsigned> intersection_ids(intersection1.size() + intersection2.size());
    
    // Find the intersection of the two vectors
    auto it = std::set_intersection(intersection1.begin(), intersection1.end(), 
                          intersection2.begin(), intersection2.end(),
                          intersection_ids.begin());
    
    intersection_ids.resize(it - intersection_ids.begin());

    return intersection_ids;    
}

//Returns the distance between two coordinates in meters
double find_distance_between_two_points(LatLon point1, LatLon point2){

    return find_distance(point1, point2);
}

//Returns the length of the given street segment in meters
double find_street_segment_length(unsigned street_segment_id){
    
    return my_map -> get_segment_length(street_segment_id);
}

//Returns the length of the specified street in meters
double find_street_length(unsigned street_id) {
    
    double street_len = 0;
    
    // adding the length of all street segments 
    std::vector<unsigned> segments = my_map-> get_segments_inside_street(street_id);
    for (unsigned i = 0; i < segments.size(); i++) {
        street_len = street_len + my_map-> get_segment_length(segments[i]);
    }
    
    return street_len;
}

//Returns the travel time to drive a street segment in seconds 
//(time = distance/speed_limit)
double find_street_segment_travel_time(unsigned street_segment_id){
    
    return my_map-> get_segment_travel_time(street_segment_id);

}

//Returns the nearest point of interest to the given position
unsigned find_closest_point_of_interest(LatLon my_position){
    // use r tree to find nearest point
    std::vector<value> result_poi;
    my_map-> get_poi_coordinate_id().query(bgi::nearest(r_point(my_position.lon(),my_position.lat()), 1), std::back_inserter(result_poi));
    return result_poi[0].second;
}

//Returns the the nearest intersection to the given position
unsigned find_closest_intersection(LatLon my_position){
    // use r tree to find nearest point
    std::vector<value> result_intersection;
    my_map-> get_intersection_coordinate_id().query(bgi::nearest(r_point(my_position.lon(), my_position.lat()), 1), std::back_inserter(result_intersection));
    return result_intersection[0].second;
}
