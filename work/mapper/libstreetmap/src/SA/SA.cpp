/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   SA.cpp
 * Author: donganzh
 *
 * Created on April 5, 2018, 2:26 AM
 */


#include "SA.h"

void two_opt(std::vector<unsigned>& path, const std::vector<DeliveryInfo>& deliveries,
        std::vector<std::vector<unsigned>>&intersection_delivery,
        std::vector<unsigned>& intersection_path, unsigned turn_penalty, std::chrono::high_resolution_clock::time_point& start_time) {

    //std::srand(std::time(NULL));
    std::mt19937 randomIndex;
    std::uniform_int_distribution<std::mt19937::result_type> distributionIndex(1, intersection_path.size() - 2);
    //    bool swap_valid = false;
        std::vector<unsigned> saved_intersection = intersection_path;
        double path_time_before = compute_path_travel_time(path, turn_penalty);
        while (true) {
            //std::cout << "size: " << intersection_path.size() << std::endl;
            auto current_time = std::chrono::high_resolution_clock::now();
            auto time_change = std::chrono::duration_cast<std::chrono::duration<double>> (current_time - start_time);
            if (time_change.count() < 20) {            
                unsigned start = distributionIndex(randomIndex);
                unsigned end = distributionIndex(randomIndex);
                //std::cout << "start: " << start << "   end: " << end << std::endl;
                if (start < end) {
                    
                    std::vector<unsigned> copy = saved_intersection;
                    std::reverse(copy.begin() + start, copy.begin() + end + 1);
//                    unsigned start2 = distributionIndex(randomIndex);
//                    unsigned end2 = distributionIndex(randomIndex);
//                    if (start2 < end2) {
//                        std::reverse(copy.begin() + start2, copy.begin() + end2 + 1);;
//                        unsigned start, end;
//                        if (start1 < start2)
//                            start = start1;
//                        else
//                            start = start2;
//                        if (end1 < end2)
//                            end = end2;
//                        else
//                            end = end1;
                        
                        
                        if (check_path_valid(copy, deliveries, intersection_delivery, start - 1, end + 1)) {
                        std::vector<unsigned> new_path;
                        for (unsigned j = 1; j < copy.size(); j++) {
                            std::vector<unsigned> temp = find_path_between_intersections(copy[j - 1], copy[j], turn_penalty);
                            new_path.insert(new_path.end(), temp.begin(), temp.end());
                        }
                        //double time = compute_path_travel_time(path, turn_penalty);
                        std::cout << "old path time: " << path_time_before;
                        double new_time = compute_path_travel_time(new_path, turn_penalty);
                        std::cout << "           new path time: " << new_time << std::endl;
                        if (new_time < path_time_before) {

                            path = new_path;
                            saved_intersection = copy;
                            path_time_before = new_time;
                            //continue;
                        }
                    }
                //}
                }
            }
            else {
                return;
            }
        }
    
}

bool check_path_valid(std::vector<unsigned>& copy, const std::vector<DeliveryInfo>& deliveries,
        std::vector<std::vector<unsigned>>&intersection_delivery,
        unsigned start, unsigned end) {

    std::vector<bool> has_dropped;//(deliveries.size(), false);
    has_dropped.resize(deliveries.size());
    for (unsigned i = 0; i < deliveries.size(); i++) {
        has_dropped[i] = false;
    }
    for (unsigned i = start; i <= end; i++) {
        std::vector<unsigned> delivery = intersection_delivery[copy[i]];
        for (unsigned j = 0; j < delivery.size(); j++) {
            if (deliveries[delivery[j]].dropOff != copy[i]) {
                if (i > start && has_dropped[delivery[j]])
                    return false;
            } else
                has_dropped[delivery[j]] = true;
        }
    }
    return true;
}


//void four_opt(std::vector<unsigned>& path, std::chrono::high_resolution_clock::time_point startTime) {
//    auto current_time = std::chrono::high_resolution_clock::now();
//    
//    auto time_change = std::chrono::duration_cast<std::chrono::duration<double>> (current_time - startTime);
//    
//    vector<int> corresponding(path.size(), 0);
//    for (unsigned i = 0; i < path.size(); i++) {
//        if (path[i] % 2 != 0) {
//            int pickID = path[i] - 1;
//            corresponding[pickID] = i;
//        }
//    }
//    
//    unsigned size = path.size();
//    int iteration = 0;
//    bool improved = true;
//    
//    while(improved && iteration <= 4){
//        iteration++;
//        improved = false;
//        
//        for(int n = 1; i < size - 4; n++){
//            auto current_time = std::chrono::high_resolution_clock::now();
//            
//            auto time_change = std::chrono::duration_cast<std::chrono::duration<double>> (current_time - startTime);
//            if(time_change.count() >= 0.95 * 30){
//                return;
//            }
//            for(int j = n + 2; j < size - 3; j++){
//                bool illegal_first = false;
//                
//                for(int k = n + 2; k < size - 2 && ! illegal_first; k++ ){
//                    bool illegal_second = false;
//                    
//                    for(int l = k + 2; l < size - 1 && !illegal_first; l++){
//                        for(int m = n; m <= j; m++){
//                            if(path[m] % 2 == 0){
//                                int pickID = path[m];
//                                int dropped = corresponding[pickID];
//                                if(dropped > = j + 1 && dropped <= l){
//                                    illegal_first = true;
//                                    j = dropped -1;
//                                    break;
//                                }
//                            }
//                        }
//                        if (illegal_first){
//                            break;
//                        }
//                        for(int index = j + 1; index <= k - 1; index++){
//                            if(path[index] % 2 == 0){
//                                int pickID = path[index];
//                                unsigned dropped = corresponding[pickID];
//                                if(dropped >= k && dropped <= l){
//                                    illegal_second = true;
//                                    k = dropped;
//                                    break;
//                                }
//                            }
//                        }
//                        if(illegal_second){
//                            break;
//                        }
//                        double swap = 0;
//                        swap -= 
//                }
//                
//            }
//        }
//    }
//}
//}
