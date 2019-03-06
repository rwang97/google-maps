/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   map_data.cpp
 * Author: wangy610
 * 
 * Created on February 5, 2018, 8:39 AM
 */

#include "map_data.h"

//std::map<OSMID, const OSMWay*>way_map;
//std::unordered_map<OSMID, const OSMNode* > node_map; 

map_data::map_data() {

    /*************** load data functions for m1 ********************/
    //================ Structure 1 =======================
    load_street_name_to_id_map();

    //=============== Structure 2,3,4,5,6 ====================
    parse_street_segments_to_data_structures();

    //=============== Structure 7 ====================
    load_intersection_rtree();

    //=============== Structure 8 ====================
    load_poi_rtree();

    /*************** load data functions for m2 ********************/
    load_intersections();

    load_poi();

    load_street_segments();

    load_features();

    load_poi_names();

    load_street_names();

    /*************** load data functions for m3 ********************/
    load_button_names();

    load_street_names_BK();

    load_poi_names_BK();
}

map_data::~map_data() {

    segments_inside_street.clear();
    segments_inside_intersection.clear();
    street_name_id.clear();
    intersection_inside_street.clear();
    segment_length.clear();
    segment_travel_time.clear();
    poi_coordinate_id.clear();
    intersection_coordinate_id.clear();
    intersections.clear();
    poi.clear();
    poi_name_id.clear();
    expressways.clear();
    other_street_segments.clear();
    delete street_names_TST;
    delete poi_names_TST;
    delete button_names_TST;
    delete street_names_BK;
    delete poi_names_BK;
}

/*************** load data functions for m1 ********************/
// load the street_name_id structure, structure 1

void map_data::load_street_name_to_id_map() {

    // Store street name as key and street id as value, into an unordered map 
    for (unsigned i = 0; i < getNumberOfStreets(); i++) {
        street_name_id[getStreetName(i)].push_back(i);
    }
}

// load structure 2,3,4,5,6 structures by looping through street segments

void map_data::parse_street_segments_to_data_structures() {

    // resize the data structures
    segments_inside_intersection.resize(getNumberOfIntersections());
    segments_inside_street.resize(getNumberOfStreets());
    intersection_inside_street.resize(getNumberOfStreets());
    segment_length.resize(getNumberOfStreetSegments());
    segment_travel_time.resize(getNumberOfStreetSegments());

    for (unsigned ss_id = 0; ss_id < getNumberOfStreetSegments(); ss_id++) {
        StreetSegmentInfo info = getStreetSegmentInfo(ss_id);
        //Structure 2
        //Store street segments into intersection
        segments_inside_intersection[info.from].push_back(ss_id);
        segments_inside_intersection[info.to].push_back(ss_id);
        //Structure 3
        //Store the segment street id in the vector type street array
        segments_inside_street[info.streetID].push_back(ss_id);
        //Structure 4
        //Store intersection into street
        intersection_inside_street[info.streetID].insert(info.from);
        intersection_inside_street[info.streetID].insert(info.to);
        //Structure 5
        //Store street segment length into a vector
        segment_length[ss_id] = find_segment_length(ss_id);
        //Structure 6
        //Store travel time for each street segment
        segment_travel_time[ss_id] = segment_length[ss_id] / info.speedLimit * 3.6;
    }
}

// load r tree structure for intersection

void map_data::load_intersection_rtree() {

    //Store intersection into r tree
    for (unsigned intersection = 0; intersection < getNumberOfIntersections(); intersection++) {

        LatLon latlon = getIntersectionPosition(intersection);
        intersection_coordinate_id.insert(std::make_pair(r_point(latlon.lon(), latlon.lat()), intersection));
    }
}

// load r tree structure for poi;

void map_data::load_poi_rtree() {

    //store point of interest into r tree
    for (unsigned poi_id = 0; poi_id < getNumberOfPointsOfInterest(); poi_id++) {
        LatLon latlon = getPointOfInterestPosition(poi_id);
        poi_coordinate_id.insert(std::make_pair(r_point(latlon.lon(), latlon.lat()), poi_id));
    }

}

