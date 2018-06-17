//
//  CreateTableStatement.cpp
//  ECEDatabase
//
//  Created by rick gessner on 4/7/18.
//  Copyright © 2018 rick gessner. All rights reserved.
//


#include "CreateTableStatement.hpp"
#include "Tokenizer.hpp"
#include "Parser.hpp"
#include "Token.hpp"
#include "Schema.hpp"

namespace SF {
  
  CreateTableStatement::CreateTableStatement(std::string aName) : Statement(Keywords::create_kw), name(aName){
  }
  
  CreateTableStatement::CreateTableStatement(const CreateTableStatement &aCopy)
  : Statement(aCopy), name(aCopy.name) {
  }
  
  CreateTableStatement::~CreateTableStatement() {
  }
  
  bool isDatatype(Keywords aKeyword) {
    switch(aKeyword) {
      case Keywords::char_kw:
      case Keywords::datetime_kw:
      case Keywords::float_kw:
      case Keywords::integer_kw:
      case Keywords::varchar_kw:
        return true;
      default: return false;
    }
  }
  
  //consume (number) and return the numeric value...
  int parseVarcharLength(Parser &aParser) {
    Token theToken{};
    aParser.nextToken(theToken);
    if(TokenType::number==theToken.type) {
      aParser.skipIf(TokenType::rparen);
      return atoi(theToken.data.c_str());
    }
    return 0; //means we have a problem...
  }
  
  StatusResult CreateTableStatement::parseAttributes(Parser &aParser) {    
    Token theToken{};
    if(aParser.nextToken(theToken)) {
      while(theToken.type==TokenType::identifier) {
        Attribute theAttribute(theToken.data);
        //aParser.skip(1); //skip identifier...
        if(aParser.nextToken(theToken)) {
          if(isDatatype(theToken.keyword)) {
            theAttribute.setType(theToken.keyword);

            bool options=true;
            while(options && aParser.nextToken(theToken)) {
              switch(theToken.type) {
                case TokenType::keyword:
                  switch(theToken.keyword) {
                    case Keywords::auto_increment_kw:
                      theAttribute.setAutoIncrement(true);
                      break;
                    case Keywords::primary_kw:
                      theAttribute.setPrimaryKey(true);
                      break;
                    case Keywords::null_kw:
                      theAttribute.setNullable(false);
                      break;                      
                    default: break;
                  }
                  break;
                  
                case TokenType::lparen:
                  if(Keywords::varchar_kw==theAttribute.getType()) {
                    int theSize=parseVarcharLength(aParser);
                    theAttribute.setSize(theSize);
                  }
                  else return StatusResult{false,gSyntaxError};
                  break;
                  
                case TokenType::comma:
                  aParser.nextToken(theToken);
                  
                case TokenType::rparen: //fall thru...
                  
                default:
                  options=false;
                  break;
              }
            }
            if(theAttribute.isValid()) {
              attributes.push_back(theAttribute);
            }
            else return StatusResult{false,gInvalidAttribute};
          }
        }
      }
    }
    aParser.skipIf(TokenType::semicolon);
    
    return StatusResult{true};
  }
  
    //expect '(', attribute-list, ')', ';'
  StatusResult CreateTableStatement::parse(Parser &aParser) {
    
    Token theToken{};
    
    aParser.nextToken(theToken);
    if(TokenType::lparen==theToken.type) {
      return parseAttributes(aParser);
    }
    
    return StatusResult{false};
  }
  
  std::string CreateTableStatement::getTableName() const {
    return name;
  }
  
  StatusResult CreateTableStatement::validate(Schema &aSchema) const {
    return StatusResult{true}; //FIX ME!
  };
}
