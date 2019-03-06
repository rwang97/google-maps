/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#define TOLERANCE 3

#include "BK_Tree.h"

BK_Tree::BK_Tree() {
    root = NULL;
}

BK_Tree::~BK_Tree() {
    delete_all(root);
}

void BK_Tree::insert(BK_Node*& current, std::string word) {
    
    if (current == NULL) {
        current = new BK_Node(word);
        return;
    }

    // get the word's distance from current Node's word
    unsigned distance = Levenshtein_distance(word, current -> word);

    if (distance == 0) return;
    
    else if (current -> children[distance] == NULL) {
        current -> children[distance] = new BK_Node(word);
        return;
    }
    
    else insert(current -> children[distance], word);

}

void BK_Tree::getWords(BK_Node*& current, std::string name, std::vector<std::string>& result, unsigned search_num) {
    
    if (current == NULL)
       return;
 
    // get the word's distance from current Node's word
    unsigned distance = Levenshtein_distance(current -> word, name);
 
    // If distance is already under tolerance
    if (distance <= TOLERANCE) {
        if (result.size() == search_num)
            return;
        else {
            result.push_back(current -> word);
            distance = TOLERANCE + 1;   
        }
    }

    for (unsigned i = distance - TOLERANCE; i < distance + TOLERANCE; i++) {
        if (!current -> children.empty() && current -> children.find(i) != current -> children.end()) 
            getWords(current -> children[i], name, result, search_num);
    }
}

// helper for recursively deleting the whole tree
void BK_Tree::delete_all(BK_Node* _root) {
        
    if (_root != NULL) {
        if (!_root -> children.empty()) {
            for (auto it = _root -> children.begin(); it != _root -> children.end();it++) {
                delete_all(it -> second);     
            }
        }
        delete _root;
    }
}

BK_Node*& BK_Tree::get_root() {
    return root;
}


unsigned Levenshtein_distance(std::string& word1, std::string& word2) {
    
    unsigned len1 = word1.size();
    unsigned len2 = word2.size();
    
    std::vector<std::vector<unsigned>> distance(len1 + 1);
    for (unsigned i = 0; i < distance.size(); i ++) {
        distance[i].resize(len2 + 1);
    }

    for (unsigned i = 0; i < len1 + 1; i++) distance[i][0] = i;
    for (unsigned i = 0; i < len2 + 1; i++) distance[0][i] = i;

    for (unsigned i = 1; i < len1 + 1; i++) {
        for (unsigned j = 1; j < len2 + 1; j++) {
            if (word1[i-1] == word2[j-1])
                distance[i][j] = distance[i-1][j-1];
            else {
                distance[i][j] = std::min(1 + distance[i-1][j-1],
                                          std::min(1 + distance[i-1][j],
                                          1 + distance[i][j-1]));
            }
        }
    }
    return distance[len1][len2];
}