//
//  Storage.cpp
//  ECEDatabase
//
//  Created by rick gessner on 4/1/18.
//  Copyright © 2018 rick gessner. All rights reserved.
//

#include "Storage.hpp"
#include <iostream>


namespace SF {
  
  const int gMultiplier = 37;
  
  //path to the folder where you want to store your DB's...
  const char* Storage::getDefaultStoragePath() {
    return "/tmp";
  }
    
  
  
  uint32_t Storage::hashString(const char *str) {
    uint32_t h{0};
    unsigned char *p;
    for (p = (unsigned char*)str; *p != '\0'; p++)
      h = gMultiplier * h + *p;
    return h;
  }
  
  Storage::Storage(const std::string aName) : master('M',0), name(aName), lastWriteBlockNum(0) {
    char thePath[128];
    lastWriteBlockNum=0;
    sprintf(thePath,"%s/%s.%s", getDefaultStoragePath(), aName.c_str(),"db");
    stream.open (thePath, std::fstream::binary | std::fstream::in | std::fstream::out);

    master.header.count=0; //init for new db...
    memset(master.data,0,sizeof(master.data));
    //std::cout<<"\t\tcalling Storage(string)"<<std::endl;
  }
  
    
  Storage::Storage(const Storage &aCopy) : name(aCopy.name), lastWriteBlockNum(aCopy.lastWriteBlockNum) {
    lastWriteBlockNum=aCopy.lastWriteBlockNum;
    //std::cout<<"\t\tcalling Storage copy constructor"<<std::endl;
  }
  
    
    
  Storage::~Storage() {
    
    master.header.dirty=false;
    writeBlock(0,master);
    stream.close();
    //std::cout<<"\t\tcalling ~Storage()"<<std::endl;
  }
  
  uint32_t Storage::getSchemaBlock(const std::string &aName, Block &aBlock) {
    uint32_t theHash = Storage::hashString(aName.c_str());
    for(int i=0; i<master.header.count; i++) {
      if(master.schemas[i].hashId==theHash) {
        if(readBlock(master.schemas[i].blockId, aBlock)) {
          return master.schemas[i].blockId;
        }
      }
    }
    return 0; //return 0 means schema block not found...
  }

  StatusResult Storage::loadMaster() {
    stream.seekg(stream.tellg(), std::ios::beg); //attemp to sync stream buffers...
    return Storage::readBlock(0, master);
  }
  
  StatusResult Storage::makeEmpty() {
    char thePath[128];
    sprintf(thePath,"%s/%s.%s", getDefaultStoragePath(), name.c_str(),"db");
    stream.clear(); // Clear Flag, then create file...
    stream.open(thePath, std::fstream::in | std::fstream::out | std::fstream::trunc);
    stream.close();
    stream.open(thePath, std::fstream::binary | std::fstream::in | std::fstream::out);
    return addBlock(master);
  }
  
  bool Storage::isReady() {
    return stream.is_open();
  }
  
  uint32_t Storage::getAutoIncrement() {
    return ++master.header.id;
  }
  
  uint32_t Storage::getTotalBlockCount() {
    stream.seekg(stream.tellg(), std::ios::beg); //force read mode; dumb c++ issue...
    stream.seekg(0, std::ios::end);
    int thePos = (int)stream.tellg();
    return thePos / kBlockSize;
  }
  
  //scan archive for free blocks, and store them here. If you need more use block number for appended blocks...
  uint32_t Storage::getFreeBlock() {
    if(master.header.id) {
      //we have a free block...
    }
    return 0;
  }
  
  StatusResult Storage::readBlock(int aBlockNumber, Block &aBlock, std::size_t aBlockSize) {
    stream.seekg(aBlockNumber * kBlockSize);
    if(stream.read ((char*)&aBlock, aBlockSize)) {
      return StatusResult{true};
    }
    return StatusResult(false, gReadError);
  }
  
    //assumes seekp has already been moved...
  StatusResult write(Block &aBlock, std::fstream &aStream) {
    if(aStream.write ((char*)&aBlock, kBlockSize)) {
      aStream.flush();
      return StatusResult{true};
    }
    return StatusResult{false, gWriteError};
  }

  StatusResult Storage::writeBlock(int aBlockNumber, Block &aBlock) {
    lastWriteBlockNum=aBlockNumber;
    stream.seekg(stream.tellg(), std::ios::beg); //attemp to sync stream buffers...
    stream.seekp(aBlockNumber * kBlockSize);
    return write(aBlock, stream);
  }
  
  StatusResult Storage::addBlock(Block &aBlock) {
    if(int theBlockNum = getFreeBlock()) {
      return writeBlock(theBlockNum, aBlock);
    }
    
    stream.seekg(stream.tellg(), std::ios::beg); //attemp to sync stream buffers...
    stream.seekp(0, std::ios::end); //append...

    master.header.dirty=true; //TEST ONLY
    lastWriteBlockNum=getTotalBlockCount();
    
    return write(aBlock, stream);
  }

  StatusResult Storage::releaseBlock(int aBlockNumber) {
      Block theblock('F',0);
      stream.seekg(stream.tellg(),std::ios::beg);
      stream.seekp(aBlockNumber * kBlockSize);
      return write(theblock, stream);
      //return StatusResult{false,gNotImplemented};
  }
  
  StatusResult Storage::addSchema(const std::string &aName, Block &aBlock) {
    if(addBlock(aBlock)) {
      master.schemas[master.header.count].hashId=Storage::hashString(aName.c_str());
      master.schemas[master.header.count++].blockId=lastWriteBlockNum;
      master.header.dirty=true;
      return StatusResult{true};
    }
    return StatusResult{false,gStorageFull};
  }
  
 StatusResult Storage::moveSchemaForward(int apos){
      if(apos < master.header.count-1){
          int prev = apos;
          int post = apos+1;
          while(post<master.header.count){
              master.schemas[prev].hashId = master.schemas[post].hashId;
              master.schemas[prev].blockId = master.schemas[post].blockId;
              prev++;
              post++;
          }
          master.schemas[prev].hashId = 0;
          master.schemas[prev].blockId = 0;
      }
      return StatusResult{true};
  }
    
  StatusResult Storage::dropSchema(const std::string &aName) {
      Block theBlock;
      uint32_t theHash = hashString(aName.c_str());
      StatusResult theResult{false};
      for(int i=0; i<master.header.count; i++) {
          if(master.schemas[i].hashId==theHash) {
              if(readBlock(master.schemas[i].blockId, theBlock)) {
                  releaseBlock(master.schemas[i].blockId);
                  master.schemas[i].hashId  = 0;
                  master.schemas[i].blockId = 0;
              }
              theResult = moveSchemaForward(i);
              if(theResult){
                  master.header.count--;
                  master.header.dirty = true;
                  return StatusResult{true};
              }
          }
      }
      return StatusResult{false,gNotImplemented};
  }
  
  uint32_t   Storage::getCurrentBlockNum(){
      if(int theBlockNum = getFreeBlock()) {
          return theBlockNum;
      }
      return lastWriteBlockNum;
  }
}

