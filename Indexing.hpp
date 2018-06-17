//
//  Indexer.hpp
//  ECEDatabase6
//
//  Created by rick gessner on 5/16/18.
//  Copyright © 2018 rick gessner. All rights reserved.
//

#ifndef Indexing_h
#define Indexing_h

#include "AVLTree.hpp"
#include "UserValue.hpp"
#include <string>
#include <vector>

namespace SF {
                    //key        value
                //primary key    blknum
  typedef AVLTree<std::string, uint32_t> IndexTreeType;
  
    
  //utility to save uint32_t of the AVL tree into a vector
  struct AVLTreeObserver : IndexTreeType::NodeObserver {
      AVLTreeObserver(std::vector<uint32_t>& aBlockList): theBlockList(aBlockList){}
      virtual void operator()(IndexTreeType::NodeType &aNode){
          theBlockList.push_back(aNode.value);
      }
      std::vector<uint32_t>& theBlockList;
    };
    
  class Index  {
  protected:
    
    IndexTreeType   index;
    bool            dirty;
    
  public:
    
    Index() : dirty(false) {}
    
    Index(const std::string path) : dirty(false) {
      //cause index to be loaded from disk
    }
    
    Index(const Index &aCopy){
        std::cout<<"Calling copy constructor of Index"<<std::endl;
    }
    ~Index() {}
    
    bool isDirty() {return dirty;}
    
    StatusResult each(IndexTreeType::NodeObserver &anObserver) {
      auto theIter = index.getBreadthFirstVisitor();
      theIter.each(anObserver);
      return StatusResult{true};
    }
    
    size_t getBlockNumbers(std::vector<uint32_t> &aBlockList) {
      //--------------------------------------------
      // ADD code here to store all values...
      //--------------------------------------------
        AVLTreeObserver theTreeObserver(aBlockList);
        if(each(theTreeObserver)){
            size_t theBlockListSize = aBlockList.size();
            if(theBlockListSize){
                return theBlockListSize;
            }
            else{
                return 0;
            }
        }
        else{
            return 0;
        }
      
    }
    
    uint32_t indexOf(std::string &aKey) {
      //--------------------------------------------
      // ADD code here to find key, return value
      //--------------------------------------------
        uint32_t theValue = index.search(aKey);
        if( theValue ){
            return theValue;
        }
        else{
            return 0;  //0 means NOT found...
        }
      
    }
    
    StatusResult addKeyValue(const std::string &aKey, const uint32_t &aValue) {
      dirty=true;
      //------------------------------------------------
      // ADD code here to insert key/value into tree...
      //------------------------------------------------
      
      index.insert(aKey, aValue); //needs to be smarter about type...
      return StatusResult{true};
    }
    
    StatusResult updateKeyValue(const std::string &aKey, const uint32_t &aValue) {
      dirty=true;
      //---------------------------------------------------
      // ADD code here to update existing value for key...
      //---------------------------------------------------
      return StatusResult{true};
    }
    
    StatusResult removeKey(const std::string &aKey) {
      dirty=true;
      //---------------------------------------------------
      // ADD code here to remove key and value from tree...
      //---------------------------------------------------
      index.remove(aKey);
      return StatusResult{true};
    }
    
  };
  
}

#endif /* Indexing_h */
