//
//  CommandProcessor.hpp
//  ECEDatabase
//
//  Created by rick gessner on 3/30/18.
//  Copyright © 2018 rick gessner. All rights reserved.
//

#ifndef CommandProcessor_hpp
#define CommandProcessor_hpp

#include <stdio.h>
#include <string>
#include "Keywords.hpp"

namespace SF {
  
  class ICommandable {
  public:
    virtual StatusResult handleCommand(std::string &aUserInput, ICommandable *aNext=nullptr)=0;
  };
  
  class CommandProcessor : public ICommandable {
  protected:
    
    virtual StatusResult handleCommand(std::string &aUserInput, ICommandable *aNext=nullptr);
    bool running;
    
  public:
    
    CommandProcessor(bool aRunning=false);
    ~CommandProcessor();
    
    static StatusResult   run(); //iteractively get user input and run commands...
    
  };
  
}

#endif /* CommandProcessor_hpp */

