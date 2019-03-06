/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include <queue>
#include "searchNode.h"
#include "m1.h"
#include "StreetsDatabaseAPI.h"
#include "UCS.h"

// pseudo code for uniform cost search found on: https://en.wikipedia.org/wiki/Dijkstra%27s_algorithm
#define NO_EDGE -1
#define NO_PARENT -1

void UCS(unsigned start, unsigned goal, double turn_penalty, std::vector<unsigned>& bestPath) {

    // A structure that contains all the nodes
    std::vector<searchNode> allNodes;
    allNodes.resize(getNumberOfIntersections());

    // Declare the root of the graph
    searchNode root(start, 0, NO_PARENT, NO_EDGE, false);

    // Intialize the start node
    allNodes[start] = root;

    // Priority queue
    std::priority_queue<searchNode, std::vector<searchNode>, greaterTime> frontier;

    // For checking whether the node is in the queue
    std::unordered_set<unsigned> frontier_set;

    // Insert the root into queue
    frontier.push(root);
    frontier_set.insert(root.id);

    // Return fail when the queue is empty
    while (!frontier.empty()) {
        // Get the minimum node in the queue and erase that
        searchNode current = frontier.top();
        frontier.pop();
        frontier_set.erase(current.id);
        if (!allNodes[current.id].explored) {
            // Set the node as visited
            allNodes[current.id].explored = true;

            // When found the best route, return result
            if (current.id == goal) {
                // Track back to start, store the segments
                unsigned i = 1;
                while (current.parent != NO_PARENT) {
                    //std::cout << "reverse: " << i << " " << current.reaching_edge << " " << current.pathTime << std::endl;
                    bestPath.push_back(current.reaching_edge);
                    current = allNodes[current.parent];
                    i++;
                }
                std::reverse(bestPath.begin(), bestPath.end());
                return;
            }


            // Loop through neighbouring intersections
            std::vector<unsigned> edge = find_intersection_street_segments(current.id);

            for (unsigned i = 0; i < edge.size(); i++) {
                StreetSegmentInfo info = getStreetSegmentInfo(edge[i]);
                if (info.from == current.id) {
                    // If the neighbouring node is not visited
                    if (!allNodes[info.to].explored) {
                        double edge_time = find_street_segment_travel_time(edge[i]);
                        if (current.reaching_edge != NO_EDGE && (info.streetID != getStreetSegmentInfo(current.reaching_edge).streetID))
                            edge_time = edge_time + turn_penalty;
                        // If not in the queue
                        auto it_f = frontier_set.find(info.to);
                        if (it_f == frontier_set.end()) {
                            searchNode temp(info.to, current.pathTime + edge_time, edge[i], current.id, false);
                            frontier.push(temp);
                            frontier_set.insert(info.to);
                            allNodes[info.to] = temp;
                        }
                            // If the neighbouring node in the queue has higher cost, replace with current one
                        else if ((current.pathTime + edge_time) < allNodes[info.to].pathTime) {
                            searchNode temp(info.to, current.pathTime + edge_time, edge[i], current.id, false);
                            allNodes[info.to] = temp;
                            frontier.push(temp);
                        }
                    }
                } else if (!info.oneWay) {
                    // If not visited
                    if (!allNodes[info.from].explored) {
                        double edge_time = find_street_segment_travel_time(edge[i]);
                        if (current.reaching_edge != NO_EDGE && (info.streetID != getStreetSegmentInfo(current.reaching_edge).streetID))
                            edge_time = edge_time + turn_penalty;
                        auto it_f = frontier_set.find(info.from);
                        // If not in the queue
                        if (it_f == frontier_set.end()) {
                            searchNode temp(info.from, current.pathTime + edge_time, edge[i], current.id, false);
                            frontier.push(temp);
                            frontier_set.insert(info.from);
                            allNodes[info.from] = temp;
                        }                            // If the node in the queue has higher cost
                        else if ((current.pathTime + edge_time) < allNodes[info.from].pathTime) {
                            searchNode temp(info.from, current.pathTime + edge_time, edge[i], current.id, false);
                            allNodes[info.from] = temp;
                            frontier.push(temp);
                        }
                    }
                }
            }
        }
    }
}

