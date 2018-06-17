//
//  SelectStatement.hpp
//  ECEDatabase
//
//  Created by rick gessner on 5/3/18.
//  Copyright © 2018 rick gessner. All rights reserved.
//

#ifndef SelectStatement_hpp
#define SelectStatement_hpp

#include <stdio.h>
#include "Statement.hpp"
#include "Attribute.hpp"
#include "Keywords.hpp"
#include "Parser.hpp"
#include "UserValue.hpp"
#include "Filters.hpp"
#include <vector>
#include <string>

namespace SF {
  
  class Schema;
  class SQLInterpreter;
  
  //---------------------------------------------------
  
  class SelectStatement : public Statement {
  protected:
    PropertyList   fields;
    PropertyList   groupBy; //reflects group by clause...
    PropertyList   orderBy; //reflects order by clause...
    Filters        filters;
    std::string    tableName;
    uint32_t       schemaHash;
    
  public:
              SelectStatement();
              SelectStatement(const SelectStatement &aCopy);
    
    virtual   ~SelectStatement();
    
              StatusResult  parsePropertyList(Parser &aParser);
              StatusResult  parseValues(Parser &aParser, size_t aCount); //capture where clause...
              StatusResult  parseOrderBy(Parser &aParser, Token &aToken);
              StatusResult  parseGroupBy(Parser &aParser, Token &aToken);

    virtual   StatusResult  parse(Parser &aParser);
    
    virtual   StatusResult  validate(Schema &aSchema) const;
    
              std::string   getTableName() const {return tableName;}
              Filters&      getFilters() {return filters;}
              PropertyList* getFields() {return &fields;}
    
              PropertyList& getOrderBy() {return orderBy;}
              PropertyList& getGroupBy() {return groupBy;}

  };
  
}

#endif /* SelectStatement_hpp */
