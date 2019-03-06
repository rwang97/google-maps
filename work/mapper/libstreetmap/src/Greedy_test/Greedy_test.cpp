/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include "Greedy_test.h"

void Greedy_test(std::vector<unsigned>& path, 
            std::unordered_map<unsigned, drop_off_info>& drop_off,
            std::unordered_map<unsigned, pick_up_info>& pick_up, 
            std::unordered_map<unsigned, std::vector<path_info>>& depot,
            unsigned size) {
    
    srand(time(NULL));
    std::unordered_set<unsigned> collision;
    // take depot.begin() as start point
    // Go to nearest pick up point
    unsigned current = 0;
    bool go_pick_up = true;
    for (auto it = depot.begin(); it != depot.end(); it++) {
        for (auto it_path = it -> second.begin(); it_path != it -> second.end(); it_path++) {
            if (it_path -> type == 1 ) {
                current = it_path -> end_id;
                path.insert(path.end(), it_path -> path.begin(), it_path -> path.end());
                break;
            }
        }
        break;
    }
    
    while (size != 0) {
        // go to pick up side of structure
        if (go_pick_up) {
            //std::cout << "pick up at: " << current << std::endl;
            // set already pick up
            pick_up[current].already_picked_up = true;
            // set the legal drop_off
            for (unsigned i = 0; i < pick_up[current].drop_off_points.size(); i++) {
                drop_off[pick_up[current].drop_off_points[i]].drop_off_done_info[current].first = true;
            }
            //and set already drop to tree
            if (pick_up[current].is_also_drop_off) {
                for (auto it = drop_off[current].drop_off_done_info.begin(); it != drop_off[current].drop_off_done_info.end(); it++) {
                    if (!it -> second.first) {
                        pick_up[current].already_picked_up = false;
                    }
                    if (it -> second.first && !it -> second.second) {
                        it -> second.second = true;
                        size = size - 1;                       
                    }
                }
            }
            if (size != 0) {
                bool valid_path_exist = false;
                for (unsigned i = 0; i < pick_up[current].all_path.size(); i++) {
                    // go to nearest pick up point
                    if (pick_up[current].all_path[i].type == 1 || pick_up[current].all_path[i].type == 4) {
                        if (!pick_up[pick_up[current].all_path[i].end_id].already_picked_up) {
                            path.insert(path.end(), pick_up[current].all_path[i].path.begin(),pick_up[current].all_path[i].path.end());
                            current = pick_up[current].all_path[i].end_id;
                            valid_path_exist = true;
                            go_pick_up = true;
                            break;
                        }
                    }
                    // go to legal drop off
                    else if (pick_up[current].all_path[i].type == 2) {
                        bool legal_drop_off = false;
                        for (auto it = drop_off[pick_up[current].all_path[i].end_id].drop_off_done_info.begin(); 
                                  it != drop_off[pick_up[current].all_path[i].end_id].drop_off_done_info.end(); it++) {
                            if (it -> second.second)
                                legal_drop_off = false;
                            else if (it -> second.first) {
                                legal_drop_off = true;
                                break;      
                            }
                            else
                                legal_drop_off = false;
                        }
                        if (legal_drop_off) {
                            path.insert(path.end(), pick_up[current].all_path[i].path.begin(),pick_up[current].all_path[i].path.end());
                            current = pick_up[current].all_path[i].end_id;                      
                            go_pick_up = false;
                            valid_path_exist = true;
                            break;
                        }               
                    }
                }
                if (!valid_path_exist) {
                    collision.insert(current);
                    for (unsigned i = 0; i < pick_up[current].all_path.size(); i++) {
                        //std::cout << "collision: " << current << "  next id: " << pick_up[current].all_path[i].end_id;
                        for (auto it = collision.begin(); it != collision.end(); it++) {
                            if (pick_up[current].all_path[i].end_id == *it) {
                                double ran_size = pick_up[current].all_path.size() / 4;
                                int actual_size = floor(ran_size);
                                i = rand() % actual_size;
                                while (pick_up[current].all_path[i].type == 0 || pick_up[current].all_path[i].type == 3) {
                                    ran_size = pick_up[current].all_path.size() / 4;
                                   actual_size = floor(ran_size);
                                    i = rand() % actual_size;
                                }
                                break;
                            }
                        }
                        if (pick_up[current].all_path[i].type == 1 || pick_up[current].all_path[i].type == 4 || pick_up[current].all_path[i].type == 2) {
                            //std::cout << "test i: " << i << std::endl;
                            //std::cout << "type: " << pick_up[current].all_path[i].type << std::endl;
                            if (pick_up[current].all_path[i].type == 1 || pick_up[current].all_path[i].type == 4)
                                go_pick_up = true;
                            else
                                go_pick_up = false;
                            path.insert(path.end(), pick_up[current].all_path[i].path.begin(),pick_up[current].all_path[i].path.end());
                            current = pick_up[current].all_path[i].end_id;
                            break;
                        }
                    }
                }
            }
        }
        else {
            //std::cout << "drop off at: " << current << std::endl;
            for (auto it = drop_off[current].drop_off_done_info.begin(); it != drop_off[current].drop_off_done_info.end(); it++) {
                if (it -> second.first && !it -> second.second) {
                    it -> second.second = true;
                    size = size - 1;
                }      
            }
            if (size != 0) {
                bool valid_path_exist = false;
                for (unsigned i = 0; i < drop_off[current].all_path.size(); i++) {
                    // go to nearest pick up point
                    if (drop_off[current].all_path[i].type == 1 || drop_off[current].all_path[i].type == 4) {
                        if (!pick_up[drop_off[current].all_path[i].end_id].already_picked_up) {
                            path.insert(path.end(), drop_off[current].all_path[i].path.begin(), drop_off[current].all_path[i].path.end());
                            current = drop_off[current].all_path[i].end_id;
                            valid_path_exist = true;
                            go_pick_up = true;
                            break;
                        }
                    }
                    // go to legal drop off
                    else if (drop_off[current].all_path[i].type == 2) {
                        bool legal_drop_off = false;
                        for (auto it = drop_off[drop_off[current].all_path[i].end_id].drop_off_done_info.begin(); 
                                  it != drop_off[drop_off[current].all_path[i].end_id].drop_off_done_info.end(); it++) {
                            if (it -> second.second)
                                legal_drop_off = false;
                            else if (it -> second.first) {
                                legal_drop_off = true;
                                break;      
                            }
                            else
                                legal_drop_off = false;
                        }
                        if (legal_drop_off) {
                            path.insert(path.end(), drop_off[current].all_path[i].path.begin(), drop_off[current].all_path[i].path.end());
                            current = drop_off[current].all_path[i].end_id;
                            valid_path_exist = true;
                            go_pick_up = false;
                            break;
                        }               
                    }
                }
                if (!valid_path_exist) {
                    collision.insert(current);
                    //bool have_added = false;
                    for (unsigned i = 0; i < drop_off[current].all_path.size(); i++) {
                        for (auto it = collision.begin(); it != collision.end(); it++) {
                            if (drop_off[current].all_path[i].end_id == *it) {
                                double ran_size = drop_off[current].all_path.size() / 3;
                                int actual_size = floor(ran_size);
                                i = rand() % actual_size;
                                while (drop_off[current].all_path[i].type == 0 || drop_off[current].all_path[i].type == 3) {
                                    ran_size = drop_off[current].all_path.size() / 3;
                                    actual_size = floor(ran_size);
                                    i = rand() % actual_size;
                                }
                                break;
                            }
                        }
                        if (drop_off[current].all_path[i].type == 1 || drop_off[current].all_path[i].type == 4 || drop_off[current].all_path[i].type == 2) {
                            if (drop_off[current].all_path[i].type == 1 || drop_off[current].all_path[i].type == 4)
                                go_pick_up = true;
                            else
                                go_pick_up = false;
                            path.insert(path.end(), drop_off[current].all_path[i].path.begin(),drop_off[current].all_path[i].path.end());
                            current = drop_off[current].all_path[i].end_id;
                            break;
                        }
                    }
                }
            }
        }
        
    }
    if (go_pick_up) {
        for (unsigned i = 0; i < pick_up[current].all_path.size(); i++) {
            // go to nearest depot
            if (pick_up[current].all_path[i].type == 3) {
                path.insert(path.end(), pick_up[current].all_path[i].path.begin(),pick_up[current].all_path[i].path.end());
                current = pick_up[current].all_path[i].end_id;
                //std::cout << "end at: " << current << std::endl;
                return;
            }
        }
        for (unsigned i = 0; i < pick_up[current].all_path.size(); i++) {
            unsigned temp = current;
            if (pick_up[current].all_path[i].type == 1 || pick_up[current].all_path[i].type == 4) {
                path.insert(path.end(), pick_up[current].all_path[i].path.begin(),pick_up[current].all_path[i].path.end());
                current = pick_up[current].all_path[i].end_id;
                for (unsigned j = 0; j < pick_up[current].all_path.size(); j++) {
                    if (pick_up[current].all_path[j].type == 3) {
                        path.insert(path.end(), pick_up[current].all_path[j].path.begin(),pick_up[current].all_path[j].path.end());
                        current = pick_up[current].all_path[j].end_id;
                        //std::cout << "end at: " << current << std::endl;
                        return; 
                    }
                }
                current = temp;
            }
            else if (pick_up[current].all_path[i].type == 2) {
                path.insert(path.end(), pick_up[current].all_path[i].path.begin(),pick_up[current].all_path[i].path.end());
                current = pick_up[current].all_path[i].end_id;
                for (unsigned j = 0; j < drop_off[current].all_path.size(); j++) {
                    if (drop_off[current].all_path[j].type == 3) {
                        path.insert(path.end(), drop_off[current].all_path[j].path.begin(),drop_off[current].all_path[j].path.end());
                        current = drop_off[current].all_path[j].end_id;
                        //std::cout << "end at: " << current << std::endl;
                        return; 
                    }
                }
                current = temp;
            } 
        }
    }
    else {
        for (unsigned i = 0; i < drop_off[current].all_path.size(); i++) {
           // go to nearest depot
           if (drop_off[current].all_path[i].type == 3) {
                path.insert(path.end(), drop_off[current].all_path[i].path.begin(), drop_off[current].all_path[i].path.end());
                current = drop_off[current].all_path[i].end_id;
                //std::cout << "end at: " << current << std::endl;
                return;
           }  
        }
        for (unsigned i = 0; i < drop_off[current].all_path.size(); i++) {
            unsigned temp = current;
            if (drop_off[current].all_path[i].type == 1 || drop_off[current].all_path[i].type == 4) {
                path.insert(path.end(), drop_off[current].all_path[i].path.begin(),drop_off[current].all_path[i].path.end());
                current = drop_off[current].all_path[i].end_id;
                for (unsigned j = 0; j < pick_up[current].all_path.size(); j++) {
                    if (pick_up[current].all_path[j].type == 3) {
                        path.insert(path.end(), pick_up[current].all_path[j].path.begin(),pick_up[current].all_path[j].path.end());
                        current = pick_up[current].all_path[j].end_id;
                        //std::cout << "end at: " << current << std::endl;
                        return; 
                    }
                }
                current = temp;
            }
            else if (drop_off[current].all_path[i].type == 2) {
                path.insert(path.end(), drop_off[current].all_path[i].path.begin(),drop_off[current].all_path[i].path.end());
                current = drop_off[current].all_path[i].end_id;
                for (unsigned j = 0; j < drop_off[current].all_path.size(); j++) {
                    if (drop_off[current].all_path[j].type == 3) {
                        path.insert(path.end(), drop_off[current].all_path[j].path.begin(),drop_off[current].all_path[j].path.end());
                        current = drop_off[current].all_path[j].end_id;
                        //std::cout << "end at: " << current << std::endl;
                        return; 
                    }
                }
                current = temp;
            } 
        }
    }
    
}