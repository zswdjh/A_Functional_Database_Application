//
//  Tokenizer.hpp
//  ECEDatabase
//
//  Created by rick gessner on 3/30/18.
//  Copyright © 2018 rick gessner. All rights reserved.
//

#ifndef Tokenizer_hpp
#define Tokenizer_hpp

#include <stdio.h>
#include "Token.hpp"
#include "Keywords.hpp"
#include <sstream>
#include <string>
#include <vector>

namespace SF {
  
  typedef bool (*callback)(char aChar);

  class Tokenizer {
  protected:
    std::vector<Token>  tokens;
    std::istringstream    input;
    
    std::string readWhile(callback aCallback);

    std::string readUntil(char aTerminal, bool addTerminal=false);
    std::string readUntil(callback aCallback, bool addTerminal=false);
    char skipSpaces();
    
    
  public:
    
    Tokenizer(std::string anInput);
    
    Tokenizer(const Tokenizer &aCopy);
    
    size_t size() {return tokens.size();}
    StatusResult tokenize();
    Token tokenAt(int anOffset);
    
    static  Keywords  getKeywordId(const std::string aKeyword);
    static  int       getFunctionId(const std::string anIdentifier);
    
    void dump();
    
  };
    
}

#endif /* Tokenizer_hpp */
