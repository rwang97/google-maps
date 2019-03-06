/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   TST.h
 * Author: wangy610
 *
 * Created on February 24, 2018, 3:00 AM
 */

//TST: http://simplestcodings.blogspot.ca/2016/10/ternary-search-tree-implementation-in-c.html
//DictionaryTrie: https://github.com/migueeelvargas/Auto-Complete/blob/master/DictionaryTrie.cpp
//TST: https://www.geeksforgeeks.org/ternary-search-tree/
#ifndef TST_H
#define TST_H

#include "TrieNode.h"
#include <vector>
#include <map>

class TST {
    
private:

  // root for the ternary search tree
  TrieNode* root;
  
  // Function to traverse the tree and return a pair that contains a pointer 
  // to last character found and index of that
  void traverse(std::string name, TrieNode*& position, unsigned& index);
  
  // For helping insert function
  void insert_left_right(std::string, TrieNode*&, unsigned id);
  
  // A recursive function to get the words in the tree
  void helper_get_words(TrieNode* current, std::string keyword, std::vector<std::string>& temp);
  
  // A function to get the complete words with given keyword
  void get_complete_words(std::string keyword, std::vector<std::string>& temp);

  // helper for recursively deleting the whole tree
  void delete_all(TrieNode* _root);
  
  
public:

  TST();
  
  ~TST();
  
  // function to check whether the word is a complete word
  bool isWord(std::string name);
  
  // check whether a given name is in the tree
  bool find(std::string name);
  
  // Insert a new word into the tree
  bool insert(std::string name);

  // Search for a vector of strings that match the keyword
  void predict_words(std::vector<std::string>& found_words, std::string keyword, unsigned search_num);

};

#endif /* TST_H */


