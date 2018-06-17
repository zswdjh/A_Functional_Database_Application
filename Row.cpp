//
//  Row.cpp
//  ECEDatabase
//
//  Created by rick gessner on 4/29/18.
//  Copyright © 2018 rick gessner. All rights reserved.
//

#include "Row.hpp"
#include "Storage.hpp"

namespace SF {

  Row::Row(int32_t aBlockNumber) : blockNumber(aBlockNumber), columns() {
  }

  Row::Row(int32_t aBlockNumber, KeyValues& aKeyValues) : blockNumber(aBlockNumber),columns(aKeyValues){
  }
    
  Row::Row(int32_t aBlockNumber, KeyValues& aKeyValues, uint32_t aAutoIncrement) : blockNumber(aBlockNumber),columns(aKeyValues),autoIncrement(aAutoIncrement){
  }
    
  Row::Row(const Row &aCopy) : blockNumber(aCopy.blockNumber), columns(aCopy.columns) {
  }
  
  Row::~Row() {
  }
  
  //---------------------------------------------------
  
  RowCollection::RowCollection() : rows() {}
  
  RowCollection::RowCollection(const RowCollection &aCopy) : rows(aCopy.rows) {}
  
  RowCollection::~RowCollection() {
    for(auto theRow : rows) {
      delete theRow;
    }
  }
  
  RowCollection& RowCollection::add(Row* aRow) {
    rows.push_back(aRow);
    return *this;
  }
  
}


