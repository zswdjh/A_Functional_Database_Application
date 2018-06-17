//
//  AVLTree2.h
//  ECEDatabase6
//
//  based on implementation by Taro Minowa
//

#ifndef AVLTree_h
#define AVLTree_h

#include <queue>
#include <string>

//My Node class for storing data, note how I add height

namespace SF {

  template <class KeyType, class ValueType>
  class AVLTree {
    
  public:
    
    struct NodeType {
      KeyType   key;   // should be a const type
      ValueType value;
      int height;
      NodeType *left;
      NodeType *right;
      
      NodeType(const KeyType &aKey, const ValueType &aValue) : key(aKey) {
        value = aValue;
        left = nullptr;
        right = nullptr;
        height = 0;
      }
      
      void updateHeight(){
        int lHeight = 0;
        int rHeight = 0;
        if (left != nullptr) {
          lHeight = left->height;
        }
        if (right != nullptr) {
          rHeight = right->height;
        }
        int max = (lHeight > rHeight) ? lHeight : rHeight;
        height = max + 1;
      }
    };
    
    //-----------------------------------------------
    // Notify callback for every node (breadth-first)
    //-----------------------------------------------
    
    struct NodeObserver {
      virtual void operator()(NodeType &aNode)=0;
    };
    
    class BreadthFirstVisitor {
    protected:
      std::queue<NodeType*> queue;
      
    public:
      
      BreadthFirstVisitor(const BreadthFirstVisitor &aCopy) : queue() {}
      
      BreadthFirstVisitor(NodeType *aNode=nullptr) {
        if(aNode) queue.push(aNode);
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
    
    //-------------------------------
    
    AVLTree(){
      root = nullptr;
    }
    
    ~AVLTree(){
      destroy(root);
    }
    
    NodeType* getRoot() {return root;}
    
    BreadthFirstVisitor getBreadthFirstVisitor() {
      return BreadthFirstVisitor(root);
    }
    
    void insert(const KeyType &aKey, const ValueType &aValue){
       insertAt(aKey, aValue, root);
    }
      
    void remove(const KeyType &aKey){
       removeAt(aKey, root);
    }
      
    ValueType search(const KeyType &aKey){
        return searchAt(aKey,root);
    }
    
    //Rotate a Node branch to the left, in order to balance things
    NodeType* rotateLeft(NodeType *&leaf){
      NodeType* temp = leaf->right;
      leaf->right = temp->left;
      temp->left = leaf;
      
      //update the Nodes new height
      leaf->updateHeight();
      
      return temp;
    }
    
    //Rotate a Node branch to the right, in order to balance things
    NodeType* rotateRight(NodeType *&leaf){
      NodeType* temp = leaf->left;
      leaf->left = temp->right;
      temp->right = leaf;
      
      //update the Nodes new height
      leaf->updateHeight();
      
      return temp;
    }
    
    //Rotate a Node branch to the right then the left, in order to balance things
    NodeType* rotateRightLeft(NodeType *&leaf){
      NodeType* temp = leaf->right;
      leaf->right = rotateRight(temp);
      return rotateLeft(leaf);
    }
    
    //Rotate a Node branch to the left then the right, in order to balance things
    NodeType* rotateLeftRight(NodeType *&leaf){
      NodeType* temp = leaf->left;
      leaf->left = rotateLeft(temp);
      return rotateRight(leaf);
    }
    
    //Function that checks each Node's left and right branches to determine if they are unbalanced
    //If they are, we rotate the branches
    void rebalance(NodeType *&leaf){
      int hDiff = getDiff(leaf);
      if (hDiff > 1){
        if (getDiff(leaf->left) > 0) {
          leaf = rotateRight(leaf);
        } else {
          leaf = rotateLeftRight(leaf);
        }
      } else if(hDiff < -1) {
        if (getDiff(leaf->right) < 0) {
          leaf = rotateLeft(leaf);
        } else {
          leaf = rotateRightLeft(leaf);
        }
      }
    }
    
  private:
    NodeType *root;
    
    void insertAt(const KeyType &aKey, const ValueType &aValue, NodeType *&leaf){
        //*& signifies the receiving the pointer by reference. It means it is an alis for the passing parameter. So, it affects the passing parameter, which is the value of root.
      if (leaf == nullptr){
        leaf = new NodeType(aKey, aValue);
        leaf->updateHeight();
      }
      else {
        if (aKey < leaf->key){
          insertAt(aKey, aValue, leaf->left);
          leaf->updateHeight();
          rebalance(leaf);
        }
        else{
          insertAt(aKey, aValue, leaf->right);
          leaf->updateHeight();
          rebalance(leaf);
        }
      }
    }
    
    void removeAt(const KeyType &aKey, NodeType *&leaf){
          if(leaf == nullptr)
              return;
         
          if(aKey < leaf->key){
              removeAt(aKey, leaf->left);
              leaf->updateHeight();
              rebalance(leaf);
              
          }
          else if(aKey > leaf->key){
              removeAt(aKey, leaf->right);
              leaf->updateHeight();
              rebalance(leaf);
          }
          else{
              if(leaf->left == nullptr && leaf->right == nullptr){  //no child
                  if(leaf == root){
                      leaf->height = 0;
                  }
                  //delete leaf;
                  leaf = nullptr;
              }
              else if(leaf->left == nullptr || leaf->right == nullptr){ //one child
                  NodeType *temp = leaf->left? leaf->left : leaf->right;
                  leaf->key   = temp->key;
                  leaf->value = temp->value;
                  leaf->left  = nullptr;
                  leaf->right = nullptr;
                  delete temp;
                  temp = nullptr;
              }
              else{//two children
                  NodeType *temp = leaf->right;
                  while(temp->left != nullptr){
                      temp = temp->left;        //find the successor of Node left;
                  }
                  leaf->key   = temp->key;
                  leaf->value = temp->value;
                  removeAt(temp->key, temp);
              }

          }
        
    }
      
    ValueType searchAt(const KeyType &aKey,NodeType *&leaf){
        if(leaf == nullptr){
             return (ValueType)0;
        }
        if(leaf->key == aKey){
            return leaf->value;
        }
        else{
            if(aKey<leaf->key){
                return searchAt(aKey, leaf->left);
            }
            else{
                return searchAt(aKey ,leaf->right);
            }
        }
    }
      
    void destroy(NodeType *&leaf){
      if (leaf != nullptr){
        destroy(leaf->left);
        destroy(leaf->right);
        delete leaf;
      }
    }
    
    //Get the difference between Node right and left branch heights, if it returns positive
    //We know the left side is greater, if negative, we know the right side is greater
    int getDiff(NodeType *leaf){
      int lHeight = 0;
      int rHeight = 0;
      if (leaf->left != nullptr) {
        lHeight =  leaf->left->height;
      }
      if (leaf->right != nullptr) {
        rHeight = leaf->right->height;
      }
      return lHeight - rHeight;
    }
    
  };
  
}


#endif /* AVLTree_h */
