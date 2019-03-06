/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "Greedy.h"

void Greedy(double turn_penalty, std::vector<unsigned>& path, std::vector<delivery_info> pick_up, 
            std::vector<save_state> pick_state, std::vector<delivery_info> drop_off, 
            std::vector<save_state> drop_state, std::vector<delivery_info> passed_intersection, std::vector<unsigned>& intersection_path, 
            std::vector<bool> has_picked, unsigned depot, const std::vector<unsigned>& depots) {
        
    unsigned stop_id;
    unsigned end_point;
    std::vector<unsigned> temp_path;
    if (UCS_Greedy_start(depot, turn_penalty, stop_id, temp_path, pick_state))
        path.insert(path.end(), temp_path.begin(), temp_path.end());
    else
        return;

    intersection_path.push_back(depot);
    // start searching
    has_picked[stop_id] = 1;
    pick_state[stop_id].explored = 0;
    passed_intersection.push_back(pick_up[pick_state[stop_id].access_id]);
    
    UCS_Greedy_search(turn_penalty, path, pick_up, pick_state, drop_off, 
                      drop_state, passed_intersection, has_picked);
    
    UCS_Greedy_end(turn_penalty, path, depots, passed_intersection, end_point);
    
    for (unsigned i = 0; i < passed_intersection.size(); i++) {
        if (intersection_path[intersection_path.size() - 1] != passed_intersection[i].id)
            intersection_path.push_back(passed_intersection[i].id);
    }
    intersection_path.push_back(end_point);
}