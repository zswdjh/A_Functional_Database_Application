//
//  AVLTree.hpp
//  ECEDatabase5
//
//  Added by rick gessner on 4/27/18.
//  From version by Domenico Porcino
//

#ifndef AVLTree_h
#define AVLTree_h

#include <queue>

const int kMaxDepth =32;

namespace SF {

  template<class KeyType, class ItemType>
  struct Node {
    Node(KeyType aKey, ItemType aValue) :
    balance(0), depth(0), key(aKey), value(aValue), left(0), right(0) {}
    
    KeyType  key;
    ItemType value;
    short    balance;
    short    depth;
    
    Node* left;
    Node* right;
  };

  template<class KeyType, class ItemType>
  class AVLTree {
  public:
    
    typedef Node<KeyType, ItemType> NodeType;
    
    NodeType *root;
    
    AVLTree() : root(0) { }
    ~AVLTree() { }
    
    NodeType* getRoot() {return root;}
    
    void insert(KeyType key, ItemType item) {
      if (!root) {
        root = new NodeType(key, item);
      }
      else _Insert(key, item, root);
    }
    
    void remove(KeyType key) {
      bool delOK = false;
      _Remove(root, key, delOK);
    }
    
    bool find(KeyType aKey, ItemType& aValue) {
      return _Find(aKey, aValue, root);
    }
    
    short getDepth() const {  return (root ? root->depth : 0);  }
    
    //-----------------------------------------------
    // Notify callback for every node (breadth-first)
    //-----------------------------------------------
    
    struct NodeObserver {
      virtual void operator()(NodeType &aNode)=0;
    };
    
    //---------------------------------------------
    // ...
    //---------------------------------------------
    
    class BreadthFirstVisitor {      
    protected:
      std::queue<NodeType*> queue;
      
    public:
      
      BreadthFirstVisitor(NodeType *aRoot) {
        if(aRoot) queue.push(aRoot);
      }
      
      void each(NodeObserver &anObserver) {
        while (queue.size()) {
          auto theNode = queue.front();

          anObserver(*theNode);

          if (nullptr!=theNode->left) {
            queue.push(theNode->left);
          }
          if (nullptr!=theNode->right) {
            queue.push(theNode->right);
          }
          queue.pop(); //yank the current first node...
        }
      }
    };
    
    //---------------------------------------------
    // ITERATOR...
    //---------------------------------------------
    
    class AVLIterator {
    public:
      
      AVLIterator(AVLIterator* aCopy) :
      tree(aCopy->tree), current(aCopy->current) {
        stackIndex = aCopy->stackIndex;
        
        for (int i = 0; i < stackIndex; ++i)
          stack[i] = aCopy->stack[i];
      }
      
      AVLIterator(AVLTree *aTree) : tree(aTree) {
        KeyType key;
        ItemType item;
        getFirst(key, item);
      }
      
      ~AVLIterator() { }
      
      bool getCurrent(KeyType& aKey, ItemType& aValue) {
        if (current) {
          aKey = current->key;
          aValue = current->value;
          return true;
        }
        return false;
      }
      
      // returns false if the tree is empty
      bool getFirst(KeyType& aKey, ItemType& aValue) {
        stackIndex = 0;
        
        if (!tree->root) {
          current = 0;
          aValue = 0;
          return false;
        }
        else {
          NodeType* theCurr = tree->root;
          NodeType* thePrev = theCurr;
          while (theCurr) {
            thePrev = theCurr;
            if (theCurr->left)
              stack[stackIndex++] = theCurr;
            theCurr = theCurr->left;
          }
          
          current = thePrev;
          aKey = current->key;
          aValue = current->value;
          return true;
        }
      }
      
      bool getNext(KeyType& aKey, ItemType& aValue) {
        if (!current) {
          aValue = 0;
          return false;
        }
        
        NodeType* theCurr = current->right;  // start looking to the right
        
        while (true) { // this while forces a traversal as far left as possible
          if (theCurr){  // if we have a pcurr, push it and go left, and repeat.
            stack[stackIndex++] = theCurr;
            theCurr = theCurr->left;
          }
          else { // backtrack
            if (stackIndex > 0) {
              
              NodeType* pCandidate = stack[--stackIndex];
              
              // did we backtrack up a right branch?
              if (current == pCandidate->right) {
                // if there is a parent, return the parent.
                if (stackIndex > 0) {
                  current = stack[--stackIndex];
                  aKey = current->key;
                  aValue = current->value;
                  return true;
                }
                else  { // if up a right branch, and no parent, traversal finished
                  current = 0;
                  aValue = 0;
                  return false;
                }
              }
              else { // up a left branch, done.
                current = pCandidate;
                aKey = current->key;
                aValue = current->value;
                return true;
              }
            }
            else{  // totally done
              current = 0;
              aValue = 0;
              return false;
            }
          }
        }
      }
      
