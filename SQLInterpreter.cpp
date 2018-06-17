//
//  SQLInterpreter.cpp
//  ECEDatabase
//
//  Created by rick gessner on 4/19/18.
//  Copyright © 2018 rick gessner. All rights reserved.
//


#include "SQLInterpreter.hpp"
#include "Database.hpp"
#include "Statement.hpp"
#include "CreateTableStatement.hpp"
#include "InsertStatement.hpp"
#include "SelectStatement.hpp"
#include "DeleteRowsStatement.hpp"
#include "Schema.hpp"
#include "Tokenizer.hpp"
#include "Parser.hpp"
#include "SelectTableView.hpp"
#include <iostream>

namespace SF {
  
  SQLInterpreter::SQLInterpreter(Database &aDatabase) : database(aDatabase) {}
  SQLInterpreter::~SQLInterpreter() {}

  
    //hidden method to run "insert into..." sql command...
  StatusResult runInsert(Tokenizer &aTokenizer, Database &aDatabase) {
    if(aTokenizer.size()>7) {
      Token theToken = aTokenizer.tokenAt(1);
      if(Keywords::into_kw==aTokenizer.getKeywordId(theToken.data)) {
        Token theToken = aTokenizer.tokenAt(2);

        Schema* theSchema = aDatabase.getSchema(theToken.data);
        theSchema->setDatabaseName(aDatabase.getName());
        if(!theSchema) {
          return StatusResult{false,gUnknownTable};}
        
        Parser theParser(aTokenizer);
        theParser.skip(2);
        
        InsertStatement theStatement(theSchema->getName());
        StatusResult theResult=theStatement.parse(theParser, *theSchema);
        
        if(theResult) {
          theResult=theStatement.validate(*theSchema);
          if(theResult) {            
            //Insert a record for each user-given valuegroup...
            size_t theCount=theStatement.getValueGroupCount();
            for(int i=0; theResult, i<theCount; i++) {
              if(KeyValues *theList = theStatement.getValueGroupAt(i)) {
                theResult=aDatabase.insertRow(*theSchema, theList->getKeyValues());
              }
            }
          }
        }
        if(theResult){
            if(!VERBOSE_MODE){
                std::cout << "\tInsertion successful." << std::endl;
            }
        }
        return theResult;
        

      }
    }
    return StatusResult{false,gInvalidCommand};
  }
  
  //hidden method to run "create table" sql command...
  StatusResult runCreateTable(Tokenizer &aTokenizer, Database &aDatabase) {
    if(aTokenizer.size()>6) {
      Token theToken = aTokenizer.tokenAt(1);
      if(Keywords::table_kw==aTokenizer.getKeywordId(theToken.data)) {
        Token theToken = aTokenizer.tokenAt(2);
        if(!aDatabase.tableExists(theToken.data)) {
          Parser theParser(aTokenizer);
          theParser.skip(2);
          
          CreateTableStatement theStatement(theToken.data);
          StatusResult theResult=theStatement.parse(theParser);
          if(theResult) {
            Schema theSchema(theStatement); //convert statement to schema, then store in the db...
            theSchema.setDatabaseName(aDatabase.getName());
            return aDatabase.createTable(theSchema);
          }
        }
        return StatusResult{false,gTableExists};
      }
    }
    return StatusResult{false,gInvalidCommand};
  }
  
