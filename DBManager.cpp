//
//  DBManager.cpp
//  ECEDatabase
//
//  Created by rick gessner on 3/30/18.
//  Copyright © 2018 rick gessner. All rights reserved.
//

#include "DBManager.hpp"
#include "Tokenizer.hpp"
#include "Database.hpp"
#include "Storage.hpp"
#include <iostream>

#include <stdio.h>  //for remove()
#include "ShowDatabaseView.hpp"
#include <sys/stat.h>
#include <dirent.h>

#include "timer.hpp"
#include "DBDumper.hpp"

namespace SF {
  
  DBManager DBManager::manager;  //initialization
  
  DBManager::DBManager(){
    active=nullptr;
    //std::cout<<"\t\tcalling DBManager()"<<std::endl;
  }
  
  DBManager::~DBManager() {
    releaseDB();
    //std::cout<<"\t\tcalling ~DBManager()"<<std::endl;
  }
  
  DBManager& DBManager::releaseDB() {
    if(active) {
      delete active;
      active=nullptr;
    }
    return *this;
  }
  
    
  StatusResult DBManager::dropAllIndexFile(std::string aName){
      DIR *dir = opendir(Storage::getDefaultStoragePath());
      if(dir){
          struct dirent *dirp = nullptr;
          while( (dirp = readdir(dir))!=NULL ){
              std::string filename = std::string(dirp->d_name);
              std::string targetFirstPrefix(aName);
              std::string targetSuffix = "ndx";
              
              size_t dotPos = filename.find(".");
              size_t underlinePos = filename.find_first_of("_");
              if(dotPos == std::string::npos || underlinePos == std::string::npos)
                  continue;
              std::string realFirstPrefix = filename.substr(0,underlinePos);
              std::string realSuffix      = filename.substr(dotPos+1,std::string::npos);
              if( targetFirstPrefix.compare(realFirstPrefix) == 0  && targetSuffix.compare(realSuffix)==0){
                  char thePath[128];
                  sprintf(thePath,"%s/%s", Storage::getDefaultStoragePath(), filename.c_str());
                  if(remove(thePath) !=0 ){
                      std::cout << "\tError: Could not delete " << thePath << std::endl;
                      return StatusResult{false};
                  }
              }
          }
          return StatusResult{true};
      }
      std::cout<<"\tCan't open "<<Storage::getDefaultStoragePath()<<"."<<std::endl;
      return StatusResult{false};
  }
    
    
  Database* DBManager::findDatabase(const std::string aName) {
    char thePath[128];
    sprintf(thePath,"/%s/%s.%s", Storage::getDefaultStoragePath(), aName.c_str(),"db");
    
    if (FILE *file = fopen(thePath, "r")) {
      fclose(file);
      Database *theDB=new Database(aName);
      return theDB;
    }
    return nullptr;
  }
  
  StatusResult DBManager::createDatabase(const std::string aName) {
    active=releaseDB().findDatabase(aName);
    if(!active) {
      active=new Database(aName);
      StatusResult ret = active->initializeStorage(true);
        if(VERBOSE_MODE){
            if(ret){
                std::cout << "\tSuccessfully created database " << aName << std::endl;
            }
        }
        
        return ret;
    }
    std::cout<<"\tDatabase "<<aName<<" already exists."<<std::endl;
    return StatusResult{true};
  }
  
  StatusResult DBManager::dropDatabase(const std::string aName) {
    //------------------------------------
    // Your code here...
      if(active){
          //TBD -- add getFileName() to confirm aName = database to be dropped
          if(aName.compare(active->getFileName()) == 0)
              releaseDB();
      }
      
      char thePath[128];
      sprintf(thePath,"%s/%s.%s", Storage::getDefaultStoragePath(), aName.c_str(),"db");
      
      if(remove(thePath) != 0){
          std::cout << "\tError: Could not delete " << aName << std::endl;
          throw StatusResult{false, gUnknownDatabase};
      }
      
      //drop all the schema index file
      if( dropAllIndexFile(aName) ){
          std::cout << "\tSuccessfully deleted database " << aName << "." << std::endl;
          return StatusResult{true};
      }
      else{
          std::cout<<"\tDrop database "<<aName<<" fails."<<std::endl;
          return StatusResult{false};
      }
      
    //------------------------------------
    
  }
  
  StatusResult DBManager::showDatabases() {
    //------------------------------------
    // Your code here...
      std::string thePath = "/tmp";
      std::string theExtension = ".db";
      
      ShowDatabaseView view(thePath);
      return view.show(theExtension, std::cout);
  }
    
  
  StatusResult DBManager::useDatabase(const std::string aName) {
      
      if(active){
          if(aName.compare(active->getName())==0){
              std::cout<<"\tAlready in database "<<aName<<std::endl;
              return StatusResult{true};
          }
      }
      active = releaseDB().findDatabase(aName);
      if(active){
          active->initializeStorage(false);
          std::cout << "\tSet " << aName << " as active database." << std::endl;
          return StatusResult{true};
      }
      else{
          std::cout<<"\tDatabase "<<aName<<" doesn't exist."<<std::endl;
          return StatusResult{false,gUnknownDatabase};
      }
      
  }
    
