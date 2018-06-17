//
//  Tokenizer.cpp
//  ECEDatabase
//
//  Created by rick gessner on 3/30/18.
//  Copyright © 2018 rick gessner. All rights reserved.
//

#include "Tokenizer.hpp"
#include <map>
#include <iostream>
#include <algorithm>

namespace SF {
  
  //const int maxlen = 2000;
  
  //if these numbers change, fix constants in header file...
  std::map<std::string, Keywords> gDictionary = {
    std::make_pair("add", Keywords::add_kw),
    std::make_pair("all", Keywords::all_kw),
    std::make_pair("alter", Keywords::alter_kw),
    std::make_pair("and", Keywords::and_kw),
    std::make_pair("auto_increment", Keywords::auto_increment_kw),
    std::make_pair("between", Keywords::between_kw),
    std::make_pair("blockNum",Keywords::idxBlockNum_kw),  //
    std::make_pair("by", Keywords::by_kw),
    std::make_pair("char", Keywords::char_kw),
    std::make_pair("column", Keywords::column_kw),
    std::make_pair("create", Keywords::create_kw),
    std::make_pair("database", Keywords::database_kw),
    std::make_pair("databases", Keywords::databases_kw),
    std::make_pair("datetime", Keywords::datetime_kw),
    std::make_pair("decimal", Keywords::decimal_kw),
    std::make_pair("delete", Keywords::delete_kw),
    std::make_pair("describe", Keywords::describe_kw),
    std::make_pair("distinct", Keywords::distinct_kw),
    std::make_pair("double", Keywords::double_kw),
    std::make_pair("drop", Keywords::drop_kw),
    std::make_pair("float", Keywords::float_kw),
    std::make_pair("foreign", Keywords::foreign_kw),
    std::make_pair("from", Keywords::from_kw),
    std::make_pair("group", Keywords::group_kw),
    std::make_pair("in", Keywords::in_kw),
    std::make_pair("insert", Keywords::insert_kw),
    std::make_pair("integer", Keywords::integer_kw),
    std::make_pair("into", Keywords::into_kw),
    std::make_pair("key", Keywords::key_kw),
    std::make_pair("not", Keywords::not_kw),
    std::make_pair("null", Keywords::null_kw),
    std::make_pair("or", Keywords::or_kw),
    std::make_pair("order", Keywords::order_kw),
    std::make_pair("primary", Keywords::primary_kw),
    std::make_pair("record", Keywords::idxRecord_kw),  //
    std::make_pair("references", Keywords::references_kw),
    std::make_pair("select", Keywords::select_kw),
    std::make_pair("set", Keywords::set_kw),
    std::make_pair("show", Keywords::show_kw),
    std::make_pair("table", Keywords::table_kw),
    std::make_pair("tables", Keywords::tables_kw),
    std::make_pair("unique", Keywords::unique_kw),
    std::make_pair("update", Keywords::update_kw),
    std::make_pair("use", Keywords::use_kw),
    std::make_pair("values", Keywords::values_kw),
    std::make_pair("varchar", Keywords::varchar_kw),
    std::make_pair("where", Keywords::where_kw),
//added help
    std::make_pair("help", Keywords::help_kw),
    std::make_pair("run1000", Keywords::run1000_kw),
    std::make_pair("dump", Keywords::dump_kw)
  };
  
  //a list of known functions...
  std::map<std::string,int> gFunctions = {
    std::make_pair("avg", 10),
    std::make_pair("count", 20),
    std::make_pair("max", 30),
    std::make_pair("min", 40)
  };
  
  const char left_paren = '(';
  const char right_paren = ')';
  const char left_bracket = '[';
  const char right_bracket = ']';
  const char left_brace = '{';
  const char right_brace = '}';
  const char colon = ':';
  const char comma = ',';
  const char quote = '"';
  
  //-----------------------------------------------------
  
  Tokenizer::Tokenizer(std::string anInput) : input(anInput)  {
  }
  
  Tokenizer::Tokenizer(const Tokenizer &aCopy) {
      tokens = aCopy.tokens;
  }
  
  bool isWhitespace(char aChar) {
    static const char* theWS = " \t\r\n\b";
    return strchr(theWS,aChar);
     /* source code of strchr  ,"\t" counts for one character.
      char *strchr(const char *s, int c)
      {
      while (*s != (char)c)
      if (!*s++)
      return nullptr;
      return (char *)s;
      }
      
      [About pointer to bool]
      Since nullptr converts implicitly to bool, and bool (unfortunately) converts implicitly to int, one could expect that nullptr should also convert to int. But the point of nullptr is that it should behave as a pointer value. And while pointers do convert implicitly to bool, they do not convert implicitly to numerical types.
      */
  }
  
  bool isNumber(char aChar) {
    return isdigit(aChar) || '.'==aChar;
  }
  
  bool isAlphaNum(char aChar) {
    return isalnum(aChar) || '_'==aChar;
  }
  
  bool isQuote(char aChar) {
    return quote==aChar;
  }
  
