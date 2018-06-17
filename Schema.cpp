//
//  Schema.cpp
//  ECEDatabase
//
//  Created by rick gessner on 4/19/18.
//  Copyright © 2018 rick gessner. All rights reserved.
//

#include "Schema.hpp"
#include "CreateTableStatement.hpp"
#include "Storage.hpp"
#include "Row.hpp"
#include "MemStream2.hpp"
#include "JSONWriter.hpp"
#include "JSONReader.hpp"
#include "Tokenizer.hpp"
#include <fstream>
#include <streambuf>  //istreambuf_iterator
#include <sys/stat.h>

#include <dirent.h>
#include <sys/types.h>
#include <stdio.h> //strcmp , remove

namespace SF {
  
  //utility to save index as json...
  struct IndexObserver : IndexTreeType::NodeObserver {
    IndexObserver(JSONWriter &aWriter) : writer(aWriter) {}
    virtual void operator()(IndexTreeType::NodeType &aNode) {
      writer.writePair(aNode.key, aNode.value);
    }
    JSONWriter &writer;
  };
  
    
  
    
  //----------------------------------------------------
  
  Schema::Schema(std::string aName)
    : name(aName), attributes(), dirty(false), count(0), blockId(0), indexes(), autoincr(0) {
    hash = Storage::hashString(aName.c_str());
        //std::cout<<"\t\tcalling  Schema(string)"<<std::endl;
  }
  
  Schema::Schema(const Schema &aCopy) :
    name(aCopy.name), attributes(aCopy.attributes), dirty(aCopy.dirty), autoincr(aCopy.autoincr),
    count(aCopy.count), blockId(aCopy.blockId), hash(aCopy.hash), indexes(aCopy.indexes) {
    //std::cout<<"\t\tcalling  copy constructor in Schema"<<std::endl;
  }

    Schema::Schema(Block &aSchemaBlock, uint32_t aBlockId) : indexes() {
    name=aSchemaBlock.attributes[0].name;
    hash = Storage::hashString(name.c_str());
    count=aSchemaBlock.header.count;
    autoincr=aSchemaBlock.header.id;
    blockId=aBlockId;
        
    for(int i=1; i<=aSchemaBlock.header.count;i++) {
      AttributeDef theDef=aSchemaBlock.attributes[i];
      attributes.push_back(Attribute(theDef));
    }
    //std::cout<<"\t\tcalling  Schema(block,uint32_t)"<<std::endl;
  }

  //conversion constructor...
    Schema::Schema(const CreateTableStatement &aStatement) : name(aStatement.getTableName()), attributes(aStatement.attributes), autoincr(0) {
      count=attributes.size();
      hash = Storage::hashString(name.c_str());
      blockId=0;
      dirty=true;
      //std::cout<<"\t\tcalling  Schema(CreateTableStatement) "<<std::endl;
  }
  
  Schema::~Schema() {  //incrementally save index as change occur
    
    for (auto theIndex : indexes) {
      if(theIndex.second->isDirty()) {
        char thePath[200];
          sprintf(thePath,"%s/%s_%u_%s.ndx", Storage::getDefaultStoragePath(), getDatabaseName().c_str(), hash, theIndex.first.c_str());
        std::ofstream theStream(thePath, std::ofstream::out);        
        JSONWriter theWriter(theStream);  //only insert and delete will change the index, select will use the index but won't change the index, so put the theWriter in ~Schema()
        IndexObserver theObserver(theWriter);
        theIndex.second->each(theObserver);
      }
      delete theIndex.second; //delete the index/tree...
    }
    //std::cout<<"\t\tcalling ~ Schema()"<<std::endl;
  }
  
  Schema& Schema::markDirty(bool aState) {
    dirty=aState;
    return *this;
  }
    
  bool Schema::describeTabular(std::ostringstream &aStream) {
    
    //------------------------------------------------
    // TODO: Describe tablular data
    //------------------------------------------------
    
    return true;
  }

