//
//  SelectStatement.cpp
//  ECEDatabase4
//
//  Created by rick gessner on 5/3/18.
//  Copyright © 2018 rick gessner. All rights reserved.
//

#include "SelectStatement.hpp"

//WARNING: THIS IS NOT WORKING YET; ONLY A PLACEHOLDER...

namespace SF {
  
  //--------------------------------------------------------------

  SelectStatement::SelectStatement() : Statement(), tableName(""), filters() {
  }
  
  SelectStatement::SelectStatement(const SelectStatement &aCopy) : filters(aCopy.filters) {
  }
  
  SelectStatement::~SelectStatement() {
  }
  
  //grab list of field names...
  StatusResult SelectStatement::parsePropertyList(Parser &aParser) {
    
    StatusResult theResult{true};
    
    std::vector<Token> theTokens;
    theResult=parseCommaSepList(aParser, theTokens, TokenType::keyword);
    for(auto theIter = theTokens.begin(); theIter != theTokens.end(); theIter++) {
      fields.push_back(Property((*theIter).data));
    }
          
    return theResult;
  }
  
  //insert into (field,field,...) values (A,B,C), (D,E,F)
  StatusResult SelectStatement::parseValues(Parser &aParser, size_t aCount) {
    StatusResult theResult{true};
    return theResult;
  }
  
  
  //group by a,b...
  StatusResult SelectStatement::parseGroupBy(Parser &aParser, Token &aToken) {
    if(Keywords::group_kw==aToken.keyword) {
      aParser.skip(1);
      if(aParser.nextToken(aToken)) {
        if(TokenType::keyword==aToken.type && Keywords::by_kw==aToken.keyword) {
          std::vector<Token> theTokens;
          StatusResult theResult=parseCommaSepList(aParser, theTokens, TokenType::keyword);
          if(theResult) {
            for(auto theIter = theTokens.begin(); theIter != theTokens.end(); theIter++) {
              groupBy.push_back(Property((*theIter).data));
            }
            return theResult;
          }
        }
      }
    }
    return StatusResult{false, gSyntaxError};
  }
  
  //order by a,b...
  StatusResult SelectStatement::parseOrderBy(Parser &aParser, Token &aToken) {
    if(Keywords::order_kw==aToken.keyword) {
      aParser.skip(1);
      if(aParser.nextToken(aToken)) {
        if(TokenType::keyword==aToken.type && Keywords::by_kw==aToken.keyword) {
          std::vector<Token> theTokens;
          StatusResult theResult=parseCommaSepList(aParser, theTokens, TokenType::keyword);
          if(theResult) {
            for(auto theIter = theTokens.begin(); theIter != theTokens.end(); theIter++) {
              orderBy.push_back(Property((*theIter).data));
            }
            return theResult;
          }
        }
      }
    }
    return StatusResult{false, gSyntaxError};
  }

  StatusResult SelectStatement::parse(Parser &aParser) {
    
    //we've consumed "select...", get field1,field2 FROM...
    StatusResult theResult=parsePropertyList(aParser);
    if(theResult) {
      Token theToken{};
      if(aParser.peekToken(theToken)) {
        if(TokenType::identifier==theToken.type) {
          tableName=theToken.data;
          aParser.skip(1); //yank the name...
          
          //iterate other tokens, looking for: WHERE, ORDER BY, GROUP BY...
          while(aParser.peekToken(theToken)) {

            switch(theToken.type) {
              case TokenType::keyword:
                
                switch(theToken.keyword) {
                    
                  case Keywords::where_kw:
                    aParser.skip(1); //yank the 'where' token...
                    theResult = filters.parse(aParser);
                    break;
                    
                  case Keywords::order_kw:
                    theResult=parseOrderBy(aParser,theToken);
                    break;

                  case Keywords::group_kw:
                    theResult=parseGroupBy(aParser, theToken);
                    break;

                  default: break;
                }
                //aParser.skip(1); //yank the name...
                break;

              default:
                aParser.skip(1); //yank the name...
                break;
            }
          }
          
        }
      }
    }
    return theResult;
  }
  
  //validating this statement implies validating field and argument lists...
  StatusResult SelectStatement::validate(Schema &aSchema) const {
    return StatusResult{false, gInvalidArguments}; //needs to be exact multiple...
  };
  
}
