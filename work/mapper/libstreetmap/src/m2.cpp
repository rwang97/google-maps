/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "m1.h"
#include "m2.h"
#include "m3.h"
#include "m4.h"
#include "Greedy.h"
#include "SA.h"
#include "StreetsDatabaseAPI.h"
#include "draw_map.h"
#include "map_data.h"
#include "UCS.h"
#include <cmath>
#include <chrono>

using namespace std::chrono;
/******************** Global Variables **********************/
// vector for the path that we found
std::vector<unsigned> found_path;
int page_num = 0;
int curr_page = 0;
std::vector<std::vector<navi_info>>route;
t_bound_box next_page_icon;
t_bound_box prev_page_icon;
t_bound_box navi_box;

// vectors for navigation boxes
std::vector<t_bound_box> navigate_display1;
std::vector<t_bound_box> navigate_display2;

bool second_click = false;
std::string LR;
// Global variable map which stores all the data structures
extern map_data* my_map;

// for converting between cartesian and LatLon
double avg_lat;

// For drawing intersection and poi circle
float radius;

// For user's input
std::string entered_text = "";
std::string entered_help_command = "";
std::string entered_start = "";
std::string entered_end = "";
std::string instruction_text = "Stay hungry, stay foolish";

// For checking whether user is entering first line or second line
bool has_entered_first = false;
bool has_entered_second = false;


// For auto complete names
std::vector<std::string> auto_names;
// For saving state
LatLon intersection_first_clicked_on;
LatLon intersection_second_clicked_on;
unsigned first_intersection;
bool has_clicked_first_Intersection = false;
bool has_clicked_second_Intersection = false;
bool has_clicked_find = false;
bool has_clicked_find_POI = false;
bool has_clicked_find_route = false;
bool need_to_clear_searched_intersections = false;
bool need_to_clear_searched_poi = false;
bool help_button_has_clicked = false;
bool isPOI = false;
bool wrong_input_route = false;
bool has_drawn_path = false;

unsigned route_first_intersection;
unsigned route_second_intersection;

bool display_find_route_result = true;
// For level of details
zoom_info world;
double zoom_level;
unsigned zoom = 0;

// For buttons created
buttons all_buttons;
Loaded_surface surface_collection;

// For whether to draw intersections
bool has_clicked_navigation = false;
bool intersection_draw = false;

// For whether to draw feature name
bool feature_name_draw = false;

/*************** Call back function for buttons ******************/

// Function for button "Find"

void find_intersections(void (draw_screen) (void)) {
    if (help_button_has_clicked || has_clicked_find_route)
        draw_screen();
    found_path.clear();
    entered_text = "Eg. College Street & Bay Street";
    has_clicked_find = true;
    has_clicked_find_POI = false;
    help_button_has_clicked = false;
    has_clicked_find_route = false;
    draw_search_bar();
}

// Function for button "Find POI"

void find_poi(void (draw_screen) (void)) {
    if (help_button_has_clicked || has_clicked_find_route)
        draw_screen();
    found_path.clear();
    entered_text = "Eg. Eaton Centre";
    has_clicked_find_POI = true;
    has_clicked_find = false;
    help_button_has_clicked = false;
    has_clicked_find_route = false;
    has_clicked_navigation = false;
    draw_search_bar();
}

// Function for button "Find Route"
void find_route(void (draw_screen) (void)) {
    if (has_clicked_find_POI || has_clicked_find || help_button_has_clicked)
        draw_screen();
    found_path.clear();
    entered_start = "Start Point";
    entered_end = "End Point";
    has_entered_first = false;
    has_entered_second = false;
    has_clicked_find_POI = false;
    has_clicked_find = false;
    help_button_has_clicked = false;
    has_clicked_navigation = false;
    has_clicked_find_route = true;

    draw_find_route_search_bar();
}

// Function for help button
void help_button(void (draw_screen) (void)) {
    if (has_clicked_find_POI || has_clicked_find || has_clicked_find_route)
        draw_screen();
    found_path.clear();
    has_clicked_find = false;
    has_clicked_find_POI = false;
    has_clicked_find_route = false;
    has_clicked_navigation = false;
    help_button_has_clicked = true;

    //draw the text box            
    entered_help_command = "Eg. Find";

    draw_help_box();
    draw_help_box_text();

    draw_start_help_text();
}


void(button_navigation) (void (draw_screen) (void)) {

    if (has_drawn_path) {
        if (has_clicked_find || has_clicked_find_POI || help_button_has_clicked)
            draw_screen();

        has_clicked_find = false;
        has_clicked_find_POI = false;
        has_clicked_find_route = false;
        help_button_has_clicked = false;
        has_clicked_navigation = true;

        design_navigation_box();
        draw_navigation_box();

    }
}

// Function for clear screen button to clear the screen
void clear_screen(void (draw_screen) (void)) {

    found_path.clear();
    draw_screen();
}

// Functions for buttons that are used for displaying points of interest

void(button_func_uni_off) (void (draw_screen) (void)) {
    found_path.clear();
    if (all_buttons.uni_on) {
        //turn it off
        all_buttons.uni_on = false;
        change_button_text("University Off", "University On");
        //change draw to not draw
        for (unsigned i = 0; i < my_map -> get_poi().size(); ++i) {
            if (my_map -> get_poi()[i].type == "university")
                my_map -> get_poi()[i].draw = false;
        }
    } else {
        //turn if on
        all_buttons.uni_on = true;
        change_button_text("University On", "University Off");
        for (unsigned i = 0; i < my_map -> get_poi().size(); ++i) {
            if (my_map -> get_poi()[i].type == "university")
                my_map -> get_poi()[i].draw = true;
        }
    }
    draw_screen();
}

void(button_func_hospital_off) (void (draw_screen) (void)) {
    found_path.clear();
    if (all_buttons.hospital_on) {
        //turn it off
        all_buttons.hospital_on = false;
        change_button_text("Hospital Off", "Hospital On");
        //change draw to not draw
        for (unsigned i = 0; i < my_map -> get_poi().size(); ++i) {
            if (my_map -> get_poi()[i].type == "hospital")
                my_map -> get_poi()[i].draw = false;
        }
    } else {
        //turn if on
        all_buttons.hospital_on = true;
        change_button_text("Hospital On", "Hospital Off");
        for (unsigned i = 0; i < my_map -> get_poi().size(); ++i) {
            if (my_map -> get_poi()[i].type == "hospital")
                my_map -> get_poi()[i].draw = true;
        }
    }
    draw_screen();
}

void(button_func_dinning_off) (void (draw_screen) (void)) {
    found_path.clear();
    if (all_buttons.help_on) {
        if (!second_click) {
            draw_help_box();
            draw_restaurent_help_text();
            second_click = !second_click;
        } else {
            draw_help_box();
            second_click = !second_click;
        }
    } else {
        if (all_buttons.dinning_on) {
            //turn it off
            all_buttons.dinning_on = false;
            change_button_text("Dinning Off", "Dinning On");
            //change draw to not draw
            for (unsigned i = 0; i < my_map -> get_poi().size(); ++i) {
                if (my_map -> get_poi()[i].type == "restaurant")
                    my_map -> get_poi()[i].draw = false;
            }
        } else {
            //turn if on
            all_buttons.dinning_on = true;
            change_button_text("Dinning On", "Dinning Off");
            for (unsigned i = 0; i < my_map -> get_poi().size(); ++i) {
                if (my_map -> get_poi()[i].type == "restaurant")
                    my_map -> get_poi()[i].draw = true;
            }
        }
        draw_screen();
    }
}

void(button_func_bank_off) (void (draw_screen) (void)) {
    found_path.clear();
    if (all_buttons.bank_on) {
        //turn it off
        all_buttons.bank_on = false;
        change_button_text("Bank Off", "Bank On");
        //change draw to not draw
        for (unsigned i = 0; i < my_map -> get_poi().size(); ++i) {
            if (my_map -> get_poi()[i].type == "bank")
                my_map -> get_poi()[i].draw = false;
        }
    } else {
        //turn if on
        all_buttons.bank_on = true;
        change_button_text("Bank On", "Bank Off");
        for (unsigned i = 0; i < my_map -> get_poi().size(); ++i) {
            if (my_map -> get_poi()[i].type == "bank")
                my_map -> get_poi()[i].draw = true;
        }
    }
    draw_screen();
}

void(button_func_gas_off) (void (draw_screen) (void)) {
    found_path.clear();
    if (all_buttons.petro_on) {
        //turn it off
        all_buttons.petro_on = false;
        change_button_text("Gas Off", "Gas On");
        //change draw to not draw
        for (unsigned i = 0; i < my_map -> get_poi().size(); ++i) {
            if (my_map -> get_poi()[i].type == "fuel")
                my_map -> get_poi()[i].draw = false;
        }
    } else {
        //turn if on
        all_buttons.petro_on = true;
        change_button_text("Gas On", "Gas Off");
        for (unsigned i = 0; i < my_map -> get_poi().size(); ++i) {
            if (my_map -> get_poi()[i].type == "fuel")
                my_map -> get_poi()[i].draw = true;
        }
    }
    draw_screen();
}

void(button_func_parking_off) (void (draw_screen) (void)) {
    found_path.clear();
    if (all_buttons.park_on) {
        //turn it off
        all_buttons.park_on = false;
        change_button_text("Parking Off", "Parking On");
        //change draw to not draw
        for (unsigned i = 0; i < my_map -> get_poi().size(); ++i) {
            if (my_map -> get_poi()[i].type == "parking")
                my_map -> get_poi()[i].draw = false;
        }
    } else {
        //turn if on
        all_buttons.park_on = true;
        change_button_text("Parking On", "Parking Off");
        for (unsigned i = 0; i < my_map -> get_poi().size(); ++i) {
            if (my_map -> get_poi()[i].type == "parking")
                my_map -> get_poi()[i].draw = true;
        }
    }
    draw_screen();
}

void(button_func_all_off) (void (draw_screen) (void)) {
    found_path.clear();
    if (all_buttons.all_on) {
        //turn it off
        all_buttons.all_on = false;
        change_button_text("All POI Off", "All POI On");
        //change draw to not draw
        for (unsigned i = 0; i < my_map -> get_poi().size(); ++i) {
            my_map -> get_poi()[i].draw = false;
        }
    } else {
        //turn if on
        all_buttons.all_on = true;
        change_button_text("All POI On", "All POI Off");
        for (unsigned i = 0; i < my_map -> get_poi().size(); ++i) {
            my_map -> get_poi()[i].draw = true;
        }
    }
    draw_screen();
}

void(button_func_feature_off) (void (draw_screen) (void)) {
    found_path.clear();
    if (all_buttons.feature_on) {
        //turn it off
        all_buttons.feature_on = false;
        change_button_text("FeaName Off", "FeaName On");
        //feature_data feature = my_map -> get_features(); 
        feature_name_draw = false;
    } else {
        //turn if on
        //feature_data feature = my_map -> get_features();
        all_buttons.feature_on = true;
        change_button_text("FeaName On", "FeaName Off");
        feature_name_draw = true;
    }
    draw_screen();
}

/*************** Other Functions Implementations *******************/

// For presentation purpose of finding path
std::vector<unsigned> traveling_courier_2(const std::vector<DeliveryInfo>& deliveries, 
                                        const std::vector<unsigned>& depots, 
                                        const float turn_penalty) {
    std::vector<unsigned> path;
    std::vector<unsigned> intersection_path;
    std::vector<delivery_info> pick_up, drop_off, passed_intersection;
    std::vector<save_state> pick_state, drop_state;
    pick_state.resize(getNumberOfIntersections());
    drop_state.resize(getNumberOfIntersections());
    std::vector<std::vector<unsigned>> intersection_delivery;
    intersection_delivery.resize(getNumberOfIntersections());
    std::vector<bool> has_picked(getNumberOfIntersections(), 0);
    
    for (unsigned i = 0; i < deliveries.size(); i++) {
        intersection_delivery[deliveries[i].pickUp].push_back(i);
        intersection_delivery[deliveries[i].dropOff].push_back(i);
        if (pick_state[deliveries[i].pickUp].explored) 
            pick_up[pick_state[deliveries[i].pickUp].access_id].relation.push_back(deliveries[i].dropOff);   
        else {
            pick_state[deliveries[i].pickUp].access_id = pick_up.size();
            pick_state[deliveries[i].pickUp].explored = 1;

            delivery_info temp;
            temp.id = deliveries[i].pickUp;
            pick_up.push_back(temp);
        }
        if (drop_state[deliveries[i].dropOff].explored) 
            drop_off[drop_state[deliveries[i].dropOff].access_id].relation.push_back(deliveries[i].pickUp);
        else {
            drop_state[deliveries[i].dropOff].explored= 1;
            drop_state[deliveries[i].dropOff].access_id = drop_off.size();
            
            delivery_info temp;
            temp.id = deliveries[i].dropOff;
            temp.relation.push_back(deliveries[i].pickUp);
            drop_off.push_back(temp);
        }
    }
    
    std::vector<std::vector<unsigned>> all_path;
    std::vector<std::vector<unsigned>> all_intersection_path;
    #pragma omp parallel for
    //unsigned start_depot;
    for (unsigned i = 0; i < depots.size(); i++) {
        std::vector<unsigned> temp_path;
        std::vector<unsigned> intersection_temp_path;
        Greedy(turn_penalty, temp_path, pick_up, pick_state, drop_off, 
               drop_state, passed_intersection, intersection_temp_path, has_picked, depots[i], depots);
        if (!temp_path.empty()) {
            #pragma omp critical 
            {
                //std::cout << "i: " << i << std::endl;
                all_path.push_back(temp_path);
                all_intersection_path.push_back(intersection_temp_path);
            }
        } 
    }
    
    
    double time = compute_path_travel_time(all_path[0], turn_penalty);
    //std::cout << "time " << time << std::endl; 
    path = all_path[0];
    intersection_path = all_intersection_path[0];
//    unsigned index = 0;
//    for (unsigned i = 1; i < all_path.size(); i++) {
//        double new_time = compute_path_travel_time(all_path[i], turn_penalty);
//        if (new_time < time) {
//            time = new_time;
//            index = i;
//        }    
//    }
//    path = all_path[index];
//    intersection_path = all_intersection_path[index];
    
    route_first_intersection = intersection_path[0];
    route_second_intersection = intersection_path[intersection_path.size() - 1];
    //two_opt(path, deliveries,intersection_delivery, intersection_path, turn_penalty, start_time);
    return path;
        
}

// Function to draw path for finding route
void draw_path(std::vector<unsigned>& temp) {

    if (temp.empty()) return;

    std::vector<street_segment_data> seg_data;

    for (unsigned i = 0; i < temp.size(); i++) {
        StreetSegmentInfo info = getStreetSegmentInfo(temp[i]);
        street_segment_data new_data;

        // store the starting point position
        new_data.position_pts.push_back(getIntersectionPosition(info.from));

        // store the points in middle
        if (info.curvePointCount != 0) {
            for (unsigned j = 0; j < info.curvePointCount - 1; j++) {
                new_data.position_pts.push_back(getStreetSegmentCurvePoint(temp[i], j));
            }
        }
        // store the ending point 
        new_data.position_pts.push_back(getIntersectionPosition(info.to));
        seg_data.push_back(new_data);
    }

    float x1 = 0, y1 = 0, x2 = 0, y2 = 0;

    double interval = (255 - 60) / (seg_data.size() + 1);
    if (interval < 1) interval = 1;
    else interval = floor(interval);
    int color_interval = interval;
    int color_b = 60;
    int color_r = 255;

    for (unsigned i = 0; i < seg_data.size(); ++i) {

        if (color_b + color_interval <= 255)
            color_b = color_b + color_interval;
        if (color_r - color_interval >= 0)
            color_r = color_r - color_interval;
        setcolor(t_color(color_r, 0, color_b));

        for (unsigned j = 0; j < seg_data[i].position_pts.size() - 1; ++j) {

            x1 = longitude_to_cartesian(seg_data[i].position_pts[j].lon(), avg_lat);
            y1 = latitude_to_cartesian(seg_data[i].position_pts[j].lat());
            x2 = longitude_to_cartesian(seg_data[i].position_pts[j + 1].lon(), avg_lat);
            y2 = latitude_to_cartesian(seg_data[i].position_pts[j + 1].lat());
            setlinewidth(6);
            drawline(x1, y1, x2, y2);
        }
    }

    if (!seg_data.empty()) {
        get_zoom_level();
        radius = 0.00001;
        if (zoom == 5) radius = 0.00001;
        else if (zoom == 6) radius = 0.0000065;
        else if (zoom == 4) radius = 0.000017;
        else if (zoom == 3) radius = 0.000025;
        else if (zoom == 7) radius = 0.000004;
        else if (zoom == 2) radius = 0.000038;
        else if (zoom == 1) radius = 0.000055;
        else if (zoom == 8) radius = 0.000003;
        else if (zoom == 0) radius = 0.0001;
        else if (zoom > 8) radius = 0.0000015;

        x1 = longitude_to_cartesian(getIntersectionPosition(route_first_intersection).lon(), avg_lat);
        y1 = latitude_to_cartesian(getIntersectionPosition(route_first_intersection).lat());

        x2 = longitude_to_cartesian(getIntersectionPosition(route_second_intersection).lon(), avg_lat);
        y2 = latitude_to_cartesian(getIntersectionPosition(route_second_intersection).lat());

        if (!(has_clicked_first_Intersection && has_clicked_second_Intersection)) {
            Surface pic = load_png_from_file("libstreetmap/resources/start_3.png");
            if (zoom == 0) draw_surface(pic, x1 - 1.25 * radius, y1 + 2.2 * radius);
            else if (zoom == 1) draw_surface(pic, x1 - 1.25 * radius, y1 + 2.2 * radius);
            else if (zoom == 2) draw_surface(pic, x1 - 1.15 * radius, y1 + 2 * radius);
            else if (zoom == 3) draw_surface(pic, x1 - 1.15 * radius, y1 + 2 * radius);
            else if (zoom == 4) draw_surface(pic, x1 - 1.15 * radius, y1 + 1.90 * radius);
            else if (zoom == 5) draw_surface(pic, x1 - 1.15 * radius, y1 + 1.90 * radius);
            else if (zoom == 6) draw_surface(pic, x1 - 1.15 * radius, y1 + 1.85 * radius);
            else if (zoom == 7) draw_surface(pic, x1 - 1.15 * radius, y1 + 1.85 * radius);
            else if (zoom == 8) draw_surface(pic, x1 - 0.7 * radius, y1 + 1.3 * radius);
            else if (zoom > 8) draw_surface(pic, x1 - 0.8 * radius, y1 + 2 * radius);
        }

        Surface pic = load_png_from_file("libstreetmap/resources/pin_icon.png");
        if (zoom == 0) draw_surface(pic, x2 - 1.15 * radius, y2 + 2.2 * radius);
        else if (zoom == 1) draw_surface(pic, x2 - 0.9 * radius, y2 + 2.2 * radius);
        else if (zoom == 2) draw_surface(pic, x2 - 1.1 * radius, y2 + 2 * radius);
        else if (zoom == 3) draw_surface(pic, x2 - radius, y2 + 2 * radius);
        else if (zoom == 4) draw_surface(pic, x2 - 0.95 * radius, y2 + 1.95 * radius);
        else if (zoom == 5) draw_surface(pic, x2 - radius, y2 + 1.9 * radius);
        else if (zoom == 6) draw_surface(pic, x2 - 0.95 * radius, y2 + 1.85 * radius);
        else if (zoom == 7) draw_surface(pic, x2 - 0.95 * radius, y2 + 1.9 * radius);
        else if (zoom == 8) draw_surface(pic, x2 - 0.65 * radius, y2 + 1.5 * radius);
        else if (zoom > 8) draw_surface(pic, x2 - 0.8 * radius, y2 + 2 * radius);

        has_drawn_path = true;
    }

}

