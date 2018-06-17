//
//  Statement.hpp
//  ECEDatabase
//
//  Created by rick gessner on 4/7/18.
//  Copyright © 2018 rick gessner. All rights reserved.
//

#ifndef Statement_hpp
#define Statement_hpp

#include <stdio.h>
#include "Token.hpp"
#include <vector>
#include "UserValue.hpp"

namespace SF {
  
  class Schema;
  class Parser;
  
  class Statement {
  protected:
    Keywords  stmtType;
    
  public:
    Statement(Keywords aStatementType=Keywords::unknown_kw);
    Statement(const Statement &aCopy);
    virtual   ~Statement();
    
              Keywords      getStatementType();

              StatusResult  parseKeyValueList(Parser &aParser, KeyValueList &aList);
              StatusResult  parseCommaSepList(Parser &aParser, std::vector<Token> &aList,
                                              TokenType aTerminal);
    virtual   StatusResult  parse(Parser &aParser);
    virtual   StatusResult  validate(Schema &aSchema) const;

  };
  
}

#endif /* Statement_hpp */
