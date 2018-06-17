//
//  Storage.hpp
//  ECEDatabase
//
//  Created by rick gessner on 4/1/18.
//  Copyright © 2018 rick gessner. All rights reserved.
//

#ifndef Storage_hpp
#define Storage_hpp

#include <stdio.h>
#include <string>
#include <fstream>
#include <queue>
#include "Keywords.hpp"

namespace SF {
  
  static const int kBlockSize   = 1536; //completely arbitary...
  
  //simple block header...
  struct BlockHeader {
    
    BlockHeader() : type('D'), dirty(false), id(0) {}
    
    BlockHeader(char aType, uint32_t anId) : type(aType), dirty(false), id(anId) {}
    
    BlockHeader(const BlockHeader &aCopy) {
      type=aCopy.type;
      dirty=aCopy.dirty;
      id=aCopy.id;
    }
    
    char      type;   //block type {[D]ata, [F]ree, [M]aster(singleton), [S]chema, }
    bool      dirty;  //currently unused...
    uint32_t  count;  //sometimes a count, sometimes generic usage...
    uint32_t  id;     //id in most blocks; 1st free block for master...
  };
  
  struct SchemaDef {
    uint32_t  hashId;
    uint32_t  blockId;
  };
  
  struct AttributeDef {
    char    name[17];  //len=16, and a null-terminator...
    char    type; //[B]ool, [C]har, [D]ate, [F]loat, [T]able, [V]archar...
    uint8_t length;
    uint8_t extra; //bits to represent other features...
  };
  
  //----- represents a single block in the storage file... -----------------
  
  struct Block {
    
    Block(char aType='D', uint32_t id=0) : header(aType, id) {
      memset(data,0,sizeof(data));
    }
    
    Block(BlockHeader &aHeader) : header(aHeader), data() {}
    
    //we use attributes[0] as table name...
    BlockHeader   header;
    union {
      SchemaDef     schemas[(kBlockSize-sizeof(BlockHeader)/sizeof(SchemaDef))];
      AttributeDef  attributes[(kBlockSize-sizeof(BlockHeader)/sizeof(AttributeDef))];
      char          data[kBlockSize-sizeof(BlockHeader)];
    };
    char       getBlockType()  {return header.type;}
    uint32_t   getBlockId()    {return header.id;}
    uint32_t   getBlockCount() {return header.count;}
  };
  
  //------ actual i/o object...
  
  class Storage {
  protected:
    Block           master;
    std::fstream    stream;
    std::string     name;
    int             lastWriteBlockNum;

    bool            isReady();
    uint32_t        getFreeBlock();
    uint32_t        getTotalBlockCount();
    uint32_t        getAutoIncrement();
    int             getLastWrittenBlockNum() {return lastWriteBlockNum;}
    
    Storage(const Storage &aCopy); //prevent access...
    
    uint32_t        getSchemaBlock(const std::string &aName, Block &aBlock);
    StatusResult    moveSchemaForward(int aPos);
      
  public:
    
    Storage(const std::string aName);
    ~Storage();
    
    static const char* getDefaultStoragePath();
    static uint32_t hashString(const char* aString);
      
    StatusResult    loadMaster();
    StatusResult    makeEmpty();
    
    StatusResult    readBlock(int aBlockNumber, Block &aBlock, std::size_t aBlockSize=kBlockSize);
    StatusResult    writeBlock(int aBlockNumber, Block &aBlock);

    StatusResult    addBlock(Block &aBlock);
    StatusResult    releaseBlock(int aBlockNumber); //tell storage to free up the block...

    StatusResult    addSchema(const std::string &aName, Block &aBlock);
    StatusResult    dropSchema(const std::string &aName);

    uint32_t        getCurrentBlockNum();
    
    friend class Database;
    
  };
  
}

#endif /* Storage_hpp */
