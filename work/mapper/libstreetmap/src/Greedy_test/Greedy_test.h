/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Greedy_test.h
 * Author: wangy610
 *
 * Created on April 5, 2018, 8:15 PM
 */

#ifndef GREEDY_TEST_H
#define GREEDY_TEST_H

#include "UCS.h"
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <cmath>
#include "m3.h"

void Greedy_test(std::vector<unsigned>& path, 
            std::unordered_map<unsigned, drop_off_info>& drop_off,
            std::unordered_map<unsigned, pick_up_info>& pick_up, 
            std::unordered_map<unsigned, std::vector<path_info>>& depot,
            unsigned size);


#endif /* GREEDY_TEST_H */