void UCS_multi_target(unsigned start, std::vector<unsigned>& goal, double turn_penalty, std::vector<unsigned>& bestPath) {

    // A structure that contains all the nodes
    std::vector<searchNode> allNodes;
    allNodes.resize(getNumberOfIntersections());

    // Declare the root of the graph
    searchNode root(start, 0, NO_PARENT, NO_EDGE, false);

    // Intialize the start node
    allNodes[start] = root;

    // Priority queue
    std::priority_queue<searchNode, std::vector<searchNode>, greaterTime> frontier;

    // For checking whether the node is in the queue
    std::unordered_set<unsigned> frontier_set;

    // Insert the root into queue
    frontier.push(root);
    frontier_set.insert(root.id);

    // Return fail when the queue is empty
    while (!frontier.empty()) {
        // Get the minimum node in the queue and erase that
        searchNode current = frontier.top();
        frontier.pop();
        frontier_set.erase(current.id);
        if (!allNodes[current.id].explored) {
            // Set the node as visited
            allNodes[current.id].explored = true;

            // When found the best route, return result
            for (unsigned i = 0; i < goal.size(); i++) {
                if (current.id == goal[i]) {
                    // Track back to start, store the segments
                    while (current.parent != NO_PARENT) {
                        //std::cout << "reverse: " << i << " " << current.reaching_edge << " " << current.pathTime << std::endl;
                        bestPath.push_back(current.reaching_edge);
                        current = allNodes[current.parent];
                    }
                    std::reverse(bestPath.begin(), bestPath.end());
                    return;                    
                }
            }

            // Loop through neighbouring intersections
            std::vector<unsigned> edge = find_intersection_street_segments(current.id);

            for (unsigned i = 0; i < edge.size(); i++) {
                StreetSegmentInfo info = getStreetSegmentInfo(edge[i]);
                if (info.from == current.id) {
                    // If the neighbouring node is not visited
                    if (!allNodes[info.to].explored) {
                        double edge_time = find_street_segment_travel_time(edge[i]);
                        if (current.reaching_edge != NO_EDGE && (info.streetID != getStreetSegmentInfo(current.reaching_edge).streetID))
                            edge_time = edge_time + turn_penalty;
                        // If not in the queue
                        auto it_f = frontier_set.find(info.to);
                        if (it_f == frontier_set.end()) {
                            searchNode temp(info.to, current.pathTime + edge_time, edge[i], current.id, false);
                            frontier.push(temp);
                            frontier_set.insert(info.to);
                            allNodes[info.to] = temp;
                        }
                            // If the neighbouring node in the queue has higher cost, replace with current one
                        else if ((current.pathTime + edge_time) < allNodes[info.to].pathTime) {
                            searchNode temp(info.to, current.pathTime + edge_time, edge[i], current.id, false);
                            allNodes[info.to] = temp;
                            frontier.push(temp);
                        }
                    }
                } else if (!info.oneWay) {
                    // If not visited
                    if (!allNodes[info.from].explored) {
                        double edge_time = find_street_segment_travel_time(edge[i]);
                        if (current.reaching_edge != NO_EDGE && (info.streetID != getStreetSegmentInfo(current.reaching_edge).streetID))
                            edge_time = edge_time + turn_penalty;
                        auto it_f = frontier_set.find(info.from);
                        // If not in the queue
                        if (it_f == frontier_set.end()) {
                            searchNode temp(info.from, current.pathTime + edge_time, edge[i], current.id, false);
                            frontier.push(temp);
                            frontier_set.insert(info.from);
                            allNodes[info.from] = temp;
                        }                            // If the node in the queue has higher cost
                        else if ((current.pathTime + edge_time) < allNodes[info.from].pathTime) {
                            searchNode temp(info.from, current.pathTime + edge_time, edge[i], current.id, false);
                            allNodes[info.from] = temp;
                            frontier.push(temp);
                        }
                    }
                }
            }
        }
    }
}