/*************** Getters for m1 ********************/
std::unordered_map<std::string, std::vector<unsigned>>&map_data::get_street_name_id() {
    return street_name_id;
}

std::vector<unsigned>& map_data::get_id_from_street_name(std::string& street_name) {
    return street_name_id[street_name];
}

std::vector<unsigned>& map_data::get_segments_inside_intersection(unsigned& intersection_id) {
    return segments_inside_intersection[intersection_id];
}

std::vector<unsigned>& map_data::get_segments_inside_street(unsigned& street_id) {
    return segments_inside_street[street_id];
}

std::unordered_set<unsigned>& map_data::get_intersection_inside_street(unsigned& street_id) {
    return intersection_inside_street[street_id];
}

double& map_data::get_segment_length(unsigned& street_segment) {
    return segment_length[street_segment];
}

double& map_data::get_segment_travel_time(unsigned& street_segment) {
    return segment_travel_time[street_segment];
}

bgi::rtree<value, bgi::quadratic < 16 >> &map_data::get_intersection_coordinate_id() {
    return intersection_coordinate_id;
}

bgi::rtree<value, bgi::quadratic < 16 >> &map_data::get_poi_coordinate_id() {
    return poi_coordinate_id;
}

/*************** load data functions for m2 ********************/
// loading intersection data structures

void map_data::load_intersections() {

    intersections.resize(getNumberOfIntersections());

    for (unsigned id = 0; id < getNumberOfIntersections(); id++) {
        intersections[id].position = getIntersectionPosition(id);
        intersections[id].name = getIntersectionName(id);
//        std::vector<unsigned> this_connected_intersection =
//                find_adjacent_intersections(id);
//        std::vector<unsigned> this_connected_segment =
//                find_intersection_street_segments(id);
//        std::vector<intersection_edge_info>this_connected_edge_info;
//        if (this_connected_intersection.size() > 0) {
//            for (unsigned i = 0; i < this_connected_intersection.size(); i++) {
//                for (unsigned j = 0; j < this_connected_segment.size(); j++) {
//                    StreetSegmentInfo checkInfo = getStreetSegmentInfo(
//                            this_connected_segment[j]);
//
//                    if (checkInfo.from = this_connected_intersection[i] &&
//                            (!checkInfo.oneWay) && (checkInfo.to == id)) {
//
//                        double this_edge_length = segment_length[this_connected_segment[j]];
//                        intersection_edge_info this_edge_info;
//
//                        this_edge_info.connected_intersection = checkInfo.from;
//                        this_edge_info.edge_index = this_connected_segment[j];
//                        this_edge_info.time_to_next_intersection = (this_edge_length
//                                / checkInfo.speedLimit * 3.6);
//                        this_edge_info.length_to_next_intersection = this_edge_length;
//                        this_edge_info.streetOfSeg = checkInfo.streetID;
//                        this_connected_edge_info.push_back(this_edge_info);
//
//                    } else if (checkInfo.to == this_connected_intersection[i] && checkInfo.from == id) {
//
//                        double this_edge_length = segment_length[this_connected_segment[j]];
//                        intersection_edge_info this_edge_info;
//                        this_edge_info.connected_intersection = checkInfo.to;
//                        this_edge_info.edge_index = this_connected_segment[j];
//                        this_edge_info.time_to_next_intersection = this_edge_length
//                                / checkInfo.speedLimit * 3.6;
//                        this_edge_info.length_to_next_intersection = this_edge_length;
//                        this_edge_info.streetOfSeg = checkInfo.streetID;
//                        this_connected_edge_info.push_back(this_edge_info);
//                    }
//                }
//            }
//        }
//        connected_intersections.push_back(this_connected_edge_info);
    }
}

// loading poi data structure