void draw_TSP_path(std::vector<unsigned>& temp) {

    if (temp.empty()) return;

    std::vector<street_segment_data> seg_data;

    for (unsigned i = 0; i < temp.size(); i++) {
        StreetSegmentInfo info = getStreetSegmentInfo(temp[i]);
        street_segment_data new_data;

        // store the starting point position
        new_data.position_pts.push_back(getIntersectionPosition(info.from));

        // store the points in middle
        if (info.curvePointCount != 0) {
            for (unsigned j = 0; j < info.curvePointCount - 1; j++) {
                new_data.position_pts.push_back(getStreetSegmentCurvePoint(temp[i], j));
            }
        }
        // store the ending point 
        new_data.position_pts.push_back(getIntersectionPosition(info.to));
        seg_data.push_back(new_data);
    }

    float x1 = 0, y1 = 0, x2 = 0, y2 = 0;

    double interval = (255 - 60) / (seg_data.size() + 1);
    if (interval < 1) interval = 1;
    else interval = floor(interval);
    int color_interval = interval;
    int color_b = 60;
    int color_r = 255;
    unsigned num = 0;

    for (unsigned i = 0; i < seg_data.size(); ++i) {
        if (num % 10 == 0) {
            if (color_b + color_interval <= 255)
                color_b = color_b + color_interval;
            if (color_r - color_interval >= 0)
                color_r = color_r - color_interval;
        }
        setcolor(t_color(color_r, 0, color_b));

        for (unsigned j = 0; j < seg_data[i].position_pts.size() - 1; ++j) {

            x1 = longitude_to_cartesian(seg_data[i].position_pts[j].lon(), avg_lat);
            y1 = latitude_to_cartesian(seg_data[i].position_pts[j].lat());
            x2 = longitude_to_cartesian(seg_data[i].position_pts[j + 1].lon(), avg_lat);
            y2 = latitude_to_cartesian(seg_data[i].position_pts[j + 1].lat());
            setlinewidth(6);
            drawline(x1, y1, x2, y2);
        }
        num++;
    }

    if (!seg_data.empty()) {
        get_zoom_level();
        radius = 0.00001;
        if (zoom == 5) radius = 0.00001;
        else if (zoom == 6) radius = 0.0000065;
        else if (zoom == 4) radius = 0.000017;
        else if (zoom == 3) radius = 0.000025;
        else if (zoom == 7) radius = 0.000004;
        else if (zoom == 2) radius = 0.000038;
        else if (zoom == 1) radius = 0.000055;
        else if (zoom == 8) radius = 0.000003;
        else if (zoom == 0) radius = 0.0001;
        else if (zoom > 8) radius = 0.0000015;

        x1 = longitude_to_cartesian(getIntersectionPosition(route_first_intersection).lon(), avg_lat);
        y1 = latitude_to_cartesian(getIntersectionPosition(route_first_intersection).lat());

        x2 = longitude_to_cartesian(getIntersectionPosition(route_second_intersection).lon(), avg_lat);
        y2 = latitude_to_cartesian(getIntersectionPosition(route_second_intersection).lat());

       
        Surface pic = load_png_from_file("libstreetmap/resources/start_3.png");
        if (zoom == 0) draw_surface(pic, x1 - 1.25 * radius, y1 + 2.2 * radius);
        else if (zoom == 1) draw_surface(pic, x1 - 1.25 * radius, y1 + 2.2 * radius);
        else if (zoom == 2) draw_surface(pic, x1 - 1.15 * radius, y1 + 2 * radius);
        else if (zoom == 3) draw_surface(pic, x1 - 1.15 * radius, y1 + 2 * radius);
        else if (zoom == 4) draw_surface(pic, x1 - 1.15 * radius, y1 + 1.90 * radius);
        else if (zoom == 5) draw_surface(pic, x1 - 1.15 * radius, y1 + 1.90 * radius);
        else if (zoom == 6) draw_surface(pic, x1 - 1.15 * radius, y1 + 1.85 * radius);
        else if (zoom == 7) draw_surface(pic, x1 - 1.15 * radius, y1 + 1.85 * radius);
        else if (zoom == 8) draw_surface(pic, x1 - 0.7 * radius, y1 + 1.3 * radius);
        else if (zoom > 8) draw_surface(pic, x1 - 0.8 * radius, y1 + 2 * radius);
        

        pic = load_png_from_file("libstreetmap/resources/pin_icon.png");
        if (zoom == 0) draw_surface(pic, x2 - 1.15 * radius, y2 + 2.2 * radius);
        else if (zoom == 1) draw_surface(pic, x2 - 0.9 * radius, y2 + 2.2 * radius);
        else if (zoom == 2) draw_surface(pic, x2 - 1.1 * radius, y2 + 2 * radius);
        else if (zoom == 3) draw_surface(pic, x2 - radius, y2 + 2 * radius);
        else if (zoom == 4) draw_surface(pic, x2 - 0.95 * radius, y2 + 1.95 * radius);
        else if (zoom == 5) draw_surface(pic, x2 - radius, y2 + 1.9 * radius);
        else if (zoom == 6) draw_surface(pic, x2 - 0.95 * radius, y2 + 1.85 * radius);
        else if (zoom == 7) draw_surface(pic, x2 - 0.95 * radius, y2 + 1.9 * radius);
        else if (zoom == 8) draw_surface(pic, x2 - 0.65 * radius, y2 + 1.5 * radius);
        else if (zoom > 8) draw_surface(pic, x2 - 0.8 * radius, y2 + 2 * radius);

        has_drawn_path = true;
    }

}

// helper function for zoom_to_find, from graphics.cpp

void zoom_to_find_helper() {

    float mult, diff, y1, y2, x1, x2;
    /* X Window coordinates go from (0,0) to (width-1,height-1) */
    diff = trans_coord.xright - trans_coord.xleft;
    trans_coord.wtos_xmult = (trans_coord.top_width - 1 - 1) / diff;
    diff = trans_coord.ybot - trans_coord.ytop;
    trans_coord.wtos_ymult = (trans_coord.top_height - 1 - 1) / diff;

    /* Need to use same scaling factor to preserve aspect ratio */
    if (fabs(trans_coord.wtos_xmult) <= fabs(trans_coord.wtos_ymult)) {
        mult = fabs(trans_coord.wtos_ymult / trans_coord.wtos_xmult);
        y1 = trans_coord.ytop - (trans_coord.ybot - trans_coord.ytop)*(mult - 1) / 2;
        y2 = trans_coord.ybot + (trans_coord.ybot - trans_coord.ytop)*(mult - 1) / 2;
        trans_coord.ytop = y1;
        trans_coord.ybot = y2;
    } else {
        mult = fabs(trans_coord.wtos_xmult / trans_coord.wtos_ymult);
        x1 = trans_coord.xleft - (trans_coord.xright - trans_coord.xleft)*(mult - 1) / 2;
        x2 = trans_coord.xright + (trans_coord.xright - trans_coord.xleft)*(mult - 1) / 2;
        trans_coord.xleft = x1;
        trans_coord.xright = x2;
    }
    diff = trans_coord.xright - trans_coord.xleft;
    trans_coord.wtos_xmult = (trans_coord.top_width - 1 - 1) / diff;
    diff = trans_coord.ybot - trans_coord.ytop;
    trans_coord.wtos_ymult = (trans_coord.top_height - 1 - 1) / diff;

    trans_coord.stow_xmult = 1 / trans_coord.wtos_xmult;
    trans_coord.stow_ymult = 1 / trans_coord.wtos_ymult;
    get_zoom_level();
}

// Modified from graphics.cpp by concatenating handle_zoom_in, handle_zoom_out and update trasform

void zoom_to_find(float x, float y) {

    get_zoom_level();
    // zoom in to level 7
    while (zoom <= 6) {
        trans_coord.xleft = x - (x - trans_coord.xleft) / 1.1;
        trans_coord.xright = x + (trans_coord.xright - x) / 1.1;

        trans_coord.ytop = y - (y - trans_coord.ytop) / 1.1;
        trans_coord.ybot = y + (trans_coord.ybot - y) / 1.1;
        zoom_to_find_helper();
    }
    while (zoom == 8) {
        //restore the original distances before previous zoom in
        trans_coord.xleft = x - (x - trans_coord.xleft) * 1.1;
        trans_coord.xright = x + (trans_coord.xright - x) * 1.1;
        trans_coord.ytop = y - (y - trans_coord.ytop) * 1.1;
        trans_coord.ybot = y + (trans_coord.ybot - y) * 1.1;
        zoom_to_find_helper();
    }
    if (zoom == 7) zoom_to_find_helper();
    draw_screen();

}

// for returning colors depending on which feature it is

t_color get_feature_color(FeatureType type) {
    t_color color;
    if (type == Greenspace || type == Golfcourse)
        return t_color(202, 241, 188);
    else if (type == Park)
        return t_color(179, 236, 156);
    else if (type == Lake || type == River || type == Stream || type == Shoreline)
        return t_color(156, 209, 254);
    else if (type == Building)
        return t_color(211, 211, 211);
    else if (type == Beach || type == Island)
        return KHAKI;
    else
        return BLACK;
}

// for returning segment width and its color depending on what speedLimit (highway, major, minor)

street_segment_drawing get_street_segment_drawing(float speedLimit) {

    street_segment_drawing segment;
    if (speedLimit >= 100) {
        segment.color = t_color(255, 233, 139);
        segment.linewidth = 12;
    } else if (speedLimit >= 80) {
        segment.color = t_color(255, 233, 139);
        segment.linewidth = 10;
    } else if (speedLimit >= 60) {
        segment.color = WHITE;
        segment.linewidth = 8;
    } else if (speedLimit >= 50) {
        segment.color = WHITE;
        segment.linewidth = 6;
    } else if (speedLimit >= 40) {
        segment.color = WHITE;
        segment.linewidth = 4;
    } else {
        segment.color = WHITE;
        segment.linewidth = 2;
    }
    return segment;
}

// For calculating the rotation of the text

int get_text_direction(float& x1, float& y1, float& x2, float& y2) {

    // Need to check the position of x1,y1 and x2,y2
    if (x1 >= x2 && y1 >= y2) return atan((y1 - y2) / (x1 - x2)) * RADIAN_TO_DEG;
    else if (x1 < x2 && y1 >= y2) return -atan((y2 - y1) / (x1 - x2)) * RADIAN_TO_DEG;
    else if (x1 < x2 && y1 < y2) return -atan((y2 - y1) / (x1 - x2)) * RADIAN_TO_DEG;
    else return atan((y2 - y1) / (x2 - x1)) * RADIAN_TO_DEG;
}

// Helper for drawing a street segment name

void draw_segment_name(float& x1, float& y1, float& x2, float& y2, std::string name) {

    // for determining the position of x1,y1 and x2,y2
    float left, right, bottom, top;
    if (x1 <= x2) {
        left = x1;
        right = x2;
    } else {
        left = x2;
        right = x1;
    }
    if (y1 <= y2) {
        bottom = y1;
        top = y2;
    } else {
        bottom = y2;
        top = y1;
    }

    // Give a boundary for the text
    t_bound_box text_bound(left, bottom, right, top);
    settextrotation(get_text_direction(x1, y1, x2, y2));

    drawtext_in(text_bound, name);

    // Reset
    settextrotation(0);
    setcolor(WHITE);
}

// Function for drawing all the intersections

void draw_intersections() {

    if (intersection_draw && zoom >= 6) {
        setcolor(PINK);
        radius = 0.000003;
        std::vector<intersection_data> intersection = my_map -> get_intersections();
        for (unsigned i = 0; i < intersection.size(); ++i) {
            float x = longitude_to_cartesian(intersection[i].position.lon(), avg_lat);
            float y = latitude_to_cartesian(intersection[i].position.lat());
            fillarc(x, y, radius, 0, 360);
        }
    }
}

// Helper for drawing a poi name

void draw_poi_name(float& xc, float& yc, std::string name) {

    // drawing size depends on zoom level
    if (zoom == 5) {
        setfontsize(10);
        drawtext(xc, yc, name, 1000, 1000);
    } else if (zoom >= 6) {
        setfontsize(11);
        drawtext(xc, yc, name, 1000, 1000);
    }
}

// Drawing all poi with icons

void draw_poi() {

    std::vector<poi_data> poi = my_map -> get_poi();
    for (unsigned i = 0; i < poi.size(); ++i) {
        float x = longitude_to_cartesian(poi[i].position.lon(), avg_lat);
        float y = latitude_to_cartesian(poi[i].position.lat());

        // draw each poi depending on their type
        if (poi[i].draw) {
            if (poi[i].type == "university") {
                setcolor(74, 96, 107);
                if (zoom >= 5) {
                    draw_surface(surface_collection.uni, x, y);
                    draw_poi_name(x, y, poi[i].name);
                }
            } else if (poi[i].type == "townhall") {
                setcolor(74, 96, 107);
                if (zoom >= 5) {
                    draw_surface(surface_collection.townhall, x, y);
                    draw_poi_name(x, y, poi[i].name);
                }
            } else if (poi[i].type == "hospital") {
                setcolor(171, 78, 80);
                if (zoom >= 5) {
                    draw_surface(surface_collection.hospital, x, y);
                    draw_poi_name(x, y, poi[i].name);
                }
            } else if (poi[i].type == "embassy") {
                setcolor(74, 96, 107);
                if (zoom >= 6) {
                    draw_surface(surface_collection.embassy, x, y);
                    draw_poi_name(x, y, poi[i].name);
                }
            } else if (poi[i].type == "restaurant") {
                setcolor(200, 101, 57);
                if (zoom >= 7) {
                    draw_surface(surface_collection.dinning, x, y);
                    draw_poi_name(x, y, poi[i].name);
                }
            } else if (poi[i].type == "bank") {
                setcolor(93, 100, 151);
                if (zoom >= 6) {
                    draw_surface(surface_collection.bank, x, y);
                    draw_poi_name(x, y, poi[i].name);
                }
            } else if (poi[i].type == "bus_station") {
                setcolor(0, 121, 198);
                if (zoom >= 7) {
                    draw_surface(surface_collection.bus, x, y);
                    draw_poi_name(x, y, poi[i].name);
                }
            } else if (poi[i].type == "marketplace") {
                setcolor(48, 93, 167);
                if (zoom >= 6) {
                    draw_surface(surface_collection.market, x, y);
                    draw_poi_name(x, y, poi[i].name);
                }
            } else if (poi[i].type == "fuel") {
                setcolor(74, 96, 107);
                if (zoom_level >= 6) {
                    draw_surface(surface_collection.fuel, x, y);
                    draw_poi_name(x, y, poi[i].name);
                }
            } else if (poi[i].type == "parking") {
                setcolor(74, 96, 107);
                if (zoom >= 7) {
                    draw_surface(surface_collection.parking, x, y);
                    draw_poi_name(x, y, poi[i].name);
                }
            } else if (poi[i].type == "cafe") {
                setcolor(200, 101, 57);
                if (zoom >= 7) {
                    draw_surface(surface_collection.cafe, x, y);
                    draw_poi_name(x, y, poi[i].name);
                }
            } else if (poi[i].type == "post_office") {
                setcolor(74, 96, 107);
                if (zoom >= 7) {
                    draw_surface(surface_collection.post, x, y);
                    draw_poi_name(x, y, poi[i].name);
                }
            } else if (poi[i].type == "cinema") {
                setcolor(16, 112, 124);
                if (zoom >= 7) {
                    draw_surface(surface_collection.cinema, x, y);
                    draw_poi_name(x, y, poi[i].name);
                }
            } else if (poi[i].type == "police") {
                setcolor(74, 96, 107);
                if (zoom >= 6) {
                    draw_surface(surface_collection.police, x, y);
                    draw_poi_name(x, y, poi[i].name);
                }
            } else if (poi[i].type == "nightclub") {
                setcolor(74, 96, 107);
                if (zoom >= 8) {
                    draw_surface(surface_collection.club, x, y);
                    draw_poi_name(x, y, poi[i].name);
                }
            }

        }
    }
}

// For drawing expressways

void draw_expressways(bool first_time) {
    std::vector<street_segment_data> expressways = my_map -> get_expressways();
    // loop through all the expressways
    for (unsigned i = 0; i < expressways.size(); ++i) {
        float speed_limit = expressways[i].speedLimit;
        street_segment_drawing segment = get_street_segment_drawing(speed_limit);

        // loop through each curve point
        for (unsigned j = 0; j < expressways[i].position_pts.size() - 1; ++j) {

            float x1 = longitude_to_cartesian(expressways[i].position_pts[j].lon(), avg_lat);
            float y1 = latitude_to_cartesian(expressways[i].position_pts[j].lat());
            float x2 = longitude_to_cartesian(expressways[i].position_pts[j + 1].lon(), avg_lat);
            float y2 = latitude_to_cartesian(expressways[i].position_pts[j + 1].lat());

            setcolor(segment.color);
            setlinewidth(segment.linewidth);

            // Depending on zoom level, the line width should be different
            if (zoom <= 4) {
                if (first_time) {
                    setcolor(244, 200, 74);
                    setlinewidth(8);
                } else {
                    setlinewidth(7);
                    setcolor(255, 233, 139);
                }
                drawline(x1, y1, x2, y2);
            } else {
                drawline(x1, y1, x2, y2);
            }

            // For drawing the expressway name on the map
            if (!first_time) {
                if (expressways[i].street_name != "<unknown>") {
                    setcolor(BLACK);
                    //For indicating whether the expressway is one way
                    if (expressways[i].oneWay) {
                        std::string name_one_way = expressways[i].street_name + " ----->";
                        draw_segment_name(x1, y1, x2, y2, name_one_way);
                    } else
                        draw_segment_name(x1, y1, x2, y2, expressways[i].street_name);
                }
            }
        }
    }
}

