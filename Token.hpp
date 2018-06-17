//
//  Token.hpp
//  ECEDatabase
//
//  Created by rick gessner on 3/30/18.
//  Copyright © 2018 rick gessner. All rights reserved.
//

#ifndef Token_h
#define Token_h

#include <stdio.h>
#include <string>
#include "Keywords.hpp"

namespace SF {
  
  const int kNoError =0;
  
  enum class TokenType {
    comma, colon, semicolon, number, operators,
    lparen, rparen, lbracket, rbracket, lbrace, rbrace,
    slash, star, equal, plus, minus, keyword,
    identifier, function, string, unknown
  };

  struct Token {
    Token(TokenType aType=TokenType::unknown, std::string aString="", Keywords aKW=Keywords::unknown_kw) :
      type(aType), data(aString), keyword(aKW) {}
    
    TokenType type;
    Keywords  keyword;
    std::string data;
  };
    
}


#endif /* Token_h */

