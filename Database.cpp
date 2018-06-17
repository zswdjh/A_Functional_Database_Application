//
//  Database.cpp
//  ECEDatabase
//
//  Created by rick gessner on 3/30/18.
//  Copyright © 2018 rick gessner. All rights reserved.
//

#include "Database.hpp"
#include "Parser.hpp"
#include "CreateTableStatement.hpp"
#include "Tokenizer.hpp"
#include "Schema.hpp"
#include "SQLInterpreter.hpp"
#include "DescribeTableView.hpp"
#include <iostream>
#include <numeric>

#include <iomanip>

#include "timer.hpp"
#include "DBDumper.hpp"

namespace SF {
  
  //utility to get block #'s from index...
  struct BlockNumExtractor : IndexTreeType::NodeObserver {
    BlockNumExtractor(std::vector<uint32_t> &aList) : list(aList) {}
    virtual void operator()(IndexTreeType::NodeType &aNode) {
      list.push_back(aNode.value);
    }
    std::vector<uint32_t> &list;
  };
  
  //-----------------------------------------------
  
  Database::Database(const std::string aName) : name(aName), storage(aName) {
      //std::cout<<"\t\tcalling Database(string)"<<std::endl;
  }
  
  Database::Database(const Database &aCopy) : name(aCopy.name), storage(aCopy.storage) {
      //std::cout<<"\t\tcalling Database copy constructor"<<std::endl;
  }
  
  Database::~Database() {
    
    for(auto theSchema : schemas) {
      if(theSchema->dirty) {
        theSchema->markDirty(false);
        Block theBlock=(*theSchema); //encode schema to block...
        storage.writeBlock(theSchema->blockId, theBlock);
      }
      delete theSchema;
    }

    //std::cout<<"\t\tcalling ~Database()"<<std::endl;
  }
  
    StatusResult Database::dumpBlocks(){
        DBDumper theDumper(std::cout);
        
        size_t blkNum = storage.getTotalBlockCount();
        
        for(int i = 0; i < blkNum; i++){
            Block theBlock;
            if(storage.readBlock(i, theBlock)){
                std::string blkType;
                char theType = theBlock.getBlockType();
                switch(theType){
                        case 'D':
                        blkType = "Data";
                        break;
                        
                        case 'S':
                        blkType = "Schema";
                        break;
                        
                        case 'M':
                        blkType = "Master";
                        break;
                }
                
                
                int tableId = theBlock.getBlockCount();    //this is wrong
                
                theDumper.writeBlock(i, blkType, tableId);
            }
        }
        
        
        return StatusResult{true};
    }
    
    
  StatusResult Database::initializeStorage(bool reset /*=false*/) {
    if(reset) {
      return storage.makeEmpty();
    }
    return storage.loadMaster();
  }
  
  bool Database::tableExists(const std::string aName) {
    Block theBlock;
    return storage.getSchemaBlock(aName,theBlock);
  }
  
  //given the schema, create an associated table, allocate storage, update the DB...
  StatusResult Database::createTable(Schema &aSchema) {
    
    Block temp;
    if(storage.getSchemaBlock(aSchema.getName(),temp)) {
      return StatusResult{false,gTableExists};
    }
    
    Block theBlock=aSchema; //encode schema to block...
    StatusResult theResult= storage.addSchema(aSchema.getName(), theBlock);
    if(theResult) {
      Schema *theSchema = new Schema(aSchema);
      theSchema->blockId=storage.getLastWrittenBlockNum();
      schemas.push_back(theSchema);
      
        if(VERBOSE_MODE){
            std::cout << "\tSuccessfully created table " << theSchema->name << "." << std::endl;
        }

    }
    
    //If the table created contains primary key, then initJSON file
    if(aSchema.getPrimaryKey().length()){
        aSchema.initJSONFile(aSchema);
    }
    return theResult;
  }
  