// For drawing all the street segments

void draw_street_segments() {
    std::vector<street_segment_data> other_street_segments = my_map -> get_other_street_segments();
    // Loop through all the street segments
    for (unsigned i = 0; i < other_street_segments.size(); ++i) {
        float speed_limit = other_street_segments[i].speedLimit;
        street_segment_drawing segment = get_street_segment_drawing(speed_limit);

        for (unsigned j = 0; j < other_street_segments[i].position_pts.size() - 1; ++j) {

            float x1 = longitude_to_cartesian(other_street_segments[i].position_pts[j].lon(), avg_lat);
            float y1 = latitude_to_cartesian(other_street_segments[i].position_pts[j].lat());
            float x2 = longitude_to_cartesian(other_street_segments[i].position_pts[j + 1].lon(), avg_lat);
            float y2 = latitude_to_cartesian(other_street_segments[i].position_pts[j + 1].lat());

            setcolor(segment.color);
            setlinewidth(segment.linewidth);

            // Depedning on the zoom level, draw each type of segment with their own linewidth
            if (zoom <= 1) {
                //                if (speed_limit >= 60) {
                //                    setlinewidth(1);
                //                    drawline(x1, y1, x2, y2);  
                //                }
            } else if (zoom == 2) {
                if (speed_limit >= 50) {
                    //if (speed_limit < 50) setlinewidth(1);
                    if (speed_limit < 60) setlinewidth(1);
                    else setlinewidth(2);
                    drawline(x1, y1, x2, y2);
                }
            } else if (zoom == 3) {
                if (speed_limit >= 40) {
                    if (speed_limit < 50) setlinewidth(1);
                    else if (speed_limit < 60) setlinewidth(2);
                    else setlinewidth(4);
                    drawline(x1, y1, x2, y2);
                }
            } else if (zoom == 4) {
                if (speed_limit >= 40) {
                    if (speed_limit < 50) setlinewidth(1);
                    else if (speed_limit < 60) setlinewidth(3);
                    else setlinewidth(5);
                    drawline(x1, y1, x2, y2);
                }
            } else if (zoom == 5) {
                if (speed_limit >= 40) {
                    if (speed_limit < 50) setlinewidth(2);
                    else if (speed_limit < 60) setlinewidth(4);
                    else setlinewidth(6);
                    drawline(x1, y1, x2, y2);
                }
            } else if (zoom == 6) {
                if (speed_limit >= 40) {
                    if (speed_limit < 50) setlinewidth(2);
                    else if (speed_limit < 60) setlinewidth(4);
                    else setlinewidth(6);
                    drawline(x1, y1, x2, y2);
                }
            } else
                drawline(x1, y1, x2, y2);
            // Draw name of each street segment
            if (other_street_segments[i].street_name != "<unknown>") {
                setcolor(BLACK);
                //setfontsize(7);
                if (other_street_segments[i].oneWay) {
                    std::string name_one_way;
                    if (x1 <= x2)
                        name_one_way = other_street_segments[i].street_name + " ----->";
                    else
                        name_one_way = " <-----" + other_street_segments[i].street_name;
                    draw_segment_name(x1, y1, x2, y2, name_one_way); //street_segments[i].street_name);
                } else
                    draw_segment_name(x1, y1, x2, y2, other_street_segments[i].street_name);
            }
        }
    }
    // Draw all the expressways
    draw_expressways(true);
    draw_expressways(false);
}

// For drawing one features name

void draw_feature_name(std::vector<feature_info>& feature) {

    // loop through the size of this feature
    for (unsigned i = 0; i < feature.size(); ++i) {
        float xc = 0;
        float yc = 0;
        t_point* position = new t_point[feature[i].position_pts.size()];

        // loop through all points of current feature
        for (unsigned j = 0; j < feature[i].position_pts.size(); ++j) {
            position[j].x = longitude_to_cartesian(feature[i].position_pts[j].lon(), avg_lat);
            position[j].y = latitude_to_cartesian(feature[i].position_pts[j].lat());
            xc = xc + position[j].x;
            yc = yc + position[j].y;
        }

        // if the feature is closed
        if (position[0].x == position[feature[i].position_pts.size() - 1].x &&
                position[0].y == position[feature[i].position_pts.size() - 1].y) {

            // draw feature name
            xc = xc / feature[i].position_pts.size();
            yc = yc / feature[i].position_pts.size();

            if (feature[i].name != "<noname>" && feature[i].name != "<unspecified>") {
                setcolor(SADDLEBROWN);
                drawtext(xc, yc, feature[i].name, 100, 100);
            }
        }// if the feature is not closed
        else {
            setlinewidth(2);
            for (unsigned j = 0; j < feature[i].position_pts.size() - 1; ++j) {
                if (feature[i].name != "<noname>" && feature[i].name != "<unspecified>") {
                    setcolor(SADDLEBROWN);
                    draw_segment_name(position[j].x, position[j].y, position[j + 1].x, position[j + 1].y, feature[i].name);
                }
            }
        }
        delete [] position;

    }

}

// For drawing one feature

void draw_feature(std::vector<feature_info>& feature, t_color color) {
    // loop through all features
    for (unsigned i = 0; i < feature.size(); ++i) {

        float xc = 0;
        float yc = 0;
        t_point* position = new t_point[feature[i].position_pts.size()];

        // loop through all the points of a feature
        for (unsigned j = 0; j < feature[i].position_pts.size(); ++j) {
            position[j].x = longitude_to_cartesian(feature[i].position_pts[j].lon(), avg_lat);
            position[j].y = latitude_to_cartesian(feature[i].position_pts[j].lat());
            xc = xc + position[j].x;
            yc = yc + position[j].y;
        }

        // if the feature is closed
        if (position[0].x == position[feature[i].position_pts.size() - 1].x &&
                position[0].y == position[feature[i].position_pts.size() - 1].y) {
            setcolor(color);
            fillpoly(position, feature[i].position_pts.size());

        }// if the feature is not closed
        else {
            setlinewidth(2);
            for (unsigned j = 0; j < feature[i].position_pts.size() - 1; ++j) {
                setcolor(color);
                drawline(position[j].x, position[j].y, position[j + 1].x, position[j + 1].y);
            }
        }
        delete [] position;
    }

}

// For drawing all the feature names depending on their type

void draw_all_feature_name() {
    if (feature_name_draw) {
        feature_data features = my_map -> get_features();
        // depending on the zoom level, draw names of selected feature types
        setfontsize(10);
        if (zoom == 6 || zoom == 7) {
            draw_feature_name(features.beach);
            draw_feature_name(features.lake);
            draw_feature_name(features.river);
            draw_feature_name(features.island);
            draw_feature_name(features.shoreline);
            draw_feature_name(features.park);
            draw_feature_name(features.greenspace);
            draw_feature_name(features.golfcourse);
            draw_feature_name(features.stream);
        } else if (zoom > 7) {
            draw_feature_name(features.beach);
            draw_feature_name(features.lake);
            draw_feature_name(features.river);
            draw_feature_name(features.island);
            draw_feature_name(features.building);
            draw_feature_name(features.shoreline);
            draw_feature_name(features.park);
            draw_feature_name(features.greenspace);
            draw_feature_name(features.golfcourse);
            draw_feature_name(features.stream);
        }
    }
}

// For drawing all the features with colours

void draw_all_features() {
    feature_data features = my_map -> get_features();
    setfontsize(9);
    // Depending on zoom level, draw different type of features
    if (zoom <= 2) {
        draw_feature(features.beach, get_feature_color(Beach));
        draw_feature(features.lake, get_feature_color(Lake));
        draw_feature(features.river, get_feature_color(River));
        draw_feature(features.island, get_feature_color(Island));
        draw_feature(features.shoreline, get_feature_color(Shoreline));
        draw_feature(features.park, get_feature_color(Park));
        draw_feature(features.greenspace, get_feature_color(Greenspace));
        draw_feature(features.golfcourse, get_feature_color(Golfcourse));
    } else if (zoom == 3 || zoom == 4 || zoom == 5 || zoom == 6) {
        draw_feature(features.beach, get_feature_color(Beach));
        draw_feature(features.lake, get_feature_color(Lake));
        draw_feature(features.river, get_feature_color(River));
        draw_feature(features.island, get_feature_color(Island));
        draw_feature(features.shoreline, get_feature_color(Shoreline));
        draw_feature(features.park, get_feature_color(Park));
        draw_feature(features.greenspace, get_feature_color(Greenspace));
        draw_feature(features.golfcourse, get_feature_color(Golfcourse));
        draw_feature(features.stream, get_feature_color(Stream));
    } else {
        draw_feature(features.beach, get_feature_color(Beach));
        draw_feature(features.lake, get_feature_color(Lake));
        draw_feature(features.river, get_feature_color(River));
        draw_feature(features.island, get_feature_color(Island));
        draw_feature(features.shoreline, get_feature_color(Shoreline));
        draw_feature(features.building, get_feature_color(Building));
        draw_feature(features.park, get_feature_color(Park));
        draw_feature(features.greenspace, get_feature_color(Greenspace));
        draw_feature(features.golfcourse, get_feature_color(Golfcourse));
        draw_feature(features.stream, get_feature_color(Stream));
    }

}

// For drawing intersection box

void draw_intersection_box(float& x_left, float& x_right, float& y_bottom, float& y_top) {
    // get current coordinates of the world
    t_bound_box box = get_visible_world();

    // to get current coordinate
    float xc = (box.left() + box.right()) * 0.5;
    float x_length = box.right() - box.left();
    float y_length = box.top() - box.bottom();

    // 1/6 of the total length
    float x_factor = 0.166667;

    x_left = xc - x_factor * x_length;

    x_right = xc + x_factor * x_length;
    // give 0.0625 * y_length left on the bottom
    y_bottom = box.bottom() + 0.0625 * y_length;
    // give 0.1 * y_length of height to the rectangle
    y_top = y_bottom + 0.1 * y_length;

    // draw a rectangle to show the intersection name and LatLon
    setcolor(WHITE);
    fillrect(x_left, y_bottom, x_right, y_top);
}

// Draw a box at the bottom of the screen and show the info of that intersection after the user clicks on the intersection

void draw_intersection_info(std::string name) {

    float x_left, x_right, y_bottom, y_top;

    draw_intersection_box(x_left, x_right, y_bottom, y_top);

    // determine where to draw the text
    float yc = (y_bottom + y_top) * 0.5;
    std::string location = std::to_string(intersection_first_clicked_on.lat()) + ", " + std::to_string(intersection_first_clicked_on.lon());

    // draw the actual data
    setcolor(BLACK);
    setfontsize(11);
    t_bound_box upper(x_left, yc, x_right, y_top);
    t_bound_box lower(x_left, y_bottom, x_right, yc);
    drawtext_in(upper, name);
    drawtext_in(lower, location);
}

// For clearing what has been clicked in the same scale

void clear_intersection_click() {
    draw_screen();
}

// For returning a box below the search bar

void find_box_below(float& y_bottom, float& y_top) {

    float temp;
    temp = y_top;
    y_top = y_bottom;
    y_bottom = y_top - (temp - y_top);
}

// For drawing boxes below the search bar

void draw_box_below() {

    // For finding the location of the search bar
    t_bound_box box = get_visible_world();
    float height = box.top() - box.bottom();
    float width = box.right() - box.left();

    float room_reserved = 0.008 * width;
    float x_length = 0.26 * width;

    float x_left = box.left() + room_reserved;
    float y_top = box.top() - room_reserved;
    float x_right = x_left + x_length;
    float y_bottom = y_top - 0.07 * height;

    // draw auto complete results in the box below the search bar
    for (unsigned i = 0; i < 3; i++) {
        if (i < auto_names.size()) {
            // For determining whether user input has exceeded the search bar box
            float length = auto_names[i].length();
            // If so, change font size to fit the box
            if (length >= 30 && length < 35) setfontsize(11);
            else if (length >= 35 && length < 40) setfontsize(10);
            else if (length >= 40) setfontsize(9);
            find_box_below(y_bottom, y_top);
            setcolor(WHITE);
            fillrect(x_left, y_bottom, x_right, y_top);

            setcolor(BLACK);
            t_bound_box text_bound(x_left, y_bottom, x_right, y_top);
            drawtext_in(text_bound, auto_names[i]);
        } else {
            setfontsize(12);
            find_box_below(y_bottom, y_top);
            setcolor(WHITE);
            fillrect(x_left, y_bottom, x_right, y_top);

            setcolor(BLACK);
            t_bound_box text_bound(x_left, y_bottom, x_right, y_top);
            drawtext_in(text_bound, "Not Available");
        }
    }
}

void draw_help_box_below() {

    // For finding the location of the search bar
    t_bound_box box = get_visible_world();
    float height = box.top() - box.bottom();
    float width = box.right() - box.left();

    float x_length = 0.30 * width;
    float y_length = 0.04 * height;

    float x_left = box.right() - x_length - 0.077 * width;
    float y_top = box.top() - height + 0.27 * height;
    float x_right = box.right() - 0.077 * width;
    float y_bottom = y_top - y_length;

    // draw auto complete results in the box below the search bar
    for (unsigned i = 0; i < auto_names.size(); i++) {
        // For determining whether user input has exceeded the search bar box
        float length = auto_names[i].length();
        // If so, change font size to fit the box
        if (length >= 30 && length < 35) setfontsize(11);
        else if (length >= 35 && length < 40) setfontsize(10);
        else if (length >= 40) setfontsize(9);
        find_box_below(y_bottom, y_top);
        setcolor(DARKGREY);
        fillrect(x_left, y_bottom, x_right, y_top);

        setcolor(BLACK);
        t_bound_box text_bound(x_left, y_bottom, x_right, y_top);
        drawtext_in(text_bound, auto_names[i]);
    }
}

void draw_find_route_box_below() {

    // For finding the location of the search bar
    t_bound_box box = get_visible_world();
    float height = box.top() - box.bottom();
    float width = box.right() - box.left();
    float x_length = 0.26 * width;

    float x_left = box.left();
    float x_right = x_left + x_length;
    float y_bottom = box.top() - 0.2 * height;
    float y_top = box.top() - 0.75 * (box.top() - y_bottom);

    float room_reserved = 0.05 * width;
    float line_x_left = x_left + room_reserved;
    float line_x_right = x_right - room_reserved;

    // draw auto complete results in the box below the search bar
    for (unsigned i = 0; i < 3; i++) {
        if (i < auto_names.size()) {
            // For determining whether user input has exceeded the search bar box
            float length = auto_names[i].length();
            // If so, change font size to fit the box
            if (length >= 25 && length < 30) setfontsize(11);
            else if (length >= 30 && length < 35) setfontsize(10);
            else if (length >= 35) setfontsize(9);
            find_box_below(y_bottom, y_top);
            setcolor(WHITE);
            fillrect(x_left, y_bottom, x_right, y_top);

            setcolor(BLACK);
            t_bound_box text_bound(x_left, y_bottom, x_right, y_top);
            drawtext_in(text_bound, auto_names[i]);

        } else {
            setfontsize(12);
            find_box_below(y_bottom, y_top);
            setcolor(WHITE);
            fillrect(x_left, y_bottom, x_right, y_top);

            setcolor(BLACK);
            t_bound_box text_bound(x_left, y_bottom, x_right, y_top);
            drawtext_in(text_bound, "Not Available");
        }
    }

    y_bottom = box.top() - 0.2 * height;
    y_top = box.top() - 0.75 * (box.top() - y_bottom);
    for (unsigned i = 0; i < 3; i++) {
        find_box_below(y_bottom, y_top);
        setcolor(t_color(224, 224, 224));
        drawline(line_x_left, y_bottom, line_x_right, y_bottom);
    }
}

// For searching auto complete result and drawing it

void draw_auto_complete() {

    unsigned search_num = 3;

    if (has_clicked_find) {
        // check whether there is an auto-complete suggestion available, if not, check whether there's auto correction available
        my_map -> get_street_names_TST() -> predict_words(auto_names, entered_text, search_num);
        if (auto_names.empty() && entered_text.length() > 3)
            my_map -> get_street_names_BK() -> getWords(my_map -> get_street_names_BK() -> get_root(), entered_text, auto_names, search_num);
    } else if (has_clicked_find_POI) {
        // check whether there is an auto-complete suggestion available, if not, check whether there's auto correction available
        my_map -> get_poi_names_TST() -> predict_words(auto_names, entered_text, search_num);
        if (auto_names.empty() && entered_text.length() > 3)
            my_map -> get_poi_names_BK() -> getWords(my_map -> get_poi_names_BK() -> get_root(), entered_text, auto_names, search_num);
    }

    draw_box_below();

}

// For searching auto complete result for help button and drawing it
void draw_auto_complete_help() {

    unsigned search_num = 1;

    if (help_button_has_clicked) my_map ->get_help_button_names_TST() -> predict_words(auto_names, entered_help_command, search_num);

    draw_help_box_below();
}

// For searching auto complete result for find route button and drawing it
void draw_find_route_auto_complete() {

    unsigned search_num = 3;

    if (has_clicked_find_route && !has_entered_first) {
        if (entered_start != "Start Point") {
            // check whether there is an auto-complete suggestion available, if not, check whether there's auto correction available
            my_map -> get_street_names_TST() -> predict_words(auto_names, entered_start, search_num);
            if (auto_names.empty() && entered_start.length() > 3)
                my_map -> get_street_names_BK() -> getWords(my_map -> get_street_names_BK() -> get_root(), entered_start, auto_names, search_num);
        }

        draw_find_route_box_below();
    } else if (has_clicked_find_route && !has_entered_second) {

        if (entered_end != "End Point") {
            // check whether there is an auto-complete suggestion available, if not, check whether there's auto correction available
            my_map -> get_poi_names_TST() -> predict_words(auto_names, entered_end, search_num);
            if (auto_names.size() < 3) {
                my_map -> get_street_names_TST() -> predict_words(auto_names, entered_end, search_num - auto_names.size());
                if (auto_names.empty() && entered_end.length() > 3) {
                    my_map -> get_poi_names_BK() -> getWords(my_map -> get_poi_names_BK() -> get_root(), entered_end, auto_names, search_num);
                    if (auto_names.empty() && entered_end.length() > 3)
                        my_map -> get_street_names_BK() -> getWords(my_map -> get_street_names_BK() -> get_root(), entered_end, auto_names, search_num);
                }
            }
        }

        draw_find_route_box_below();
    }
}

