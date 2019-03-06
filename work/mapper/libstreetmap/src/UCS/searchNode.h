/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Graph.h
 * Author: wangy610
 *
 * Created on March 16, 2018, 8:01 PM
 */

#ifndef GRAPH_H
#define GRAPH_H

class searchNode {
public:
    unsigned id;
    double pathTime;
    int reaching_edge;
    int parent;
    bool explored;
    searchNode(unsigned id_, double pathTime_, int reaching_edge_, int parent_, bool explored_);
    searchNode();
};


struct greaterTime {
    bool operator()(searchNode const & n1, searchNode const & n2) {
        return n1.pathTime > n2.pathTime;
    }
};

#endif /* GRAPH_H */