void map_data::load_poi() {

    poi.resize(getNumberOfPointsOfInterest());

    for (unsigned id = 0; id < getNumberOfPointsOfInterest(); ++id) {
        poi[id].position = getPointOfInterestPosition(id);
        poi[id].name = getPointOfInterestName(id);
        poi[id].type = getPointOfInterestType(id);
        poi_name_id[getPointOfInterestName(id)].push_back(id);
    }
}

// helper to load street segment structure

void map_data::load_segments_data(unsigned id, StreetSegmentInfo& info, bool isExpressWay) {

    street_segment_data new_data;

    // get the information about whether its oneway, speed limit and position of 
    //start, end, and curve points in between
    new_data.oneWay = info.oneWay;
    new_data.speedLimit = info.speedLimit;
    new_data.street_name = getStreetName(info.streetID);

    // store the starting point position
    new_data.position_pts.push_back(getIntersectionPosition(info.from));
    //double start_x = longitude_to_cartesian(getIntersectionPosition(info.from).lon(), avg_lat);
    //double start_y = latitude_to_cartesian(getIntersectionPosition(info.from).lat());
    //new_data.position_xy.push_back(std::make_pair(start_x, start_y));

    // store the points in middle
    if (info.curvePointCount != 0) {
        for (unsigned i = 0; i < info.curvePointCount - 1; i++) {
            new_data.position_pts.push_back(getStreetSegmentCurvePoint(id, i));
            //double mid_x = longitude_to_cartesian(getStreetSegmentCurvePoint(id, i).lon(), avg_lat);
            //double mid_y = latitude_to_cartesian(getStreetSegmentCurvePoint(id, i).lat());
            //new_data.position_xy.push_back(std::make_pair(mid_x, mid_y));
        }
    }
    // store the ending point 
    new_data.position_pts.push_back(getIntersectionPosition(info.to));
    //double end_x = longitude_to_cartesian(getIntersectionPosition(info.to).lon(), avg_lat);
    //double end_y = latitude_to_cartesian(getIntersectionPosition(info.to).lat());
    //new_data.position_xy.push_back(std::make_pair(end_x, end_y));

    if (isExpressWay) expressways.push_back(new_data);
    else other_street_segments.push_back(new_data);

    //segment_drawing_table.insert({id, new_data});

    //OSMID seg_OSMID = info.wayOSMID;

    //unsigned num_tag = getTagCount(way_map[seg_OSMID]);
    //To set the type of the segment
    //    for (unsigned i = 0; i < num_tag; i++){
    //        std::pair<std::string, std::string>tag = getTagPair(way_map[seg_OSMID], i);
    //        
    //        if(tag.first == "highway"){
    //            if(tag.second == "motorway" || tag.second == "motorway_link"){
    //                new_data.type = "motorway";
    //                break;
    //            }else if(tag.second == "trunk" || tag.second == "trunk_link"){
    //                new_data.type = "trunk";
    //                break;
    //            }else if(tag.second == "primary" || tag.second == "primary_link"){
    //                new_data.type = "primary";
    //                break;
    //            }else if(tag.second == "secondary" || tag.second == "secondary_link"){
    //                new_data.type = "secondary";
    //                break;
    //            }else if(tag.second == "tertiary" || tag.second == "tertiary_link"){
    //                new_data.type = "tertiary";
    //                break;
    //            }else if(tag.second == "unclassified"){
    //                new_data.type = "unclassified";
    //                break;
    //            }else if(tag.second == "residential"){
    //                new_data.type = "residential";
    //                break;
    //            }else if(tag.second == "service"){
    //                new_data.type = "service";
    //                break;
    //            }
    //        }
    //    }
}

// loading street segments structure

void map_data::load_street_segments() {

    for (unsigned id = 0; id < getNumberOfStreetSegments(); ++id) {
        StreetSegmentInfo info = getStreetSegmentInfo(id);
        if (info.speedLimit >= 80)
            load_segments_data(id, info, true);
        else
            load_segments_data(id, info, false);
    }
}

// loading feature structure

