//
//  Treap.hpp
//  ECEDatabase5
//
//  Created by rick gessner on 4/27/18.
//  Copyright © 2018 rick gessner. All rights reserved.
//

#ifndef Treap_hpp
#define Treap_hpp

#include <iostream>
#include <assert.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>


#include <assert.h>
#include <stdlib.h>
#include <limits.h>


// adapted from treap code by Bobby Blumofe


template <class KeyType, class ValueType>
class Treap {
  
public:
  
  class Node {   // A node in the treap.
    friend class Treap;
    uint32_t  priority; //   The priority.
    KeyType   key;      //   The key.
    ValueType value;  //   The value.
    Node*     parent; //   Pointer to parent.
    Node*     left;   //   Pointer to left child.
    Node*     right;  //   Pointer to right child.
    
  public:
    // Construct node.
    Node (void) : left (NULL), right (NULL) {}
    
    Node (uint32_t aPriority, KeyType aKey, ValueType aValue, Node* aParent)
    : priority (aPriority), key (aKey), value (aValue),
    parent (aParent), left (nullptr), right (nullptr) {}
    
    KeyType   getKey (void) const { return key; }
    ValueType getValue (void) const { return value; }
  };
  
  //-------------------------------------------------------
  
  Treap (void) : root{nullptr} {}
  
  ~Treap (void) {
    deleteTreap (root);
  }
  
  // Return value of key or 0 if not found.
  // Return a matching node (or NULL if not found).
  Node* lookup(KeyType key) const {
    return lookup_ (key);
  }
  
  // Return a matching node (or NULL if not found).
  Node* lookupGreater (KeyType key) const {
    return lookupGreater_ (key);
  }

  // Set the given key to have the given value.
  void insert(Node* n, KeyType aKey, ValueType aValue, uint32_t priority) {
    
    // Start at the root.
    Node* parent = 0;
    Node* node = root;
    
    // While subtreap rooted at node not empty...
    while (node) {
      
#if 0
      // If found, then update value and done.
      if (aKey == node->key) {
        node->value = aValue;
        return;
      }
#endif
      
      // Otherwise, search left or right subtreap.
      parent = node;
            
      if (aKey < node->key)
        node = node->left;
      else
        node = node->right;
    }
    
    
    // Not found, so create new node.
    // EDB was
    // node = new Node (lrand48(), key, value, parent);
    node = new (n) Node (priority, aKey, aValue, parent);
    // node = new Node (priority, key, value, parent);
    
    // If the treap was empty, then new node is root.
    if (!parent)
      root = node;
    
    // Otherwise, add node as left or right child.
    else if (aKey < parent->key)
      parent->left = node;
    else
      parent->right = node;
    
    // While heap property not satisfied...
    while (parent && parent->priority > node->priority) {
      
      // Perform rotation.
      if (parent->left == node)
        rotateRight (parent);
      else
        rotateLeft (parent);
      
      // Move up.
      parent = node->parent;
    }
    
    // print();
    
    // Check treap properties.
    // assert (heapProperty (root, INT_MIN));
    // assert (bstProperty (root, INT_MIN, INT_MAX));
  }
  
  // Remove entry with given key.
  // Remove entry.
  Node* remove (Node* node) {
#if 0
    // Search for node with given key.
    Node* node = lookup_ (key);
#endif
    
    // If not found, then do nothing.
    if (!node)
      return NULL;
    
    // While node is not a leaf...
    while (node->left || node->right) {
      
      // If left child only, rotate right.
      if (!node->right)
        rotateRight (node);
      
      // If right child only, rotate left.
      else if (!node->left)
        rotateLeft (node);
      
      // If both children,
      else {
        if (node->left->priority < node->right->priority)
          rotateRight (node);
        else
          rotateLeft (node);
      }
    }
    
    // Clip off node.
    Node* parent = node->parent;
    if (!parent) {
      assert (root == node);
      root = 0;
    }
    else {
      if (parent->left == node)
        parent->left = 0;
      else
        parent->right = 0;
    }
    
    // Check treap properties.
    // assert (heapProperty (root, INT_MIN));
    // assert (bstProperty (root, INT_MIN, INT_MAX));
    
#if 0
    delete node;
    return NULL;
#else
    // Return the removed node.
    
    return node;
#endif
  }
  
  //---------------------------------
  
  void print (void) const { reallyPrint (root); std::cout << std::endl; }
  
  void reallyPrint (Node * node) const {
    if (node == NULL) return;
    reallyPrint (node->left);
    std::cout << "[" << node->key << "] ";
    reallyPrint (node->right);
  }
  
private:
  
  Node* root;  // Pointer to root node of treap.
  