  Schema& Schema::addAttribute(std::string aName, Keywords aType, uint8_t aSize, bool autoIncr) {
    attributes.push_back(Attribute(aName, aType, aSize));
    return *this;
  }
  
  Schema& Schema::addAttribute(const Attribute &anAttribute) {
    attributes.push_back(anAttribute);
    return *this;
  }
  
  Attribute& Schema::getAttribute(std::string &aName) {
    for(auto theIter = attributes.begin(); theIter != attributes.end(); theIter++) {
      if(theIter->getName()==aName) {
        return *theIter;
      }
    }
    throw std::runtime_error("Attribute not found");
  }

   //pack this schema into a storage block...
  Schema::operator Block() {
    Block theBlock{'S',0}; //0 because location is unknown...
    int   theIndex=1;
    
    strcpy(theBlock.attributes[0].name, name.c_str());
    theBlock.attributes[0].type='S'; //schema...
    for(auto theIter = attributes.begin(); theIter != attributes.end(); theIter++) {
      theBlock.attributes[theIndex++]=(*theIter);
    }
    theBlock.header.count=(uint32_t)attributes.size();
    theBlock.header.id=autoincr; //save current auto-incr value...

    return theBlock;
  }
  
  StatusResult Schema::hasCompatibleAttribute(const std::string &aName, UserValue &aValue) const {
    for (auto theAttribute : attributes) {
      if(theAttribute.getName()==aName) {
        if(theAttribute.isCompatible(aValue.type)) {
          return StatusResult{true};
        }
        return StatusResult{false, gInvalidAttribute};
      }
    }
    return StatusResult{false, gInvalidAttribute};
  }

  StatusResult Schema::validate(KeyValueList &aList) {
    for (auto theKV : aList) {
      if(!hasCompatibleAttribute(theKV.first, theKV.second)) {
        return StatusResult{false, gInvalidAttribute};
      }
    }
    return StatusResult{true};
  }

    //initialize a datablock from a key/value list...
  StatusResult Schema::encodeBlock(Block &aBlock, KeyValueList &aList) {
    aBlock.header.type='D';
    aBlock.header.dirty=true;
    aBlock.header.count=hash;    //store has in header for manual block search against table...
    aBlock.header.id=++autoincr; //record number
    
    //now copy given data (list) into data section of block...
    
    StatusResult theResult{true};
    
    OutMemStream theStream(aBlock.data, sizeof(aBlock.data));
    
    //int theOffset=0;
    for (auto theAttribute : attributes) {
      std::string theName=theAttribute.getName();
      bool        theValueSet=false;
      Keywords    theType = theAttribute.getType();
      
      for(auto theKV : aList) {
        if(theKV.first==theName) {
          theKV.second.become(theType);
          theStream << theKV.second;
          theValueSet=true;
          break;
        }
      } //for
      
      if(!theValueSet) { //provide a default value...
        
        UserValue theDefault; //empty

        if(theAttribute.isPrimaryKey() && theAttribute.isAutoIncrement()) { //we have to use primaryKey and auto_increment together in order to get the default value.
          theDefault.value= std::to_string(aBlock.header.id);
          theDefault.type= TokenType::number;
          theDefault.become(theType);          
        }

        theDefault.become(theType); //make sure the format is right...
        theStream << theDefault;

      }

      if(!theResult) return theResult; //something went wrong...

    } //for
        
    markDirty(true); //autoincr changed...
    return theResult;
  }
  
  bool inGivenFields(std::string &aFieldName, PropertyList *aFields) {
    if(aFields) {
      for(auto theProperty : *aFields) {
        if(theProperty.name==aFieldName) {
          return true;
        }
      }
    }
    return false;
  }
  