  StatusResult Database::describeTable(std::string aTableName) {
    //------------------------------------
    // Your code here...
      uint32_t theHash = Storage::hashString(aTableName.c_str());
      for(int i=0;i<storage.master.header.count;i++)
      {
          if(storage.master.schemas[i].hashId==theHash){
              int schemaBlockId = storage.master.schemas[i].blockId;
              Block receiveBlock;
              if( storage.readBlock(schemaBlockId,receiveBlock) ){
                  
                  if(receiveBlock.header.count!=0){
                      
                      DescribeTableView theView(&receiveBlock);
                      theView.display(std::cout);
                      
                  }else{
                      std::cout<<"\t" << aTableName << " is empty."<<std::endl;
                  }
                  
                  return StatusResult{true};
                  
              }else{
                  throw StatusResult{false,gReadError};
              }
              
          }
          
      }
      std::cout<<"\tError: Table "<<aTableName<<" does not exist!"<<std::endl;
      throw StatusResult{false, gUnknownTable};
    //------------------------------------
   
  }
  
  StatusResult Database::showTables() {
    //------------------------------------
    // Your code here...
      if(storage.master.header.count==0){
          std::cout<<"\t" << name << " is empty." <<std::endl;
          return StatusResult{true};    //empty file does not mean failure
      }
      
      std::cout << "\t" << name << " contains:\n";
      for(int i=0; i<storage.master.header.count; i++) {
          int schemaBlockId = storage.master.schemas[i].blockId;
          Block receiveBlock;
          //if(schemaBlockId==0)
          //    continue;
          if( storage.readBlock(schemaBlockId,receiveBlock) ){
              std::cout<< "\t - " << receiveBlock.attributes[0].name <<std::endl;
          }
          else{
              return StatusResult{false, gNotImplemented};
          }
      }
      return StatusResult{true};
    //------------------------------------

  }
  
  //CALLER owns the schema returned; make sure you delete it!
  Schema* Database::getSchema(const std::string aName) {
    
    //try to pull from schema cache...
    for(auto theSchema : schemas) {
      if(theSchema->name==aName) {
        return theSchema;
      }
    }
  
    Block theBlock;
    if(uint32_t theBlockId=storage.getSchemaBlock(aName,theBlock)) {
      Schema *theSchema = new Schema(theBlock,theBlockId);
      theSchema->setDatabaseName(getName());
      schemas.push_back(theSchema);
      return theSchema;
    }
    return nullptr;
  }
  
    //add new record to primary index (primary key must be integer!)...
  StatusResult addToPrimaryIndex(Schema &aSchema, uint32_t aRecordId, uint32_t aBlockNumber) {
    //------------------------------------
    // Your code here...
    //------------------------------------
    return StatusResult{false,gUnknownTable};
  }
  
    //Add a new row into DB using given data and schema...
  StatusResult Database::insertRow(Schema &aSchema, KeyValueList &aList) {
    //------------------------------------
    // Your code here...
      
      
      Block dataBlock;
      if(aSchema.encodeBlock(dataBlock, aList)){
          if(storage.addBlock(dataBlock)){
              //adding index after adding block successfully in the storage
              
                  std::string thePrimaryKeyName = aSchema.getPrimaryKey();
                  Index* theIndex = aSchema.getPrimaryIndex();
                  if(theIndex){
                      
                        uint32_t theBlockNum = storage.getCurrentBlockNum();
                        theIndex->addKeyValue(std::to_string(dataBlock.getBlockId()), theBlockNum);
                      
                        cache.add(dataBlock, theBlockNum);  //cache add
                      
                        return StatusResult{true};
                      
                  }
                  else{
                        return StatusResult{true};
                  }
              
          }
      }
    //------------------------------------
    return StatusResult{false, gUnknownTable};
  }

  StatusResult Database::updateRow(Schema &aSchema, Filters &aFilters, KeyValueList &aList) {
    //------------------------------------
    // Your code here...
    //------------------------------------
    return StatusResult{true};
  }
  
