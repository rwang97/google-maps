/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "TrieNode.h"

TrieNode::TrieNode() {
    left = 0;
    right = 0;
    middle = 0;
    isWord = false;

}

TrieNode::TrieNode(char _data) {
    data = _data;
    left = 0;
    right = 0;
    middle = 0;
    isWord = false;

}