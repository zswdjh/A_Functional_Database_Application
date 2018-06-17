//
//  JSONWriter.hpp
//  ECEDatabase6
//
//  Created by rick gessner on 5/11/18.
//  Copyright © 2018 rick gessner. All rights reserved.
//

#ifndef JSONReader_h
#define JSONReader_h

#include <iostream>
#include <string>
#include <vector>
#include "Token.hpp"
#include "Parser.hpp"
#include "indexing.hpp"
namespace SF {
  
  const int kOpenBracketExpected = 100; //[
  const int kCloseBracketExpected = 200; //]
  const int kOpenBraceExpected = 300; //{
  const int kCloseBraceExpected = 400; //}
  
  class Tokenizer;
  //class Index;
  
  class JSONReader : public Parser {
  public:
    Index &theIndex;
    
    JSONReader(Tokenizer &aTokenize, Index &anIndex);
    
    JSONReader(const JSONReader &aReader);
    
    virtual ~JSONReader();
    
    virtual StatusResult parse();

  };
  
}


#endif /* JSONReader_h */
