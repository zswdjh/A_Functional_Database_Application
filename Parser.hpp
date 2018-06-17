
//
//  Parser.hpp
//  ECEDatabase
//
//  Created by rick gessner on 3/30/18.
//  Copyright © 2018 rick gessner. All rights reserved.
//

#ifndef Parser_hpp
#define Parser_hpp

#include "Token.hpp"

namespace SF {
  
  const int kFunctionExpected = 100;
  const int kFromExpected = 200;
  const int kWhereExpected = 300;
  const int kTableNameExpected = 400;
  
  class Tokenizer;
  
  class Parser {
  protected:
    
    int  pos;
    
  public:
    
    Tokenizer &tokenizer;
    
    Parser(Tokenizer &aTokenizer);
    
    Parser(const Parser &aCopy);
    
    virtual ~Parser();  //make base class destructors virtual when they are meant to be manipulated polymorphically. JSONReader inherite Parser.
    
    bool          peekToken(Token &aToken, int anOffset=1); //peek ahead
    bool          getToken(Token &aToken); //get current
    bool          nextToken(Token &aToken); //move ptr ahead...
    
    bool          skip(int aCount=1); //skip ahead N tokens...
    bool          skipIf(TokenType aTokenType); //skip token if matches type...
    bool          skipKeyword(Keywords aKeyword, Token &aToken, int aLimit=100);
        
    Keywords      getKeywordId(std::string aKeyword); //delegated to tokenizer...
    int           getFunctionId(std::string anIdentifier);
    
    virtual
    StatusResult parse(); //this is a member function not a constructor since it has the return type!

  };
    
}

#endif /* Parser_hpp */