  bool isOperator(char aChar) {
    return strchr("+-/*%=", aChar);
  }
  
  bool isSign(char aChar) {
    return strchr("+-", aChar);
  }
  
  bool isPunctuation(char aChar) {
    return strchr("()[]{}:,", aChar);
  }
  
  std::string Tokenizer::readWhile(callback aCallback) {
    std::string theResult;
    while (!input.eof() && (*aCallback)(input.peek())) {
      theResult+=input.get();
    }
    return theResult;
  }
  
  std::string Tokenizer::readUntil(callback aCallback, bool addTerminal) {
    std::string theResult;
    while (!input.eof() && !(*aCallback)(input.peek())) {
      theResult+=input.get();
    }
    if(addTerminal) {
      if(!input.eof()) {
        theResult+=input.get();
      }
    }
    return theResult;
  }
  
  std::string Tokenizer::readUntil(char aTerminalChar, bool addTerminal) {
    std::string theResult;
    while (!input.eof() && input.peek()!=aTerminalChar) {
      theResult+=input.get();
    }
    if(addTerminal) {
      if(!input.eof()) {
        theResult+=input.get();
      }
    }
    return theResult;
  }
  
  int Tokenizer::getFunctionId(const std::string anIdentifier) {
    auto theIter = gFunctions.find(anIdentifier);
    if (theIter != gFunctions.end()) {
      return theIter->second;
    }
    return 0;
  }
  
  Keywords Tokenizer::getKeywordId(const std::string aKeyword) {
    auto theIter = gDictionary.find(aKeyword);
    if (theIter != gDictionary.end()) {
      return theIter->second;
    }
    return Keywords::unknown_kw;
  }
  
  Token Tokenizer::tokenAt(int anOffset) {
    if(anOffset>=0 && anOffset<tokens.size()) {
      return tokens[anOffset];
    }
    return Token(TokenType::string, "?");
  }
  
  //run on input provided in constructor; produce vector of tokens...
  StatusResult Tokenizer::tokenize() {
    
    StatusResult theResult;
    
    while(!input.eof()) {
      char  theChar = input.peek();
      if(isPunctuation(theChar)) {
        std::string temp;
        TokenType theType = TokenType::unknown;
        temp.push_back(input.get());
        switch(theChar) {
          case comma : theType=TokenType::comma; break;
          case colon : theType=TokenType::colon; break;
          case left_paren : theType=TokenType::lparen; break;
          case right_paren : theType=TokenType::rparen; break;
          case left_bracket   : theType=TokenType::lbracket; break;
          case right_bracket  : theType=TokenType::rbracket; break;
          case left_brace     : theType=TokenType::lbrace; break;
          case right_brace    : theType=TokenType::rbrace; break;
        }
        tokens.push_back(Token(theType,temp));
      }
      else if(isOperator(theChar)) {
        std::string temp;
        temp.push_back(input.get());
        tokens.push_back(Token(TokenType::operators,temp));
      }
      else if(isNumber(theChar)) {
        std::string theString=readWhile(isNumber);
        tokens.push_back(Token(TokenType::number,theString));
      }
      else if(isQuote(theChar)) {
        char theQuote=input.get(); //skip first quote...
        std::string theString = readUntil(quote);
        tokens.push_back(Token(TokenType::identifier,theString));
        theQuote=input.get(); //skip last quote...
      }
      else if(isAlphaNum(theChar)) {
        std::string theString = readWhile(isAlphaNum);
        std::string temp(theString);
        std::transform(temp.begin(), temp.end(), temp.begin(), ::tolower);
        
        Keywords theKeyword = getKeywordId(temp);
        if(Keywords::unknown_kw!=theKeyword) {           //deal with sth like auto_increment
          tokens.push_back(Token(TokenType::keyword, temp, theKeyword));
        }
        //else if(getFunctionId(temp)) {
        //  tokens.push_back(Token(TokenType::function,temp));
        //}
        else {
          tokens.push_back(Token(TokenType::identifier,theString));
        }
      }
      readWhile(isWhitespace);
    }
    return theResult;
  }
  
  void Tokenizer::dump() {
    for (auto &theToken : tokens) {
      std::cout << "type ";
      switch(theToken.type) {
        case TokenType::comma:
        case TokenType::lparen:
        case TokenType::rparen:
        case TokenType::slash:
          std::cout << "punct " << theToken.data << std::endl;
          break;
          
        case TokenType::operators:
        case TokenType::star:
        case TokenType::equal:
        case TokenType::plus:
        case TokenType::minus:
          std::cout << "operator " << theToken.data << std::endl;
          break;
          
        case TokenType::number:
          std::cout << "number " << theToken.data << std::endl;
          break;
          
        case TokenType::string:
          std::cout << "string " << theToken.data << std::endl;
          break;
          
        case TokenType::identifier:
          std::cout << "identifier " << theToken.data << std::endl;
          break;
          
        case TokenType::keyword:
          std::cout << "keyword " << theToken.data << std::endl;
          break;
          
        default:
          break;
      }
    }
    
  }
  
}
