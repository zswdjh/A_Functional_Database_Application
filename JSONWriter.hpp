//
//  JSONWriter.hpp
//  ECEDatabase6
//
//  Created by rick gessner on 5/11/18.
//  Copyright © 2018 rick gessner. All rights reserved.
//

#ifndef JSONWriter_h
#define JSONWriter_h

#include "AVLTree.hpp"

namespace SF {
  
  class JSONWriter  {
  protected:
    std::ostream  &stream;
    int           count;
    
  public:
    JSONWriter(std::ostream &aStream) : stream(aStream), count(0) {
      aStream << "[";
    }
    
    ~JSONWriter() {
      stream << "]" << std::endl;
    }
    
    template<typename KeyType, typename ValueType>
    JSONWriter& writePair(KeyType &aKey, ValueType &aValue) {
      if(count++) stream << "," << std::endl;//count++ evaluate before increments, ++count evaluate after increments
      stream << "{\"record\":" << aKey << ", \"blockNum\":" << aValue << "}";
      return *this;
    }

  };
}

#endif /* JSONWriter_h */