  //if caller provides aFields, then only return specified fields...
  StatusResult Schema::decodeBlock(Block &aBlock, KeyValueList &aList, PropertyList *aFields) {
    
    size_t      theSize = sizeof(aBlock.data);
    InMemStream theStream(aBlock.data, theSize, theSize);
    bool        theWholeList = (1 ==aFields->size() && "*"==(*aFields)[0].name);  
    
    for (auto theAttribute : attributes) {
      std::string theName=theAttribute.getName();
      Keywords    theType=theAttribute.getType();
      
      if(theWholeList || inGivenFields(theName, aFields)) {
        UserValue theValue(theType); //setup in unknown state...
        theStream >> theValue; //now read data and set stuff up...
        aList.push_back(KeyValuePair(theName, theValue));
      }
    }
      
    return StatusResult{true};
  }
  
    Index* Schema::searchForIndex( std::string &aKeyName ){
        char theFileName[200];
        sprintf(theFileName,"%s_%u_%s.ndx", getDatabaseName().c_str() ,hash, aKeyName.c_str());
        char theFullName[200];
        sprintf(theFullName,"%s/%s_%u_%s.ndx", Storage::getDefaultStoragePath(),getDatabaseName().c_str() ,hash, aKeyName.c_str());
        
        DIR *dir = opendir(Storage::getDefaultStoragePath());  //pointer to a directory
        if(dir){
            struct dirent *dirp = nullptr;      //used when reading a directory
            while( (dirp = readdir(dir))!=NULL ){
                if(strcmp(dirp->d_name,theFileName) == 0){  //find index file
                    
                    
                    Index* theParsedIndex = new Index;
                    std::ifstream ifs(theFullName,std::ifstream::in);
                    std::string theContent( (std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>() );
                    Tokenizer theTokenizer(theContent);
                    JSONReader theReader(theTokenizer,*theParsedIndex);
                    
                    if( theReader.tokenizer.tokenize() ){
    
                        if( theReader.parse()){   //always return true
                            closedir(dir);
                            return &theReader.theIndex;
                        }
                    }
                    
                    closedir(dir);
                    return nullptr;
                }
            }
            
            //std::cout<<"\t Can't find "<<theFileName <<" in "<<Storage::getDefaultStoragePath()<<" ."<<std::endl;
            
        }
        else{
            std::cout<<"\tError: Can't open directory "<<Storage::getDefaultStoragePath()<<" ."<<std::endl;
        }
        closedir(dir);
        return nullptr;
    }
    
    
  //return ptr to named index, if it exists...
    Index* Schema::getIndex(std::string &aKeyName) {  //style is similar to database::getSchema()

    //--------------------------------------------
    // Your code here:
    //   if index is loaded use it
    //   otherwise load the index (jsonreader?)...
    //--------------------------------------------
      for(auto theIndex : indexes){
          if(theIndex.first == aKeyName){
              return theIndex.second;
          }
      }
      
      if(Index* theSearchedIndex = searchForIndex( aKeyName) ){
         indexes.insert(std::pair<std::string,Index*>(aKeyName,theSearchedIndex) );
         return theSearchedIndex;
      }
     
      return nullptr;
  }
  
  //return ptr to primary key index, if it exists...
  Index* Schema::getPrimaryIndex() {
    std::string thePrimaryKey=getPrimaryKey();
    if(thePrimaryKey.length()) {
      return getIndex(thePrimaryKey);
    }
    return nullptr;
  }
  
  //return name of primary key, or empty_string
  std::string Schema::getPrimaryKey() {
    for (auto theAttribute : attributes) {
      if(theAttribute.isPrimaryKey()) {
        return theAttribute.getName();
      }
    }
    return std::string("");
  }

  void   Schema::initJSONFile(Schema& aSchema){
      char theFullName[200];
      sprintf(theFullName,"%s/%s_%u_%s.ndx", Storage::getDefaultStoragePath(),getDatabaseName().c_str(),hash, aSchema.getPrimaryKey().c_str());
      std::fstream newJSONFile(theFullName,std::fstream::in | std::fstream::out |std::fstream::trunc);
      newJSONFile.close();
  }
  
   
    
}
