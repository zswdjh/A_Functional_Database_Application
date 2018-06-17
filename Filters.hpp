//
//  Filters.hpp
//  ECEDatabase
//
//  Created by rick gessner on 4/29/18.
//  Copyright © 2018 rick gessner. All rights reserved.
//

#ifndef Filters_hpp
#define Filters_hpp

#include <stdio.h>
#include "UserValue.hpp"
#include "Attribute.hpp"
#include <vector>
#include <string>

namespace SF {
  
  class Parser;
  class Row;
  
  struct Expression {
    Property    key;
    UserValue   value;
    Operators   op;
    
    Expression(Property aKey, UserValue aValue, Operators anOp) :
      key(aKey), value(aValue), op(anOp) {}
    
    Expression(std::string aKey, UserValue aValue, Operators anOp ) :
      key(Property(aKey)), value(aValue), op(anOp) {}
    
  };
  
  class Filters {
  protected:
    std::vector<Expression*>  expressions;
    uint32_t                  schemaHash;

  public:
    
    Filters();
    Filters(const Filters &aFilters);
    ~Filters();
        
    std::string   getHash();
    StatusResult  parse(Parser &aParser);
    bool          matches(Row &aRow);
    
  };

}

#endif /* Filters_hpp */
