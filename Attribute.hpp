//
//  Attribute.hpp
//  ECEDatabase
//
//  Created by rick gessner on 4/19/18.
//  Copyright © 2018 rick gessner. All rights reserved.
//

#ifndef Attribute_hpp
#define Attribute_hpp

#include <stdio.h>
#include <string>
#include "Keywords.hpp"
#include "Token.hpp"
#include "Storage.hpp"
#include <sstream>

namespace SF {
  
  struct Property {
    Property(std::string aName, int aTableId=0) : name(aName), tableId(aTableId) {}
    std::string     name;
    bool            desc;
    int             tableId;
  };
  
  typedef std::vector<Property> PropertyList;
  
  struct FunctionCall : public Property {
    FunctionCall(std::string aName, int aFuncId=0, int aTableId=0)
    : Property(aName, aTableId), funcId(aFuncId) {};
    int             funcId;
  };
  
  class Attribute {
  protected:
    std::string   name;
    Keywords      type;
    uint8_t       size; //doesn't include null terminator; only varies for varchar...
    bool          autoIncrement;
    bool          primary;
    bool          nullable;
    
  public:
    
    static Keywords charToKeyword(char aChar);
    static const char* keywordToString(Keywords aType);
      
    Attribute(Keywords aType=Keywords::unknown_kw);
    
    Attribute(std::string aName, Keywords aType=Keywords::unknown_kw,
              uint8_t aSize=0, bool autoIncr=false, bool primary=false);
    
    Attribute(AttributeDef &aDef);

    Attribute(const Attribute &aCopy);
    
    operator AttributeDef();

    ~Attribute();
    
    Attribute&    setName(std::string &aName);
    Attribute&    setType(Keywords aKeyword);
    Attribute&    setSize(int aSize);
    Attribute&    setAutoIncrement(bool anAuto);
    Attribute&    setPrimaryKey(bool anAuto);
    Attribute&    setNullable(bool aNullable);

    std::string&  getName() {return name;}
    Keywords      getType() const {return type;}
    size_t        getSize() const;  //used for storage
    size_t        getWidth() const;  //used for table display...
    bool          getAutoIncrement() {return autoIncrement;}
    bool          getNullable()       {return nullable;}
      //other getters as necessary here...
    
    bool          isCompatible(TokenType aType) const;
    bool          isPrimaryKey() const {return primary;}
    bool          isAutoIncrement() const {return autoIncrement;}

    std::string   describeTabular();
    
    bool isValid(); //self validation...
        
  };
    
}

#endif /* Attribute_hpp */
