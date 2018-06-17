//
//  JSONReader.cpp
//  ECEDatabase7
//
//  Created by rick gessner on 5/20/18.
//  Copyright © 2018 rick gessner. All rights reserved.
//

#include "JSONReader.hpp"
#include "Tokenizer.hpp"

namespace SF {
  
  JSONReader::JSONReader(Tokenizer &aTokenizer, Index &anIndex) : Parser(aTokenizer), theIndex(anIndex) {
  }
  
  JSONReader::JSONReader(const JSONReader &aCopy) : Parser(aCopy.tokenizer), theIndex(aCopy.theIndex) {
  }
  
  JSONReader::~JSONReader() {}

  StatusResult JSONReader::parse() {
      size_t tokenSize = tokenizer.size();
      Token  theToken{};
      std::string recordAttribute;
      uint32_t    blockNum;
      for(int i=0;i<tokenSize;i++){
          theToken = tokenizer.tokenAt(i);
          Keywords theKw = tokenizer.getKeywordId(theToken.data);
          switch(theKw){
              case Keywords::idxRecord_kw:
                  recordAttribute = tokenizer.tokenAt(i+2).data;
                  i+=2;
                  break;
                  
              case Keywords::idxBlockNum_kw:
                  blockNum = std::strtoul(tokenizer.tokenAt(i+2).data.c_str(),nullptr,10);
                  i+=2;
                  
                  theIndex.addKeyValue(recordAttribute,blockNum);
                  break;
                  
              default:
                  break;
          }
      }
    
    return StatusResult{true};
  }

  
}

