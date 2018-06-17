//
//  CommandProcessor.cpp
//  ECEDatabase
//
//  Created by rick gessner on 3/30/18.
//  Copyright © 2018 rick gessner. All rights reserved.
//

//
//  SFCommandProcessor.cpp
//  SFDatabase
//
//  KNOWN COMMANDS:
//    create   database |  table
//    delete * from ...
//    describe table
//    drop     database | table
//    help     output available commands on terminal...
//    insert   into ...
//    select * from
//    show     databases | tables
//    update   ...
//    use      database foo
//
//  Created by rick gessner on 3/28/18.
//  Copyright © 2018 rick gessner. All rights reserved.
//

#include <iostream>
#include <map>
#include "CommandProcessor.hpp"
#include "DBManager.hpp"

#define THROWCODES 0
namespace SF {
  
  CommandProcessor::CommandProcessor(bool aRunning) : running(aRunning){}
  
  CommandProcessor::~CommandProcessor() {}
  
  size_t getUserInput(std::string &anInput) {
    std::cout << "> ";
    getline(std::cin, anInput);
    return anInput.length();
  }
  
  StatusResult CommandProcessor::handleCommand(std::string &aUserInput, ICommandable *aNext) {
    std::string theKeyword=aUserInput.substr(0, aUserInput.find(" "));
    
    if("quit"==theKeyword) {
      running=false;
      return StatusResult();
    }
    if("version"==theKeyword || "ver"==theKeyword) {
      std::cout << "\tVersion: " << __DATE__ << std::endl;
      return StatusResult();
    }
    if("help"==theKeyword) {
      std::cout << "Available Commands:"<< std::endl;
      std::cout << "\tGeneral Commands:" << std::endl;

      std::cout << "\t - version (or ver) - echoes version of SFDatabase." << std::endl;
      std::cout << "\t - quit - quits SFDatabase." << std::endl;
    }
    if(aNext) {
      auto handled = aNext->handleCommand(aUserInput,nullptr); //handle DBmanage, database tasks
      if(!handled){
        std::cout << "\tCommand not recognized!" << std::endl;
      }
      return handled;
    }else{
        std::cout<<"\tCommand not recognized!"<<std::endl;
    }
    
    return StatusResult(false);
  }
  
  StatusResult CommandProcessor::run() {
    
    StatusResult theResult{};
    
    CommandProcessor theProcessor;
    DBManager theManager = DBManager::getManager();
    
    std::string theUserInput;
    theProcessor.running=true; 
    while(theProcessor.running && getUserInput(theUserInput)) {
        try{
            theResult = theProcessor.handleCommand(theUserInput,&theManager);
        }catch(StatusResult result){
            if(THROWCODES){
                //tbd...
                std::cout << "Error: " << result.code << std::endl;
            }
            
        }
    }
    return theResult;
  }
  
}