void UCS_delivery(unsigned start, std::unordered_map<unsigned, unsigned>& end_points, double turn_penalty, std::vector<path_info>& all_path) {

//    unsigned temp_type = end_points[start];
//    end_points.erase(start);
    all_path.resize(end_points.size() - 1);
    //std::unordered_set<unsigned> id_stored;
    
    // A structure that contains all the nodes
    std::vector<searchNode> allNodes;
    allNodes.resize(getNumberOfIntersections());

    // Declare the root of the graph
    searchNode root(start, 0, NO_PARENT, NO_EDGE, false);

    // Intialize the start node
    allNodes[start] = root;

    // Priority queue
    std::priority_queue<searchNode, std::vector<searchNode>, greaterTime> frontier;

    // For checking whether the node is in the queue
    std::unordered_set<unsigned> frontier_set;

    // Insert the root into queue
    frontier.push(root);
    frontier_set.insert(root.id);
    
    unsigned j = 0;

    // Return fail when the queue is empty
    while (!frontier.empty()) {
        // Get the minimum node in the queue and erase that
        searchNode current = frontier.top();
        frontier.pop();
        frontier_set.erase(current.id);
        if (!allNodes[current.id].explored) {
            //std::cout << "not visited node: " << current.id << std::endl;
            // Set the node as visited
            allNodes[current.id].explored = true;
            if (j == all_path.size()) {
                //end_points.insert(std::make_pair(start, temp_type));
                std::sort(all_path.begin(), all_path.end());
                return;
            }
            else {
                // When found the best route, return result
                for (auto it = end_points.begin(); it != end_points.end(); it++) {
                    if (current.id == it -> first && current.id != start) {

                        //std::cout << "test iterator value: " << it -> first << std::endl;
                        //std::cout << "test current value: " << current.id<< std::endl;
                        //id_stored.insert(current.id);
                        all_path[j].end_id = current.id;
                        //std::cout << "test stored id: " << all_path[j].end_id << std::endl;
                        all_path[j].travel_time = current.pathTime;
                        //std::cout << "test stored travel time: " << all_path[j].travel_time << std::endl;
                        all_path[j].type = end_points[current.id];
                        //std::cout << "test stored type: " << all_path[j].type << std::endl;
                        // Track back to start, store the segments
                        while (current.parent != NO_PARENT) {
                            //std::cout << "reverse: " << i << " " << current.reaching_edge << " " << current.pathTime << std::endl;
                            all_path[j].path.push_back(current.reaching_edge);
                            current = allNodes[current.parent];
                        }
                        std::reverse(all_path[j].path.begin(), all_path[j].path.end());
                        j = j + 1;                  
                    }
                }

                // Loop through neighbouring intersections
                std::vector<unsigned> edge = find_intersection_street_segments(current.id);

                for (unsigned i = 0; i < edge.size(); i++) {
                    StreetSegmentInfo info = getStreetSegmentInfo(edge[i]);
                    if (info.from == current.id) {
                        // If the neighbouring node is not visited
                        if (!allNodes[info.to].explored) {
                            double edge_time = find_street_segment_travel_time(edge[i]);
                            if (current.reaching_edge != NO_EDGE && (info.streetID != getStreetSegmentInfo(current.reaching_edge).streetID))
                                edge_time = edge_time + turn_penalty;
                            // If not in the queue
                            auto it_f = frontier_set.find(info.to);
                            if (it_f == frontier_set.end()) {
                                searchNode temp(info.to, current.pathTime + edge_time, edge[i], current.id, false);
                                frontier.push(temp);
                                frontier_set.insert(info.to);
                                allNodes[info.to] = temp;
                            }
                                // If the neighbouring node in the queue has higher cost, replace with current one
                            else if ((current.pathTime + edge_time) < allNodes[info.to].pathTime) {
                                searchNode temp(info.to, current.pathTime + edge_time, edge[i], current.id, false);
                                allNodes[info.to] = temp;
                                frontier.push(temp);
                            }
                        }
                    } else if (!info.oneWay) {
                        // If not visited
                        if (!allNodes[info.from].explored) {
                            double edge_time = find_street_segment_travel_time(edge[i]);
                            if (current.reaching_edge != NO_EDGE && (info.streetID != getStreetSegmentInfo(current.reaching_edge).streetID))
                                edge_time = edge_time + turn_penalty;
                            auto it_f = frontier_set.find(info.from);
                            // If not in the queue
                            if (it_f == frontier_set.end()) {
                                searchNode temp(info.from, current.pathTime + edge_time, edge[i], current.id, false);
                                frontier.push(temp);
                                frontier_set.insert(info.from);
                                allNodes[info.from] = temp;
                            }                            // If the node in the queue has higher cost
                            else if ((current.pathTime + edge_time) < allNodes[info.from].pathTime) {
                                searchNode temp(info.from, current.pathTime + edge_time, edge[i], current.id, false);
                                allNodes[info.from] = temp;
                                frontier.push(temp);
                            }
                        }
                    }
                }           
            }

        }
    } 
}

