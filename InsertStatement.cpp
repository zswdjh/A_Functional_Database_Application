//
//  InsertStatement.cpp
//  ECEDatabase
//
//  Created by rick gessner on 4/21/18.
//  Copyright © 2018 rick gessner. All rights reserved.
//

#include "InsertStatement.hpp"
#include "Schema.hpp"

namespace SF {

  InsertStatement::InsertStatement(std::string &aTableName) : Statement(), tableName(aTableName) {
  }

  InsertStatement::InsertStatement(const InsertStatement &aCopy) {
  }
  
  InsertStatement::~InsertStatement() {
    for (auto theList : values) {
      delete theList;
    }
  }
  
  //grab list of field names...
  StatusResult InsertStatement::parsePropertyList(Parser &aParser) {
    
    StatusResult theResult{true};
    
    Token theToken{};
    if(aParser.peekToken(theToken) && TokenType::lparen==theToken.type) {
      aParser.skip(1);
      std::vector<Token> theTokens;
      theResult=parseCommaSepList(aParser, theTokens, TokenType::rparen);
      for(auto theIter = theTokens.begin(); theIter != theTokens.end(); theIter++) {
        fields.push_back(Property((*theIter).data));
      }
      
      if(aParser.peekToken(theToken)) {
        if(TokenType::rparen==theToken.type) {
          aParser.skip(1);
        }
      }
    }
    
    return theResult;
  }
  
  //insert into (field,field,...) values (A,B,C), (D,E,F)
  StatusResult InsertStatement::parseValues(Parser &aParser, Schema &aSchema, size_t aCount) {
    StatusResult theResult{true};
    
    Token theToken{};
    while(aParser.peekToken(theToken)) {
      if(TokenType::lparen==theToken.type) {
        aParser.skip(1);
        
        std::vector<Token> theValueTokens;
        
        theResult=parseCommaSepList(aParser, theValueTokens, TokenType::rparen);
        if(theResult) {
          
          KeyValues *theList = new KeyValues;
          for(int i=0;i<fields.size();i++) {
            try {
              UserValue theValue(theValueTokens[i]);
              Attribute &theAttribute=aSchema.getAttribute(fields[i].name);
              theValue.become(theAttribute.getType());
              theList->addKeyValue(fields[i].name, theValue);
            }
            catch(...) {
            }
          }
          values.push_back(theList);
          
          if(aParser.peekToken(theToken)) {
            if(TokenType::comma!=theToken.type) {
              return StatusResult{true};
            }
            aParser.skip(1);
          }
        }
      }
    }
    
    return theResult;
  }

  StatusResult InsertStatement::parse(Parser &aParser, Schema &aSchema) {
    
    //we've already consumed... "insert into {name}"...
    StatusResult theResult=parsePropertyList(aParser);
    if(theResult) {
      Token theToken{};
      if(aParser.peekToken(theToken)) {
        if(Keywords::values_kw==theToken.keyword) {
          aParser.skip(1);
          theResult=parseValues(aParser, aSchema, fields.size());
        }
      }
    }
    return theResult;
  }

  //validating this statement implies validating field and argument lists...
  StatusResult InsertStatement::validate(Schema &aSchema) const {
    
    size_t fsize=fields.size();
    size_t vsize=values.size();
    
    if(fsize && vsize) {
      
      //Each list should have N elements, where N==fsize. One list per insert...
      for (auto theList : values) {
        if(fsize!=theList->size() || !aSchema.validate(theList->getKeyValues())) {
          return StatusResult{false, gInvalidArguments};
        }
      }
      return StatusResult{true};
    }
    
    return StatusResult{false, gInvalidArguments}; //needs to be exact multiple...
  };

  KeyValues* InsertStatement::getValueGroupAt(int anIndex) {
    if(anIndex>=0 && anIndex<values.size()) {
      KeyValues *theList=values[anIndex];
      size_t theSize=(*theList).size();
      return theList;
    }
    return nullptr;
  }

  
}