// For drawing text of user input in the search bar

void draw_search_bar_text() {

    setcolor(BLACK);
    setfontsize(12);
    if (entered_text == "Eg. College Street & Bay Street" || entered_text == "Eg. Eaton Centre")
        setcolor(DARKGREY);
    // Find the location for the search bar
    t_bound_box box = get_visible_world();
    float height = box.top() - box.bottom();
    float width = box.right() - box.left();

    float room_reserved = 0.008 * width;
    float x_length = 0.26 * width;

    float x_left = box.left() + room_reserved;
    float y_top = box.top() - room_reserved;
    float x_right = x_left + x_length;
    float y_bottom = y_top - 0.06 * height;

    // set different kind of font size for different input length
    unsigned length = entered_text.length();
    if (length >= 30 && length < 35) setfontsize(11);
    else if (length >= 35 && length < 40) setfontsize(10);
    else if (length >= 40) setfontsize(9);

    t_bound_box text_bound(x_left, y_bottom, x_right, y_top);
    drawtext_in(text_bound, entered_text);
    draw_auto_complete();
}

// For drawing search bar on the top left of the screen

void draw_search_bar() {

    // For finding the location of the search bar
    t_bound_box box = get_visible_world();
    float height = box.top() - box.bottom();
    float width = box.right() - box.left();

    float room_reserved = 0.008 * width;
    float x_length = 0.26 * width;

    float x_left = box.left() + room_reserved;
    float y_top = box.top() - room_reserved;
    float x_right = x_left + x_length;
    float y_bottom = y_top - 0.06 * height;

    setcolor(WHITE);
    fillrect(x_left, y_bottom, x_right, y_top);

    // For drawing the amplifying glass in the search bar
    float x = x_right - 0.03 * width;
    float y = y_top - room_reserved;
    Surface pic = load_png_from_file("libstreetmap/resources/searching-1.png");
    draw_surface(pic, x, y);

    draw_search_bar_text();
}

void draw_find_route_search_bar_text() {

    setcolor(WHITE);
    setfontsize(12);

    // Find the location for the search bar 
    t_bound_box box = get_visible_world();
    float height = box.top() - box.bottom();
    float width = box.right() - box.left();

    float x_length = 0.26 * width;

    float x_left = box.left();
    float y_top = box.top();
    float x_right = box.left() + x_length;
    float y_bottom = y_top - 0.2 * height;


    float top_bar_height = y_top - y_bottom;
    y_bottom = y_top - 0.5 * top_bar_height;
    y_top = y_bottom + 0.15 * top_bar_height;

    // draw first text for start point
    if (entered_start == "Start Point") setcolor(t_color(155, 155, 155));
    else if (has_entered_first) setcolor(DARKGREY);//setcolor(t_color(133, 172, 248));
    t_bound_box text_bound1(x_left, y_bottom, x_right, y_top);
    unsigned length = entered_start.length();
    setfontsize(12);
    if (length >= 25 && length < 30) setfontsize(11);
    else if (length >= 30 && length < 35) setfontsize(10);
    else if (length >= 35) setfontsize(9);
    drawtext_in(text_bound1, entered_start);

    setcolor(WHITE);
    // draw second text for end point
    if (entered_end == "End Point") setcolor(t_color(155, 155, 155));
    else if (has_entered_second) setcolor(DARKGREY);//setcolor(t_color(133, 172, 248));
    y_bottom = y_bottom - 0.3 * top_bar_height;
    y_top = y_bottom + 0.15 * top_bar_height;
    t_bound_box text_bound2(x_left, y_bottom, x_right, y_top);
    length = entered_end.length();
    setfontsize(12);
    if (length >= 25 && length < 30) setfontsize(11);
    else if (length >= 30 && length < 35) setfontsize(10);
    else if (length >= 35) setfontsize(9);
    drawtext_in(text_bound2, entered_end);

    draw_find_route_auto_complete();
}

void draw_find_route_search_bar() {

    // For finding the location of the search bar
    t_bound_box box = get_visible_world();
    float height = box.top() - box.bottom();
    float width = box.right() - box.left();
    float x_length = 0.26 * width;

    float x_left = box.left();
    float y_top = box.top();
    float x_right = x_left + x_length;
    float y_bottom = box.bottom();

    // fill out the color of the top
    y_bottom = y_top - 0.2 * height;
    setcolor(PLUM);
    //setcolor(t_color(252, 187, 204));
    //setcolor(t_color(52, 109, 241));
    fillrect(x_left, y_bottom, x_right, y_top);
    
    // fill out the white part of the map for finding route
    setcolor(WHITE);
    fillrect(x_left, box.bottom(), x_right, y_bottom);

    // draw help text for users 
    y_bottom = y_top - 0.3 * (y_top - y_bottom);
    setcolor(WHITE);
    //setcolor(t_color(133, 172, 248));
    t_bound_box text_bound(x_left, y_bottom, x_right, y_top);
    setfontsize(11);
    drawtext_in(text_bound, "Press enter when finishing typing");

    // draw two search lines
    y_bottom = y_top - 0.2 * height;
    setcolor(WHITE);
    float room_reserved = 0.05 * width;
    x_left = x_left + room_reserved;
    x_right = x_right - room_reserved;
    setlinewidth(1);
    //setcolor(t_color(252, 187, 204));
    //setcolor(t_color(133, 172, 248));

    float top_bar_height = y_top - y_bottom;
    float y = y_top - 0.5 * top_bar_height;
    drawline(x_left, y, x_right, y);

    y = y - 0.3 * top_bar_height;
    drawline(x_left, y, x_right, y);

    draw_find_route_search_bar_text();
}

void draw_help_box() {

    // find the location of the box
    t_bound_box box = get_visible_world();

    float height = box.top() - box.bottom();
    float width = box.right() - box.left();

    float x_length = 0.30 * width;
    float y_length = 0.30 * height;

    float x_left = box.right() - x_length - 0.077 * width;
    float y_top = box.top() - height + y_length;
    float x_right = box.right() - 0.077 * width;
    float y_bottom = box.top() - height;

    setcolor(WHITE);
    fillrect(x_left, y_bottom, x_right, y_top);

    float x = x_left + 0.01 * width;
    float y = y_top - 0.01 * height;
    Surface pic = load_png_from_file("libstreetmap/resources/help_button.png");
    draw_surface(pic, x, y);
}

void draw_help_box_text() {

    setcolor(BLACK);
    setfontsize(12);
    // set a grey color if it's an example input
    if (entered_help_command == "Eg. Find") setcolor(DARKGREY);

    // Find the location for the search bar
    t_bound_box box = get_visible_world();


    float height = box.top() - box.bottom();
    float width = box.right() - box.left();


    float x_length = 0.30 * width;
    float y_length = 0.30 * height;

    float x_left = box.right() - x_length - 0.077 * width;
    float y_top = box.bottom() + y_length;
    float x_right = box.right() - 0.077 * width;
    float y_bottom = y_top - 0.2 * y_length;
    // draw user input in this
    t_bound_box text_bound(x_left, y_bottom, x_right, y_top);
    drawtext_in(text_bound, entered_help_command);
    draw_auto_complete_help();

}

void draw_start_help_text() {

    setcolor(DARKGREY);
    setfontsize(11);

    t_bound_box box = get_visible_world();

    float height = box.top() - box.bottom();
    float width = box.right() - box.left();
    float x_length = 0.30 * width;
    float y_length = 0.30 * height;

    float x_left = box.right() - x_length - 0.077 * width;
    float y_top = box.top() - height + y_length;
    float x_right = box.right() - 0.077 * width;
    float y_bottom = box.top() - height;

    // Get the four locations of the start help text
    t_bound_box text_bound1(x_left, y_bottom, x_right, y_top + 0.1 * width);
    t_bound_box text_bound2(x_left, y_bottom, x_right, y_top - 0.03 * width + 0.1 * width);
    t_bound_box text_bound3(x_left, y_bottom, x_right, y_top - 0.03 * 5 * width + 0.1 * width);
    t_bound_box text_bound4(x_left, y_bottom, x_right, y_top - 0.03 * 6 * width + 0.1 * width);

    // Draw the texts in different lines
    drawtext_in(text_bound1, "This is a help button for users to learn our map.");
    drawtext_in(text_bound2, "Type in button names and press ENTER to see its functionality");
    drawtext_in(text_bound3, "Note: On/Off does not need to be included in typing");
    drawtext_in(text_bound4, "Type ALL to see all the functionalities");
}

// Function to draw text if user types ALL in the help

void draw_all_help_box() {

    t_bound_box box = get_visible_world();
    float height = box.top() - box.bottom();
    float width = box.right() - box.left();

    float xcenter = box.get_xcenter();
    float ycenter = box.get_ycenter();

    float x = xcenter - 0.27 * width;
    float y = ycenter + 0.35 * height;
    Surface pic = load_png_from_file("libstreetmap/resources/allhelpfinal.png");
    draw_surface(pic, x, y);

}

// Functions to draw each type of help text

void draw_restaurent_help_text() {
    setcolor(BLACK);
    setfontsize(12);

    t_bound_box box = get_visible_world();

    float height = box.top() - box.bottom();
    float width = box.right() - box.left();
    float x_length = 0.30 * width;
    float y_length = 0.30 * height;

    float x_left = box.right() - x_length - 0.078 * width;
    float y_top = box.top() - height + y_length;
    float x_right = box.right() - 0.078 * width;
    float y_bottom = box.top() - height;

    t_bound_box text_bound(x_left, y_bottom, x_right, y_top);
    t_bound_box text_bound2(x_left, y_bottom, x_right, y_top - 0.03 * width);
    drawtext_in(text_bound, "After pressing this button,");
    drawtext_in(text_bound2,"all the restaurant names in the area will be shown");
 }

void draw_find_help_text(){
    setcolor(BLACK);
    setfontsize(12);

    t_bound_box box = get_visible_world();

    float height = box.top() - box.bottom();
    float width = box.right() - box.left();

    float x_length = 0.30 * width;
    float y_length = 0.30 * height;

    float x_left = box.right() - x_length - 0.077 * width;
    float y_top = box.top() - height + y_length;
    float x_right = box.right() - 0.077 * width;
    float y_bottom = box.top() - height;

    t_bound_box text_bound(x_left, y_bottom, x_right, y_top);
    t_bound_box text_bound2(x_left, y_bottom, x_right, y_top - 0.03 * width);

    drawtext_in(text_bound, "Users type wanted intersection names on search bar,");
    drawtext_in(text_bound2, "and press enter. POI will be highlighted using red dots. ");

}

void draw_findPOI_help_text() {

    setcolor(BLACK);
    setfontsize(12);

    t_bound_box box = get_visible_world();

    float height = box.top() - box.bottom();
    float width = box.right() - box.left();

    float x_length = 0.30 * width;
    float y_length = 0.30 * height;

    float x_left = box.right() - x_length - 0.077 * width;
    float y_top = box.top() - height + y_length;
    float x_right = box.right() - 0.077 * width;
    float y_bottom = box.top() - height;


    t_bound_box text_bound(x_left, y_bottom, x_right, y_top);
    t_bound_box text_bound2(x_left, y_bottom, x_right, y_top - 0.03 * width);
    drawtext_in(text_bound, "Users type wanted point of interest in search bar,");
    drawtext_in(text_bound2, "and press enter. POI will be highlighted using red dots ");
}

void draw_university_help_text() {
    setcolor(BLACK);
    setfontsize(12);

    t_bound_box box = get_visible_world();

    float height = box.top() - box.bottom();
    float width = box.right() - box.left();

    float x_length = 0.30 * width;
    float y_length = 0.30 * height;

    float x_left = box.right() - x_length - 0.077 * width;
    float y_top = box.top() - height + y_length;
    float x_right = box.right() - 0.077 * width;
    float y_bottom = box.top() - height;


    t_bound_box text_bound(x_left, y_bottom, x_right, y_top);
    t_bound_box text_bound2(x_left, y_bottom, x_right, y_top - 0.03 * width);
    drawtext_in(text_bound, "After pressing this button,");
    drawtext_in(text_bound2, "all the university names in the area will be shown");

}

void draw__hospital_help_text() {

    setcolor(BLACK);
    setfontsize(12);

    t_bound_box box = get_visible_world();

    float height = box.top() - box.bottom();
    float width = box.right() - box.left();

    float x_length = 0.30 * width;
    float y_length = 0.30 * height;

    float x_left = box.right() - x_length - 0.077 * width;
    float y_top = box.top() - height + y_length;
    float x_right = box.right() - 0.077 * width;
    float y_bottom = box.top() - height;


    t_bound_box text_bound(x_left, y_bottom, x_right, y_top);
    t_bound_box text_bound2(x_left, y_bottom, x_right, y_top - 0.03 * width);
    drawtext_in(text_bound, "After pressing this button,");
    drawtext_in(text_bound2, "all the hospital names in the area will be shown");
}

void draw_bank_help_text() {

    setcolor(BLACK);
    setfontsize(12);

    t_bound_box box = get_visible_world();

    float height = box.top() - box.bottom();
    float width = box.right() - box.left();

    float x_length = 0.30 * width;
    float y_length = 0.30 * height;

    float x_left = box.right() - x_length - 0.078 * width;
    float y_top = box.top() - height + y_length;
    float x_right = box.right() - 0.078 * width;
    float y_bottom = box.top() - height;


    t_bound_box text_bound(x_left, y_bottom, x_right, y_top);
    t_bound_box text_bound2(x_left, y_bottom, x_right, y_top - 0.03 * width);
    drawtext_in(text_bound, "After pressing this button,");
    drawtext_in(text_bound2, "all the bank name in the area will be shown");
}

void draw_gas_help_text() {

    setcolor(BLACK);
    setfontsize(12);

    t_bound_box box = get_visible_world();

    float height = box.top() - box.bottom();
    float width = box.right() - box.left();

    float x_length = 0.30 * width;
    float y_length = 0.30 * height;

    float x_left = box.right() - x_length - 0.077 * width;
    float y_top = box.top() - height + y_length;
    float x_right = box.right() - 0.077 * width;
    float y_bottom = box.top() - height;


    t_bound_box text_bound(x_left, y_bottom, x_right, y_top);
    t_bound_box text_bound2(x_left, y_bottom, x_right, y_top - 0.03 * width);
    drawtext_in(text_bound, "After pressing this button,");
    drawtext_in(text_bound2, "all the gas station bames in the area will be shown");
}

void draw_parking_help_text() {

    setcolor(BLACK);
    setfontsize(12);

    t_bound_box box = get_visible_world();

    float height = box.top() - box.bottom();
    float width = box.right() - box.left();

    float x_length = 0.30 * width;
    float y_length = 0.30 * height;

    float x_left = box.right() - x_length - 0.077 * width;
    float y_top = box.top() - height + y_length;
    float x_right = box.right() - 0.077 * width;
    float y_bottom = box.top() - height;


    t_bound_box text_bound(x_left, y_bottom, x_right, y_top);
    t_bound_box text_bound2(x_left, y_bottom, x_right, y_top - 0.03 * width);
    drawtext_in(text_bound, "After pressing this button,");
    drawtext_in(text_bound2, "all the parking place names in the area will be shown");
}

void draw_all_poi_help_text() {
    setcolor(BLACK);
    setfontsize(12);

    t_bound_box box = get_visible_world();

    float height = box.top() - box.bottom();
    float width = box.right() - box.left();

    float x_length = 0.30 * width;
    float y_length = 0.30 * height;

    float x_left = box.right() - x_length - 0.077 * width;
    float y_top = box.top() - height + y_length;
    float x_right = box.right() - 0.077 * width;
    float y_bottom = box.top() - height;


    t_bound_box text_bound(x_left, y_bottom, x_right, y_top);
    t_bound_box text_bound2(x_left, y_bottom, x_right, y_top - 0.03 * width);
    drawtext_in(text_bound, "After pressing this button,");
    drawtext_in(text_bound2, "all the point of interests in the area will be highlighted");
}

void draw_feaName_help_text() {
    setcolor(BLACK);
    setfontsize(12);

    t_bound_box box = get_visible_world();

    float height = box.top() - box.bottom();
    float width = box.right() - box.left();

    float x_length = 0.30 * width;
    float y_length = 0.30 * height;

    float x_left = box.right() - x_length - 0.077 * width;
    float y_top = box.top() - height + y_length;
    float x_right = box.right() - 0.077 * width;
    float y_bottom = box.top() - height;


    t_bound_box text_bound(x_left, y_bottom, x_right, y_top);
    t_bound_box text_bound2(x_left, y_bottom, x_right, y_top - 0.03 * width);
    t_bound_box text_bound3(x_left, y_bottom, x_right, y_top - 0.03 * 2 * width);

    drawtext_in(text_bound, "After pressing this button,");
    drawtext_in(text_bound2, "all the features, such as rivers, parks in the area");
    drawtext_in(text_bound3, "will be highlighted");


}

void draw_findRout_help_text() {
    setcolor(BLACK);
    setfontsize(12);

    t_bound_box box = get_visible_world();

    float height = box.top() - box.bottom();
    float width = box.right() - box.left();

    float x_length = 0.30 * width;
    float y_length = 0.30 * height;

    float x_left = box.right() - x_length - 0.077 * width;
    float y_top = box.top() - height + y_length;
    float x_right = box.right() - 0.077 * width;
    float y_bottom = box.top() - height;


    t_bound_box text_bound(x_left, y_bottom, x_right, y_top);
    t_bound_box text_bound2(x_left, y_bottom, x_right, y_top - 0.03 * width);
    t_bound_box text_bound3(x_left, y_bottom, x_right, y_top - 0.03 * 2 * width);
    drawtext_in(text_bound, "Users type start intersection in first line, press enter.");
    drawtext_in(text_bound2, "Then type finish intersection/POI in second line, and ");
    drawtext_in(text_bound3, "press enter again. The shortest path will be highlighted");


}

void draw_NaviOn_help_text() {

    setcolor(BLACK);
    setfontsize(12);

    t_bound_box box = get_visible_world();

    float height = box.top() - box.bottom();
    float width = box.right() - box.left();

    float x_length = 0.30 * width;
    float y_length = 0.30 * height;

    float x_left = box.right() - x_length - 0.077 * width;
    float y_top = box.top() - height + y_length;
    float x_right = box.right() - 0.077 * width;
    float y_bottom = box.top() - height;


    t_bound_box text_bound(x_left, y_bottom, x_right, y_top);
    t_bound_box text_bound2(x_left, y_bottom, x_right, y_top - 0.03 * width);
    t_bound_box text_bound3(x_left, y_bottom, x_right, y_top - 0.03 * 2 * width);
    drawtext_in(text_bound, "After users use Find Route or click on any two intersections,");
    drawtext_in(text_bound2, "they can press this button to see the directions of the");
    drawtext_in(text_bound3, "shortest path. The shortest path will be highlighted");

}

