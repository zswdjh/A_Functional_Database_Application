//
//  MemStream.hpp
//  ECEDatabase5
//
//  Created by rick gessner on 5/8/18.
//  Copyright © 2018 rick gessner. All rights reserved.
//

#ifndef MemStream_h
#define MemStream_h

#include <stdint.h>
#include <cstdio>
#include <string>
#include <cstring>
#include <stdexcept>
#include "UserValue.hpp"
#include "Keywords.hpp"

namespace SF {
  
  //-------------------------------------------
  
  class InMemStream {
  protected:
    char*   buffer;
    size_t  size; //actual size
    size_t  max;  //max size...
    int     pos;  //read pos
    
  public:
    InMemStream() { close();}
    
    InMemStream(char* aBuffer, size_t aSize, size_t aMax) : buffer(aBuffer) {
      size=aSize; //#bytes written into buffer...
      max=aMax;   //actual buffer len
      pos=0;
      memcpy(buffer, aBuffer, aSize);
    }
    
    void close() {
      memset(buffer,0,max);
      pos=size=max=0;
    }

    bool eof() const {
      return pos >= size;
    }

    char* getBuffer() {return (char*)&buffer[0];}

    std::ifstream::pos_type tellg() {return pos;}

    bool seekg (size_t aPos) {
      if(pos<size)
        pos = (int)aPos;
      else return false;
      return true;
    }
    
    bool seekg (std::streamoff offset, std::ios_base::seekdir way) {
      if(way==std::ios_base::beg && offset < size)
        pos = (int)offset;
      else if(way==std::ios_base::cur && (pos + offset) < size)
        pos += offset;
      else if(way==std::ios_base::end && (size + offset) < size)
        pos = (int)(size + offset);
      else return false;
      return true;
    }
    
    template<typename T>
    void read(T& t) {
      if(eof())
        throw std::runtime_error("EOF");
      
      if((pos + sizeof(T)) > size)
        throw std::runtime_error("EOF");
      
      std::memcpy(reinterpret_cast<void*>(&t), &buffer[pos], sizeof(T));
      pos += sizeof(T);
    }
    
    void read(char* aPtr, size_t aSize) {
      if(eof())
        throw std::runtime_error("EOF");

      if((pos + aSize) > size)
        throw std::runtime_error("EOF");

      std::memcpy(reinterpret_cast<void*>(aPtr), &buffer[pos], aSize);
      pos += aSize;
    }
    
    void read(std::string& str, const unsigned int aSize) {
      if (eof())
        throw std::runtime_error("EOF");

      if ((pos + aSize) > size)
        throw std::runtime_error("EOF");

      str.assign(&buffer[pos], aSize);
      
      pos += str.size();
    }
  };
  
  InMemStream& operator >> (InMemStream& istm, UserValue& aValue) {

    switch(aValue.datatype) {
      case Keywords::integer_kw:
        istm.read(aValue.anInteger);
        aValue.value=std::to_string(aValue.anInteger);
        break;
      case Keywords::float_kw:
        istm.read(aValue.aFloat);
        aValue.value=std::to_string(aValue.aFloat);
        break;
      case Keywords::datetime_kw:
        istm.read(aValue.aDateTime);
        break;
      case Keywords::varchar_kw:
        {
          uint8_t theSize=0;
          istm.read(theSize); //grab the length...
          char theBuffer[256]; //YIKES -- FIX ME! Bad idea!!!!
          memset(theBuffer, 0, sizeof(theBuffer));
          istm.read(theBuffer, theSize);
          aValue.value=theBuffer;
        }
        break;
        
      default: break;
    }
    return istm;
  }
  
  template<typename T>
  InMemStream& operator >> ( InMemStream& istm, T& aValue) {
    istm.read(aValue);
    
    return istm;
  }
  
  InMemStream& operator >> (InMemStream& istm, std::string& aValue) {
    aValue.clear();
    
    int theSize = 0;
    istm.read(theSize);
    
    if(theSize<=0)
      return istm;
    
    istm.read(aValue, theSize);
    return istm;
  }
  
  //-------------------------------------------------
  
  class OutMemStream {
  protected:
    char*   buffer;
    size_t  max;
    size_t  pos; //curr write offset...
    
  public:
    OutMemStream(char* aBuffer, size_t aMax) : max(aMax), buffer(aBuffer) {
      close();
    }
    
    void close() {
      memset(buffer,0,max);
      pos=0;
    }
    
    char* getBuffer() {return (char*)&buffer[0];}
    std::ifstream::pos_type tellp() {return pos;}
    
    template<typename T>
    void write(const T& t) {
      size_t theSize=sizeof(T);
      std::memcpy(reinterpret_cast<void*>(&buffer[pos]), reinterpret_cast<const void*>(&t), theSize);
      pos+=theSize;
    }

    void write(const char* aPtr, size_t aSize) {
      std::memcpy(reinterpret_cast<void*>(&buffer[pos]), reinterpret_cast<const void*>(aPtr), aSize);
      pos+=aSize;
    }
  };
  /*
   reinterpret_cast it is used to convert one pointer of another pointer of any type, no matter either the class is related to each other or not. It doesn't check if the pointer type and the data pointed by the pointer is same or not.
   */
  OutMemStream& operator << (OutMemStream& ostm, const UserValue& aValue) {
    uint8_t theSize = (uint8_t)aValue.value.size();
    switch(aValue.datatype) {
      case Keywords::integer_kw:
        ostm.write(aValue.anInteger);
        break;
      case Keywords::float_kw:
        ostm.write(aValue.aFloat);
        break;
      case Keywords::datetime_kw:
        ostm.write(aValue.aDateTime);
        break;
      case Keywords::varchar_kw:
        ostm.write(theSize);
        ostm.write(aValue.value.c_str(), theSize);
        break;

      default: break;
    }
    return ostm;
  }
  
  template<typename T>
  OutMemStream& operator << (OutMemStream& ostm, const T& aValue) {
    ostm.write(aValue);
    return ostm;
  }

  OutMemStream& operator << (OutMemStream& ostm, const std::string& aValue) {
    uint8_t theSize = (uint8_t)aValue.size();
    ostm.write(theSize);
    
    if(aValue.size()<=0)
      return ostm;
    
    ostm.write(aValue.c_str(), theSize);
    
    return ostm;
  }

  OutMemStream& operator << (OutMemStream& ostm, const char* aValue) {
    int theSize = (int)std::strlen(aValue);
    ostm.write(theSize);
    
    if(theSize<=0)
      return ostm;
    
    ostm.write(aValue, theSize);
    
    return ostm;
  }

} //namespace

#endif /* MemStream_h */
