//
//  Row.hpp
//  ECEDatabase
//
//  Created by rick gessner on 4/29/18.
//  Copyright © 2018 rick gessner. All rights reserved.
//

#ifndef Row_hpp
#define Row_hpp

#include <stdio.h>
#include <vector>
#include <iostream>
#include "UserValue.hpp"

namespace SF {

  class Schema;
  class Block;
  
  class Row {
  protected:
    int32_t       blockNumber;
    KeyValues     columns;
    uint32_t      autoIncrement;  //
    
  public:
    Row(int32_t aBlockNumber=0);
    Row(int32_t aBlockNumber, KeyValues& aKeyValues);
    Row(int32_t aBlockNumber, KeyValues& aKeyValues, uint32_t aAutoIncrement);
    Row(const Row &aRow);
    ~Row();
    
    int32_t       getBlockNumber() {return blockNumber;}
    KeyValues&    getColumns() {return columns;}
    uint32_t      getAutoIncrement(){return autoIncrement;} //
      
    friend class RowCollection;
  };
  
  typedef std::vector<Row*> RowList;
  
  class RowCollection {
  protected:
    RowList rows;
  public:
    RowCollection();
    RowCollection(const RowCollection &aCopy);
    ~RowCollection();
    
    RowList&          getRows() {return rows;}
    
    RowCollection&    add(Row* aRow);    
  };

  //--------------------------------
  
/*
  class CollectionCache : public LRUCache<std::string, RowCollection*> {
  public:
    
                    CollectionCache(size_t aSize);
    
    bool            hasKey(std::string aKey);
    bool            addCollection(std::string aKey);
    RowCollection*  getCollection(std::string aKey); //can throw exception

  };
*/
 
 }

#endif /* Row_hpp */

//-------------------------------------------------