  StatusResult Database::dropTable(std::string aTableName) {
    //------------------------------------
    // Your code here...
      StatusResult theResult{false};
      Schema* theSchema = getSchema(aTableName);

      if(theSchema){
          Filters      theFilters;
          theResult =  deleteRows( *theSchema, theFilters);  //drop records
          
          
          if(theResult){
              
              theSchema->setDatabaseName(getName());
              /*
              std::string thePrimaryKeyName = theSchema->getPrimaryKey(); //drop index file if exists along with schema
              Index* theIndex = theSchema->getPrimaryIndex();
              if(theIndex){
                  theSchema->dropJSONFile(*theSchema);
              }
              */
              theResult = storage.dropSchema(aTableName);   //release schema block and drop schema in master block
              if(theResult){
                  
                  std::string thePrimaryKeyName = theSchema->getPrimaryKey(); //drop index file if exists along with schema
                  Index* theIndex = theSchema->getPrimaryIndex();
                  uint32_t theHash = theSchema->getHash();
                  std::string thePrimaryKey = theSchema->getPrimaryKey();
                  
                  schemas.erase(std::remove(schemas.begin(),schemas.end(),theSchema),schemas.end());
                  delete theSchema;
                  theSchema = nullptr;
                  
                  if(theIndex){
                     dropJSONFile(theHash,thePrimaryKey);
                  }
                  
                  
                  if(VERBOSE_MODE){
                      std::cout<<"\tDropped table " << aTableName << std::endl;

                  }
                  
                  return StatusResult{true};
              }
          }
      }
    //------------------------------------
    return StatusResult{false, gUnknownTable};
  }
  
  std::string getQueryKey(uint32_t aHash, PropertyList *aFields=nullptr) {
    std::string theResult(std::to_string(aHash));
    if(aFields) {
      for(auto theField: *aFields) {
        theResult+=":";
        theResult+=theField.name;
      }
    }
    return theResult;
  }
  
  RowCollection* Database::selectRows(Schema &aSchema, Filters &aFilters, PropertyList *aFields) {
    
    //--------------------------------------------------
    // If you have a cache, check for query/rows...
    //--------------------------------------------------
      
    //-------------------------------------------------------
    // Otherwise, iterate all blocks and get associated rows
    //-------------------------------------------------------
      
      RowCollection* theRowCollection = new RowCollection();
      uint32_t schemaHash = aSchema.getHash();
      
      
      std::string thePrimaryKeyName = aSchema.getPrimaryKey();
      Index* theIndex = aSchema.getPrimaryIndex();
      
      Timer timer(true);
      
      
      if(theIndex){  //use indexing on select
          std::vector<uint32_t> theBlockList;
          size_t theBlockListSize = theIndex->getBlockNumbers(theBlockList);
          
          if(theBlockListSize==0)
              return nullptr;
          
          std::sort(theBlockList.begin(),theBlockList.end()); //since the primary keys in vector theBlockList are stored in breadth search order, in order to output to console in increasing order, use sort
         
          if(theBlockListSize){
              for(int i = 0; i< theBlockListSize; i++){
                  Block theBlock;
                  //check cache first
                  if(!cache.get(theBlock, theBlockList[i])){
                      //cache miss
                      storage.readBlock(theBlockList[i], theBlock);
                      //add the Block (once read) to the cache
                      cache.add(theBlock, theBlockList[i]);
                  }
                  
                  KeyValues theKeyValues;
                  if(aSchema.decodeBlock(theBlock,theKeyValues.getKeyValues(),aFields)){
                      Row* theRow = new Row(theBlockList[i],theKeyValues,theBlock.getBlockId());
                      theRowCollection->add(theRow);
                  }
                  else
                  {
                      return nullptr;
                  }
                      
              }
          }
          else
          {
              return nullptr;        //nullptr means we didn't find anything
          }
          
      }
      else{     //iterate all records since no indexing for schema
          
          for(int i = 1; i<storage.getTotalBlockCount(); i++){
              Block theBlock;
              if(storage.readBlock(i, theBlock)){
                  if(theBlock.getBlockType()=='D' && theBlock.header.count == schemaHash){
                      
                      //add to cache
                      cache.add(theBlock, i);
                      
                      KeyValues theKeyValues;
                      if(aSchema.decodeBlock(theBlock,theKeyValues.getKeyValues(),aFields)){
                          Row* theRow = new Row(i,theKeyValues);
                          theRowCollection->add(theRow);
                      }
                      else{
                          return nullptr;  //nullptr means we didn't find anything
                      }
                  }
              }else{
                  return nullptr;
              }
          }
          
      }
      
      
      std::cout << "Elapsed: " << std::fixed << timer << "ms" << std::endl;
      if(theRowCollection->getRows().size()){
           return theRowCollection;
      }
      else{
          return nullptr;
      }
    
  }