// For drawing text when user inputs invalid text

void draw_invalid_input() {
    float x_left, x_right, y_bottom, y_top;
    draw_intersection_box(x_left, x_right, y_bottom, y_top);

    // determine where to draw the text
    t_bound_box bound(x_left, y_bottom, x_right, y_top);

    // draw the actual data
    setcolor(BLACK);
    setfontsize(12);

    drawtext_in(bound, "Invalid, please enter again!");

}

// For drawing text when the map can't find the intersection the user is searching for

void draw_intersection_not_found() {

    float x_left, x_right, y_bottom, y_top;
    draw_intersection_box(x_left, x_right, y_bottom, y_top);

    // determine where to draw the text
    t_bound_box bound(x_left, y_bottom, x_right, y_top);

    // draw the actual data
    setcolor(BLACK);
    setfontsize(12);

    drawtext_in(bound, "Sorry, intersection not found!");
}

// For drawing text when the map can't find the poi the user is searching for

void draw_poi_not_found() {

    float x_left, x_right, y_bottom, y_top;
    draw_intersection_box(x_left, x_right, y_bottom, y_top);

    // determine where to draw the text
    t_bound_box bound(x_left, y_bottom, x_right, y_top);

    // draw the actual data
    setcolor(BLACK);
    setfontsize(12);

    std::string text = "Sorry, " + entered_text + " not found!";
    drawtext_in(bound, text);
}

// For printing invalid input for finding route

void draw_start_point_not_found() {

    float x_left, x_right, y_bottom, y_top;
    draw_intersection_box(x_left, x_right, y_bottom, y_top);

    // determine where to draw the text
    t_bound_box bound(x_left, y_bottom, x_right, y_top);

    // draw the actual data
    setcolor(BLACK);
    setfontsize(12);

    drawtext_in(bound, "Sorry, start point not found!");
}

void draw_invalid_first_input() {

    float x_left, x_right, y_bottom, y_top;
    draw_intersection_box(x_left, x_right, y_bottom, y_top);

    // determine where to draw the text
    t_bound_box bound(x_left, y_bottom, x_right, y_top);

    // draw the actual data
    setcolor(BLACK);
    setfontsize(12);

    drawtext_in(bound, "Please enter an intersection as a start point");
}

void draw_invalid_second_input() {

    float x_left, x_right, y_bottom, y_top;
    draw_intersection_box(x_left, x_right, y_bottom, y_top);

    // determine where to draw the text
    t_bound_box bound(x_left, y_bottom, x_right, y_top);

    // draw the actual data
    setcolor(BLACK);
    setfontsize(12);

    drawtext_in(bound, "Please enter an intersection/POI as an end point");
}

void draw_end_point_not_found() {

    float x_left, x_right, y_bottom, y_top;
    draw_intersection_box(x_left, x_right, y_bottom, y_top);

    // determine where to draw the text
    t_bound_box bound(x_left, y_bottom, x_right, y_top);

    // draw the actual data
    setcolor(BLACK);
    setfontsize(12);

    drawtext_in(bound, "Sorry, end point not found!");
}

// helpers for drawing found intersections

void helper_draw_one_found_intersection(std::vector<unsigned>& intersection_ids) {

    std::vector<intersection_data> intersections = my_map -> get_intersections();

    // for zooming in to the level where the first poi          
    float x = longitude_to_cartesian(intersections[intersection_ids[0]].position.lon(), avg_lat);
    float y = latitude_to_cartesian(intersections[intersection_ids[0]].position.lat());
    set_visible_world(x - 0.0003, y - 0.0003, x + 0.0003, y + 0.0003);
    zoom_to_find(x, y);

    radius = 0.000004;
    setcolor(RED);
    fillarc(x, y, radius, 0, 360);

    trans_coord.init_xleft = world.left;
    trans_coord.init_xright = world.right;
    trans_coord.init_ytop = world.top;
    trans_coord.init_ybot = world.bottom;

    float x_left, x_right, y_bottom, y_top;

    draw_intersection_box(x_left, x_right, y_bottom, y_top);

    // determine where to draw the text

    float yc = (y_bottom + y_top) * 0.5;
    std::string location = std::to_string(intersections[intersection_ids[0]].position.lat()) + ", " +
            std::to_string(intersections[intersection_ids[0]].position.lon());

    // draw the actual data
    setcolor(BLACK);
    setfontsize(11);
    std::string name = getIntersectionName(intersection_ids[0]);

    unsigned length = name.length();
    if (length >= 35 && length < 40) setfontsize(11);
    else if (length >= 40 && length < 45) setfontsize(10);
    else if (length >= 45) setfontsize(9);
    t_bound_box upper(x_left, yc, x_right, y_top);
    t_bound_box lower(x_left, y_bottom, x_right, yc);
    drawtext_in(upper, intersections[intersection_ids[0]].name);
    drawtext_in(lower, location);
}

void helper_draw_multiple_found_intersection(std::vector<unsigned>& intersection_ids) {

    std::vector<intersection_data> intersections = my_map -> get_intersections();

    zoom_fit(draw_screen);
    float x_left, x_right, y_bottom, y_top;
    draw_intersection_box(x_left, x_right, y_bottom, y_top);

    setcolor(BLACK);
    setfontsize(12);

    std::string numbers = "Found " + std::to_string(intersection_ids.size()) + " intersections";
    // determine where to draw the text
    float yc = (y_bottom + y_top) * 0.5;
    t_bound_box upper(x_left, yc, x_right, y_top);
    t_bound_box lower(x_left, y_bottom, x_right, yc);
    drawtext_in(upper, numbers);

    // For not exceeding the box boundary
    std::string name = getIntersectionName(intersection_ids[0]);
    unsigned length = name.length();
    if (length >= 35 && length < 40) setfontsize(11);
    else if (length >= 40 && length < 45) setfontsize(10);
    else if (length >= 45) setfontsize(9);
    drawtext_in(lower, name);

    radius = 0.0001;

    color_types color = RED;

    for (unsigned i = 0; i < intersection_ids.size(); i++) {
        auto const x = longitude_to_cartesian(intersections[intersection_ids[i]].position.lon(), avg_lat);
        auto const y = latitude_to_cartesian(intersections[intersection_ids[i]].position.lat());
        setcolor(color + i);
        fillarc(x, y, radius, 0, 360);
    }
}

// For drawing the intersection that the user searches for

void draw_found_intersections(std::vector<unsigned>& intersection_ids) {

    if (intersection_ids.size() == 1)
        helper_draw_one_found_intersection(intersection_ids);
    else
        helper_draw_multiple_found_intersection(intersection_ids);
}

// For highlighting the POIs that were found

void highlight_found_poi(std::vector<unsigned>& ids) {

    std::vector<poi_data> poi = my_map -> get_poi();

    // level of detail, depending on zoom level, set different radius for intersection
    radius = 0.00001;
    if (zoom == 5) radius = 0.00001;
    else if (zoom == 6) radius = 0.0000065;
    else if (zoom == 4) radius = 0.000017;
    else if (zoom == 3) radius = 0.000025;
    else if (zoom == 7) radius = 0.000004;
    else if (zoom == 2) radius = 0.000038;
    else if (zoom == 1) radius = 0.000055;
    else if (zoom == 8) radius = 0.000003;
    else if (zoom == 0) radius = 0.0001;
    else if (zoom > 8) radius = 0.0000015;

    color_types color = RED;

    for (unsigned i = 0; i < ids.size(); i++) {
        auto const x = longitude_to_cartesian(poi[ids[i]].position.lon(), avg_lat);
        auto const y = latitude_to_cartesian(poi[ids[i]].position.lat());
        setcolor(color + i);
        fillarc(x, y, radius, 0, 360);
    }
}

// helpers to draw_found poi depending on the size of found poi

void helper_draw_one_found_poi(std::vector<unsigned>& ids, float& x_left, float& x_right, float& y_bottom, float& y_top, std::string name) {

    std::vector<poi_data> poi = my_map -> get_poi();

    // for zooming in to the level where the first poi
    float x = longitude_to_cartesian(poi[ids[0]].position.lon(), avg_lat);
    float y = latitude_to_cartesian(poi[ids[0]].position.lat());
    set_visible_world(x - 0.0003, y - 0.0003, x + 0.0003, y + 0.0003);
    zoom_to_find(x, y);
    highlight_found_poi(ids);

    trans_coord.init_xleft = world.left;
    trans_coord.init_xright = world.right;
    trans_coord.init_ytop = world.top;
    trans_coord.init_ybot = world.bottom;

    draw_intersection_box(x_left, x_right, y_bottom, y_top);
    setcolor(BLACK);
    setfontsize(12);

    // determine where to draw the text    
    float yc = (y_bottom + y_top) * 0.5;
    t_bound_box upper(x_left, yc, x_right, y_top);
    t_bound_box lower(x_left, y_bottom, x_right, yc);

    std::string location = std::to_string(poi[ids[0]].position.lat()) + ", " + std::to_string(poi[ids[0]].position.lon());
    std::string type_position = "Type: " + poi[ids[0]].type + "   Position: " + location;

    // For not exceeding the box boundary
    unsigned length1 = name.length();
    if (length1 >= 35 && length1 < 40) setfontsize(11);
    else if (length1 >= 40 && length1 < 45) setfontsize(10);
    else if (length1 >= 45) setfontsize(9);
    drawtext_in(upper, name);

    unsigned length2 = type_position.length();
    setfontsize(12);
    if (length2 >= 35 && length2 < 40) setfontsize(11);
    else if (length2 >= 40 && length2 < 45) setfontsize(10);
    else if (length2 >= 45) setfontsize(9);
    drawtext_in(lower, type_position);
}

void helper_draw_multiple_found_poi(std::vector<unsigned>& ids, float& x_left, float& x_right, float& y_bottom, float& y_top, std::string name) {
    // zoom fit to max screen to show multiple pois
    zoom_fit(draw_screen);
    highlight_found_poi(ids);

    draw_intersection_box(x_left, x_right, y_bottom, y_top);
    setcolor(BLACK);
    setfontsize(12);

    // determine where to draw the text    
    float yc = (y_bottom + y_top) * 0.5;
    t_bound_box upper(x_left, yc, x_right, y_top);
    t_bound_box lower(x_left, y_bottom, x_right, yc);

    std::string numbers = "Found " + std::to_string(ids.size()) + " " + name;
    // For not exceeding the box boundary
    unsigned length2 = numbers.length();
    setfontsize(12);
    if (length2 >= 35 && length2 < 40) setfontsize(11);
    else if (length2 >= 40 && length2 < 45) setfontsize(10);
    else if (length2 >= 45) setfontsize(9);
    t_bound_box box(x_left, y_bottom, x_right, y_top);
    drawtext_in(box, numbers);
}

// For drawing the found poi that the user searches for

void draw_found_poi(std::string name) {

    std::vector<unsigned> ids = my_map -> get_poi_name_id()[name];

    float x_left, x_right, y_bottom, y_top;

    // print poi type and position   
    if (ids.size() == 1)
        helper_draw_one_found_poi(ids, x_left, x_right, y_bottom, y_top, name);
    else
        helper_draw_multiple_found_poi(ids, x_left, x_right, y_bottom, y_top, name);
}

// For filtering minor errors of the input from user

void filter_minor_input_errors(std::string& name) {

    // if the user input is empty
    if (name.empty()) return;

    std::string temp_str;
    std::stringstream ss(name);
    std::vector<std::string> temp;
    while (!ss.eof()) {
        ss >> temp_str;
        // convert the first letter to upper case
        if (islower(temp_str[0])) temp_str[0] = toupper(temp_str[0]);
        // convert everything else to lower letter
        for (unsigned i = 1; i < temp_str.length(); i++) {
            if (isupper(temp_str[i])) temp_str[i] = tolower(temp_str[i]);
        }
        temp.push_back(temp_str);
    }

    name.clear();

    // get the filtered name
    for (unsigned i = 0; i < temp.size(); i++) {
        name = name + ' ';
        name = name + temp[i];
    }
    // erase the first space
    name.erase(0, 1);

}

// For parsing user input for poi name

void parse_poi_input_name(std::string& name) {

    if (name.empty()) return;

    std::string temp_name;
    std::stringstream ss(name);
    std::vector<std::string> temp;
    while (!ss.eof()) {
        ss >> temp_name;
        temp.push_back(temp_name);
    }
    name.clear();

    // get the filtered name
    for (unsigned i = 0; i < temp.size(); i++) {
        name = name + ' ';
        name = name + temp[i];
    }
    // erase the first space
    name.erase(0, 1);
}

// For getting two street names from user input

void parse_input(std::size_t i, std::string& street_name1, std::string& street_name2) {

    // erase all the space at front
    while (entered_text[0] == ' ')
        entered_text.erase(0, 1);

    // erase all the space before &  
    i = entered_text.find('&');
    i = i - 1;
    while (entered_text.at(i) == ' ') {
        entered_text.erase(i, 1);
        i = i - 1;
    }

    // erase all space at the end
    while (entered_text.back() == ' ')
        entered_text.erase(entered_text.length() - 1, 1);

    // erase all space after &
    i = entered_text.find('&');
    if (entered_text.length() > i + 1) {
        i = i + 1;
        while (entered_text.at(i) == ' ' && i < entered_text.length())
            entered_text.erase(i, 1);
    }

    // get the two street names that user inputs
    i = entered_text.find('&');
    street_name1 = entered_text.substr(0, i);
    if (entered_text.length() > i + 1)
        street_name2 = entered_text.substr(i + 1, entered_text.length() - i - 1);
    else
        street_name2 = "";

    filter_minor_input_errors(street_name1);
    filter_minor_input_errors(street_name2);

}

void parse_input_route_first(std::size_t i, std::string& first_street_name1, std::string& first_street_name2) {

    // erase all the space at front
    while (entered_start[0] == ' ')
        entered_start.erase(0, 1);

    // erase all the space before &  
    i = entered_start.find('&');
    i = i - 1;
    while (entered_start.at(i) == ' ') {
        entered_start.erase(i, 1);
        i = i - 1;
    }

    // erase all space at the end
    while (entered_start.back() == ' ')
        entered_start.erase(entered_start.length() - 1, 1);

    // erase all space after &
    i = entered_start.find('&');
    if (entered_start.length() > i + 1) {
        i = i + 1;
        while (entered_start.at(i) == ' ' && i < entered_start.length())
            entered_start.erase(i, 1);
    }

    // get the two street names that user inputs
    i = entered_start.find('&');
    first_street_name1 = entered_start.substr(0, i);
    if (entered_start.length() > i + 1)
        first_street_name2 = entered_start.substr(i + 1, entered_start.length() - i - 1);
    else
        first_street_name2 = "";

    filter_minor_input_errors(first_street_name1);
    filter_minor_input_errors(first_street_name2);
}

void parse_input_route_second(std::size_t i, std::string& second_street_name1, std::string& second_street_name2) {

    // erase all the space at front
    while (entered_end[0] == ' ')
        entered_end.erase(0, 1);

    // erase all the space before &  
    i = entered_end.find('&');
    i = i - 1;
    while (entered_end.at(i) == ' ') {
        entered_end.erase(i, 1);
        i = i - 1;
    }

    // erase all space at the end
    while (entered_end.back() == ' ')
        entered_end.erase(entered_end.length() - 1, 1);

    // erase all space after &
    i = entered_end.find('&');
    if (entered_end.length() > i + 1) {
        i = i + 1;
        while (entered_end.at(i) == ' ' && i < entered_end.length())
            entered_end.erase(i, 1);
    }

    // get the two street names that user inputs
    i = entered_end.find('&');
    second_street_name1 = entered_end.substr(0, i);
    if (entered_end.length() > i + 1)
        second_street_name2 = entered_end.substr(i + 1, entered_end.length() - i - 1);
    else
        second_street_name2 = "";

    filter_minor_input_errors(second_street_name1);
    filter_minor_input_errors(second_street_name2);
}

// For finding the intersection that the user wants to find,
// If not found, gives invalid input notice for user

void intersection_find() {

    std::string street_name1, street_name2;

    // erase all space at the beginning of the input
    while (entered_text[0] == ' ')
        entered_text.erase(0, 1);

    std::size_t i = entered_text.find('&');
    if (i != std::string::npos && i != 0) {

        parse_input(i, street_name1, street_name2);

        // find all the intersection from the two street names
        std::vector<unsigned> intersection_ids = find_intersection_ids_from_street_names(street_name1, street_name2);

        // if not found
        if (intersection_ids.size() == 0)
            draw_intersection_not_found();
        else {
            // if found
            draw_found_intersections(intersection_ids);
            need_to_clear_searched_intersections = true;
        }
    }// if invalid input
    else draw_invalid_input();
}

// For finding the poi that the user wants to find,
// If not found, gives invalid input notice for user

void poi_find() {

    // erase all space at the beginning of the input
    while (entered_text[0] == ' ')
        entered_text.erase(0, 1);

    std::string temp_str = entered_text;

    // if the user enters nothing
    if (entered_text.empty())
        draw_invalid_input();

    else {
        parse_poi_input_name(temp_str);
        if (!(my_map -> get_poi_names_TST() -> isWord(temp_str)))
            draw_poi_not_found();
        else {
            draw_found_poi(temp_str);
            need_to_clear_searched_poi = true;
        }
    }
}

// For checking the first input from the user to find route
bool check_route_first_input(std::vector<unsigned>& first_intersection_ids) {

    std::string first_street_name1, first_street_name2;

    // erase all space at the beginning of the input
    while (entered_start[0] == ' ')
        entered_start.erase(0, 1);

    std::size_t i = entered_start.find('&');
    if (i != std::string::npos && i != 0) {

        parse_input_route_first(i, first_street_name1, first_street_name2);

        // find all the intersection from the two street names
        first_intersection_ids = find_intersection_ids_from_street_names(first_street_name1, first_street_name2);

        // if not found
        if (first_intersection_ids.size() == 0) {
            wrong_input_route = true;
            draw_start_point_not_found(); // Sorry, start point not found
            return false;
        }
    }// if invalid input
    else {
        draw_invalid_first_input(); // please enter an intersection as a start point
        wrong_input_route = true;
        return false;
    }
    return true;
}

