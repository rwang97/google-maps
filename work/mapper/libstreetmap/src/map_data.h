/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   map_data.h
 * Author: wangy610
 *
 * Created on February 5, 2018, 8:39 AM
 * A class for all my data structures
 */

#ifndef MAP_DATA_H
#define MAP_DATA_H

#define MAP_SIZE_REFERENCE  0.43998

#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/geometries/register/point.hpp>
#include <boost/geometry/index/rtree.hpp>
#include "StreetsDatabaseAPI.h"
#include "OSMDatabaseAPI.h"
#include "TST.h"
#include "graphics.h"
#include "BK_Tree.h"
#include "m1.h"
#include <libstreetmap.h>

#define TURNING_TOLERANCE  90
#define STRAIGHT_TOLERANCE  9
/*********** constants & structs for m1 ****************/
const double EARTH_RADIUS_METERS = 6372797.560856;
const double DEG_TO_RADIAN = 0.017453292519943295769236907684886;
const double RADIAN_TO_DEG = 57.295779513;

namespace bg = boost::geometry;
namespace bgi = boost::geometry::index;
typedef bg::model::point<double, 2, bg::cs::spherical_equatorial<bg::degree>> r_point;
typedef std::pair<r_point, unsigned> value;

typedef bg::model::point<double, 2, bg::cs::cartesian> n_point;
typedef std::pair<n_point, POIIndex> POI_position;

extern double avg_lat;
/*************** structs for m2 ********************/

// struct for intersections
struct intersection_data {
    LatLon position;
    std::string name;
};

// struct for point_of_interest
struct poi_data {
    unsigned POI_index;
    LatLon position;
    std::string name; 
    std::string type;
    OSMID POI_OSM;
    bool draw = false;
    float distance = INT_MAX;
    unsigned cloest_intersection;
};

// struct for street segments
struct street_segment_data {
    std::string type;
    std::string street_name;
    float speedLimit;
    bool oneWay;
    std::vector<LatLon> position_pts;
    std::vector<std::pair<double, double>> position_xy;
};

// struct for info for each feature
struct feature_info {
    
    std::string name;
    FeatureType type;
    std::vector<LatLon> position_pts;
    OSMID featureOSMID;
    //bool draw = false;
};

// struct for features
struct feature_data {
    std::vector<feature_info> unknown;
    std::vector<feature_info> park;
    std::vector<feature_info> beach;
    std::vector<feature_info> lake;
    std::vector<feature_info> river;
    std::vector<feature_info> island;
    std::vector<feature_info> shoreline;
    std::vector<feature_info> building;
    std::vector<feature_info> greenspace;
    std::vector<feature_info> golfcourse;
    std::vector<feature_info> stream;

};

// struct for street_segment
struct street_segment_drawing {
    t_color color;
    int linewidth;
};

// struct for zoom in and out
struct zoom_info{
    float left;
    float right;
    float top;
    float bottom;
};

// struct for display button options
struct buttons{
    bool uni_on = false;
    bool hospital_on = false;
    bool bank_on = false;
    bool dinning_on = false;
    bool petro_on = false;
    bool park_on = false;
    bool all_on = false;
    bool feature_on = false;
    bool navi_on = false;
    bool help_on=false;
};