  StatusResult Database::deleteRows(Schema &aSchema, Filters &aFilters) {
    //------------------------------------
    // Your code here...
      PropertyList thePropertyList;
      thePropertyList.push_back(Property("*",aSchema.getHash()));  //drop all records
      RowCollection* theRowCollection = selectRows(aSchema,aFilters,&thePropertyList);
      
      std::string thePrimaryKeyName = aSchema.getPrimaryKey();
      Index* theIndex = aSchema.getPrimaryIndex();
      
      if(theRowCollection){
          RowList &theRows = theRowCollection->getRows();

          for(auto theRow : theRows){
        
              StatusResult theResult = storage.releaseBlock(theRow->getBlockNumber());//free up the block...
              
              if(!theResult){
                  return theResult;
              }
              if(theIndex){
                  std::string theKey = std::to_string(theRow->getAutoIncrement() );
                  theIndex->removeKey(theKey);
                  
                  //remove from cache
                  cache.remove(theRow->getBlockNumber());
              }
              
          }
          delete theRowCollection;
      }
      return StatusResult{true};
    //------------------------------------
    // return StatusResult{false, gUnknownTable};
  }
  
  StatusResult Database::reorder(RowCollection &aCollection, PropertyList &anOrder) {
    if(anOrder.size()) {
      //do the reordering here...
    }
    return StatusResult{true};
  }

    //Enable class to be part of ICommandable chain...
 /*
  StatusResult Database::handleCommand(std::string &aUserInput,
                                       ICommandable *aNext) {
    Tokenizer theTokenizer(aUserInput);
    StatusResult theResult=theTokenizer.tokenize();
    
    if(theResult) {
      SQLInterpreter theInterpreter(*this);
      theResult=theInterpreter.runSQL(theTokenizer);
    }
    
    return theResult;
  }
 */
    StatusResult Database::handleCommand(std::string &aUserInput,
                                         ICommandable *aNext) {
        Tokenizer theTokenizer(aUserInput);
        StatusResult theResult=theTokenizer.tokenize();
        
        if(theResult) {
            SQLInterpreter theInterpreter(*this);
            theResult=theInterpreter.runSQL(theTokenizer);
            
            if(!theResult){
                //SQL was not handled...must be command for the Database object
                Token theToken = theTokenizer.tokenAt(0);
                Keywords theKW = theTokenizer.getKeywordId(theToken.data);
                
                switch(theKW){
                        
                    case Keywords::help_kw:
                        std::cout << "\n\tThe following commands can be used to interact with tables in the active database (" << name << ")." << std::endl;
                        
                        std::cout << "\t - show tables - lists tables in the active database." << std::endl;
                        
                        std::cout << "\t - create table {table_name} - creates new table named {table_name}, if it doesn't already exist." << std::endl;
                        
                        std::cout << "\t - drop table {table_name} - removes data associated with {table_name} from database." << std::endl;
                        
                        std::cout << "\t - describe {table_name} - shows attributes in {table_name}'s schema." << std::endl;
                        
                        std::cout << "\t - insert into {table_name} (field A, field B, field C) values (value A, value B, value C) - inserts records (A,B,C) into {table_name}. \n\t   Multiple records can be inserted at once with 'values (value A, value B, value C), (value M, value N, value O), etc.' " << std::endl;
                        
                        std::cout << "\t - select * from {table_name}  - selects all records from {table_name}." << std::endl;
                        
                        std::cout << "\t - delete from {table_name}  - delete records from {table_name}." << std::endl;
                        
                        std::cout << std::endl;
                        
                        return StatusResult{true};
                        break;
                        
                    default:
                        theResult = StatusResult{false, gUnknownCommand};
                        break;
                }
            }
            
        }
        
        return theResult;
    }
    
    void   Database::dropJSONFile(uint32_t aHash,std::string aPrimaryKey){
        char thePath[200];
        sprintf(thePath,"%s/%s_%u_%s.ndx", Storage::getDefaultStoragePath(),getName().c_str(),aHash, aPrimaryKey.c_str());
        if(remove(thePath) != 0){
            std::cout << "\tError: Could not delete " << thePath << std::endl;
            throw StatusResult{false, gUnknownDatabase};
        }
    }
    
    
}

