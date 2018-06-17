//
//  Attribute.cpp
//  ECEDatabase
//
//  Created by rick gessner on 4/19/18.
//  Copyright © 2018 rick gessner. All rights reserved.
//

#include "Attribute.hpp"

namespace SF {
  
  const char auto_increment_flag  = 0x1; //for packing into extras...
  const char primary_key_flag     = 0x2; //for packing into extras...
  const char nullable_flag        = 0x4; //for packing into extras...

  
  Attribute::Attribute(std::string aName, Keywords aType, uint8_t aSize, bool autoIncr, bool aPrimary) :
  name(aName), type(aType), size(aSize) {
    autoIncrement=autoIncr;
    primary=aPrimary;
    nullable=true;
  }
  
  Attribute::Attribute(Keywords aType) : type(aType) {
    autoIncrement=primary=false;
    nullable=true;
  }
  
  Attribute::Attribute(AttributeDef &aDef) : name(aDef.name), size(aDef.length) {
    type = Attribute::charToKeyword(aDef.type);
    primary = aDef.extra & primary_key_flag;
    autoIncrement = aDef.extra & auto_increment_flag;
    nullable = aDef.extra & nullable_flag;
  }
  
  Attribute::Attribute(const Attribute &aCopy) : name(aCopy.name), type(aCopy.type), size(aCopy.size) {
    autoIncrement=aCopy.autoIncrement;
    nullable=aCopy.nullable;
    primary=aCopy.primary;
  }
  
  Attribute::~Attribute() {
  }
  
  Attribute& Attribute::setName(std::string &aName) {name=aName; return *this;}
  Attribute& Attribute::setType(Keywords aType) {type=aType; return *this;}
  Attribute& Attribute::setSize(int aSize) {size=aSize; return *this;}
  Attribute& Attribute::setAutoIncrement(bool anAuto) {autoIncrement=anAuto; return *this;}
  Attribute& Attribute::setPrimaryKey(bool aPrimary) {primary=aPrimary; return *this;}
  Attribute& Attribute::setNullable(bool aNullable) {nullable=aNullable; return *this;}

  bool Attribute::isValid() {
    return true;
  }

  bool isNumericKeyword(Keywords aKeyword) {
    static Keywords theTypes[]={Keywords::decimal_kw, Keywords::double_kw, Keywords::float_kw, Keywords::integer_kw};
    for(auto k : theTypes) {
      if(aKeyword==k) return true;
    }
    return false;
  }
  
  bool Attribute::isCompatible(TokenType aType) const {
    switch(aType) {
      case TokenType::number:
        return isNumericKeyword(type);
      case TokenType::identifier:
      case TokenType::string:
        return Keywords::varchar_kw==type;
      default: return false;
    }
    return false;
  }

 const char* Attribute::keywordToString(Keywords aType) {
    switch(aType) {
      case Keywords::integer_kw:  return "integer";
      case Keywords::float_kw:    return "float";
      case Keywords::datetime_kw: return "datetime";
      case Keywords::varchar_kw:  return "varchar";
      default:                    return "unknown";
    }
  }
  
  //convert from char to keyword (storage to object)...
  Keywords Attribute::charToKeyword(char aChar) {
    switch(toupper(aChar)) {
      case 'I': return Keywords::integer_kw;
      case 'D': return Keywords::datetime_kw;
      case 'F': return Keywords::float_kw;
      case 'V': return Keywords::varchar_kw;
      default:  return Keywords::unknown_kw;
    }
  }
  
  //how big a given attribute...
  size_t Attribute::getSize() const {
    switch(type) {
      case Keywords::datetime_kw:
      case Keywords::integer_kw:
        return sizeof(int32_t);
      case Keywords::float_kw:
        return sizeof(float);
      case Keywords::varchar_kw:
        return size;
      default: break;
    }
    return 0;
  }
  
  //used to determine width in view...
  size_t Attribute::getWidth() const {
    switch(type) {
      case Keywords::datetime_kw: return 20;
      case Keywords::integer_kw:
      case Keywords::float_kw:    return 12;
      case Keywords::varchar_kw:  return size+2; //really should be based on actual data...
      default: break;
    }
    return 0;
  }
  

  //pack attribute into something suitable for block storage...
  Attribute::operator AttributeDef() {
    AttributeDef theResult;
    strcpy(theResult.name, name.c_str());
    theResult.length=size;
    const char* theType = keywordToString(type);
    theResult.type=toupper(theType[0]);

    theResult.extra=0; //pack bits for primary key, auto_increment,...
    if(primary) {
      theResult.extra |= primary_key_flag;
    }

    if(autoIncrement) {
      theResult.extra |= auto_increment_flag;
    }
    
    if(nullable) {
      theResult.extra |= nullable_flag;
    }

    return theResult;
  }
  
  std::string Attribute::describeTabular() {
    std::string theResult;
    char theBuffer[100];
    std::sprintf(theBuffer, "| %-17s |",name.c_str()); theResult+=theBuffer;
    std::sprintf(theBuffer," %-12s |",keywordToString(type)); theResult+=theBuffer;
    std::sprintf(theBuffer," %-4s |", "NOT"); theResult+=theBuffer; //nullable...
    
    static const char* keys[]={" ","PRI"};
    const char *temp = keys[autoIncrement];
    
    std::sprintf(theBuffer," %-4s |", temp); theResult+=theBuffer; //key...
    std::sprintf(theBuffer," %-14s |", "   "); theResult+=theBuffer;
    std::sprintf(theBuffer," %-16s |", "extra"); theResult+=theBuffer;
    return theResult;
  }

}