void UCS_depot(unsigned start, std::unordered_map<unsigned, unsigned>& end_points, double turn_penalty, std::vector<path_info>& all_path) {

//    unsigned temp_type = end_points[start];
//    end_points.erase(start);
    all_path.resize(end_points.size());
    //std::unordered_set<unsigned> id_stored;
    
    // A structure that contains all the nodes
    std::vector<searchNode> allNodes;
    allNodes.resize(getNumberOfIntersections());

    // Declare the root of the graph
    searchNode root(start, 0, NO_PARENT, NO_EDGE, false);

    // Intialize the start node
    allNodes[start] = root;

    // Priority queue
    std::priority_queue<searchNode, std::vector<searchNode>, greaterTime> frontier;

    // For checking whether the node is in the queue
    std::unordered_set<unsigned> frontier_set;

    // Insert the root into queue
    frontier.push(root);
    frontier_set.insert(root.id);
    
    unsigned j = 0;

    // Return fail when the queue is empty
    while (!frontier.empty()) {
        // Get the minimum node in the queue and erase that
        searchNode current = frontier.top();
        frontier.pop();
        frontier_set.erase(current.id);
        if (!allNodes[current.id].explored) {
            //std::cout << "not visited node: " << current.id << std::endl;
            // Set the node as visited
            allNodes[current.id].explored = true;
            if (j == all_path.size()) {
                //end_points.insert(std::make_pair(start, temp_type));
                std::sort(all_path.begin(), all_path.end());
                return;
            }
            else {
                // When found the best route, return result
                for (auto it = end_points.begin(); it != end_points.end(); it++) {
                    if (current.id == it -> first && current.id != start) {

                        //std::cout << "test iterator value: " << it -> first << std::endl;
                        //std::cout << "test current value: " << current.id<< std::endl;
                        //id_stored.insert(current.id);
                        all_path[j].end_id = current.id;
                        //std::cout << "test stored id: " << all_path[j].end_id << std::endl;
                        all_path[j].travel_time = current.pathTime;
                        //std::cout << "test stored travel time: " << all_path[j].travel_time << std::endl;
                        all_path[j].type = end_points[current.id];
                        //std::cout << "test stored type: " << all_path[j].type << std::endl;
                        // Track back to start, store the segments
                        while (current.parent != NO_PARENT) {
                            //std::cout << "reverse: " << i << " " << current.reaching_edge << " " << current.pathTime << std::endl;
                            all_path[j].path.push_back(current.reaching_edge);
                            current = allNodes[current.parent];
                        }
                        std::reverse(all_path[j].path.begin(), all_path[j].path.end());
                        j = j + 1;                  
                    }
                }

                // Loop through neighbouring intersections
                std::vector<unsigned> edge = find_intersection_street_segments(current.id);

                for (unsigned i = 0; i < edge.size(); i++) {
                    StreetSegmentInfo info = getStreetSegmentInfo(edge[i]);
                    if (info.from == current.id) {
                        // If the neighbouring node is not visited
                        if (!allNodes[info.to].explored) {
                            double edge_time = find_street_segment_travel_time(edge[i]);
                            if (current.reaching_edge != NO_EDGE && (info.streetID != getStreetSegmentInfo(current.reaching_edge).streetID))
                                edge_time = edge_time + turn_penalty;
                            // If not in the queue
                            auto it_f = frontier_set.find(info.to);
                            if (it_f == frontier_set.end()) {
                                searchNode temp(info.to, current.pathTime + edge_time, edge[i], current.id, false);
                                frontier.push(temp);
                                frontier_set.insert(info.to);
                                allNodes[info.to] = temp;
                            }
                                // If the neighbouring node in the queue has higher cost, replace with current one
                            else if ((current.pathTime + edge_time) < allNodes[info.to].pathTime) {
                                searchNode temp(info.to, current.pathTime + edge_time, edge[i], current.id, false);
                                allNodes[info.to] = temp;
                                frontier.push(temp);
                            }
                        }
                    } else if (!info.oneWay) {
                        // If not visited
                        if (!allNodes[info.from].explored) {
                            double edge_time = find_street_segment_travel_time(edge[i]);
                            if (current.reaching_edge != NO_EDGE && (info.streetID != getStreetSegmentInfo(current.reaching_edge).streetID))
                                edge_time = edge_time + turn_penalty;
                            auto it_f = frontier_set.find(info.from);
                            // If not in the queue
                            if (it_f == frontier_set.end()) {
                                searchNode temp(info.from, current.pathTime + edge_time, edge[i], current.id, false);
                                frontier.push(temp);
                                frontier_set.insert(info.from);
                                allNodes[info.from] = temp;
                            }                            // If the node in the queue has higher cost
                            else if ((current.pathTime + edge_time) < allNodes[info.from].pathTime) {
                                searchNode temp(info.from, current.pathTime + edge_time, edge[i], current.id, false);
                                allNodes[info.from] = temp;
                                frontier.push(temp);
                            }
                        }
                    }
                }           
            }

        }
    }
}


