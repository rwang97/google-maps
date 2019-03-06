/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "searchNode.h"

searchNode::searchNode() {
    id = 0;
    pathTime = 0;
    reaching_edge = -2;
    parent = -2;
    explored = false;
    
}
searchNode::searchNode(unsigned id_, double pathTime_, int reaching_edge_, int parent_, bool explored_) {
    id = id_;
    pathTime = pathTime_;
    reaching_edge = reaching_edge_;
    parent = parent_;
    explored = explored_;
}