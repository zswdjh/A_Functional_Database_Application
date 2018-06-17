//
//  Parser.cpp
//  ECEDatabase
//
//  Created by rick gessner on 3/30/18.
//  Copyright © 2018 rick gessner. All rights reserved.
//


#include <vector>
#include <stdio.h>
#include "Parser.hpp"
#include "Tokenizer.hpp"

namespace SF {
  
  Parser::Parser(Tokenizer &aTokenizer) : tokenizer(aTokenizer), pos(0) {}
  
  Parser::Parser(const Parser &aCopy) : tokenizer(aCopy.tokenizer), pos(aCopy.pos) {}
  
  Parser::~Parser() {}
  
  int Parser::getFunctionId(std::string anIdentifier) {
    return tokenizer.getFunctionId(anIdentifier);  //pay attention to the difference between calling a non-static function and static function
  }
  
  Keywords Parser::getKeywordId(std::string aKeyword) {
    return Tokenizer::getKeywordId(aKeyword);    //this is the way to call static function
  }
  
  //peek ahead to next token
  bool Parser::peekToken(Token &aToken, int anOffset) {
    if(pos+anOffset<tokenizer.size()) {
      aToken=tokenizer.tokenAt(pos+anOffset);
      return true;
    }
    return false;
  }
  
  bool Parser::getToken(Token &aToken) {
    if(pos<tokenizer.size()) {
      aToken=tokenizer.tokenAt(pos);
      return true;
    }
    return false;
  }
  
  bool Parser::nextToken(Token &aToken) {
    pos++;
    return getToken(aToken);
  }
  
  bool Parser::skip(int aCount) {
    pos+=aCount;
    return true;
  }
  
  bool Parser::skipIf(TokenType aTokenType) {
    Token theToken{};
    if(peekToken(theToken)) {
      if(theToken.type==aTokenType) {
        skip(1);
        return true;
      }
    }
    return false;
  }
  
  bool Parser::skipKeyword(Keywords aKeyword, Token &aToken, int aLimit) {
    for(int i=0;i<aLimit, getToken(aToken); i++) {
      nextToken(aToken);
      if (aKeyword==tokenizer.getKeywordId(aToken.data)) {
        return true;
      }
    }
    return false;
  }
  
  StatusResult Parser::parse() {
    return StatusResult{false, gEOF};
  }
  
}