bool UCS_Greedy_start(unsigned start, double turn_penalty, unsigned& stop_id, std::vector<unsigned>& bestPath, 
                      std::vector<save_state>& pick_state) {

    // A structure that contains all the nodes
    std::vector<searchNode> allNodes;
    allNodes.resize(getNumberOfIntersections());

    // Declare the root of the graph
    searchNode root(start, 0, NO_PARENT, NO_EDGE, false);

    // Intialize the start node
    allNodes[start] = root;

    // Priority queue
    std::priority_queue<searchNode, std::vector<searchNode>, greaterTime> frontier;

    // For checking whether the node is in the queue
    std::unordered_set<unsigned> frontier_set;

    // Insert the root into queue
    frontier.push(root);
    frontier_set.insert(root.id);

    // Return fail when the queue is empty
    while (!frontier.empty()) {
        // Get the minimum node in the queue and erase that
        searchNode current = frontier.top();
        frontier.pop();
        frontier_set.erase(current.id);
        if (!allNodes[current.id].explored) {
            // Set the node as visited
            allNodes[current.id].explored = true;

            // When found the best route, return result
            if (pick_state[current.id].explored == 1) {
                stop_id = current.id;
                // Track back to start, store the segments
                while (current.parent != NO_PARENT) {
                    //std::cout << "reverse: " << i << " " << current.reaching_edge << " " << current.pathTime << std::endl;
                    bestPath.push_back(current.reaching_edge);
                    current = allNodes[current.parent];
                }
                std::reverse(bestPath.begin(), bestPath.end());
                return true;
            }


            // Loop through neighbouring intersections
            std::vector<unsigned> edge = find_intersection_street_segments(current.id);

            for (unsigned i = 0; i < edge.size(); i++) {
                StreetSegmentInfo info = getStreetSegmentInfo(edge[i]);
                if (info.from == current.id) {
                    // If the neighbouring node is not visited
                    if (!allNodes[info.to].explored) {
                        double edge_time = find_street_segment_travel_time(edge[i]);
                        if (current.reaching_edge != NO_EDGE && (info.streetID != getStreetSegmentInfo(current.reaching_edge).streetID))
                            edge_time = edge_time + turn_penalty;
                        // If not in the queue
                        auto it_f = frontier_set.find(info.to);
                        if (it_f == frontier_set.end()) {
                            searchNode temp(info.to, current.pathTime + edge_time, edge[i], current.id, false);
                            frontier.push(temp);
                            frontier_set.insert(info.to);
                            allNodes[info.to] = temp;
                        }
                            // If the neighbouring node in the queue has higher cost, replace with current one
                        else if ((current.pathTime + edge_time) < allNodes[info.to].pathTime) {
                            searchNode temp(info.to, current.pathTime + edge_time, edge[i], current.id, false);
                            allNodes[info.to] = temp;
                            frontier.push(temp);
                        }
                    }
                } else if (!info.oneWay) {
                    // If not visited
                    if (!allNodes[info.from].explored) {
                        double edge_time = find_street_segment_travel_time(edge[i]);
                        if (current.reaching_edge != NO_EDGE && (info.streetID != getStreetSegmentInfo(current.reaching_edge).streetID))
                            edge_time = edge_time + turn_penalty;
                        auto it_f = frontier_set.find(info.from);
                        // If not in the queue
                        if (it_f == frontier_set.end()) {
                            searchNode temp(info.from, current.pathTime + edge_time, edge[i], current.id, false);
                            frontier.push(temp);
                            frontier_set.insert(info.from);
                            allNodes[info.from] = temp;
                        }                            // If the node in the queue has higher cost
                        else if ((current.pathTime + edge_time) < allNodes[info.from].pathTime) {
                            searchNode temp(info.from, current.pathTime + edge_time, edge[i], current.id, false);
                            allNodes[info.from] = temp;
                            frontier.push(temp);
                        }
                    }
                }
            }
        }
    }
    return false;
}

