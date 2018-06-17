//
//  InsertStatement.hpp
//  ECEDatabase
//
//  Created by rick gessner on 4/21/18.
//  Copyright © 2018 rick gessner. All rights reserved.
//

#ifndef InsertStatement_hpp
#define InsertStatement_hpp

#include <stdio.h>
#include "Statement.hpp"
#include "Attribute.hpp"
#include "Keywords.hpp"
#include "Parser.hpp"
#include "UserValue.hpp"
#include <vector>
#include <string>

namespace SF {

  class Schema;
  class SQLInterpreter;
  
  class InsertStatement : public Statement {
  protected:
    std::vector<Property>     fields;
    std::vector<KeyValues*>   values;  //each is a collection of named-uservalues...
    std::string               tableName;

  public:
              InsertStatement(std::string &aTableName);
              InsertStatement(const InsertStatement &aCopy);
    virtual   ~InsertStatement();
    
              StatusResult  parsePropertyList(Parser &aParser);
              StatusResult  parseValues(Parser &aParser, Schema &aSchema, size_t aCount);
    virtual   StatusResult  parse(Parser &aParser, Schema &aSchema);

    virtual   StatusResult  validate(Schema &aSchema) const;

              std::string   getTableName() const {return tableName;}
              size_t        getValueGroupCount() {return values.size();}
              KeyValues*    getValueGroupAt(int anIndex);
  };
  
}
#endif /* InsertStatement_hpp */
