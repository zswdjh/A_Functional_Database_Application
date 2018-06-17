//
//  Statement.cpp
//  ECEDatabase
//
//  Created by rick gessner on 4/7/18.
//  Copyright © 2018 rick gessner. All rights reserved.
//

#include "Statement.hpp"
#include "Parser.hpp"
#include "Schema.hpp"

namespace SF {

  Statement::Statement(Keywords aStatementType) : stmtType(aStatementType){
  }
  
  Statement::Statement(const Statement &aCopy) {
    stmtType=aCopy.stmtType;
  }
  
  Statement::~Statement() {
  }
  
  Keywords Statement::getStatementType() {return stmtType;}

  StatusResult Statement::parse(Parser &aParser) {
    return 0;
  }
  
  StatusResult Statement::parseKeyValueList(Parser &aParser, KeyValueList &aList) {
    return StatusResult{false};
  }

  StatusResult Statement::parseCommaSepList(Parser &aParser, std::vector<Token> &aList, TokenType aTerminal) {
    
    int theStartSize = (int)aList.size();
    Token theToken{};
    while(aParser.peekToken(theToken)) {
      if(aTerminal!=theToken.type) {
        aList.push_back(theToken);
        aParser.skip(1); //skip the value
        
        if(aParser.peekToken(theToken)) {
          if(TokenType::comma==theToken.type) {
            aParser.skip(1); //yank the comma...
          }
        }
      }
      else if(aTerminal==theToken.type) {
        aParser.skip(1); //yank the terminal...
        return StatusResult{true};
      }
    }
    
    if(aList.size()!=theStartSize) {
      return StatusResult{true};
    }
    
    return StatusResult{false,gEOF};
  }
  
  StatusResult Statement::validate(Schema &aSchema) const {
    return StatusResult{true};
  }
  
}