      bool find(KeyType aKey, ItemType& aValue) {
        NodeType* pCurr = tree->root;
        stackIndex = 0;
        
        while (true) {
          NodeType* pPushMe = pCurr;
          if (pCurr->m_Key == aKey) { // already done?
            current = pCurr;
            aValue = current->value;
            return true;
          }
          
          if (pCurr->key > aKey)
            pCurr = pCurr->left;
          else
            pCurr = pCurr->right;
          
          if (pCurr) { // maintain the stack so that GetNext will work.
            stack[stackIndex++] = pPushMe;
          }
          else { // couldn't find it.
            current = 0;
            stackIndex = 0;
            return false;
          }
        }
        
        return true;
      }
      
    protected:
      AVLTree*    tree;
      NodeType*   stack[kMaxDepth];
      short       stackIndex;
      NodeType*   current;        // for iteration
    }; //iterator
    
    friend class AVLIterator;
    
  protected:
    
    void _Insert(KeyType key, ItemType item, NodeType *&root) {
      if (key < root->key) {
        if (root->left)
          _Insert(key, item, root->left);
        else
          root->left = new NodeType(key, item);
      }
      else if (key > root->key) {
        if (root->right)
          _Insert(key, item, root->right);
        else
          root->right = new NodeType(key, item);
      }
      else {
        // error - can't have duplicate keys.
        // if duplicate keys are okay, change key < to key <= above
      }
      
      computeBalance(root);
      balance(root);
    }
    
    void _Remove(NodeType*& root, KeyType key, bool& delOK) {
      if (!root) {
        delOK = false;
      }
      else if (root->m_Key > key) {  // go to left subtree
        _Remove(root->left, key, delOK);
        if (delOK) {
          computeBalance(root);
          balanceRight(root);
        }
      }
      else if (root->key < key) { // go to right subtree
        _Remove(root->right, key, delOK);
        if (delOK) {
          computeBalance(root);
          balanceLeft(root);
        }
      }
      else {  // node found!
        NodeType* pMe = root;
        if (!root->right) {
          root = root->left;
          delOK = true;
          delete pMe;
        }
        else if (!root->left){
          root = root->right;
          delOK = true;
          delete pMe;
        }
        else {
          _RemoveBothChildren(root, root->left, delOK);
          if (delOK) {
            computeBalance(root);
            balance(root);
          }
          delOK = true;
        }
      }
    }
    
    void _RemoveBothChildren(NodeType* &root, NodeType*& curr, bool& delOK) {
      if (!curr->right){
        root->key = curr->key;
        root->value = curr->value;
        NodeType* pMe = curr;
        curr = curr->left;
        delete pMe;
        delOK = true;
      }
      else {
        _RemoveBothChildren(root, curr->right, delOK);
        if (delOK) {
          computeBalance(root);
          balance(root);
        }
      }
    }
    
    bool _Find(KeyType key, ItemType& item, NodeType* root) {
      if (root) {
        if (root->key == key) {
          item = root->value;
          return true;
        }
        return (key < root->key)
        ? _Find(key, item, root->left)
        : _Find(key, item, root->right);
      }
      return false;
    }
    
    void computeBalance(NodeType*  root) {
      if (root) {
        short leftDepth  = root->left  ? root->left->depth  : 0;
        short rightDepth = root->right ? root->right->depth : 0;
        
        root->depth = 1 + ((leftDepth > rightDepth) ? leftDepth : rightDepth);
        root->balance = rightDepth - leftDepth;
      }
    }
    
    void balance(NodeType*& root) {
      if (root->balance > 1)
        balanceRight(root);
      
      if (root->balance < -1)
        balanceLeft(root);
    }
    
    void balanceRight(NodeType*& root) {
      if (root->right) {
        if (root->right->balance > 0) {
          rotateLeft(root);
        }
        else if (root->right->balance < 0) {
          rotateRight(root->right);
          rotateLeft(root);
        }
      }
    }
    
    void balanceLeft(NodeType*& root) {
      if (root->left) {
        if (root->left->balance < 0) {
          rotateRight(root);
        }
        else if (root->left->balance > 0) {
          rotateLeft(root->left);
          rotateRight(root);
        }
      }
    }
    
    void rotateLeft(NodeType*& root) {
      NodeType* pTemp = root;
      root = root->right;
      pTemp->right = root->left;
      root->left = pTemp;
      
      computeBalance(root->left);
      computeBalance(root->right);
      computeBalance(root);
    }
    
    void rotateRight(NodeType*& root) {
      NodeType* pTemp = root;
      root = root->left;
      pTemp->left = root->right;
      root->right = pTemp;
      
      computeBalance(root->left);
      computeBalance(root->right);
      computeBalance(root);
    }
    
  };
  
}


#endif /* AVLTree_h */
