/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   draw_map.h
 * Author: wangy610
 *
 * Created on February 22, 2018, 2:33 AM
 */

#ifndef DRAW_MAP_H
#define DRAW_MAP_H
#include <iostream>
#include <cstring>
#include <sstream>
#include "map_data.h"
#include "graphics.cpp"


/***************** Function Prototypes *********************/
extern double avg_lat;
/*************** Call back function for buttons ******************/
// helper function for zoom_to_find, from graphics.cpp
void zoom_to_find_helper();

// Modified from graphics.cpp by concatenating handle_zoom_in, handle_zoom_out and update trasform
void zoom_to_find(float x, float y);

// Function for button "Find"
void find_intersections(void (draw_screen)(void));

// Function for button "Find POI"
void find_poi(void (draw_screen)(void));

// Function for help button
void help_button(void (draw_screen)(void));

// Function for Find route button
void find_route(void (draw_screen) (void));

// Function for button clear screen
void clear_screen(void (draw_screen) (void));

// Functions for buttons that are for points of interest
void(button_func_uni_off) (void (draw_screen) (void));

void(button_func_hospital_off) (void (draw_screen) (void));

void(button_func_dinning_off) (void (draw_screen) (void));

void(button_func_bank_off) (void (draw_screen) (void));

void(button_func_gas_off) (void (draw_screen) (void));

void(button_func_parking_off) (void (draw_screen) (void));

void(button_func_all_off) (void (draw_screen) (void));

void(button_func_feature_off) (void (draw_screen) (void));

// Function for the button navigation
void(button_navigation) (void (draw_screen) (void));



/*************** Other Functions Implementations *******************/
// For presentation purpose of finding path
std::vector<unsigned> traveling_courier_2(const std::vector<DeliveryInfo>& deliveries, 
                                        const std::vector<unsigned>& depots, 
                                        const float turn_penalty);

// for returning colors depending on which feature it is
t_color get_feature_color(FeatureType type);

// for returning segment width and its color depending on what speedLimit (highway, major, minor)
street_segment_drawing get_street_segment_drawing(float speedLimit);

// For calculating the rotation of the text
int get_text_direction(float& x1, float& y1, float& x2, float& y2);

// Helper for drawing a street segment name
void draw_segment_name(float& x1, float& y1, float& x2, float& y2, std::string name);

// Function for drawing all the intersections
void draw_intersections();

// Helper for drawing a poi name
void draw_poi_name(float& xc, float& yc, std::string name);

// Drawing all poi with icons
void draw_poi();

// For drawing expressways
void draw_expressways(bool first_time);

// For drawing all the street segments
void draw_street_segments();

// For drawing one features name
void draw_feature_name(std::vector<feature_info>& feature);

// For drawing one feature
void draw_feature(std::vector<feature_info>& feature, t_color color);

// For drawing all the feature names depending on their type
void draw_all_feature_name();

// For drawing all the features with colours
void draw_all_features();

// For drawing intersection box
void draw_intersection_box(float& x_left, float& x_right, float& y_bottom, float& y_top);

// Draw a box at the bottom of the screen and show the info of that intersection after the user clicks on the intersection
void draw_intersection_info(std::string name);

// For clearing what has been clicked in the same scale
void clear_intersection_click();

// For returning a box below the search bar
void find_box_below(float& y_bottom, float& y_top);

// For drawing boxes below the search bar
void draw_box_below();
void draw_help_box_below() ;
void draw_find_route_box_below();
// For searching auto complete result and drawing it
void draw_auto_complete();

// For searching auto complete result for help button and drawing it
void draw_auto_complete_help();

// For searching auto complete result for find route and drawing it
void draw_find_route_auto_complete();

// For drawing text of user input in the search bar
void draw_search_bar_text();

// For drawing search bar on the top left of the screen
void draw_search_bar();

// For drawing text of user input in the find route search bar
void draw_find_route_search_bar_text();

// For drawing search bar for finding routes
void draw_find_route_search_bar();

// For drawing help box at the bottom right of the screen
void draw_help_box();
void draw_help_box_text();
void draw_start_help_text();
void draw_all_help_box();

// For drawing text in the help box
void draw_restaurent_help_text();
void draw_find_help_text();
void draw_findPOI_help_text();
void draw_university_help_text();
void draw__hospital_help_text();
void draw_bank_help_text();
void draw_gas_help_text();
void draw_parking_help_text();
void draw_all_poi_help_text();
void draw_feaName_help_text();
void draw_int_help_text();
void draw_findRout_help_text();
void draw_NaviOn_help_text();

// For drawing text when user inputs invalid text
void draw_invalid_input();

// For drawing text when the map can't find the intersection the user is searching for
void draw_intersection_not_found();

// For drawing text when the map can't find the poi the user is searching for
void draw_poi_not_found();

