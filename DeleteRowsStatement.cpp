//
//  DeleteRowsStatement.cpp
//  ECEDatabase4
//
//  Created by rick gessner on 5/2/18.
//  Copyright © 2018 rick gessner. All rights reserved.
//

#include "DeleteRowsStatement.hpp"

namespace SF {
  
  
  DeleteRowsStatement::DeleteRowsStatement() : Statement(), filters() {
  }
  
  DeleteRowsStatement::DeleteRowsStatement(const DeleteRowsStatement &aCopy)
    : Statement(aCopy), filters(aCopy.filters) {
  }
  
  DeleteRowsStatement::~DeleteRowsStatement() {
  }
  
    
    
  //delete from  TABLE where k1=v1, k2=v2...
  StatusResult DeleteRowsStatement::parseValues(Parser &aParser, size_t aCount) {
      StatusResult theResult{true};
      
      Token theToken{};
      while(aParser.peekToken(theToken)) {
          if(TokenType::lparen==theToken.type) {
              aParser.skip(1);
              std::vector<Token> theTokens;
              theResult=parseCommaSepList(aParser, theTokens, TokenType::rparen);
          }
      }
      return theResult;
  }
  
  StatusResult DeleteRowsStatement::parse(Parser &aParser) {
    //we've already consumed... "delete"...
    Token theToken{};
    if(aParser.peekToken(theToken)) {
      if(Keywords::from_kw==theToken.keyword) {
        aParser.skip(1);
        if(aParser.peekToken(theToken)) {
          if(TokenType::identifier==theToken.type) {
            tableName=theToken.data;
            aParser.skip(1); //skip table name...
              
              if(aParser.skipKeyword(Keywords::where_kw, theToken)){
                  return filters.parse(aParser);
              }
              return StatusResult{true};
              /*
              while(aParser.peekToken(theToken)){
                  if(theToken.keyword == Keywords::where_kw ){
                      aParser.skip(1); //yank the 'where' token...
                      return filters.parse(aParser);
                  }
              }
              */
        
          }
        }
      }
    }
    return StatusResult{false, gUnknownTable};
  }
  
  //validating this statement implies validating filters...
  StatusResult DeleteRowsStatement::validate(Schema &aSchema) const {
    return StatusResult{true}; 
  };
  
}