void UCS_Greedy_search(double turn_penalty, std::vector<unsigned>& path, std::vector<delivery_info>& pick_up, 
                       std::vector<save_state>& pick_state, std::vector<delivery_info>& drop_off, 
                       std::vector<save_state>& drop_state, std::vector<delivery_info>& passed_intersection, std::vector<bool>& has_picked) {
    
    for (unsigned i = 0; i < pick_up.size() + drop_off.size() - 1; i++) {
        if(drop_state[passed_intersection[i].id].explored){
            unsigned pack_picked = 0;
            for (unsigned j = 0; j < drop_off[drop_state[passed_intersection[i].id].access_id].relation.size(); j++) {
                if (has_picked[drop_off[drop_state[passed_intersection[j].id].access_id].relation[j]])
                    pack_picked ++;
                else
                    break;
            }
            if (pack_picked == drop_off[drop_state[passed_intersection[i].id].access_id].relation.size()){
                drop_state[passed_intersection[i].id].explored = 0;
                passed_intersection.push_back(drop_off[drop_state[passed_intersection[i].id].access_id]);
                continue;
            }
        }
        else if (pick_state[passed_intersection[i].id].explored) {
            has_picked[passed_intersection[i].id] = 1;
            pick_state[passed_intersection[i].id].explored = 0;
            passed_intersection.push_back(pick_up[pick_state[passed_intersection[i].id].access_id]);
            continue;
        }
        else if(drop_state[passed_intersection[i].id].explored){
            unsigned pack_picked = 0;
            for (unsigned j = 0; j < drop_off[drop_state[passed_intersection[i].id].access_id].relation.size(); i++) {
                if (!has_picked[drop_off[drop_state[passed_intersection[i].id].access_id].relation[i]])
                    break;
                else
                    pack_picked++;
            }
            if (pack_picked == drop_off[drop_state[passed_intersection[i].id].access_id].relation.size()){
                drop_state[passed_intersection[i].id].explored = 0;
                passed_intersection.push_back(drop_off[drop_state[passed_intersection[i].id].access_id]);
                continue;
            }
        }
        // A structure that contains all the nodes
        std::vector<searchNode> allNodes;
        allNodes.resize(getNumberOfIntersections());

        // Declare the root of the graph
        searchNode root(passed_intersection[i].id, 0, NO_PARENT, NO_EDGE, false);

        // Intialize the start node
        allNodes[passed_intersection[i].id] = root;

        // Priority queue
        std::priority_queue<searchNode, std::vector<searchNode>, greaterTime> frontier;

        // For checking whether the node is in the queue
        std::unordered_set<unsigned> frontier_set;

        // Insert the root into queue
        frontier.push(root);
        frontier_set.insert(root.id);

        // Return fail when the queue is empty
        while (!frontier.empty()) {
            // Get the minimum node in the queue and erase that
            searchNode current = frontier.top();
            frontier.pop();
            frontier_set.erase(current.id);
            if (!allNodes[current.id].explored) {
                // Set the node as visited
                allNodes[current.id].explored = true;
                
                if (pick_state[current.id].explored) {
                    has_picked[current.id] = 1;
                    pick_state[current.id].explored = 0;
                    passed_intersection.push_back(pick_up[pick_state[current.id].access_id]);
                    std::vector<unsigned> temp;
                    while (current.parent != NO_PARENT) {
                        //std::cout << "reverse: " << i << " " << current.reaching_edge << " " << current.pathTime << std::endl;
                        temp.push_back(current.reaching_edge);
                        current = allNodes[current.parent];
                    }
                    std::reverse(temp.begin(), temp.end());
                    path.insert(path.end(), temp.begin(), temp.end());
                    break;
                }
                else if (drop_state[current.id].explored) {
                    unsigned pack_picked = 0;
                    for (unsigned j = 0; j < drop_off[drop_state[current.id].access_id].relation.size(); j++) {
                        if (!has_picked[drop_off[drop_state[current.id].access_id].relation[j]]) 
                            break;
                        else 
                            pack_picked ++;                       
                    }
                    if (pack_picked == drop_off[drop_state[current.id].access_id].relation.size()) {
                        drop_state[current.id].explored= 0;
                        passed_intersection.push_back(drop_off[drop_state[current.id].access_id]);
                        std::vector<unsigned> temp;
                        while (current.parent != NO_PARENT) {
                        //std::cout << "reverse: " << i << " " << current.reaching_edge << " " << current.pathTime << std::endl;
                        temp.push_back(current.reaching_edge);
                        current = allNodes[current.parent];
                        }
                        std::reverse(temp.begin(), temp.end());
                        path.insert(path.end(), temp.begin(), temp.end());
                        break;
                    } 
                }

                // Loop through neighbouring intersections
                std::vector<unsigned> edge = find_intersection_street_segments(current.id);

                for (unsigned j = 0; j < edge.size(); j++) {
                    StreetSegmentInfo info = getStreetSegmentInfo(edge[j]);
                    if (info.from == current.id) {
                        // If the neighbouring node is not visited
                        if (!allNodes[info.to].explored) {
                            double edge_time = find_street_segment_travel_time(edge[j]);
                            if (current.reaching_edge != NO_EDGE && (info.streetID != getStreetSegmentInfo(current.reaching_edge).streetID))
                                edge_time = edge_time + turn_penalty;
                            // If not in the queue
                            auto it_f = frontier_set.find(info.to);
                            if (it_f == frontier_set.end()) {
                                searchNode temp(info.to, current.pathTime + edge_time, edge[j], current.id, false);
                                frontier.push(temp);
                                frontier_set.insert(info.to);
                                allNodes[info.to] = temp;
                            }
                                // If the neighbouring node in the queue has higher cost, replace with current one
                            else if ((current.pathTime + edge_time) < allNodes[info.to].pathTime) {
                                searchNode temp(info.to, current.pathTime + edge_time, edge[j], current.id, false);
                                allNodes[info.to] = temp;
                                frontier.push(temp);
                            }
                        }
                    } else if (!info.oneWay) {
                        // If not visited
                        if (!allNodes[info.from].explored) {
                            double edge_time = find_street_segment_travel_time(edge[j]);
                            if (current.reaching_edge != NO_EDGE && (info.streetID != getStreetSegmentInfo(current.reaching_edge).streetID))
                                edge_time = edge_time + turn_penalty;
                            auto it_f = frontier_set.find(info.from);
                            // If not in the queue
                            if (it_f == frontier_set.end()) {
                                searchNode temp(info.from, current.pathTime + edge_time, edge[j], current.id, false);
                                frontier.push(temp);
                                frontier_set.insert(info.from);
                                allNodes[info.from] = temp;
                            }                            // If the node in the queue has higher cost
                            else if ((current.pathTime + edge_time) < allNodes[info.from].pathTime) {
                                searchNode temp(info.from, current.pathTime + edge_time, edge[j], current.id, false);
                                allNodes[info.from] = temp;
                                frontier.push(temp);
                            }
                        }
                    }
                }
            }
        }
    }
}

