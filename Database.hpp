//
//  Database.hpp
//  ECEDatabase
//
//  Created by rick gessner on 3/30/18.
//  Copyright © 2018 rick gessner. All rights reserved.
//

#ifndef Database_hpp
#define Database_hpp

#include <stdio.h>
#include <string>
#include <vector>
#include "Storage.hpp"
#include "Tokenizer.hpp"
#include "CommandProcessor.hpp"
#include "Schema.hpp"
#include "UserValue.hpp"
#include "Filters.hpp"
#include "Row.hpp"
#include "RingCache.hpp"

#define VERBOSE_MODE 1
namespace SF {
    
  class Database : public ICommandable {
    
    std::string           name;
    Storage               storage;
    std::vector<Schema*>  schemas;
    RingCache<Block, 3> cache;
    
  public:
    
    Database(const std::string aPath);    
    Database(const Database &aDatabase);
    
    ~Database();
    
    std::string&  getName() {return name;}
    
    bool            tableExists(const std::string aName);
    Schema*         getSchema(const std::string aName);
    std::string     getFileName() {return name;};
      
    StatusResult    initializeStorage(bool reset=false);
    
    StatusResult    createTable(Schema &aSchema);
    StatusResult    describeTable(std::string aTableName);
    StatusResult    dropTable(std::string aTableName);
    StatusResult    showTables();
        
    RowCollection*  selectRows(Schema &aSchema, Filters &aFilters, PropertyList *aList=nullptr);
    StatusResult    deleteRows(Schema &aSchema, Filters &aFilters);
    StatusResult    insertRow(Schema &aSchema, KeyValueList &aList);
    StatusResult    updateRow(Schema &aSchema, Filters &aFilters, KeyValueList &aList);

    StatusResult    reorder(RowCollection &aCollection, PropertyList &anOrder);

    void          dropJSONFile(uint32_t aHash,std::string aPrimaryKey);
    StatusResult dumpBlocks();
      
    virtual StatusResult handleCommand(std::string &aUserInput, ICommandable *aNext=nullptr);
    
  };
  
}

#endif /* Database_hpp */