// struct for different icons
struct Loaded_surface {
    Surface uni = load_png_from_file("libstreetmap/resources/University2.png");
    Surface art = load_png_from_file("libstreetmap/resources/Arts_centre2.png");
    Surface bank = load_png_from_file("libstreetmap/resources/Bank2.png");
    Surface bar = load_png_from_file("libstreetmap/resources/Bar2.png");
    Surface bus = load_png_from_file("libstreetmap/resources/Bus_station2.png");
    Surface cafe = load_png_from_file("libstreetmap/resources/Cafe2.png");
    Surface cinema = load_png_from_file("libstreetmap/resources/Cinema2.png");
    Surface court = load_png_from_file("libstreetmap/resources/Courthouse2.png");
    Surface embassy = load_png_from_file("libstreetmap/resources/Embassy2.png");
    Surface ferry = load_png_from_file("libstreetmap/resources/Ferry-icon2.png");
    Surface fire = load_png_from_file("libstreetmap/resources/Fire-station2.png");
    Surface fuel = load_png_from_file("libstreetmap/resources/Fuel2.png");
    Surface hospital = load_png_from_file("libstreetmap/resources/Hospital2.png");
    Surface library = load_png_from_file("libstreetmap/resources/Library2.png");
    Surface market = load_png_from_file("libstreetmap/resources/Marketplace2.png");
    Surface club = load_png_from_file("libstreetmap/resources/Nightclub2.png");
    Surface parking = load_png_from_file("libstreetmap/resources/Parking2.png");
    Surface police = load_png_from_file("libstreetmap/resources/Police2.png");
    Surface post = load_png_from_file("libstreetmap/resources/Post_office2.png");
    Surface bicycle = load_png_from_file("libstreetmap/resources/Rental-bicycle2.png");
    Surface dinning = load_png_from_file("libstreetmap/resources/Restaurant2.png");
    Surface theatre = load_png_from_file("libstreetmap/resources/Theatre2.png");
    Surface townhall = load_png_from_file("libstreetmap/resources/Town-hall2.png");
};

struct navi_info{
    unsigned street_id;
    std::string street_name;
    double length;
    std::string direction;
};
/************************* Struct for m4 ******************************/
struct intersection_edge_info {
    unsigned streetOfSeg;
    unsigned connected_intersection;
    unsigned edge_index;
    float time_to_next_intersection;
    float length_to_next_intersection;
};

struct swap_node{
    unsigned start;
    unsigned destinataion;
    float time_cost;
};

struct delivery_node {
    unsigned nodeID;
    delivery_node* pick_up;
    //if this node is a drop off point -> pick_up = its pick up point node
    //else if the node is a pick up point -> its pick up is NULL
};

struct valid_dropoff {
    unsigned nodeID;
    n_point node_position;
};

// Function to find the distance between between two points
double find_distance(LatLon point1, LatLon point2);

// Conversions from lat,lon to cartesian or the other way around
double longitude_to_cartesian(float longitude, double average_latitude);

double latitude_to_cartesian(float latitude);

float cartesian_to_longitude(double x, double average_latitude);

float cartesian_to_latitude(double y);

//extern std::map<OSMID, const OSMWay*>way_map;
//extern std::unordered_map<OSMID, unsigned long> node_map; 

//t_point LatLon_XY_converter(LatLon input, double average_latitude);

class map_data {
    
 private:  
    // For finding the street segment length
    double find_segment_length(unsigned street_segment_id);
    
    /*************** data structures for m1 ********************/    
    // Structure 1, store street name as key and street id as value, into an unordered map 
    std::unordered_map<std::string, std::vector<unsigned>> street_name_id;

    // Structure 2, store street segments into intersection
    std::vector<std::vector<unsigned>> segments_inside_intersection;

    // Structure 3, store street segments into street
    std::vector<std::vector<unsigned>> segments_inside_street;

    // Structure 4, store intersection into street
    std::vector<std::unordered_set<unsigned>> intersection_inside_street;

    // Structure 5, store street segment length into a vector
    std::vector<double> segment_length;

    // Structure 6, store travel time for each street segment
    std::vector<double> segment_travel_time;

    // Structure 7, store intersection into r tree
    bgi::rtree<value, bgi::quadratic<16>> intersection_coordinate_id;

    // Structure 8, store point of interest into r tree
    bgi::rtree<value, bgi::quadratic<16>> poi_coordinate_id;   
          
    /*************** data structures for m2 ********************/    
    // For data structures of intersections, poi, street_segments, features
    std::vector<intersection_data> intersections;
    std::vector<poi_data> poi;
    std::unordered_map<std::string, std::vector<unsigned>> poi_name_id;
    std::vector<street_segment_data> expressways;
    std::vector<street_segment_data> other_street_segments;
    feature_data features;

