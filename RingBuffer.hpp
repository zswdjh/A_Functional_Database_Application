//
//  RingBuffer.hpp
//  RingBuffer
//
//  Created by Vladi Iotov on 6/9/18.
//  Copyright © 2018 vladi. All rights reserved.
//

#ifndef RingBuffer_hpp
#define RingBuffer_hpp

#include <stdio.h>
#include <iostream>
#include <vector>

template <typename T, unsigned int SIZE>
class RingBuffer{
    private:
    
        int theHead; //head index
        T* theArray;
    
    public:
        RingBuffer(){
            theHead = 0;
            theArray = new T[SIZE];
        }
    
        ~RingBuffer(){
            //kinda sketchy if T is a pointer to an object
            if(theArray){
                delete [] theArray;
            }
        }
    
    /*
     T& operator[](int anIndex){
     int i = theHead + anIndex;
     
     while(i >= SIZE){
         i -= SIZE;
     }
     
     theHead = i;
     
     return theArray[i];
     }
    */

    
    //agnostically add things into array
    unsigned int append(const T& aType){
        //wraparound
        if(theHead >= SIZE){
            theHead = 0;
        }
        
        theArray[theHead++] = aType;
        
        
        return theHead - 1;
        
    }
    
    T& get(int anArrayIndex){
        return theArray[anArrayIndex];
    }
    
    
    const T& operator = (const T& aType){
        return aType;
    }


};


#endif /* RingBuffer_hpp */
