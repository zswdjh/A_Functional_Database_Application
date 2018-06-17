//
//  UserValue.cpp
//  ECEDatabase
//
//  Created by rick gessner on 4/21/18.
//  Copyright © 2018 rick gessner. All rights reserved.
//

#include <string.h>
#include <stdio.h>
#include "UserValue.hpp"
#include "Token.hpp"
#include "Storage.hpp"

namespace SF {

  UserValue::UserValue() : value(""), type(TokenType::unknown), datatype(Keywords::unknown_kw) {
    anInteger=0;
  }
  
  UserValue::UserValue(std::string &aValue, TokenType aType) : value(aValue), type(aType), datatype(Keywords::unknown_kw) {
    anInteger=0;
  }
    
  UserValue::UserValue(const UserValue &aCopy) : value(aCopy.value), type(aCopy.type), datatype(aCopy.datatype) {
    become(aCopy.datatype);
  }

  UserValue::UserValue(Token &aToken) : value(aToken.data), type(aToken.type), datatype(Keywords::unknown_kw) {
  }
  
  UserValue::UserValue(Keywords aDataType) : value(""), type(TokenType::unknown), datatype(aDataType) {
  }
  
  StatusResult UserValue::set(const int32_t anIntValue) {
    datatype=Keywords::integer_kw;
    type=TokenType::number;
    anInteger=anIntValue;
    value=std::to_string(anIntValue);
    return StatusResult{true};
  }
  
  StatusResult UserValue::set(const float aFloatValue) {
    datatype=Keywords::float_kw;
    type=TokenType::number;
    aFloat=aFloatValue;
    value=std::to_string(aFloatValue);
    return StatusResult{true};
  }
  
  StatusResult UserValue::set(const std::string &aString) {
    datatype=Keywords::varchar_kw;
    type=TokenType::string;
    value=aString;
    return StatusResult{true};
  }
  
  StatusResult UserValue::becomeInteger() {
    anInteger=0;
    if(value.find_first_not_of("-0123456789") == std::string::npos) {
        if(value.empty()){
            value = "0";
            std::string::size_type theSize;
            return set(std::stoi(value,&theSize));
        }
        //std::string::size_type is a type that is big enough to represent the size of a string, no matter how big that string is
       std::string::size_type theSize;
       return set(std::stoi (value, &theSize));
    }
    return StatusResult{false,gInvalidArguments};
  }
  
  StatusResult UserValue::becomeFloat() {
    aFloat=0;
    if(value.find_first_not_of("-.0123456789") == std::string::npos) {
        if(value.empty()){
            value = "0";
            std::string::size_type theSize;
            return set(std::stof(value,&theSize));
        }
      std::string::size_type theSize;
      return set(std::stof (value, &theSize));
    }
    return StatusResult{false,gInvalidArguments};
  }

  StatusResult UserValue::becomeDateTime() {
    datatype=Keywords::datetime_kw;
    type=TokenType::string;

    //TBD -- FIX ME!...
    return StatusResult{true};
  }

  StatusResult UserValue::becomeVarchar() {
    datatype=Keywords::varchar_kw;
    type=TokenType::string;
    return StatusResult{true};
  }

  StatusResult UserValue::become(Keywords aKeyword) {
    switch(aKeyword) {
      case Keywords::integer_kw:  return becomeInteger();
      case Keywords::float_kw:    return becomeFloat();
      case Keywords::datetime_kw: return becomeDateTime();
      case Keywords::varchar_kw:  return becomeVarchar();
      default: break;
    }
    return StatusResult{false, gUnknownType};
  }

  //-----------------------------------------------------------------------------
  // KeyValues class is just a simple cover for a vector<pair<string,uservalue>>..
  //-----------------------------------------------------------------------------

  KeyValues::KeyValues() : keyValues() {
  }
  
  bool KeyValues::addKeyValue(std::string &aKey, UserValue &aValue) {
    keyValues.push_back(std::pair<std::string, UserValue>(aKey, aValue));
    return true;
  }
  
}