void map_data::load_features() {

    feature_info info;
    // feature_data feature;
    // load features by loading the name and type first
    for (unsigned id = 0; id < getNumberOfFeatures(); ++id) {

        info.name = getFeatureName(id);
        info.type = getFeatureType(id);
        for (unsigned i = 0; i < getFeaturePointCount(id); ++i) {
            info.position_pts.push_back(getFeaturePoint(id, i));
        }

        if (info.type == Unknown) features.unknown.push_back(info);
        else if (info.type == Park) features.park.push_back(info);
        else if (info.type == Beach) features.beach.push_back(info);
        else if (info.type == Lake) features.lake.push_back(info);
        else if (info.type == River) features.river.push_back(info);
        else if (info.type == Island) features.island.push_back(info);
        else if (info.type == Shoreline) features.shoreline.push_back(info);
        else if (info.type == Building) features.building.push_back(info);
        else if (info.type == Greenspace) features.greenspace.push_back(info);
        else if (info.type == Golfcourse) features.golfcourse.push_back(info);
        else if (info.type == Stream) features.stream.push_back(info);

        info.position_pts.clear();

        //OSMID feature_OSMID = getFeatureOSMID(id);
        //info.featureOSMID = feature_OSMID;

    }

    //    for(unsigned idx = 0; idx < getNumberOfNodes(); idx++){
    //        const OSMNode* node = getNodeByIndex(idx);
    //        //OSMID node_ID = node->id();
    //        //node_map.insert(std::make_pair(node_ID, node));
    //    }
}

// load poi names into a ternary tree

void map_data::load_poi_names() {
    for (unsigned id = 0; id < poi.size(); id++)
        poi_names_TST -> insert(poi[id].name);
}

// load street_names and intersection names data structure into a ternery search tree

void map_data::load_street_names() {

    // load street names
    for (unsigned id = 0; id < getNumberOfStreets(); id++) { 
        std::string words = getStreetName(id);
        street_names_TST -> insert(words);
    }
    
    // load intersecton names
    for (unsigned id = 0; id < intersections.size(); id++) {
        std::string words = intersections[id].name;
        street_names_TST -> insert(words);
    }
}

/*************** load data functions for m3 ********************/
void map_data::load_button_names() {
    //std::string asd ="Find";
    button_names_TST->insert("Bank");
    button_names_TST->insert("Find");
    button_names_TST->insert("Find POI");
    button_names_TST->insert("University");
    button_names_TST->insert("Hospital");
    button_names_TST->insert("Dinning");
    button_names_TST->insert("Gas");
    button_names_TST->insert("Parking");
    button_names_TST->insert("All POI");
    button_names_TST->insert("FeaName");
    button_names_TST->insert("Int");    
    button_names_TST->insert("ALL");
    button_names_TST->insert("Find Route");
    button_names_TST->insert("Navigation");


}

void map_data::load_street_names_BK() {
    // load street names
    for (unsigned id = 0; id < getNumberOfStreets(); id++) { 
        std::string words = getStreetName(id);
        street_names_BK -> insert(street_names_BK -> get_root(), words);
    }
   
    for (unsigned id = 0; id < intersections.size(); id++) {
        std::string words = intersections[id].name;
        street_names_BK -> insert(street_names_BK -> get_root(), words);
    }
}

void map_data::load_poi_names_BK() {
    for (unsigned id = 0; id < poi.size(); id++)
        poi_names_BK -> insert(poi_names_BK -> get_root(), poi[id].name);    
}

/*************** Getters for m2 ********************/
std::vector<intersection_data>& map_data::get_intersections() {
    return intersections;
}

std::vector<poi_data>& map_data::get_poi() {
    return poi;
}

std::unordered_map<std::string, std::vector<unsigned>>&map_data::get_poi_name_id() {
    return poi_name_id;
}

std::vector<street_segment_data>& map_data::get_expressways() {
    return expressways;
}

std::unordered_map<unsigned, street_segment_data>& map_data::get_segment_drawing_table() {
    return segment_drawing_table;
}

