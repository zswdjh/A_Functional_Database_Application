//
//  DescribeTableView.hpp
//  SFDatabase
//
//  Created by Vladi Iotov on 5/14/18.
//  Copyright © 2018 vladi. All rights reserved.
//

#ifndef DescribeTableView_hpp
#define DescribeTableView_hpp

#include <stdio.h>
#include "Schema.hpp"
#include <iostream>
#include <iomanip>

namespace SF {
    
    class DescribeTableView{
    public:
        DescribeTableView(Block* aSchemaBlock) : theSchemaBlock(aSchemaBlock){};
        StatusResult display(std::ostream& aStream);
    private:
        Block* theSchemaBlock;
        void printHeader(std::ostream& aStream);
        void printFormatted(char aFirstChar, std::string& aString, char aPaddingChar, int aFieldLength, std::ostream& outStream);
        void printAttribute(Keywords aKeyword, std::string aAttributeName, const char* aAttributeType, uint8_t aAttributeSize,bool aPrimaryKey,bool aAutoIncrement, bool aIsNullable, std::ostream& aStream);
        void printHLine(std::ostream& aStream);
        void printTitle(std::ostream& aStream);
        
    };
    
}

#endif /* DescribeTableView_hpp */