  //hidden method to run "select * from {table} where..." sql command...
  StatusResult runSelectRows(Tokenizer &aTokenizer, Database &aDatabase) {
    if(aTokenizer.size()>3) {
      Parser theParser(aTokenizer);
      SelectStatement theStatement{};
      StatusResult theResult=theStatement.parse(theParser);

      if(theResult) {
        Schema* theSchema=aDatabase.getSchema(theStatement.getTableName()); //everytime after create a Schema, use setDatabaseName() to set DatabaseName in class Schema
        
        if(theSchema) {
          theSchema->setDatabaseName(aDatabase.getName());
          RowCollection *theCollection=aDatabase.selectRows(*theSchema, theStatement.getFilters(),theStatement.getFields());
          if(theCollection) {
            aDatabase.reorder(*theCollection, theStatement.getOrderBy()); //ignore GROUPBY for now...

            //------------------------------------------------------
            // Your code here to generate a tabular view of rows...
              
                  SelectTableView theView(theSchema);
                  theView.display(*theCollection, std::cout);
           //------------------------------------------------------
              
            
          }else{
               std::cout << "\t" << aTokenizer.tokenAt(3).data << " is empty." << std::endl;
          }  
          delete theCollection;
          return StatusResult{true};
          
        }
          
        if(VERBOSE_MODE){
            std::cout << "\tError: Unknown table!" <<std::endl;
        }
        throw StatusResult{false, gUnknownTable};
      }
        
      if(VERBOSE_MODE){
        std::cout << "\tError: Invalid command!" <<std::endl;
      }
      throw StatusResult{false, gInvalidCommand};
    }
      
    if(VERBOSE_MODE){
        std::cout << "\tError: Invalid command!" <<std::endl;
    }
    return StatusResult{false,gInvalidCommand};
  }

  //hidden method to run "delete from {table} where..." sql command...
  StatusResult runDeleteRows(Tokenizer &aTokenizer, Database &aDatabase) {
    if(aTokenizer.size()>2) {
      Parser theParser(aTokenizer);
      DeleteRowsStatement theStatement{};
      
      StatusResult theResult=theStatement.parse(theParser);
      if(theResult) {
        Schema* theSchema=aDatabase.getSchema(theStatement.getTableName());
        if(theSchema) {
            theResult = aDatabase.deleteRows(*theSchema, theStatement.getFilters());
          if(theResult){
            std::cout << "\tDelete successful."<< std::endl;
            return theResult;
          }
          if(VERBOSE_MODE){
            std::cout << "\tError: Deleting fails!" <<std::endl;
          }
          throw StatusResult{false, gInvalidCommand};
        }
        
        if(VERBOSE_MODE){
            std::cout << "\tError: Invalid command!" <<std::endl;
        }
        throw StatusResult{false, gInvalidCommand};
      }
    }
      
    if(VERBOSE_MODE){
        std::cout << "\tError: Invalid command!" <<std::endl;
    }
    throw StatusResult{false, gInvalidCommand};
  }

  
  StatusResult SQLInterpreter::runSQL(Tokenizer &aTokenizer) {
    
    Token theToken = aTokenizer.tokenAt(0);
    Keywords theKW=aTokenizer.getKeywordId(theToken.data);
    switch(theKW) {
      case Keywords::create_kw: //create table...
        return runCreateTable(aTokenizer, database);
        break;
        
      case Keywords::alter_kw:  //alter table...
        break;
        
      case Keywords::drop_kw:   //drop table...
        {
          Token theToken1 = aTokenizer.tokenAt(1);
          Keywords theType= aTokenizer.getKeywordId(theToken1.data);
          if (Keywords::table_kw == theType){
              Token theName = aTokenizer.tokenAt(2);
              return database.dropTable(theName.data);
          }
          
        }
        break;
        
      case Keywords::insert_kw:  //insert record(s)...
        return runInsert(aTokenizer, database);
        break;
        
      case Keywords::update_kw:  //update record(s)...
        break;
        
      case Keywords::delete_kw:  //delete records(s)...
        return runDeleteRows(aTokenizer, database);
        break;
        
      case Keywords::select_kw:  //select records(s)...
        return runSelectRows(aTokenizer, database);
        break;

      case Keywords::describe_kw:
        {
          Token theDBName = aTokenizer.tokenAt(1);
          return database.describeTable(theDBName.data);
        }
        break;

        case Keywords::show_kw:{
            Token theToken1 = aTokenizer.tokenAt(1);
            Keywords theType= aTokenizer.getKeywordId(theToken1.data);
            if (Keywords::tables_kw == theType)
                return database.showTables();
        }
        
        break;
        
      default:
        break;
    };
    
    return StatusResult{false}; //we didn't handle it...
  }
  
}
