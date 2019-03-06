/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   SA.h
 * Author: donganzh
 *
 * Created on April 5, 2018, 2:26 AM
 */

#ifndef SA_H
#define SA_H
#include "UCS.h"
#include <thread>
#include "m4.h"
#include "m3.h"
#include <random>
#include <iterator>
#include <chrono>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */

bool check_path_valid(std::vector<unsigned>& copy, const std::vector<DeliveryInfo>& deliveries, 
                      std::vector<std::vector<unsigned>>& intersection_delivery, 
                      unsigned start, unsigned end);

void two_opt(std::vector<unsigned>& path, const std::vector<DeliveryInfo>& deliveries,
        std::vector<std::vector<unsigned>>&intersection_delivery,
        std::vector<unsigned>& intersection_path, unsigned turn_penalty, std::chrono::high_resolution_clock::time_point& start_time);


void four_opt(std::vector<unsigned>& path, std::chrono::high_resolution_clock::time_point startTime);


#endif /* SA_H */
