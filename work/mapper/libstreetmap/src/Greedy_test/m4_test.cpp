/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */


#include "StreetsDatabaseAPI.h"
#include "UCS.h"
#include "m4.h"
#include "Greedy_test.h"


std::vector<unsigned> traveling_courier2(const std::vector<DeliveryInfo>& deliveries, 
                                        const std::vector<unsigned>& depots, 
                                        const float turn_penalty);

std::vector<unsigned> traveling_courier2(const std::vector<DeliveryInfo>& deliveries, 
                                        const std::vector<unsigned>& depots, 
                                        const float turn_penalty) {
    std::vector<unsigned> path;
    std::unordered_map<unsigned, drop_off_info> drop_off;
    std::unordered_map<unsigned, pick_up_info> pick_up;
    std::unordered_map<unsigned, std::vector<path_info>> depot;
    
    std::unordered_map<unsigned, unsigned> end_points; // first for id, second for type
    
    for (unsigned i = 0; i < deliveries.size(); i++) {
        if (!end_points.insert(std::make_pair(deliveries[i].pickUp, 1)).second && end_points[deliveries[i].pickUp] == 2) {
            //drop_off[deliveries[i].pickUp].is_also_pick_up = true;
            end_points[deliveries[i].pickUp] = 4;
            pick_up[deliveries[i].pickUp].is_also_drop_off = true;        
        }        
        if(!end_points.insert(std::make_pair(deliveries[i].dropOff, 2)).second && end_points[deliveries[i].dropOff] == 1) {
            end_points[deliveries[i].dropOff] = 4;
            //drop_off[deliveries[i].dropOff].is_also_pick_up = true;
            pick_up[deliveries[i].dropOff].is_also_drop_off = true;
        }
        pick_up[deliveries[i].pickUp].drop_off_points.push_back(deliveries[i].dropOff); 
        drop_off[deliveries[i].dropOff].drop_off_done_info[deliveries[i].pickUp].first = false;
        drop_off[deliveries[i].dropOff].drop_off_done_info[deliveries[i].pickUp].second = false;
    }

    for (unsigned i = 0; i < depots.size(); i++) {
        end_points[depots[i]] = 3; 
        std::vector<path_info> temp;
        depot.insert(std::make_pair(depots[i], temp));
    }   
    unsigned size = 0;

    #pragma omp parallel for
    for (unsigned i = 0; i < depots.size(); i++) {
        UCS_depot(depots[i], end_points, turn_penalty, depot[depots[i]]);
    }

    #pragma omp parallel for
    for (unsigned i = 0; i < pick_up.size(); i++) {
        auto it = pick_up.begin();
        std::advance(it, i);
        UCS_delivery(it -> first, end_points, turn_penalty, it -> second.all_path);
    }

    #pragma omp parallel for
    for (unsigned i = 0; i < drop_off.size(); i++) {
        auto it = drop_off.begin();
        std::advance(it, i);
        UCS_delivery(it -> first, end_points, turn_penalty, it -> second.all_path);
        size = size + it -> second.drop_off_done_info.size();
    }
    //for (unsigned i = 0; i < depots.size(); i++) end_points.erase(depots[i]);
    #pragma omp master
    Greedy_test(path, drop_off, pick_up, depot, size);
        path.clear();
    
    return path;
        
}