std::vector<std::vector<intersection_edge_info> > & map_data::get_connected_intersections() {
    return connected_intersections;
}

std::vector<street_segment_data>& map_data::get_other_street_segments() {
    return other_street_segments;
}

feature_data& map_data::get_features() {
    return features;
}

TST*& map_data::get_street_names_TST() {
    return street_names_TST;
}

TST*& map_data::get_poi_names_TST() {
    return poi_names_TST;
}

//std::map<OSMID, const OSMWay*>& map_data::get_way_map(){
//    return way_map;
//}
//
//std::unordered_map<OSMID, const OSMNode* >& map_data::get_node_map(){
//    return node_map;
//} 

/*************** Getters for m3 ********************/
TST*& map_data::get_help_button_names_TST() {
    return button_names_TST;
}

BK_Tree*& map_data::get_street_names_BK() {
    return street_names_BK;
}

BK_Tree*& map_data::get_poi_names_BK() {
    return poi_names_BK;
}

// helper functions to load segment length and travel time

double map_data::find_segment_length(unsigned street_segment_id) {

    double seg_len = 0;

    StreetSegmentInfo ss_info = getStreetSegmentInfo(street_segment_id);

    unsigned num_curve_pt = ss_info.curvePointCount;

    // if there's no curve point
    if (num_curve_pt == 0) {
        seg_len = find_distance(
                getIntersectionPosition(ss_info.from), getIntersectionPosition(ss_info.to));
        return seg_len;
    }        // if there are any curve points
    else {
        unsigned i = 0;
        // distance between "from" intersection to first curve point
        seg_len = find_distance(
                getIntersectionPosition(ss_info.from),
                getStreetSegmentCurvePoint(street_segment_id, i));

        // loop for summing up all the curve points distances
        for (i = 0; i < num_curve_pt - 1; i++) {
            // distance between curve point i to curve point i + 1
            seg_len = seg_len + find_distance(
                    getStreetSegmentCurvePoint(street_segment_id, i),
                    getStreetSegmentCurvePoint(street_segment_id, i + 1));
        }

        // distance between "to" intersection to last curve point
        seg_len = seg_len + find_distance(
                getIntersectionPosition(ss_info.to),
                getStreetSegmentCurvePoint(street_segment_id, i));

        return seg_len;
    }
}

double find_distance(LatLon point1, LatLon point2) {

    double lat_avg = cos((point1.lat() + point2.lat()) * 0.5 * DEG_TO_RADIAN);

    double x1 = longitude_to_cartesian(point1.lon(), lat_avg);
    double y1 = latitude_to_cartesian(point1.lat());
    double x2 = longitude_to_cartesian(point2.lon(), lat_avg);
    double y2 = latitude_to_cartesian(point2.lat());

    return EARTH_RADIUS_METERS * sqrt((y2 - y1) * (y2 - y1) + (x2 - x1) * (x2 - x1));
}

// Conversions from lat,lon to cartesian or the other way around

double longitude_to_cartesian(float longitude, double average_latitude) {
    return longitude * average_latitude * DEG_TO_RADIAN;
}

double latitude_to_cartesian(float latitude) {
    return latitude * DEG_TO_RADIAN;
}

float cartesian_to_longitude(double x, double average_latitude) {
    return x * RADIAN_TO_DEG / average_latitude;
}

float cartesian_to_latitude(double y) {
    return y * RADIAN_TO_DEG;
}
//
//t_point LatLon_XY_converter(LatLon input, double average_latitude) {
//
//    float input_lat = input.lat();
//    float input_lon = input.lon();
//
//    float input_x = (input_lon * cos(average_latitude * DEG_TO_RADIAN));
//    float input_y = input_lat;
//
//    t_point X_Y = t_point(input_x, input_y);
//
//    return X_Y;
//}
//
//void map_data::get_osm_info(){
//    number_of_nodes = getNumberOfNodes();
//
//    numebr_of_ways = getNumberOfWays();
//
//    number_of_relations = getNumberOfRelations();
//    return;
//}