void UCS_Greedy_end(double turn_penalty, std::vector<unsigned>& path, const std::vector<unsigned>& depots, 
                    std::vector<delivery_info>& passed_intersection, unsigned& end_point) {
    
    // A structure that contains all the nodes
    std::vector<searchNode> allNodes;
    allNodes.resize(getNumberOfIntersections());

    // Declare the root of the graph
    searchNode root(passed_intersection.back().id, 0, NO_PARENT, NO_EDGE, false);

    // Intialize the start node
    allNodes[passed_intersection.back().id] = root;

    // Priority queue
    std::priority_queue<searchNode, std::vector<searchNode>, greaterTime> frontier;

    // For checking whether the node is in the queue
    std::unordered_set<unsigned> frontier_set;

    // Insert the root into queue
    frontier.push(root);
    frontier_set.insert(root.id);

    // Return fail when the queue is empty
    while (!frontier.empty()) {
        // Get the minimum node in the queue and erase that
        searchNode current = frontier.top();
        frontier.pop();
        frontier_set.erase(current.id);
        if (!allNodes[current.id].explored) {
            // Set the node as visited
            allNodes[current.id].explored = true;

            // When found the best route, return result
            for (unsigned i = 0; i < depots.size(); i++) {
                if (current.id == depots[i]) {
                    end_point = depots[i];
                    std::vector<unsigned> temp;
                    // Track back to start, store the segments
                    while (current.parent != NO_PARENT) {
                        //std::cout << "reverse: " << i << " " << current.reaching_edge << " " << current.pathTime << std::endl;
                        temp.push_back(current.reaching_edge);
                        current = allNodes[current.parent];
                    }
                    std::reverse(temp.begin(), temp.end());
                    path.insert(path.end(), temp.begin(), temp.end());
                    return;                    
                }
            }

            // Loop through neighbouring intersections
            std::vector<unsigned> edge = find_intersection_street_segments(current.id);

            for (unsigned i = 0; i < edge.size(); i++) {
                StreetSegmentInfo info = getStreetSegmentInfo(edge[i]);
                if (info.from == current.id) {
                    // If the neighbouring node is not visited
                    if (!allNodes[info.to].explored) {
                        double edge_time = find_street_segment_travel_time(edge[i]);
                        if (current.reaching_edge != NO_EDGE && (info.streetID != getStreetSegmentInfo(current.reaching_edge).streetID))
                            edge_time = edge_time + turn_penalty;
                        // If not in the queue
                        auto it_f = frontier_set.find(info.to);
                        if (it_f == frontier_set.end()) {
                            searchNode temp(info.to, current.pathTime + edge_time, edge[i], current.id, false);
                            frontier.push(temp);
                            frontier_set.insert(info.to);
                            allNodes[info.to] = temp;
                        }
                            // If the neighbouring node in the queue has higher cost, replace with current one
                        else if ((current.pathTime + edge_time) < allNodes[info.to].pathTime) {
                            searchNode temp(info.to, current.pathTime + edge_time, edge[i], current.id, false);
                            allNodes[info.to] = temp;
                            frontier.push(temp);
                        }
                    }
                } else if (!info.oneWay) {
                    // If not visited
                    if (!allNodes[info.from].explored) {
                        double edge_time = find_street_segment_travel_time(edge[i]);
                        if (current.reaching_edge != NO_EDGE && (info.streetID != getStreetSegmentInfo(current.reaching_edge).streetID))
                            edge_time = edge_time + turn_penalty;
                        auto it_f = frontier_set.find(info.from);
                        // If not in the queue
                        if (it_f == frontier_set.end()) {
                            searchNode temp(info.from, current.pathTime + edge_time, edge[i], current.id, false);
                            frontier.push(temp);
                            frontier_set.insert(info.from);
                            allNodes[info.from] = temp;
                        }                            // If the node in the queue has higher cost
                        else if ((current.pathTime + edge_time) < allNodes[info.from].pathTime) {
                            searchNode temp(info.from, current.pathTime + edge_time, edge[i], current.id, false);
                            allNodes[info.from] = temp;
                            frontier.push(temp);
                        }
                    }
                }
            }
        }
    }
}
