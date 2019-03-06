/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   UCS.h
 * Author: wangy610
 *
 * Created on March 17, 2018, 1:11 AM
 */
// pseudo code for uniform cost search found on: https://en.wikipedia.org/wiki/Dijkstra%27s_algorithm
#ifndef UCS_H
#define UCS_H
#include <vector>
#include "StreetsDatabaseAPI.h"
#include "map_data.h"
#include <unordered_map>
#include <unordered_set>
#include <algorithm> 
#include <utility>
#include <chrono>
struct path_info {
    unsigned end_id = 0;
    double travel_time = 0;
    unsigned type = 0; // 1 for pick_up, 2 for drop_off, 3 for depot, 4 for both pick_up and drop_off
    std::vector<unsigned> path; // path from this start point to one end point

    bool operator < (const path_info& val) const
    {
        return (travel_time < val.travel_time);
    }
    
};

struct pick_up_info {
    std::vector<unsigned> drop_off_points;
    bool already_picked_up = false;
    bool is_also_drop_off = false;
    std::vector<path_info> all_path; // path from start point to all possible end points
};

struct drop_off_info {
    std::unordered_map<unsigned, std::pair<bool, bool>> drop_off_done_info;
    //bool is_also_pick_up = false;
    std::vector<path_info> all_path; // path from start point to all possible end points
};

struct delivery_info {
    unsigned id;
    std::vector<unsigned> relation;
};

struct save_state {
    bool explored = 0;
    unsigned access_id;
};



void UCS(unsigned start, unsigned goal, double turn_penalty, std::vector<unsigned>& bestPath);
void UCS_multi_target(unsigned start, std::vector<unsigned>& goal, double turn_penalty, std::vector<unsigned>& bestPath);
void UCS_delivery(unsigned start, std::unordered_map<unsigned, unsigned>& end_points, double turn_penalty, std::vector<path_info>& all_path);
void UCS_depot(unsigned start, std::unordered_map<unsigned, unsigned>& end_points, double turn_penalty, std::vector<path_info>& all_path);
bool UCS_Greedy_start(unsigned start, double turn_penalty, unsigned& stop_id, std::vector<unsigned>& bestPath, 
                      std::vector<save_state>& pick_state);
void UCS_Greedy_search(double turn_penalty, std::vector<unsigned>& path, std::vector<delivery_info>& pick_up, 
                       std::vector<save_state>& pick_state, std::vector<delivery_info>& drop_off, 
                       std::vector<save_state>& drop_state, std::vector<delivery_info>& passed_intersection, std::vector<bool>& has_picked);
void UCS_Greedy_end (double turn_penalty, std::vector<unsigned>& path, const std::vector<unsigned>& depots, 
                    std::vector<delivery_info>& passed_intersection, unsigned& end_point);
#endif /* UCS_H */