  // Disable copy and assignment.
  Treap (const Treap& treap) {}
  Treap& operator= (const Treap& treap) { return *this; }
  
  // Check treap properties.
  int heapProperty (Node* node, int lbound) const;
  int bstProperty (Node* node, int lbound, int ubound) const;
  
  // Delete treap rooted at given node.
  void deleteTreap (Node* node);
  
  // Return node with given key or 0 if not found.
  Node* lookup_(KeyType aKey) const {
    Node* node = root;
    
    // While subtreap rooted at node not empty...
    while (node) {
      
      // If found, then return value.
      if (aKey == node->key)
        return node;
      
      // Otherwise, search left or right subtreap.
      else if (aKey < node->key)
        node = node->left;
      else
        node = node->right;
    }
    
    // Return.
    return node;
  }

  Node* lookupGreater_ (KeyType aKey) const {
    return lookupGeq (aKey, root);
  }

  // Return node with greater or equal key or 0 if not found.

  Node* lookupGeq (KeyType aKey, Node * rt) const {
    Node * bestSoFar = NULL;
    
    // Start at the root.
    Node* node = rt;
    
    // While subtreap rooted at node not empty...
    while (node) {
      
      // If exact match found, then return value.
      if (aKey == node->key)
        return node;
      
      // Move right -- this node is too small.
      if (node->key < aKey)
        node = node->right;
      
      // Otherwise, this one's pretty good;
      // look for a better match.
      else {
        if ((bestSoFar == nullptr) || (bestSoFar->key > node->key))
          bestSoFar = node;
        node = node->left;
      }
    }
    
    // Return.
    return bestSoFar;
  }
  
  
  // Perform rotations.
  void rotateLeft (Node* node);
  void rotateRight (Node* node);
};



// Delete treap rooted at given node.
template <class KeyType, class ValueType>
void Treap<KeyType, ValueType>::deleteTreap (Node* node) {
  // If empty, nothing to do.
  if (!node)
    return;
  
  // Delete left and right subtreaps.
  deleteTreap (node->left);
  deleteTreap (node->right);
  
  // Delete root node.
  delete node;
}

// Test heap property in subtreap rooted at node.
template <class KeyType, class ValueType>
int Treap<KeyType,ValueType>::heapProperty (Node* node, int lbound) const {
  // Empty treap satisfies.
  if (!node)
    return 1;
  
  // Check priority.
  if (node->priority < lbound)
    return 0;
  
  // Check left subtreap.
  if (!heapProperty (node->left, node->priority))
    return 0;
  
  // Check right subtreap.
  if (!heapProperty (node->right, node->priority))
    return 0;
  
  // All tests passed.
  return 1;
}

// Test bst property in subtreap rooted at node.
template <class KeyType, class ValueType>
int Treap<KeyType,ValueType>::bstProperty (Node* node, int lbound, int ubound) const {
  // Empty treap satisfies.
  if (!node)
    return 1;
  
  // Check key in range.
  if (node->key < lbound || node->key > ubound)
    return 0;
  
  // Check left subtreap.
  if (!bstProperty (node->left, lbound, node->key))
    return 0;
  
  // Check right subtreap.
  if (!bstProperty (node->right, node->key, ubound))
    return 0;
  
  // All tests passed.
  return 1;
}

// Perform a left rotation.
template <class KeyType, class ValueType>
void Treap<KeyType, ValueType>::rotateLeft (Node* node) {
  // Get right child.
  Node* right = node->right;
  assert (right);
  
  // Give node right's left child.
  node->right = right->left;
  
  // Adjust parent pointers.
  if (right->left)
    right->left->parent = node;
  right->parent = node->parent;
  
  // If node is root, change root.
  if (!node->parent) {
    assert (root == node);
    root = right;
  }
  
  // Link node parent to right.
  else {
    if (node->parent->left == node)
      node->parent->left = right;
    else
      node->parent->right = right;
  }
  
  // Put node to left of right.
  right->left = node;
  node->parent = right;
}

// Perform a right rotation.
template <class KeyType, class ValueType>
void Treap<KeyType, ValueType>::rotateRight (Node* node) {
  // Get left child.
  Node* left = node->left;
  assert (left);
  
  // Give node left's right child.
  node->left = left->right;
  
  // Adjust parent pointers.
  if (left->right)
    left->right->parent = node;
  left->parent = node->parent;
  
  // If node is root, change root.
  if (!node->parent) {
    assert (root == node);
    root = left;
  }
  
  // Link node parent to left.
  else {
    if (node->parent->left == node)
      node->parent->left = left;
    else
      node->parent->right = left;
  }
  
  // Put node to right of left.
  left->right = node;
  node->parent = left;
}



#endif /* Treap_hpp */
