//
//  SQLInterpreter.hpp
//  ECEDatabase
//
//  Created by rick gessner on 4/19/18.
//  Copyright © 2018 rick gessner. All rights reserved.
//

#ifndef SQLInterpreter_hpp
#define SQLInterpreter_hpp

#include <stdio.h>
#include "Keywords.hpp"

namespace SF {
  
  class Database;
  class Statement;
  class Tokenizer;
  
  class SQLInterpreter {
  protected:
    Database &database;
    
  public:
    SQLInterpreter(Database &aDatabase);
    ~SQLInterpreter();
    
    StatusResult runSQL(Tokenizer &aTokenizer);

  };
}

#endif /* SQLInterpreter_hpp */
