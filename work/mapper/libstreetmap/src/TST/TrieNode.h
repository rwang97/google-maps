/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   TrieNode.h
 * Author: wangy610
 *
 * Created on February 25, 2018, 1:56 PM
 */

// Important info found on: http://simplestcodings.blogspot.ca/2016/10/ternary-search-tree-implementation-in-c.html

#ifndef TRIENODE_H
#define TRIENODE_H

// A TrieNode for Ternary search tree
class TrieNode {
    
public:
  char data;
  TrieNode* left;
  TrieNode* right;
  TrieNode* middle;
  bool isWord = false;
  
  TrieNode(); 
  TrieNode(char _data);
};


#endif /* TRIENODE_H */