    //make big assumption that Foo exists, is active DB
    //make big assumption that users table exists, defined exactly the way we want
  StatusResult DBManager::insert1000TestRecords(){
      if(active->getName() == "Foo" && active->tableExists("users")){
          
          Timer timer(true);
          /*
           id         integer primary key auto_increment,
           first_name varchar(20),
           last_name  varchar(20),
           email      varchar(50),
           age        integer
           */
          
          Schema* usersTable = active->getSchema("users");
          std::vector<Attribute> theAttributes = usersTable->getAttributes();
          
          for(int i = 0; i < 1000; i++){
              std::string aFakeCmd = "insert into users (id, first_name, last_name, email, age) values (" + std::to_string(i) + ", firstname" + std::to_string(i) + ", lastname" + std::to_string(i) + ", email" + std::to_string(i) + ", " + std::to_string(1000-i) + ")";
              active->handleCommand(aFakeCmd);
          }
          
          std::cout << "Elapsed: " << std::fixed << timer << "ms" << std::endl;
          
      }else{
          std::cout << "You're doing it wrong bud" << std::endl;
          return StatusResult{false, gUnknownDatabase};
      }
      
      
      return StatusResult{true};
  }
  
    
    StatusResult DBManager::dumpDB(std::string& aDBName){
        return active->dumpBlocks();
    }
    
    
  StatusResult DBManager::handleCommand(std::string &aUserInput, ICommandable *aNext) {
    
    //std::istringstream theUserInput(aUserInput);
    
    Tokenizer theTokenizer(aUserInput);
    StatusResult theResult=theTokenizer.tokenize();
    
    if(theResult) {
      Token theToken = theTokenizer.tokenAt(0);
      Keywords theKW=theTokenizer.getKeywordId(theToken.data);
      
      Token theToken1 = theTokenizer.tokenAt(1);
      Keywords theType=theTokenizer.getKeywordId(theToken1.data);
      
      theResult=StatusResult{false};
      switch(theKW) {
          case Keywords::dump_kw:
              return dumpDB(theToken1.data);
              break;
              
              
        case Keywords::create_kw:
          
          if(Keywords::database_kw==theType) {
            Token theNameToken = theTokenizer.tokenAt(2);
            return createDatabase(theNameToken.data);
          }
          else if(Keywords::table_kw==theType) {
            if(active) {
              return active->handleCommand(aUserInput);
            }
            return StatusResult(false,gNoDatabaseSpecified);
          }
          
          break;
          
        case Keywords::drop_kw:
          if(Keywords::database_kw==theType) {
            Token theNameToken = theTokenizer.tokenAt(2);
            return dropDatabase(theNameToken.data);
          }
          else if(Keywords::table_kw==theType) {
            if(active) {
              return active->handleCommand(aUserInput);
            }
            std::cout << "\tNo database in use" << std::endl;
            return StatusResult(false,gNoDatabaseSpecified);
          }
          break;
          
        case Keywords::show_kw:
            if(Keywords::databases_kw==theType) {
              return showDatabases();
            }
            else {
              if(active) {
                return active->handleCommand(aUserInput);
              }
              return StatusResult(false,gNoDatabaseSpecified);
            }
            break;
          
        case Keywords::use_kw:
            {
              Token theDBName = theTokenizer.tokenAt(2);
              return useDatabase(theDBName.data);
            }
            break;
              
        case Keywords::help_kw:
          std::cout << "\n\tDatabase Commands:" << std::endl;
          
          std::cout << "\t - create database {database_name} - creates new database named {database_name}, if it doesn't already exist." << std::endl;
          
          std::cout << "\t - use database {database_name} - selects database named {database_name} for use, if it exists." << std::endl;
          
          std::cout << "\t - drop database {database_name} - deletes database file associated with {database_name}." << std::endl;
          
          std::cout << "\t - show databases - lists available databases." << std::endl;
          
          if(active){
              return active->handleCommand(aUserInput);  //get all the help commands that can be use d so far
          }else{
              std::cout << std::endl;   //spacing
              return StatusResult{true};
          }
          break;
              
          case Keywords::run1000_kw:
          return insert1000TestRecords();
        
        default:
          break;
      };
      
      if(active) {   //deal with more commands about records and table
        theResult= active->handleCommand(aUserInput);
      }
      
      /*
      if(!theResult && aNext) {
        return aNext->handleCommand(aUserInput);
      }
       */
    }
    
    return theResult;
  }
  
  //-------------------------
  
  DBManager& DBManager::getManager() {return manager;}
  
}