// For checking the second input from the user to find route
bool check_route_second_input(std::string& temp_str, std::vector<unsigned>& second_intersection_ids) {

    std::string second_street_name1, second_street_name2;
    // Check Second input
    // erase all space at the beginning of the input
    while (entered_end[0] == ' ')
        entered_end.erase(0, 1);

    std::size_t i = entered_end.find('&');

    if (i != std::string::npos) {
        isPOI = false;

        if (i != 0) {

            parse_input_route_second(i, second_street_name1, second_street_name2);

            // find all the intersection from the two street names
            second_intersection_ids = find_intersection_ids_from_street_names(second_street_name1, second_street_name2);

            // if not found
            if (second_intersection_ids.size() == 0) {
                draw_end_point_not_found(); //Sorry, end point not found
                wrong_input_route = true;
                return false;
            }
        }
    } else {

        isPOI = true;
        temp_str = entered_end;

        // if the user enters nothing
        if (entered_end.empty()) {
            draw_invalid_second_input(); // please enter an intersection/POI as an end point
            wrong_input_route = true;
            return false;
        } else {
            if (temp_str.empty()) {
                draw_end_point_not_found(); //Sorry, end point not found
                wrong_input_route = true;
            } else {
                parse_poi_input_name(temp_str);
                if (!(my_map -> get_poi_names_TST() -> isWord(temp_str))) {
                    draw_end_point_not_found(); //Sorry, end point not found
                    wrong_input_route = true;
                    return false;
                }
            }
        }
    }
    return true;
}

// For find the route of two intersections or intersection - POI
void route_find() {

    has_entered_first = false;
    has_entered_second = false;
    std::vector<unsigned> first_intersection_ids;
    std::vector<unsigned> second_intersection_ids;
    std::string temp_str;

    if (check_route_first_input(first_intersection_ids)) {
        if (check_route_second_input(temp_str, second_intersection_ids)) {
            if (isPOI) {
                found_path = find_path_to_point_of_interest(first_intersection_ids[0], temp_str, 15);
                route_first_intersection = first_intersection_ids[0];
                if (!found_path.empty()) route_second_intersection = getStreetSegmentInfo(found_path.back()).to;
                draw_path(found_path);
            } else {
                found_path = find_path_between_intersections(first_intersection_ids[0], second_intersection_ids[0], 15);
                route_first_intersection = first_intersection_ids[0];
                route_second_intersection = second_intersection_ids[0];
                draw_path(found_path);
            }
        }
    }
}

// When user clicks on an intersection

void act_on_click_intersection(float x, float y) {

    if (has_clicked_second_Intersection) {
        found_path.clear();
        draw_screen();
    }
    // level of detail, determine what radius
    radius = 0.00001;
    if (zoom == 5) radius = 0.00001;
    else if (zoom == 6) radius = 0.0000065;
    else if (zoom == 4) radius = 0.000017;
    else if (zoom == 3) radius = 0.000025;
    else if (zoom == 7) radius = 0.000004;
    else if (zoom == 2) radius = 0.000038;
    else if (zoom == 1) radius = 0.000055;
    else if (zoom == 8) radius = 0.000003;
    else if (zoom == 0) radius = 0.0001;
    else if (zoom > 8) radius = 0.0000015;

    // convert the clicked location into LatLon and find the closest intersection
    auto const lon = cartesian_to_longitude(x, avg_lat);
    auto const lat = cartesian_to_latitude(y);

    LatLon position(lat, lon);
    auto const id = find_closest_intersection(position);

    // find the x center and y center of the closest intersection
    auto const xc = longitude_to_cartesian(my_map -> get_intersections()[id].position.lon(), avg_lat);
    auto const yc = latitude_to_cartesian(my_map -> get_intersections()[id].position.lat());

    // check whether the clicked location is within the box of that intersection
    if (x >= (xc - radius) && x <= (xc + radius) && y >= (yc - radius) && y <= (yc + radius)) {
        if (!has_clicked_first_Intersection) {
            // level of detail, determine the radius to draw the highlight intersection
            radius = 0.00001;
            if (zoom == 5) radius = 0.00001;
            else if (zoom == 6) radius = 0.0000065;
            else if (zoom == 4) radius = 0.000017;
            else if (zoom == 3) radius = 0.000025;
            else if (zoom == 7) radius = 0.000004;
            else if (zoom == 2) radius = 0.000038;
            else if (zoom == 1) radius = 0.000055;
            else if (zoom == 8) radius = 0.000003;
            else if (zoom == 0) radius = 0.0001;
            else if (zoom > 8) radius = 0.0000015;

            // Draw the start intersection
            Surface pic = load_png_from_file("libstreetmap/resources/start_3.png");
            if (zoom == 0) draw_surface(pic, xc - 1.15 * radius, yc + 2.2 * radius);
            else if (zoom == 1) draw_surface(pic, xc - 0.9 * radius, yc + 1.8 * radius);
            else if (zoom == 2) draw_surface(pic, xc - 1.1 * radius, yc + 2 * radius);
            else if (zoom == 3) draw_surface(pic, xc - radius, yc + 2 * radius);
            else if (zoom == 4) draw_surface(pic, xc - 0.95 * radius, yc + 1.95 * radius);
            else if (zoom == 5) draw_surface(pic, xc - radius, yc + 2 * radius);
            else if (zoom == 6) draw_surface(pic, xc - 0.95 * radius, yc + 1.9 * radius);
            else if (zoom == 7) draw_surface(pic, xc - 0.95 * radius, yc + 1.9 * radius);
            else if (zoom == 8) draw_surface(pic, xc - 0.65 * radius, yc + 1.5 * radius);
            else if (zoom > 8) draw_surface(pic, xc - 0.8 * radius, yc + 2 * radius);

            draw_intersection_info(my_map -> get_intersections()[id].name);
            // save state for current value
            has_clicked_first_Intersection = true;
            has_clicked_second_Intersection = false;
            intersection_first_clicked_on = my_map -> get_intersections()[id].position;

            first_intersection = id;
        } else {
            if (first_intersection != id) {
                intersection_second_clicked_on = my_map -> get_intersections()[id].position;
                if (!found_path.empty()) {
                    found_path.clear();
                    draw_screen();
                }
                found_path = find_path_between_intersections(first_intersection, id, 15);
                route_first_intersection = first_intersection;
                route_second_intersection = id;
                has_clicked_second_Intersection = true;
                draw_path(found_path);
                draw_intersection_info(my_map -> get_intersections()[id].name);


            }
        }

    }// clear the screen if user clicks on somewhere else
    else if (has_clicked_first_Intersection)
        draw_screen();
}

// When user clicks on the auto complete location

void act_on_click_auto_complete(float x, float y) {

    // get the location of the auto complete box
    t_bound_box box = get_visible_world();
    float height = box.top() - box.bottom();
    float width = box.right() - box.left();

    float room_reserved = 0.008 * width;
    float x_length = 0.28 * width;

    float x_left = box.left() + room_reserved;
    float y_top = box.top() - room_reserved;
    float x_right = x_left + x_length;
    float y_bottom = y_top - 0.07 * height;

    std::vector<std::string> temp;

    // get the location of the box
    for (unsigned i = 0; i < auto_names.size(); i++) {
        find_box_below(y_bottom, y_top);

        // compare the location that the user clicks on to the location of the box
        if (x >= x_left && x <= x_right && y >= y_bottom && y <= y_top) {
            entered_text = auto_names[i];
            temp = auto_names;
            draw_search_bar();
            auto_names = temp;
            draw_box_below();
            return;
        }
    }
}

void act_on_click_auto_complete_help(float x, float y) {

    // For finding the location of the search bar
    t_bound_box box = get_visible_world();
    float height = box.top() - box.bottom();
    float width = box.right() - box.left();

    float x_length = 0.30 * width;
    float y_length = 0.04 * height;

    float x_left = box.right() - x_length - 0.078 * width;
    float y_top = box.top() - height + 0.27 * height;
    float x_right = box.right() - 0.078 * width;
    float y_bottom = y_top - y_length;

    std::vector<std::string> temp;

    // get the location of the box
    for (unsigned i = 0; i < auto_names.size(); i++) {
        find_box_below(y_bottom, y_top);

        // compare the location that the user clicks on to the location of the box
        if (x >= x_left && x <= x_right && y >= y_bottom && y <= y_top) {
            entered_help_command = auto_names[i];
            temp = auto_names;
            draw_help_box();
            draw_help_box_text();
            auto_names = temp;
            draw_help_box_below();
            return;
        }
    }
}

void act_on_click_auto_complete_find_route(float x, float y) {

    // For finding the location of the search bar
    t_bound_box box = get_visible_world();
    float height = box.top() - box.bottom();
    float width = box.right() - box.left();
    float x_length = 0.26 * width;

    float x_left = box.left();
    float x_right = x_left + x_length;
    float y_bottom = box.top() - 0.2 * height;
    float y_top = box.top() - 0.75 * (box.top() - y_bottom);


    std::vector<std::string> temp;

    // get the location of the box
    for (unsigned i = 0; i < auto_names.size(); i++) {
        find_box_below(y_bottom, y_top);

        // compare the location that the user clicks on to the location of the box
        if (x >= x_left && x <= x_right && y >= y_bottom && y <= y_top) {
            if (!has_entered_first) {
                entered_start = auto_names[i];
                temp = auto_names;
                draw_find_route_search_bar();
                auto_names = temp;
                draw_find_route_box_below();
                return;
            } else if (!has_entered_second) {
                entered_end = auto_names[i];
                temp = auto_names;
                draw_find_route_search_bar();
                auto_names = temp;
                draw_find_route_box_below();
                return;
            }
        }
    }
}

void act_on_click_flip_page(float x, float y) {

    t_bound_box box = get_visible_world();


    float height = box.top() - box.bottom();
    float width = box.right() - box.left();

    float x_length = 0.26 * width;

    float x_left = box.left();
    float y_bottom = box.bottom();

    float x_next_left = x_left + 0.18 * width;
    float y_next_top = box.bottom() + 0.04 * height;

    float x_next_right = x_next_left + 0.091 * x_length;
    float y_next_bottom = y_bottom + height * 0.01;

    float x_prev_left = box.left() + 0.011 * width;
    float y_prev_top = box.bottom() + 0.04 * height;

    float x_prev_right = x_prev_left + 0.091 * x_length;
    float y_prev_bottom = y_bottom + height * 0.01;


    if (x >= x_next_left && x <= x_next_right && y >= y_next_bottom && y <= y_next_top) {
        curr_page++;
        draw_navigation_box();
    } else if (x >= x_prev_left && x <= x_prev_right && y >= y_prev_bottom && y <= y_prev_top) {
        curr_page--;
        draw_navigation_box();
    }

}

// When user presses Find and enter from keyboard

void key_press_for_intersection(char key_pressed, int keysym) {

    if (entered_text == "Eg. College Street & Bay Street")
        entered_text = "";

    // Check whether to clear the screen after user has searched a intersection
    if (need_to_clear_searched_intersections) {
        draw_screen();
        has_clicked_find = true;
        draw_search_bar();
        need_to_clear_searched_intersections = false;
    }
    // For different user input 
    switch (keysym) {
        case XK_BackSpace:
            if (entered_text.length() != 0) {
                entered_text.pop_back();
                draw_search_bar();
            } else {
                entered_text = "Eg. College Street & Bay Street";
                has_clicked_find = true;
                draw_search_bar();
            }
            break;

        case XK_Shift_L:
            break;

        case XK_Shift_R:
            break;

        case XK_Caps_Lock:
            break;

        case XK_Return:
            intersection_find();
            has_clicked_find = true;
            entered_text = "Eg. College Street & Bay Street";
            draw_search_bar();
            break;

        default:
            if (has_clicked_find) {
                entered_text += key_pressed;
                break;
            }
    }

    // after user input, draw search bar
    if (has_clicked_find) draw_search_bar();
}

// When user presses Find POI and enter from keyboard

void key_press_for_poi(char key_pressed, int keysym) {

    if (entered_text == "Eg. Eaton Centre")
        entered_text = "";

    // Check whether to clear the screen after user has searched a intersection
    if (need_to_clear_searched_poi) {
        draw_screen();
        has_clicked_find_POI = true;
        draw_search_bar();
        need_to_clear_searched_poi = false;
    }
    // For different user input 
    switch (keysym) {
        case XK_BackSpace:
            if (entered_text.length() != 0) {
                entered_text.pop_back();
                draw_search_bar();
            } else {
                entered_text = "Eg. Eaton Centre";
                has_clicked_find_POI = true;
                draw_search_bar();
            }
            break;

        case XK_Shift_L:
            break;

        case XK_Shift_R:
            break;

        case XK_Caps_Lock:
            break;

        case XK_Return:
            poi_find();
            has_clicked_find_POI = true;
            entered_text = "Eg. Eaton Centre";
            draw_search_bar();
            break;

        default:
            if (has_clicked_find_POI) {
                entered_text += key_pressed;
                break;
            }
    }

    // after user input, draw search bar
    if (has_clicked_find_POI) draw_search_bar();
}

// When user presses Find route and enter from keyboard
void key_press_for_entering_first(char key_pressed, int keysym) {

    if (entered_start == "Start Point")
        entered_start = "";

    // For different user input 
    switch (keysym) {
        case XK_BackSpace:
            if (entered_start.length() != 0) {
                entered_start.pop_back();
                draw_find_route_search_bar();
            } else {
                entered_start = "Start Point";
                //has_clicked_find_route = true;
                draw_find_route_search_bar();
            }
            break;

        case XK_Shift_L:
            break;

        case XK_Shift_R:
            break;

        case XK_Caps_Lock:
            break;

        case XK_Return:
            if (has_entered_second) {
                if (has_drawn_path || wrong_input_route || (has_clicked_first_Intersection && has_clicked_second_Intersection)) {
                    found_path.clear();
                    draw_screen();
                }

                route_find();
                entered_start = "Start Point";
                entered_end = "End Point";
                draw_find_route_search_bar();
                has_clicked_find_route = true;
            } else
                has_entered_first = true;
            break;

        default:
            if (has_clicked_find_route) {
                entered_start += key_pressed;
                break;
            }
    }

    // after user input, draw search bar
    if (has_clicked_find_route) draw_find_route_search_bar();


}

// When user presses Find route and enter from keyboard
void key_press_for_entering_second(char key_pressed, int keysym) {

    if (entered_end == "End Point")
        entered_end = "";

    // For different user input 
    switch (keysym) {
        case XK_BackSpace:
            if (entered_end.length() != 0) {
                entered_end.pop_back();
                draw_find_route_search_bar();
            } else {
                entered_end = "End Point";
                //has_clicked_find_route = true;
                draw_find_route_search_bar();
            }
            break;

        case XK_Shift_L:
            break;

        case XK_Shift_R:
            break;

        case XK_Caps_Lock:
            break;

        case XK_Return:
            if (has_entered_first) {
                if (has_drawn_path || wrong_input_route || (has_clicked_first_Intersection && has_clicked_second_Intersection)) {
                    found_path.clear();
                    draw_screen();
                }
                route_find();
                entered_start = "Start Point";
                entered_end = "End Point";
                draw_find_route_search_bar();
                has_clicked_find_route = true;
            } else
                has_entered_second = true;
            break;

        default:
            if (has_clicked_find_route) {
                entered_end += key_pressed;
                break;
            }
    }

    // after user input, draw search bar
    if (has_clicked_find_route) draw_find_route_search_bar();
}

// When user presses Find Route and enter from keyboard

void key_press_for_find_route(char key_pressed, int keysym) {

    if (!has_entered_first) key_press_for_entering_first(key_pressed, keysym);
    else if (!has_entered_second) key_press_for_entering_second(key_pressed, keysym);

}

// When user presses help and enter from keyboard

void key_press_help_command(char key_pressed, int keysym) {

    if (entered_help_command == "Eg. Find")
        entered_help_command = "";

    // For different user input 
    switch (keysym) {
        case XK_BackSpace:
            if (entered_help_command.length() != 0) {
                entered_help_command.pop_back();
                draw_help_box();
                draw_help_box_text();
            } else {
                entered_help_command = "Eg. Find";
                help_button_has_clicked = true;
                draw_help_box();
                draw_help_box_text();
                draw_start_help_text();
                return;
            }
            break;

        case XK_Shift_L:
            break;

        case XK_Shift_R:
            break;

        case XK_Caps_Lock:
            break;

        case XK_Return:

            if (entered_help_command == "Dinning") {

                //draw_screen();
                draw_help_box();
                draw_restaurent_help_text();
            } else if (entered_help_command == "Find") {

                //draw_screen();
                draw_help_box();
                draw_find_help_text();
            } else if (entered_help_command == "Find POI") {
                //draw_screen();
                draw_help_box();
                draw_findPOI_help_text();
            } else if (entered_help_command == "University") {
                //draw_screen();
                draw_help_box();
                draw_university_help_text();
            } else if (entered_help_command == "Hospital") {
                //draw_screen();
                draw_help_box();
                draw__hospital_help_text();
            } else if (entered_help_command == "Bank") {
                //draw_screen();
                draw_help_box();
                draw_bank_help_text();
            } else if (entered_help_command == "All POI") {
                //draw_screen();
                draw_help_box();
                draw_all_poi_help_text();
            } else if (entered_help_command == "FeaName") {
                //draw_screen();
                draw_help_box();
                draw_feaName_help_text();
            } else if (entered_help_command == "ALL") {
                draw_screen();
                draw_all_help_box();
            } else if (entered_help_command == "Find Route") {
                //draw_screen();
                draw_help_box();
                draw_findRout_help_text();

            } else if (entered_help_command == "Navigation") {
                //draw_screen();
                draw_help_box();
                draw_NaviOn_help_text();

            }
            entered_help_command = "Eg. Find";
            help_button_has_clicked = true;

            return;

        default:
            if (help_button_has_clicked) {
                entered_help_command += key_pressed;
                break;
            }
    }

    // after user input, draw search bar
    if (help_button_has_clicked) {
        draw_help_box();
        draw_help_box_text();
    }
}

// Function to create new buttons

void create_new_buttons() {

    // Create buttons for find
    create_button("Zoom Fit", "Find", find_intersections);
    create_button("Find", "Find POI", find_poi);
    create_button("Find POI", "Find Route", find_route);
    // Create other buttons for drawing poi names, icons, and feature names
//    enable_or_disable_button(0, true);
//    create_button("Find Route", "University On", button_func_uni_off);
//    enable_or_disable_button(1, true);
//    create_button("University On", "Hospital On", button_func_hospital_off);
//    enable_or_disable_button(2, true);
//    create_button("Hospital On", "Bank On", button_func_bank_off);
//    enable_or_disable_button(3, true);
//    create_button("Bank On", "Dinning On", button_func_dinning_off);
//    enable_or_disable_button(4, true);
//    create_button("Dinning On", "Gas On", button_func_gas_off);
//    enable_or_disable_button(5, true);
//    create_button("Gas On", "Parking On", button_func_parking_off);
//    enable_or_disable_button(6, true);
//    create_button("Parking On", "All POI On", button_func_all_off);
//    enable_or_disable_button(7, true);
    //enable_or_disable_button(8, true);
    create_button("Find Route", "Navigation", button_navigation);
    create_button("Navigation", "All POI On", button_func_all_off);
    //enable_or_disable_button (9, true); 
    create_button("All POI On", "Help", help_button);
    //create_button("Help", "Clear", clear_screen);

}

