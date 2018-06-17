//
//  Filters.cpp
//  ECEDatabase
//
//  Created by rick gessner on 4/29/18.
//  Copyright © 2018 rick gessner. All rights reserved.
//

#include "Filters.hpp"
#include "Parser.hpp"
#include "Row.hpp"
#include <map>
#include <string>

namespace SF {

  std::map<std::string, Operators> gOperators = {
    std::make_pair("=", Operators::equal_op),
    std::make_pair("!=", Operators::notequal_op),
    std::make_pair("not", Operators::notequal_op),
    std::make_pair("<", Operators::lt_op),
    std::make_pair("<=", Operators::lte_op),
    std::make_pair(">", Operators::gt_op),
    std::make_pair(">=", Operators::gte_op),
    std::make_pair("and", Operators::and_op),
    std::make_pair("or", Operators::or_op),
    std::make_pair("between", Operators::between_op)
  };
  
  //--------------------------------------------------------------

  Filters::Filters() : schemaHash(0)  {
  }
  
  //TBD
  Filters::Filters(const Filters &aCopy) : schemaHash(aCopy.schemaHash){
  }
  
  Filters::~Filters() {
  }
  
  std::string Filters::getHash() {
    return std::string("ummm");
  }
  
  bool isOperator(Token &aToken) {
    if(TokenType::operators==aToken.type)
      return true;
    return false;
  }
  
  Operators asOperator(std::string aString) {
    auto theIter = gOperators.find(aString);
    if (theIter != gOperators.end()) {
      return theIter->second;
    }
    return Operators::unknown_op;
  }

  //grab identifier, OP , uservalue...
  Expression* getExpression(Parser &aParser, Token &aToken) {    
    if(TokenType::identifier==aToken.type) {
      Property theProperty(aToken.data);
      aParser.skip(1);
      if(aParser.nextToken(aToken)) {
        if(isOperator(aToken)) {
          Operators theOp = asOperator(aToken.data);
          if(aParser.nextToken(aToken)) {
            switch(aToken.type) {
              case TokenType::number:
              case TokenType::identifier:
                return new Expression(theProperty, UserValue(aToken), theOp);
              default: break;
            }
          }
        }
      }
    }
    
    return nullptr;
  }
  
  //already got 'where', so go get key/value pairs until reach keyword or EOF...
  StatusResult Filters::parse(Parser &aParser) {

    Token theToken{};
    KeyValuePair thePair;
    StatusResult theResult{true};

    while(theResult && aParser.peekToken(theToken)) {
      switch(theToken.type) {
        case TokenType::identifier:
          {
            Expression *theExpression=getExpression(aParser, theToken);
            if(theExpression) {
              expressions.push_back(theExpression);
            }
          }
          break;
          
        case TokenType::keyword:
          return StatusResult{true}; //done with expr...
          
        default:
          if(!expressions.size()) {
            theResult=StatusResult{false,gIdentifierExpected};
          }
          break;
      }
    }
    return theResult;
  }
  
  //compare expressions to row; if all match, then return true
  bool Filters::matches(Row &aRow) {
    bool theResult=true;
    if(expressions.size()) {
      
    }
    return theResult;
  }

}
