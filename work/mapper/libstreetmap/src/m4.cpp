/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "StreetsDatabaseAPI.h"
#include "UCS.h"
#include "m4.h"
#include "Greedy.h"
#include "m3.h"
#include "SA.h"

std::vector<unsigned> traveling_courier(const std::vector<DeliveryInfo>& deliveries, 
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
    auto start_time = std::chrono::high_resolution_clock::now();
    
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
//    unsigned start_depot;
    for (unsigned i = 0; i < depots.size(); i++) {
        std::vector<unsigned> temp_path;
        std::vector<unsigned> intersection_temp_path;
        Greedy(turn_penalty, temp_path, pick_up, pick_state, drop_off, 
               drop_state, passed_intersection, intersection_temp_path, has_picked, depots[i], depots);
        if (!temp_path.empty()) {
            #pragma omp critical 
            {
                all_path.push_back(temp_path);
                all_intersection_path.push_back(intersection_temp_path);
            }
        } 
    }
//    std::vector<unsigned> intersection_temp_path;
//    Greedy(turn_penalty, path, pick_up, pick_state, drop_off, 
//           drop_state, passed_intersection, intersection_temp_path, has_picked, depots[0], depots);
    
    
    double time = compute_path_travel_time(all_path[0], turn_penalty);
    path = all_path[0];
    unsigned index = 0;
    for (unsigned i = 1; i < all_path.size(); i++) {
        double new_time = compute_path_travel_time(all_path[i], turn_penalty);
        if (new_time < time) {
            time = new_time;
            index = i;
        }    
    }
    path = all_path[index];
    intersection_path = all_intersection_path[index];
    //two_opt(path, deliveries,intersection_delivery, intersection_path, turn_penalty, start_time);
    return path;
        
}
