/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   BK_Tree.h
 * Author: wangy610
 *
 * Created on March 18, 2018, 7:49 PM
 */

#ifndef BK_TREE_H
#define BK_TREE_H
#include <vector>
#include <unordered_map>

//BK Tree implmentation: https://www.geeksforgeeks.org/bk-tree-introduction-implementation/

unsigned Levenshtein_distance(std::string& word1, std::string& word2);


class BK_Node {
public:
    std::string word;
    std::unordered_map<unsigned, BK_Node*> children;
    BK_Node();   
    BK_Node(std::string word_)  {word = word_;}
};

class BK_Tree {
private:
    // root for the BK tree
    BK_Node* root;
    
    // helper for recursively deleting the whole tree
    void delete_all(BK_Node* _root);
    
public:
    BK_Tree();
    ~BK_Tree();
    
    void insert(BK_Node*& current, std::string word);
    void getWords(BK_Node*& current, std::string word, std::vector<std::string>& result, unsigned search_num);
    BK_Node*& get_root();
};

#endif /* BK_TREE_H */

