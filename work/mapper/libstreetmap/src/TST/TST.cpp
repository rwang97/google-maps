/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */


#include "TST.h"

TST::TST() {
    root = NULL;
}

TST::~TST() {
    delete_all(root);
}

void TST::insert_left_right(std::string name, TrieNode*& current, unsigned id) {
    
    // add to right subtree
    if (name[id] > current -> data) {
        current-> right = new TrieNode(name[id]);
        current = current -> right;
    }        // add to left subtree
    else {
        current-> left = new TrieNode(name[id]);
        current = current -> left;
    }
}
// Insert a new word into the tree
bool TST::insert(std::string name) {
    
    // Check for empty strings and existence
    if (name.length() == 0) //|| find(name))
        return false;

    else {      
        // If not in TST, traverse TST
        TrieNode* current;
        unsigned index;
        traverse(name, current, index);

        // Check if TST is empty
        if (root == NULL) {
            root = new TrieNode(name[0]);
            current = root;
        }

        else if (index == name.length()) {
            current -> isWord = true;
            return true;
        }

        else if (current -> data != name[index]) {
            insert_left_right(name, current, index);
        }

        // add the rest to middle
        for (unsigned j = index + 1; j < name.length(); ++j) {
            current-> middle = new TrieNode(name[j]);
            current = current-> middle;
        }

        current-> isWord = true;

        return true;
    }

}

// function to check whether the word is a complete word
bool TST::isWord(std::string name) {
    
    // If TST is empty, exit.
    if (root == NULL)
        return false;
    
    TrieNode* position;
    unsigned index;

    // Traverse the tree, looking for word.
    traverse(name, position, index);

    if ((name[name.length() - 1] == position-> data) && (index == name.length() - 1) && (position -> isWord))
        return true;
    else
        return false;    
}

// check whether a given name is in the tree
bool TST::find(std::string name) {
    // If TST is empty, exit.
    if (root == NULL)
        return false;
    
    TrieNode* position;
    unsigned index;

    // Traverse the tree, looking for word.
    traverse(name, position, index);

    if ((name[name.length() - 1] == position-> data) && (index == name.length() - 1))
        return true;
    else
        return false;
}

// Function to traverse the tree and return a pair that contains a pointer 
// to last character found and index of that
void TST::traverse(std::string name, TrieNode*& position, unsigned& index) {

    position = root;
    index = 0;
    
    // if the tree is empty
    if (root == NULL) 
        return;

    TrieNode* current = root;
    
    unsigned i = 0;   

    // loop to traverse the tree
    while (i < name.length()) {   
        position = current;
        index = i;
        
        // if name[i] < the current character, go to left subtree
        if (name[i] < current -> data) {
            if (current -> left == NULL)
                return;           
            else
                current = current -> left;
        }  
        
        // if name[i] > the current character, go to the right subtree       
        else if (name[i] > current -> data) {
            if (current -> right == NULL)
                return;
            else
                current = current-> right;            
        }
        
        // if name[i] = the current character, go to the middle subtree
        else {           
            if (current -> middle == NULL || i == name.length() - 1)
                return;
            else {
                current = current -> middle;
                i = i + 1;
            }
        } 
    }
    
}

// A recursive function to get the words in the tree
void TST::helper_get_words(TrieNode* current, std::string keyword, std::vector<std::string>& temp) {

    if (current != NULL) {

        // Traverse left subtree
        helper_get_words(current -> right, keyword, temp);
        helper_get_words(current -> left, keyword, temp);

        // Check if it is a word
        if (current -> isWord) temp.push_back(keyword + current -> data);

        // Middle tree
        helper_get_words(current -> middle, keyword + current -> data, temp);
    }
}

// A function to get the complete words with given keyword
void TST::get_complete_words(std::string keyword, std::vector<std::string>& temp) {
    
    TrieNode* current = NULL;
    unsigned id = 0;
    traverse(keyword, current, id);
    
    if (current != NULL) {
        // if it is already a word
        if (current -> isWord) temp.push_back(keyword);

        // else recursively call the helper
        else helper_get_words(current -> middle, keyword, temp);
    }
}

// Search for a vector of strings that match the keyword
void TST::predict_words(std::vector<std::string>& found_words, std::string keyword, unsigned search_num) {
    
    // clear what's already in the vector
    found_words.clear();
    
    std::vector<std::string> temp;
    // check whether the user input is in the tree and whether user set searching number to 0
    if (search_num == 0 || !find(keyword))  return;

    // Finds all the words with the given prefix
    get_complete_words(keyword, temp);

    // Only loop for search_num times of the found words
    unsigned i = 0;
    for (unsigned j = 0; (i < search_num) && j < temp.size(); j++) {
        found_words.push_back(temp[j]);
        i = i + 1;
    }
}

// helper for recursively deleting the whole tree
void TST::delete_all(TrieNode* _root) {
    
    if (_root != NULL) {
        delete_all(_root->left);
        delete_all(_root->middle);
        delete_all(_root->right);
        delete _root;       
    }

}