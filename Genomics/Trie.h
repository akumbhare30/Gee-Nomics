//
//  Trie.h
//  Project4
//
//  Created by Ayesha Kumbhare on 3/9/19.
//  Copyright © 2019 Ayesha Kumbhare. All rights reserved.
//

#ifndef TRIE_INCLUDED
#define TRIE_INCLUDED

#include <string>
#include <vector>
#include <iostream>

template<typename ValueType>
class Trie
{
public:
    Trie();
    ~Trie();
    void reset();
    void insert(const std::string& key, const ValueType& value);
    std::vector<ValueType> find(const std::string& key, bool exactMatchOnly) const;
    
    // C++11 syntax for preventing copying and assignment
    Trie(const Trie&) = delete;
    Trie& operator=(const Trie&) = delete;
private:
    struct Node
    {
        char m_label;
        std::vector<Node*> m_children;
        std::vector<ValueType> m_values;
    };
    Node* root;
    void freeTree(Node* curr);
    void insertHelper(const std::string &key, const ValueType &value, int index, Node* curr);
    void findHelper(const std::string& key, bool exactMatchOnly, int index, Node* curr, std::vector<ValueType> &totalValues) const;
};

template<typename ValueType>
Trie<ValueType>::Trie()
{
    root = new Node;
}

template<typename ValueType>
Trie<ValueType>::~Trie()
{
    freeTree(root);
}

template<typename ValueType>
void Trie<ValueType>::freeTree(Node* curr)
{
    if (curr == nullptr)
    {
        return;
    }
    for (int i = 0; i < curr->m_children.size(); i++)
    {
        freeTree(curr->m_children[i]);
    }
    delete curr;
}

template<typename ValueType>
void Trie<ValueType>::reset()
{
    freeTree(root);
    root = new Node;
}

template<typename ValueType>
void Trie<ValueType>::insert(const std::string &key, const ValueType &value)
{
    insertHelper(key, value, 0, root);
}

template<typename ValueType>
void Trie<ValueType>::insertHelper(const std::string &key, const ValueType &value, int index, Node* curr)
{
    if (index >= key.size()) // if we reach the end of the key string we add the value to the vector
    {
        curr->m_values.push_back(value);
        return;
    }
    
    for(int i = 0; i < curr->m_children.size(); i++)
    {
        if (curr->m_children[i]->m_label == key[index])
        {
            index++; // looking at the next character in the key
            insertHelper(key, value, index, curr->m_children[i]); // looking through it's child now
            return;
        }
    }
    // if we finish the loop and dont find the character we create a node for that character as its label and continue
    Node* inserting;
    inserting = new Node;
    inserting->m_label = key[index];
    curr->m_children.push_back(inserting); // adding inserting to curr's children
    index++;
    insertHelper(key, value, index, inserting); // call the reccursive function so that it gets to this point and adds the remaining necessary chars
}

template<typename ValueType>
std::vector<ValueType> Trie<ValueType>::find(const std::string& key, bool exactMatchOnly) const
{
    std::vector<ValueType> totalValues = std::vector<ValueType>(); // initialize as the empty vector
    for (int i = 0; i < root->m_children.size(); i++)
    {
        if (root->m_children[i]->m_label == key[0]) // looking for the the first char of key in root's children; only if one of the childs of the root has the first char of key do we proceed (otherwise, we return the empty vector)
        {
            findHelper(key, exactMatchOnly, 1, root->m_children[i], totalValues); // we search deeper
        }
    }
    return totalValues; 
}

template<typename ValueType>
void Trie<ValueType>::findHelper(const std::string& key, bool exactMatchOnly, int index, Node* curr, std::vector<ValueType>& totalValues) const // if we call this function, it ensures that the first char of the key is found
{
    
    if (key.size() == index)
    {
        totalValues.insert(totalValues.end(), curr->m_values.begin(), curr->m_values.end()); // and snips? 
        return;
    }
    
    for (int i = 0; i < curr->m_children.size(); i++)
    {
        if (curr->m_children[i]->m_label == key[index]) // if the child matches the key we are looking for
        {
            findHelper(key, exactMatchOnly, index + 1, curr->m_children[i], totalValues); // look through the child
        }
        else if(curr->m_children[i]->m_label != key[index] && !exactMatchOnly) // allowing only one mismatch (SNip)
        {
            findHelper(key, true, index + 1, curr->m_children[i], totalValues); // continue to search but no more mismatches
        }
    }
    return;
}



#endif // TRIE_INCLUDED
