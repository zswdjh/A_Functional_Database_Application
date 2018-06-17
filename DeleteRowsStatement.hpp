//
//  InsertStatement.hpp
//  ECEDatabase
//
//  Created by rick gessner on 4/21/18.
//  Copyright © 2018 rick gessner. All rights reserved.
//

#ifndef DeleteRowsStatement_hpp
#define DeleteRowsStatement_hpp

#include <stdio.h>
#include "SelectStatement.hpp"
#include "Attribute.hpp"
#include "Keywords.hpp"
#include "Parser.hpp"
#include "Filters.hpp"
#include <vector>
#include <string>

namespace SF {
  
  class Schema;
  class SQLInterpreter;
  
  class DeleteRowsStatement : public Statement {
  protected:
    std::string    tableName;
    Filters        filters;
   
  public:
                  DeleteRowsStatement();
                  DeleteRowsStatement(const DeleteRowsStatement &aCopy);
    virtual       ~DeleteRowsStatement();
    
                  
                  StatusResult  parseValues(Parser &aParser, size_t aCount);
    virtual       StatusResult  parse(Parser &aParser);
    
    virtual       StatusResult  validate(Schema &aSchema) const;
    
    std::string   getTableName() const {return tableName;}
    Filters&      getFilters() {return filters;}
  };
  
}
#endif /* DeleteRowsStatement_hpp */
