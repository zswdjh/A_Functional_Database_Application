//
//  CreateTableStatement.hpp
//  ECEDatabase
//
//  Created by rick gessner on 4/7/18.
//  Copyright © 2018 rick gessner. All rights reserved.
//

#ifndef CreateTableStatement_hpp
#define CreateTableStatement_hpp

#include <stdio.h>
#include "Statement.hpp"
#include "Attribute.hpp"
#include <vector>

namespace SF {
  
  class Schema;
  
  class CreateTableStatement : public Statement {
  protected:
    std::vector<Attribute>  attributes;
    std::string             name;
    
  public:
    CreateTableStatement(std::string aName="");
    CreateTableStatement(const CreateTableStatement &aCopy);
    virtual ~CreateTableStatement();

    std::string getTableName() const;

    virtual StatusResult parseAttributes(Parser &aParser);
    virtual StatusResult parse(Parser &aParser);
    virtual StatusResult validate(Schema &aSchema) const;

    friend class Schema;
  };
  
}

#endif /* CreateTableStatement_hpp */
