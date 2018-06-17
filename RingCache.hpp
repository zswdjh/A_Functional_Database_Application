//
//  RingCache.hpp
//  RingBuffer
//
//  Created by Vladi Iotov on 6/10/18.
//  Copyright © 2018 vladi. All rights reserved.
//

#ifndef RingCache_hpp
#define RingCache_hpp

#include <stdio.h>
#include "RingBuffer.hpp"
#include <map>
#include <iterator>

template <typename T, unsigned int SIZE>
class RingCache{
private:
    unsigned int VectorIndex;
    
    /*
        Underlying ringbuffer is a templatized circular buffer...pretty trivial
     */
    RingBuffer<T, SIZE> RingBuffer;
    
    
    /*
        The RingCache uses the map to get an element at an index from the underlying 
        RingBuffer when we give it a BlockNumber from the Index/AVLTree
     
                        blocknum    ringbuf index                                  */
    typedef std::pair<unsigned int,  int> IndexPair;
    std::vector<IndexPair> Lookup;
    
    void flush(){
        VectorIndex = 0;
        Lookup.clear();
        Lookup.resize(SIZE);
    }
    
public:
    RingCache(){
        VectorIndex = 0;
        Lookup.resize(SIZE);
    };
    
    ~RingCache(){
        flush();
    }
    
    /*
     We add an element to the cache when 
        1. request a cache element misses (returns nullptr)
        2. when a new record is added to a DB
     
     To add something to the cache, it needs a reference to the element
     and it needs the blocknumber which we'll use to request it in later operations via
     RingCache.get()...this is kinda hacky, but we crawl before we walk...
     
     */
    void add(const T& aThing, unsigned int aBlockNum){
        bool dirty = false;
        unsigned int insertedLocation = RingBuffer.append(aThing);
        
        for(int i = 0; i < SIZE; i++){
            IndexPair element = Lookup[i];
            if(element.second == insertedLocation){
                //this IndexPair needs to be updated
                element.first = aBlockNum;
                element.second = insertedLocation; //redundant
                dirty = true;
                VectorIndex = i;
            }
        }
    
        if(!dirty){
            Lookup[VectorIndex].first = aBlockNum;
            Lookup[VectorIndex].second = insertedLocation;
            VectorIndex++;
        }
        
        if(VectorIndex >= SIZE){
            VectorIndex = 0;
        }
        
        
    }
    
    //returns true if aThing is from underlying ringbuffer, after we use the map to find its index
    //returns false if not found in cache
    bool get(T& aThing, int aBlockNum){
        //Traverse Lookup array to see if we can find an RingBufferIndex for the BlockNum
        for(IndexPair &element : Lookup){
            if(element.first == aBlockNum){
                //this IndexPair needs to be updated
                aThing = RingBuffer.get(element.second);
                return true;
            }
        }
        
        
        return false;
    }

    
    bool remove(unsigned int aBlockNum){
        //Traverse Lookup array to see if we can find an the IndexPair for the BlockNum we're trying to delete
         for(IndexPair &element : Lookup){
             if(element.first == aBlockNum){
                 //this IndexPair needs to be cleared...RingBuffer's element doesn't actually have to get cleared, as it's only accessed through the Lookup Map
                 element.first = 0;
                 element.second = -1;   //clear flag
                 return true;
             }
         }

        //Didn't remove anything
        return false;
    }
    
    
    
    
};

#endif /* RingCache_hpp */