    // For tenary search tree, strore street names and intersection names, poi names
    TST* street_names_TST = new TST();
    TST* poi_names_TST = new TST();  
    std::vector<std::vector<intersection_edge_info> > connected_intersections;
    //std::map<OSMID, const OSMWay*>way_map;
    //std::unordered_map<OSMID, const OSMNode* > node_map; 
    
    /*************** data structures for m3 ********************/   
    TST* button_names_TST = new TST();   
    BK_Tree* street_names_BK = new BK_Tree();
    BK_Tree* poi_names_BK = new BK_Tree();
    
    std::unordered_map<unsigned, street_segment_data> segment_drawing_table;
    
    /*************** load data functions for m1 ********************/   
    // load the street_name_id structure, structure 1
    void load_street_name_to_id_map();
    
    // load structure 2,3,4,5,6 structures by looping through street segments
    void parse_street_segments_to_data_structures();
    
    // load r tree structure for intersection
    void load_intersection_rtree();
    
    // load r tree structure for poi;
    void load_poi_rtree();
   
    /*************** load data functions for m2 ********************/   
    // loading intersection data structures
    void load_intersections();

    // loading poi data structure
    void load_poi();

    // helper to load street segment structure
    void load_segments_data(unsigned id, StreetSegmentInfo& info, bool isExpressWay);

    // loading street segments structure
    void load_street_segments();

    // loading feature structure
    void load_features();

    // load poi names into a ternary tree
    void load_poi_names();

    // load street_names and intersection names data structure into a ternary search tree
    void load_street_names();
    
    /*************** load data functions for m3 ********************/  
    // load all button names for help button
    void load_button_names();
    
    // load street names for auto correction
    void load_street_names_BK();
    
    // load poi names for auto correction
    void load_poi_names_BK();
    
    //void load_way_map();
    
public:
    // Constructor and Destructor
    map_data();
    
    virtual ~map_data();
    
    /*************** Getters for m1 ********************/ 
    std::unordered_map<std::string, std::vector<unsigned>>& get_street_name_id();
    
    std::vector<unsigned>& get_id_from_street_name(std::string& street_name);
  
    std::vector<unsigned>& get_segments_inside_intersection(unsigned& intersection_id);
    
    std::vector<unsigned>& get_segments_inside_street(unsigned& street_id);
    
    std::unordered_set<unsigned>& get_intersection_inside_street(unsigned& street_id);
    
    double& get_segment_length(unsigned& street_segment);
    
    double& get_segment_travel_time(unsigned& street_segment);
    
    bgi::rtree<value, bgi::quadratic<16>>& get_intersection_coordinate_id();
    
    bgi::rtree<value, bgi::quadratic<16>>& get_poi_coordinate_id();
    
    /*************** Getters for m2 ********************/ 
    std::vector<intersection_data>& get_intersections();
    
    std::vector<poi_data>& get_poi();
    
    std::unordered_map<std::string, std::vector<unsigned>>& get_poi_name_id();
    
    std::vector<street_segment_data>& get_expressways();
    
    std::unordered_map<unsigned, street_segment_data>& get_segment_drawing_table();
    
    std::vector<std::vector<intersection_edge_info> > & get_connected_intersections();
    
    std::vector<street_segment_data>& get_other_street_segments();
    
    feature_data& get_features();

    TST*& get_street_names_TST();
    
    TST*& get_poi_names_TST(); 
    
    //std::map<OSMID, const OSMWay*>& get_way_map();
    //std::unordered_map<OSMID, const OSMNode* > & get_node_map(); 
    
    /*************** Getters for m3 ********************/ 
    TST*& get_help_button_names_TST(); 
   
    BK_Tree*& get_street_names_BK();
    
    BK_Tree*& get_poi_names_BK();
};


#endif /* MAP_DATA_H */