// For printing invalid input for finding route
void draw_start_point_not_found();   // Sorry, start point not found
void draw_invalid_first_input(); // please enter an intersection as a start point
void draw_invalid_second_input(); // please enter an intersection/POI as an end point
void draw_end_point_not_found(); //Sorry, end point not found

void helper_draw_multiple_found_intersection(std::vector<unsigned>& intersection_ids);

// helpers for drawing found intersections
void helper_draw_one_found_intersection(std::vector<unsigned>& intersection_ids);

// For drawing the intersection that the user searches for
void draw_found_intersections(std::vector<unsigned>& intersection_ids);

// For highlighting the POIs that were found
void highlight_found_poi(std::vector<unsigned>& ids);

// helpers to draw_found poi depending on the size of found poi
void helper_draw_one_found_poi(std::vector<unsigned>& ids, float& x_left, float& x_right, float& y_bottom, float& y_top, std::string name);
void helper_draw_multiple_found_poi(std::vector<unsigned>& ids, float& x_left, float& x_right, float& y_bottom, float& y_top, std::string name);

// For drawing the found poi that the user searches for
void draw_found_poi(std::string name);

// For filtering minor errors of the input from user
void filter_minor_input_errors(std::string& street_name);

// For parsing user input for poi name
void parse_poi_input_name(std::string& name);

// For getting two street names from user input
void parse_input(std::size_t i, std::string& street_name1, std::string& street_name2);

void parse_input_route_first(std::size_t i, std::string& first_street_name1, std::string& first_street_name2);

void parse_input_route_second(std::size_t i, std::string& second_street_name1, std::string& second_street_name2);

// For finding the intersection that the user wants to find,
// If not found, gives invalid input notice for user
void intersection_find();

// For finding the poi that the user wants to find,
// If not found, gives invalid input notice for user
void poi_find();

// check input for finding route
bool check_route_first_input(std::vector<unsigned>& first_intersection_ids);

bool check_route_second_input(std::string& temp_str, std::vector<unsigned>& second_intersection_ids);

// Function to find route
void route_find();

// When user clicks on an intersection
void act_on_click_intersection(float x, float y);

// When user clicks on the auto complete location
void act_on_click_auto_complete(float x, float y);

void act_on_click_auto_complete_help(float x, float y) ;

void act_on_click_auto_complete_find_route(float x, float y);

void act_on_click_flip_page(float x, float y);
// When user presses Find and enter from keyboard
void key_press_for_intersection(char key_pressed, int keysym);

// When user presses Find POI and enter from keyboard
void key_press_for_poi(char key_pressed, int keysym);

// For different input for finding route
void key_press_for_entering_first(char key_pressed, int keysym);

void key_press_for_entering_second(char key_pressed, int keysym);

// When user presses Find Route and enter from keyboard
void key_press_for_find_route(char key_pressed, int keysym);

// When user presses help and enter from keyboard
void key_press_help_command(char key_pressed, int keysym);

// Function to create new buttons
void create_new_buttons();

// Function to get zoom level of the current world
void get_zoom_level();

// Function to get world coordinate in latlon
void get_world_latlon(double& min_lat, double& max_lat, double& min_lon, double& max_lon);

/*************** Call back function for draw_map ********************/
// After user presses any key
void act_on_key_press(char key_pressed, int keysym);

// Whenever user clicks on the map, come to this function
void act_on_button_press(float x, float y, t_event_buttonPressed event);

void draw_screen();

void draw_path(std::vector<unsigned>& temp);

void draw_TSP_path(std::vector<unsigned>& temp);

void draw_arrow(pair <double, double> start, pair <double, double> second_last, pair <double, double> end);

//void draw_path(std::vector<unsigned> input_vector);

void print_instruction(std::vector<unsigned> input_vector);

string set_LR(street_segment_data seg1, street_segment_data seg2);

string set_direction(double angle1, double angle2) ;

void calculate_angle(unsigned seg_id, unsigned start_pt, double& angle);

void draw_navigation_box();

void design_navigation_box();

void draw_navigation_box_text();

void draw_navigation_direction(std::string direction, double x, double y, bool isEnd);
void draw_instruction_window();
void find_start_end(unsigned id1, unsigned id2, std::vector<unsigned>& start_end);
////=============== Transportation OSM ==================//
//void initialize_OSMNode_Map();
//
//void initialize_OSMWay_Map();
//
//void initialize_OSMRelation_Map();
//
//void initialize_station_info();
//
//void initialize_transport_lines();
//
//void initialize_station_info_helper(std::string transport, 
//        const OSMNode* this_node, 
//        std::unordered_map<std::string, std::string> check_tags);
//
//void initialize_transport_lines_helper(std::string transport, const OSMWay* this_way);
//
//void draw_bus_staions();
//
//void draw_ferry_staions();
//
//void draw_subway_staions();
//
//std::string get_bus_station_name(t_point station_position) ;
//
//std::string get_subway_station_name(t_point station_position) ;
#endif /* DRAW_MAP_H */