// Function to get zoom level of the current world

void get_zoom_level() {

    //determine zoom level
    zoom_level = (get_visible_world().top() - get_visible_world().bottom()) /
            (fabs(world.top - world.bottom));
    zoom = 0;
    //Different levels for zooming
    if (zoom_level > 0.61)
        zoom = 0;
    else if (zoom_level > 0.361 && zoom_level < 0.61)
        zoom = 1;
    else if (zoom_level > 0.22 && zoom_level < 0.361)
        zoom = 2;
    else if (zoom_level > 0.13 && zoom_level < 0.22)
        zoom = 3;
    else if (zoom_level > 0.078 && zoom_level < 0.13)
        zoom = 4;
    else if (zoom_level > 0.047 && zoom_level < 0.078)
        zoom = 5;
    else if (zoom_level > 0.029 && zoom_level < 0.047)
        zoom = 6;
    else if (zoom_level > 0.017 && zoom_level < 0.029)
        zoom = 7;
    else if (zoom_level > 0.011 && zoom_level < 0.017)
        zoom = 8;
    else if (zoom_level > 0.0061 && zoom_level < 0.011)
        zoom = 9;
    else if (zoom_level < 0.0061)
        zoom = 10;

}

// Function to get world coordinate in latlon

void get_world_latlon(double& min_lat, double& max_lat, double& min_lon, double& max_lon) {

    max_lat = getIntersectionPosition(0).lat();
    min_lat = max_lat;
    max_lon = getIntersectionPosition(0).lon();
    min_lon = max_lon;

    std::vector<intersection_data> intersections = my_map -> get_intersections();
    for (unsigned id = 0; id < getNumberOfIntersections(); ++id) {

        max_lat = std::max(max_lat, intersections[id].position.lat());
        min_lat = std::min(min_lat, intersections[id].position.lat());

        max_lon = std::max(max_lon, intersections[id].position.lon());
        min_lon = std::min(min_lon, intersections[id].position.lon());
    }

    avg_lat = cos(0.5 * (max_lat + min_lat) * DEG_TO_RADIAN);
}


/*************** Call back function for draw_map ********************/
// After user presses any key
void act_on_key_press(char key_pressed, int keysym) {
    if (has_clicked_find)
        key_press_for_intersection(key_pressed, keysym);
    else if (help_button_has_clicked)
        key_press_help_command(key_pressed, keysym);
    else if (has_clicked_find_POI)
        key_press_for_poi(key_pressed, keysym);
    else if (has_clicked_find_route)
        key_press_for_find_route(key_pressed, keysym);
}

// Whenever user clicks on the map, come to this function

void act_on_button_press(float x, float y, t_event_buttonPressed event) {
    (void) event;
    if (has_clicked_find || has_clicked_find_POI) act_on_click_auto_complete(x, y);
    else if (help_button_has_clicked) act_on_click_auto_complete_help(x, y);
    else if (has_clicked_find_route) act_on_click_auto_complete_find_route(x, y);
    else if (has_clicked_navigation) act_on_click_flip_page(x, y);
    else if (!has_clicked_find && !has_clicked_find_POI && !help_button_has_clicked && !has_clicked_find_route) act_on_click_intersection(x, y);
}

void draw_screen() {

    get_zoom_level();
    // reset the save state
    has_clicked_first_Intersection = false;
    has_clicked_find = false;
    has_clicked_find_POI = false;
    has_clicked_find_route = false;
    help_button_has_clicked = false;
    has_clicked_navigation = false;
    wrong_input_route = false;

    // drawing everything
    set_drawing_buffer(OFF_SCREEN);

    clearscreen();
    set_keypress_input(true);

    draw_all_features();
    draw_street_segments();

//    std::vector<DeliveryInfo> deliveries;
//    std::vector<unsigned> depots;
//    deliveries = {DeliveryInfo(90183, 84955), DeliveryInfo(57973, 86285), DeliveryInfo(57973, 63719), DeliveryInfo(99338, 85306), DeliveryInfo(1024, 16646), DeliveryInfo(86476, 62274), DeliveryInfo(57973, 103682), DeliveryInfo(90183, 16062)};
//    depots = {81982, 75396, 34731};
//    found_path = traveling_courier_2(deliveries, depots, 15);
    
    
    //draw_path(result_path);

    // draw found path at each level
    if (!found_path.empty()) draw_TSP_path(found_path);
    
//    get_zoom_level();
//    radius = 0.00001;
//    if (zoom == 5) radius = 0.00001;
//    else if (zoom == 6) radius = 0.0000065;
//    else if (zoom == 4) radius = 0.000017;
//    else if (zoom == 3) radius = 0.000025;
//    else if (zoom == 7) radius = 0.000004;
//    else if (zoom == 2) radius = 0.000038;
//    else if (zoom == 1) radius = 0.000055;
//    else if (zoom == 8) radius = 0.000003;
//    else if (zoom == 0) radius = 0.0001;
//    else if (zoom > 8) radius = 0.0000015;
//
//    for (unsigned i = 0; i < deliveries.size(); i++) {
//        setcolor(RED);
//        float x = longitude_to_cartesian(my_map -> get_intersections()[deliveries[i].pickUp].position.lon(), avg_lat);
//        float y = latitude_to_cartesian(my_map -> get_intersections()[deliveries[i].pickUp].position.lat());
//        fillarc(x, y, radius, 0, 360);          
//        x = longitude_to_cartesian(my_map -> get_intersections()[deliveries[i].dropOff].position.lon(), avg_lat);
//        y = latitude_to_cartesian(my_map -> get_intersections()[deliveries[i].dropOff].position.lat());
//        fillarc(x, y, radius, 0, 360);     
//    }
    
    // draw different features for different levels
    if (zoom >= 5) {
        draw_all_feature_name();
        draw_intersections();
        draw_poi();
    }

    copy_off_screen_buffer_to_screen();
    set_drawing_buffer(ON_SCREEN);

}

void draw_map() {
       
//    deliveries = {DeliveryInfo(102343, 40664), DeliveryInfo(66501, 33150), DeliveryInfo(64220, 70192), DeliveryInfo(10143, 89556), DeliveryInfo(51798, 85214)};
//    depots = {14526, 21436, 41416};
//    result_path = traveling_courier(deliveries, depots, 15);
//    
//    draw_path(result_path);
    // get the world coordinates and initialize the map
    double min_lat, max_lat, min_lon, max_lon;

    get_world_latlon(min_lat, max_lat, min_lon, max_lon);

    init_graphics("GOGOMAP", t_color(232, 232, 238));

    // get the world coordinates to initialize the world
    world.bottom = latitude_to_cartesian(min_lat);
    world.top = latitude_to_cartesian(max_lat);
    world.left = longitude_to_cartesian(min_lon, avg_lat);
    world.right = longitude_to_cartesian(max_lon, avg_lat);

    set_visible_world(world.left, world.bottom, world.right, world.top);

    set_keypress_input(true);
    set_mouse_move_input(true);
    // create new buttons
    destroy_button("Window");
    destroy_button("PostScript");
    create_new_buttons();

    update_message("ECE297 Mapper Project - cd008");
    // handing control to easygl
    event_loop(act_on_button_press, nullptr, act_on_key_press, draw_screen);

    close_graphics();
}

// return a string for direction
string set_direction(double angle1, double angle2) {
    double diff = angle2 - angle1;
    if(diff < 0) diff += 360;
    
    if (diff < 20 || diff > 340) return "Keep Straight";
    else if (diff >= 20 && diff < 170) return "Turn Right";
    else if (diff > 190 && diff <= 340) return "Turn Left";
    
    return "Make a Turn";
    return "make a U turn";
}
// get an angle depending on start point and id
void calculate_angle(unsigned id, unsigned start_pt, double& angle) {

    
    unsigned end_pts;
    if (getStreetSegmentInfo(id).to == start_pt) end_pts = getStreetSegmentInfo(id).from;
    else end_pts = getStreetSegmentInfo(id).to;
    
    // get info for both points
    LatLon start_info = getIntersectionPosition(start_pt);
    LatLon end_info = getIntersectionPosition(end_pts);
    float y = end_info.lat() - start_info.lat();
    float x = end_info.lon() - start_info.lon();
    angle = atan(x / y) * 180 / PI;

    // get angle from start point to end point
    if (y <= 0 && x <= 0) angle = angle - 180;
     
    if (y <= 0 && x >= 0) angle = angle + 180;

    if (angle <= 0) angle = angle + 360;
}

// get the start, end vector
void find_start_end(unsigned id1, unsigned id2, std::vector<unsigned>& start_end) {

    StreetSegmentInfo info1 = getStreetSegmentInfo(id1);
    StreetSegmentInfo info2 = getStreetSegmentInfo(id2);
    
    // compare two points and store into the vector start_end
    if (info1.from == info2.from || info1.from == info2.to) {
        start_end.push_back(info1.to);
        start_end.push_back(info1.from);

    }
    if (info1.to == info2.from || info1.to == info2.to) {
        start_end.push_back(info1.from);
        start_end.push_back(info1.to);
    }
}

// Function to draw the navigation box on the left of the screen
void design_navigation_box() {

    // reset state for route
    route.clear();
    page_num = 0;
    curr_page = 0;
    
    unsigned count = 0;
    unsigned size = found_path.size();

    std::vector<navi_info>page_single;

    if (size == 0) {
        page_num = -1;
        return;
    } 

    // Set the first previous to the first element of the found path
    navi_info prev;
    unsigned prev_street_seg_id = found_path[0];
    StreetSegmentInfo first = getStreetSegmentInfo(found_path[0]);
    prev.street_id = first.streetID;
    prev.street_name = getStreetName(first.streetID);
    prev.length = find_street_segment_length(found_path[0]);

    // Loop through the path size, and get the route vector which stores each page of paths, directions
    for (unsigned i = 1; i < size; i++) {
        if (count >= 5) {
            page_num++;
            count = 0;
            route.push_back(page_single);
            page_single.clear();
        }

        // get the current page information
        StreetSegmentInfo curr_seg = getStreetSegmentInfo(found_path[i]);
        navi_info temp;
        temp.street_id = curr_seg.streetID;
        temp.street_name = getStreetName(curr_seg.streetID);
        temp.length = find_street_segment_length(found_path[i]);
    
        if (prev.street_name != temp.street_name) {
            count ++;
            std::vector<unsigned> start_end;
            double angle1, angle2;

            find_start_end(prev_street_seg_id, found_path[i], start_end);
            calculate_angle(prev_street_seg_id, start_end[0], angle1);
            calculate_angle(found_path[i], start_end[1], angle2);
            
            // reset the previous 
            prev.direction = set_direction(angle1, angle2);
            page_single.push_back(prev);
            prev.street_name = temp.street_name;
            prev.length = temp.length;
            prev.street_id = temp.street_id;           
        }
        
        else if (prev.street_name == temp.street_name) {
            prev.length = prev.length + temp.length;
        } 

        prev_street_seg_id = found_path[i];
    }
    
    // check if there's only one page
    if (page_num == 0) {
        page_num ++;
        route.push_back(page_single);
    }

    // store end point
    if (!found_path.empty()) {
        navi_info end;
        end.street_name = getIntersectionName(getStreetSegmentInfo(found_path.back()).to);
        //end.direction = prev.direction;
        if (!route.empty()) {
            if (route.back().size() < 5) {
                route.back().push_back(end);
            }
            else {
                page_single.clear();
                page_num++;
                page_single.push_back(end);
                route.push_back(page_single);
            }
        }
    }

    return;
}

// Draw the navigation window(Background)
void draw_navigation_box() {
    
    // Get location of drawing
    t_bound_box box = get_visible_world();

    float height = box.top() - box.bottom();
    float width = box.right() - box.left();

    float x_length = 0.26 * width;

    float x_left = box.left();
    float y_top = box.top() - 0.2 * height;
    float x_right = x_left + x_length;
    float y_bottom = box.bottom();
    setcolor(WHITE);
    fillrect(x_left, y_bottom, x_right, y_top);
    setlinewidth(2);
    drawrect(x_left, y_bottom, x_right, y_top);

    float x = x_left + 0.01 * width;
    float y = y_top - 0.01 * height;
    // draw the compass picture
    Surface pic = load_png_from_file("libstreetmap/resources/Compass.png");
    draw_surface(pic, x, y);

    t_bound_box background(x_left, y_top - 0.05 * height, x_right, y_bottom);
    setcolor(WHITE);
    fillrect(background);

    // depending on the page num, draw icons for flipping pages
    if (page_num > 1) {
        draw_surface(load_png_from_file("libstreetmap/resources/turnright.png"),
                box.left() + 0.19 * width, box.bottom() + 0.04 * height);
        next_page_icon = t_bound_box(x_left + 0.18 * width, y_bottom,
                x_right, y_bottom + 0.04 * height);


        draw_surface(load_png_from_file("libstreetmap/resources/turnleft.png"),
                box.left() + 0.012 * width, box.bottom() + 0.04 * height);
        prev_page_icon = t_bound_box(x_left + 0.010 * width, y_bottom + height * 0.01,
                x_left + width * 0.028, y_bottom + 0.04 * height);
    }


    draw_navigation_box_text();
    return;
}

// draw instruction for each street segments
void draw_navigation_box_text() {
    
    if (page_num != -1) {
        // let user flip between pages
        if (curr_page >= page_num) curr_page = 0;
        else if (curr_page < 0) curr_page = page_num - 1;
        if ((curr_page < page_num) && (curr_page >= 0)) {

            unsigned size = route[curr_page].size();
            draw_instruction_window();

            t_bound_box box = get_visible_world();

            float height = box.top() - box.bottom();
            float width = box.right() - box.left();

            float x_left = box.left() + 0.01 * width;
            float y_top = box.top() - 0.245 * height;
            float y_bottom = box.bottom() + 0.045 * height;

            double row = (y_top - y_bottom) / 6;
            
            // Loop through the route size
            for (unsigned i = 0; i < size; i++) {
                
                navi_info temp = route[curr_page][i];
                std::string street_name = temp.street_name;
                std::string direction = temp.direction;
                unsigned number = temp.length;
                std::string length = std::to_string(number);
                std::string display1 = "On " + street_name;
                if (street_name == "") display1 = "On <unknown>";
                std::string display2 = "Go " + length + " m " + "and " + direction;
                if (curr_page == page_num - 1 && i == size - 1) display2 = "Go " + length + " m " + "and Reach Destination";
                setlinewidth(7);
                settextrotation(0);
                // if it's the endpoint
                if (curr_page == page_num - 1 && i == size - 1) {
                    if (i == 0) {
                        setcolor(BLACK);
                        setfontsize(10);
                        if (display1.size() >= 30 && display1.size() < 35) setfontsize(9);
                        else if (display1.size() >= 35 && display1.size() < 40) setfontsize(8);
                        else if (display1.size() >= 40 && display1.size() < 45) setfontsize(7);
                        else if (display1.size() >= 45 && display1.size() < 50) setfontsize(6);
                        drawtext_in(navigate_display1[0], display1);
                        setcolor(BLUE);
                        setfontsize(8);
                        drawtext_in(navigate_display2[0], display2);
                        draw_navigation_direction(direction, x_left + width * 0.016, y_top - row * 0.5, true);
                    } else if (i == 1) {
                        setcolor(BLACK);
                        setfontsize(10);
                        if (display1.size() >= 30 && display1.size() < 35) setfontsize(9);
                        else if (display1.size() >= 35 && display1.size() < 40) setfontsize(8);
                        else if (display1.size() >= 40 && display1.size() < 45) setfontsize(7);
                        else if (display1.size() >= 45 && display1.size() < 50) setfontsize(6);
                        drawtext_in(navigate_display1[1], display1);
                        setcolor(BLUE);
                        setfontsize(8);
                        drawtext_in(navigate_display2[1], display2);
                        draw_navigation_direction(direction, x_left + width * 0.016, y_top - row * 1.5, true);
                    } else if (i == 2) {
                        setcolor(BLACK);
                        setfontsize(10);
                        if (display1.size() >= 30 && display1.size() < 35) setfontsize(9);
                        else if (display1.size() >= 35 && display1.size() < 40) setfontsize(8);
                        else if (display1.size() >= 40 && display1.size() < 45) setfontsize(7);
                        else if (display1.size() >= 45 && display1.size() < 50) setfontsize(6);
                        drawtext_in(navigate_display1[2], display1);
                        setcolor(BLUE);
                        setfontsize(8);
                        drawtext_in(navigate_display2[2], display2);
                        draw_navigation_direction(direction, x_left + width * 0.016, y_top - row * 2.5, true);
                    } else if (i == 3) {
                        setcolor(BLACK);
                        setfontsize(10);
                        if (display1.size() >= 30 && display1.size() < 35) setfontsize(9);
                        else if (display1.size() >= 35 && display1.size() < 40) setfontsize(8);
                        else if (display1.size() >= 40 && display1.size() < 45) setfontsize(7);
                        else if (display1.size() >= 45 && display1.size() < 50) setfontsize(6);
                        drawtext_in(navigate_display1[3], display1);
                        setcolor(BLUE);
                        setfontsize(8);
                        drawtext_in(navigate_display2[3], display2);
                        draw_navigation_direction(direction, x_left + width * 0.016, y_top - row * 3.5, true);
                    } else if (i == 4) {
                        setcolor(BLACK);
                        setfontsize(10);
                        if (display1.size() >= 30 && display1.size() < 35) setfontsize(9);
                        else if (display1.size() >= 35 && display1.size() < 40) setfontsize(8);
                        else if (display1.size() >= 40 && display1.size() < 45) setfontsize(7);
                        else if (display1.size() >= 45 && display1.size() < 50) setfontsize(6);
                        drawtext_in(navigate_display1[4], display1);
                        setcolor(BLUE);
                        setfontsize(8);
                        drawtext_in(navigate_display2[4], display2);
                        draw_navigation_direction(direction, x_left + width * 0.016, y_top - row * 4.5, true);
                    }                        
                }
                // If not end point
                else {
                    if (i == 0) {
                        setcolor(BLACK);
                        setfontsize(10);
                        if (display1.size() >= 30 && display1.size() < 35) setfontsize(9);
                        else if (display1.size() >= 35 && display1.size() < 40) setfontsize(8);
                        else if (display1.size() >= 40 && display1.size() < 45) setfontsize(7);
                        else if (display1.size() >= 45 && display1.size() < 50) setfontsize(6);
                        drawtext_in(navigate_display1[0], display1);
                        setcolor(BLUE);
                        setfontsize(8);
                        drawtext_in(navigate_display2[0], display2);
                        draw_navigation_direction(direction, x_left + width * 0.016, y_top - row * 0.5, false);
                    } else if (i == 1) {
                        setcolor(BLACK);
                        setfontsize(10);
                        if (display1.size() >= 30 && display1.size() < 35) setfontsize(9);
                        else if (display1.size() >= 35 && display1.size() < 40) setfontsize(8);
                        else if (display1.size() >= 40 && display1.size() < 45) setfontsize(7);
                        else if (display1.size() >= 45 && display1.size() < 50) setfontsize(6);
                        drawtext_in(navigate_display1[1], display1);
                        setcolor(BLUE);
                        setfontsize(8);
                        drawtext_in(navigate_display2[1], display2);
                        draw_navigation_direction(direction, x_left + width * 0.016, y_top - row * 1.5, false);
                    } else if (i == 2) {
                        setcolor(BLACK);
                        setfontsize(10);
                        if (display1.size() >= 30 && display1.size() < 35) setfontsize(9);
                        else if (display1.size() >= 35 && display1.size() < 40) setfontsize(8);
                        else if (display1.size() >= 40 && display1.size() < 45) setfontsize(7);
                        else if (display1.size() >= 45 && display1.size() < 50) setfontsize(6);
                        drawtext_in(navigate_display1[2], display1);
                        setcolor(BLUE);
                        setfontsize(8);
                        drawtext_in(navigate_display2[2], display2);
                        draw_navigation_direction(direction, x_left + width * 0.016, y_top - row * 2.5, false);
                    } else if (i == 3) {
                        setcolor(BLACK);
                        setfontsize(10);
                        if (display1.size() >= 30 && display1.size() < 35) setfontsize(9);
                        else if (display1.size() >= 35 && display1.size() < 40) setfontsize(8);
                        else if (display1.size() >= 40 && display1.size() < 45) setfontsize(7);
                        else if (display1.size() >= 45 && display1.size() < 50) setfontsize(6);
                        drawtext_in(navigate_display1[3], display1);
                        setcolor(BLUE);
                        setfontsize(8);
                        drawtext_in(navigate_display2[3], display2);
                        draw_navigation_direction(direction, x_left + width * 0.016, y_top - row * 3.5, false);
                    } else if (i == 4) {
                        setcolor(BLACK);
                        setfontsize(10);
                        if (display1.size() >= 30 && display1.size() < 35) setfontsize(9);
                        else if (display1.size() >= 35 && display1.size() < 40) setfontsize(8);
                        else if (display1.size() >= 40 && display1.size() < 45) setfontsize(7);
                        else if (display1.size() >= 45 && display1.size() < 50) setfontsize(6);
                        drawtext_in(navigate_display1[4], display1);
                        setcolor(BLUE);
                        setfontsize(8);
                        drawtext_in(navigate_display2[4], display2);
                        draw_navigation_direction(direction, x_left + width * 0.016, y_top - row * 4.5, false);
                    }                      
                }
            }
        }
    }
}

