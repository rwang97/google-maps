/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Greedy.h
 * Author: wangy610
 *
 * Created on April 6, 2018, 2:34 AM
 */

#ifndef GREEDY_H
#define GREEDY_H

#include "UCS.h"
#include "m4.h"
#include <vector>

void Greedy(double turn_penalty, std::vector<unsigned>& path, std::vector<delivery_info> pick_up, 
            std::vector<save_state> pick_state, std::vector<delivery_info> drop_off, 
            std::vector<save_state> drop_state, std::vector<delivery_info> passed_intersection, std::vector<unsigned>& intersection_path, 
            std::vector<bool> has_picked, unsigned depot, const std::vector<unsigned>& depots);

#endif /* GREEDY_H */