// draw the icon for different direction
void draw_navigation_direction(std::string direction, double x, double y, bool isEnd) {
    if (!isEnd) {
        if (direction == "Turn Left") {
            draw_surface(load_png_from_file("libstreetmap/resources/turnleft.png"), x, y);
        } else if (direction == "Turn Right") {
            draw_surface(load_png_from_file("libstreetmap/resources/turnright.png"), x, y);
        } else if (direction == "Make a U Turn") {
            draw_surface(load_png_from_file("libstreetmap/resources/uturnleft.png"), x, y);
        } else if (direction == "Keep Straight") {
            draw_surface(load_png_from_file("libstreetmap/resources/gostraight.png"), x, y);
        } 
    }
    else draw_surface(load_png_from_file("libstreetmap/resources/pin_icon.png"), x, y);

}

// draw the space for instructions 
void draw_instruction_window() {
    navigate_display1.clear();
    navigate_display2.clear();
    t_bound_box box = get_visible_world();

    float height = box.top() - box.bottom();
    float width = box.right() - box.left();

    float x_length = 0.26 * width;

    float x_left = box.left() + 0.01 * width;
    float y_top = box.top() - 0.25 * height;
    float x_right = x_left + x_length;
    float y_bottom = box.bottom() + 0.045 * height;
    double row = (y_top - y_bottom) / 6;
    
    // different rows
    navigate_display1.push_back(t_bound_box(x_left , y_top - 0.5 * row, x_right - 0.01 * width, y_top));
    navigate_display1.push_back(t_bound_box(x_left, y_top - 1.5 * row, x_right- 0.01 * width, y_top - 1 * row));
    navigate_display1.push_back(t_bound_box(x_left , y_top - 2.5 * row, x_right- 0.01 * width, y_top - 2 * row));
    navigate_display1.push_back(t_bound_box(x_left, y_top - 3.5 * row, x_right- 0.01 * width, y_top - 3 * row));
    navigate_display1.push_back(t_bound_box(x_left , y_top - 4.5 * row, x_right- 0.01 * width, y_top - 4 * row));
   
    navigate_display2.push_back(t_bound_box(x_left , y_top - 1 * row, x_right- 0.01 * width, y_top - 0.5 * row));
    navigate_display2.push_back(t_bound_box(x_left, y_top - 2 * row, x_right- 0.01 * width, y_top - 1.5 * row));
    navigate_display2.push_back(t_bound_box(x_left, y_top - 3 * row, x_right- 0.01 * width, y_top - 2.5 * row));
    navigate_display2.push_back(t_bound_box(x_left, y_top - 4 * row, x_right- 0.01 * width, y_top - 3.5 * row));
    navigate_display2.push_back(t_bound_box(x_left , y_top - 5 * row, x_right- 0.01 * width, y_top - 4.5 * row));
    
    setcolor(WHITE);
    fillrect(x_left, y_top - row , x_right- 0.015 * width, y_top);
    fillrect(x_left, y_top - 2 * row , x_right- 0.015 * width, y_top - 1 * row);
    fillrect(x_left, y_top - 3 * row , x_right- 0.015 * width, y_top - 2 * row);
    fillrect(x_left, y_top - 4 * row , x_right- 0.015 * width, y_top - 3 * row);
    fillrect(x_left, y_top - 5 * row , x_right- 0.015 * width, y_top - 4 * row);
    
}
////================Transportation Information from OSMID===================//
//
//void initialize_OSMNode_Map() {
//
//    for (unsigned i = 0; i < map; i++) {
//
//        const OSMNode* this_node = getNodeByIndex(i);
//
//        OSMNode_vector.push_back(this_node);
//
//        unsigned tag_count = getTagCount(this_node);
//
//        std::unordered_map<std::string, std::string> Tag_Pair;
//
//        for (unsigned j = 0; j < tag_count; j++) {
//
//            Tag_Pair.insert(getTagPair(this_node, j));
//        }
//
//        OSMEntity_Point_Tags.insert(std::make_pair(this_node, Tag_Pair));
//
//        OSMID this_id = this_node->id();
//
//        OSMID_OSMNode.insert(std::make_pair(this_id, this_node));
//
//        OSMID_OSMEntity.insert(std::make_pair(this_id, this_node));
//    }
//
//
//}
//
////store OSMWay information
//
//void initialize_OSMWay_Map() {
//
//    for (unsigned i = 0; i < map->numebr_of_ways; i++) {
//
//        const OSMWay* this_node = getWayByIndex(i);
//
//        OSMWay_vector.push_back(this_node);
//
//        unsigned tag_count = getTagCount(this_node);
//
//        std::unordered_map<std::string, std::string> Tag_Pair;
//
//        for (unsigned j = 0; j < tag_count; j++) {
//
//            Tag_Pair.insert(getTagPair(this_node, j));
//        }
//
//        OSMEntity_Point_Tags.insert(std::make_pair(this_node, Tag_Pair));
//
//        OSMID this_id = this_node->id();
//        draw_subway_staions();
//
//        OSMID_OSMWay.insert(std::make_pair(this_id, this_node));
//
//        OSMID_OSMEntity.insert(std::make_pair(this_id, this_node));
//    }
//
//}
//
////store OSMRelation information
//
//void initialize_OSMRelation_Map() {
//
//
//    for (unsigned i = 0; i < map->number_of_relations; i++) {
//
//        const OSMRelation* this_node = getRelationByIndex(i);
//
//        OSMRelation_vector.push_back(this_node);
//
//        unsigned tag_count = getTagCount(this_node);
//
//        std::unordered_map<std::string, std::string> Tag_Pair;
//
//        for (unsigned j = 0; j < tag_count; j++) {
//
//            Tag_Pair.insert(getTagPair(this_node, j));
//        }
//
//        OSMEntity_Point_Tags.insert(std::make_pair(this_node, Tag_Pair));
//
//        OSMID this_id = this_node->id();
//
//        OSMID_OSMRelation.insert(std::make_pair(this_id, this_node));
//
//        OSMID_OSMEntity.insert(std::make_pair(this_id, this_node));
//    }
//}
//
////stores transportation stations information (location & station name)
//
//void initialize_station_info() {
//
//    for (unsigned i = 0; i < map->number_of_nodes; i++) {
//
//        bool is_station = false;
//
//        const OSMNode* this_node = OSMNode_vector[i];
//
//        std::unordered_map<std::string, std::string> check_tags = OSMEntity_Point_Tags.find(this_node) -> second;
//
//        auto itr = check_tags.find("public_transport");
//
//        itr = check_tags.find("highway");
//
//        if (itr != check_tags.end()) {
//
//            if (itr->second == "bus_stop") {
//                initialize_station_info_helper("bus", this_node, check_tags);
//            }
//
//        } else {
//
//            itr = check_tags.find("railway");
//
//            if (itr != check_tags.end()) {
//
//                if (itr->second == "station" || itr->second == "subway_entrance") {
//                    initialize_station_info_helper("subway", this_node, check_tags);
//                }
//            } else {
//
//                itr = check_tags.find("aeroway");
//
//                if (itr != check_tags.end()) {
//
//                    itr = check_tags.find("amenity");
//
//                    if (itr != check_tags.end()) {
//
//                        if (itr->second == "ferry_terminal") {
//
//                            initialize_station_info_helper("ferry", this_node, check_tags);
//                        }
//                    }
//
//                }
//
//            }
//        }
//    }
//}
//
////store stations into different vectors based on its type
//
//void initialize_station_info_helper(std::string transport, const OSMNode* this_node, std::unordered_map<std::string, std::string> check_tags) {
//
//    LatLon position_latlon = this_node->coords();
//    t_point position = LatLon_XY_converter(position_latlon, avg_lat);
//
//    std::string station_name;
//
//    auto name = check_tags.find("name");
//    if (name != check_tags.end() && name ->second != "<unknown>")
//        station_name = name ->second;
//    else
//        station_name = "\0";
//
//
//    if (transport == "bus") {
//        bus_station_location.push_back(position);
//        bus_station_name.push_back(station_name);
//    } else if (transport == "subway") {
//        subway_station_location.push_back(position);
//        subway_station_name.push_back(station_name);
//    } else if (transport == "ferry") {
//
//        ferry_station_location.push_back(position);
//        ferry_station_name.push_back(station_name);
//    }
//}
//
////insert station locations into r_tree
//
//void initialize_station_tree() {
//
//    for (unsigned i = 0; i < bus_station_location.size(); i++) {
//
//        t_point position1 = bus_station_location[i];
//        t_point_point point_posistion1 = t_point_point(position1.x, position1.y);
//        bus_station_rtree.insert(std::make_pair(point_posistion1, i));
//    }
//
//    for (unsigned j = 0; j < subway_station_location.size(); j++) {
//
//        t_point position2 = subway_station_location[j];
//        t_point_point point_posistion2 = t_point_point(position2.x, position2.y);
//        subway_station_rtree.insert(std::make_pair(point_posistion2, j));
//    }
//}
//
//
//// accessing OSMID to retrieve transportation information
//
//void initialize_transport_lines() {
//
//    for (unsigned i = 0; i < map->numebr_of_ways; i++) {
//
//        const OSMWay* this_way = OSMWay_vector[i];
//
//        std::unordered_map<string, string> check_tags = OSMEntity_Point_Tags.find(this_way) -> second;
//
//        auto itr = check_tags.find("railway");
//
//        if (itr != check_tags.end()) {
//
//            if (itr ->second == "subway") {
//
//                std::vector<OSMID> noderef = this_way -> ndrefs();
//
//                initialize_transport_lines_helper("subway", this_way);
//
//                transport_line.push_back(noderef);
//            }
//        } else {
//
//            auto itr = check_tags.find("route");
//
//            if (itr != check_tags.end()) {
//
//                if (itr ->second == "ferry") {
//
//                    std::vector<OSMID> noderef = this_way -> ndrefs();
//
//                    initialize_transport_lines_helper("ferry", this_way);
//
//                    transport_line.push_back(noderef);
//                }
//            }
//        }
//    }
//}
//
//void initialize_transport_lines_helper(std::string transport, const OSMWay * this_way) {
//
//
//    if (transport == "subway") {
//
//        std::vector<OSMID> noderef = this_way -> ndrefs();
//
//        subway_line.push_back(noderef);
//
//    } else if (transport == "ferry") {
//
//        std::vector<OSMID> noderef = this_way ->ndrefs();
//
//        ferry_line.push_back(noderef);
//
//    }
//
//}
//
//// Distinguish different transportation methods
//
//void draw_transport_lines(std::string transport) {
//
//    if (transport == "subway") {
//
//        for (unsigned i = 0; i < subway_line.size(); i++) {
//
//            std::vector<OSMID> this_subway_line = subway_line[i];
//
//            for (unsigned j = 1; j < this_subway_line.size(); j++) {
//
//                t_point node_position1 = LatLon_XY_converter((OSMID_OSMNode.find(this_subway_line[j - 1])->second->coords()),avg_lat);//avg_lat
//                
//
//                t_point node_position2 = LatLon_XY_converter((OSMID_OSMNode.find(this_subway_line[j])->second->coords()),avg_lat);
//
//                setlinewidth(3);
//
//                if (zoom <= 3)
//                    setlinewidth(1);
//
//                setcolor(128, 0, 0);
//                drawline(node_position1, node_position2);
//            }
//        }
//
//    } else if (transport == "ferry") {
//
//        for (unsigned i = 0; i < ferry_line.size(); i++) {
//
//            std::vector<OSMID> this_ferry_line = ferry_line[i];
//
//            for (unsigned j = 1; j < this_ferry_line.size(); j++) {
//
//                t_point node_position1 = LatLon_XY_converter((OSMID_OSMNode.find(this_ferry_line[j - 1])->second->coords()));
//
//                t_point node_position2 = LatLon_XY_converter((OSMID_OSMNode.find(this_ferry_line[j])->second->coords()));
//
//                setlinewidth(3);
//
//                if (zoom < 5)
//                    setlinewidth(1);
//
//                setcolor(220, 220, 220);
//
//                drawline(node_position1, node_position2);
//            }
//        }
//    }
//}
//
//void draw_bus_staions() {
//
//    t_bound_box current_screen_bound = get_visible_world();
//
//    for (unsigned i = 0; i < bus_station_location.size(); i++) {
//
//        t_point station_position = bus_station_location[i];
//
//        if (station_position.x > current_screen_bound.left() && station_position.x < current_screen_bound.right() && station_position.y > current_screen_bound.bottom() && station_position.y < current_screen_bound.top()) {
//
//            Surface bus = load_png_from_file("libstreetmap/resources/bus.png");
//
//            draw_surface(bus, station_position.x, station_position.y);
//        }
//    }
//}
//
//void draw_ferry_staions() {
//
//    t_bound_box initial_coords = world;
//
//    t_bound_box current_screen_bound = get_visible_world();
//
//    for (unsigned i = 0; i < ferry_station_location.size(); i++) {
//
//        t_point station_position = ferry_station_location[i];
//
//        if (station_position.x > current_screen_bound.left() && station_position.x < current_screen_bound.right() && station_position.y > current_screen_bound.bottom() && station_position.y < current_screen_bound.top()) {
//
//            std::string station_name = ferry_station_name[i];
//
//            Surface ferry = load_png_from_file("libstreetmap/resources/ferry.png");
//
//            draw_surface(ferry, station_position.x, station_position.y);
//
//            if (zoom > 6) {
//                set_draw_mode(DRAW_NORMAL);
//                setlinestyle(SOLID, ROUND);
//                setlinewidth(1);
//                setcolor(BLACK);
//                setfontsize(10);
//                settextrotation(0);
//                drawtext(station_position, station_name, initial_coords);
//            }
//        }
//    }
//}
//
//void draw_subway_staions() {
//
//    std::unordered_map <std::string, unsigned> check_drew;
//
//    t_bound_box initial_coords = world;
//
//    for (unsigned i = 0; i < subway_station_location.size(); i++) {
//
//        t_point station_position = subway_station_location[i];
//
//        std::string station_name = get_subway_station_name(station_position);
//
//        check_drew.insert(std::make_pair(station_name, i));
//
//        Surface subway = load_png_from_file("libstreetmap/resources/subway.png");
//
//        draw_surface(subway, station_position.x, station_position.y);
//
//        if (zoom >= 6) {
//            set_draw_mode(DRAW_NORMAL);
//            setlinestyle(SOLID, ROUND);
//            setlinewidth(1);
//            setcolor(BLACK);
//            setfontsize(10);
//            settextrotation(0);
//            drawtext(station_position, station_name, initial_coords);
//        }
//    }
//}
//
////return bus station name for a bus station position
//
//std::string get_bus_station_name(t_point station_position) {
//
//    std::string station_name;
//
//    for (unsigned i = 0; i < bus_station_location.size(); i++) {
//
//        if (bus_station_location[i].x == station_position.x && bus_station_location[i].y == station_position.y)
//            station_name = bus_station_name[i];
//    }
//
//    return station_name;
//}
//
////return subway station name for a subway station position
//
//std::string get_subway_station_name(t_point station_position) {
//
//    std::string station_name;
//
//    for (unsigned i = 0; i < subway_station_location.size(); i++) {
//
//        if (subway_station_location[i].x == station_position.x && subway_station_location[i].y == station_position.y)
//            station_name = subway_station_name[i];
//    }
//
//    return station_name;
////}
