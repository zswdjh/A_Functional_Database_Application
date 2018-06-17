//
//  DescribeTableView.cpp
//  SFDatabase
//
//  Created by Vladi Iotov on 5/14/18.
//  Copyright © 2018 vladi. All rights reserved.
//

#include "DescribeTableView.hpp"

//the holy grails
#define nameLen 21
#define typeLen 21
#define keyLen  9
#define defaultLen 12
#define extraLen 18
#define nullLen  12
namespace SF {
    /*
     Helper function for describeTable for printing stuff like:
     +--------------------+--------------------+-------+----------+
     |  Field             |  Type              |  Key  | Extra    |
     +--------------------+--------------------+-------+----------+
     
     aFirstChar = first character (usually + or |)
     aString = the "meat" of what goes in the box
     aPaddingChar = type of char to pad the rest of the line with (usually ' ' or '-' )
     aFieldLength = length of entire field
     
     to print '+---------':
     aFirstChar = +
     aString = "" (empty)
     aPaddingChar = '-'
     aFieldLength = 9 (don't count first char)
     
     to print '| Field      ':
     aFirstChar = ' ' (space)
     aString = "Field"
     aPaddingChar = ' ' (space)
     aFieldLength = 12 (don't count first char)
     
     */
    void DescribeTableView::printFormatted(char aFirstChar, std::string& aString, char aPaddingChar, int aFieldLength, std::ostream& outStream){
        outStream << aFirstChar << aString;
        outStream << std::setfill(aPaddingChar) << std::setw((int)(aFieldLength-aString.length()));
    }
    
    void DescribeTableView::printHLine(std::ostream& aStream){   //print horizontal line of the table
        std::string empty;
        std::string newLine = "\n";
        
        printFormatted('+', empty, '-', nameLen, aStream);
        printFormatted('+', empty, '-', typeLen, aStream);
        printFormatted('+', empty, '-', keyLen, aStream);
        printFormatted('+', empty, '-', nullLen, aStream);
        printFormatted('+', empty, '-', defaultLen, aStream);
        printFormatted('+', empty, '-', extraLen, aStream);
        printFormatted('+', newLine, '\0', (int)newLine.length(), aStream);
        
    }
    
    void DescribeTableView::printTitle(std::ostream& aStream){
        std::string field = " Field";
        std::string type = " Type";
        std::string key = " Key";
        std::string null = " Null";
        std::string def = " Default";
        std::string extra = " Extra";
        std::string empty;
        
        printFormatted('|', field, ' ', nameLen, aStream);
        printFormatted('|', type, ' ', typeLen, aStream);
        printFormatted('|', key, ' ', keyLen, aStream);
        printFormatted('|', null, ' ', nullLen, aStream);
        printFormatted('|', def, ' ', defaultLen, aStream);
        printFormatted('|', extra, ' ', extraLen, aStream);
        printFormatted('|', empty, ' ', (int)empty.length(), aStream);
        
    }

    void DescribeTableView::printAttribute(Keywords aKeyword,std::string aAttributeName, const char* aAttributeType, uint8_t aAttributeSize,bool aPrimaryKey,bool aAutoIncrement, bool aIsNullable, std::ostream& aStream){
       
        std::string name = " " + aAttributeName;
        std::string type = " " + std::string(aAttributeType);
        std::string empty;
        std::string incr = " AUTO INCREMENT";
        std::string prKey = " PRI";
        std::string nullStr = " NULL";
        std::string nNullStr = " NOT";
        
        std::string defaultInt = " 0";
        std::string defaultFloat = " 0";
        std::string defaultDateTime = " 0";
        std::string defaultVarchar = " \"\"";
        
        printFormatted('|', name, ' ', nameLen, aStream);
        
        if(strcmp(aAttributeType,"varchar")==0 && aAttributeSize>0){
            std::string newType = type+std::string(" (") + std::to_string(aAttributeSize) +std::string(")");
            printFormatted('|', newType, ' ', typeLen, aStream);
        }else{
            printFormatted('|', type, ' ', typeLen, aStream);
        }
        
        if(aPrimaryKey){
            printFormatted('|', prKey, ' ', keyLen, aStream);
        }else{
            printFormatted('|', empty, ' ', keyLen, aStream);
        }
        
        if(aIsNullable){
            printFormatted('|', empty, ' ', nullLen, aStream);
        }else{
            printFormatted('|', nNullStr, ' ', nullLen, aStream);
        }
        
       
        switch (aKeyword) {
            case Keywords::integer_kw:
                printFormatted('|', defaultInt, ' ', defaultLen, aStream);
                break;
            case Keywords::float_kw:
                printFormatted('|', defaultFloat, ' ', defaultLen, aStream);
                break;
            case Keywords::datetime_kw:
                printFormatted('|', defaultDateTime, ' ', defaultLen, aStream);
                break;
            case Keywords::varchar_kw:
                printFormatted('|', defaultVarchar, ' ', defaultLen, aStream);
                break;
            default:
                break;
        }
        
        
        if(aAutoIncrement){
            printFormatted('|', incr, ' ', extraLen, aStream);
        }else{
            printFormatted('|' , empty, ' ', extraLen, aStream);
        }
        
        printFormatted('|', empty, ' ', (int)empty.length(), aStream);
        
        printHLine(aStream);
    }
 
    
    void DescribeTableView::printHeader(std::ostream& aStream){
        printHLine(aStream);
        printTitle(aStream);
        printHLine(aStream);
    }
    
    StatusResult DescribeTableView::display(std::ostream& aStream){
        printHeader(aStream);
        
        for(int i=1;i<=theSchemaBlock->header.count;i++){
            Attribute theAttribute = theSchemaBlock->attributes[i];
            
            std::string name = theAttribute.getName();
            const char* type = Attribute::keywordToString(theAttribute.getType());
            size_t size = theAttribute.getSize();
            bool isAutoIncr = theAttribute.isAutoIncrement();
            bool isPrimaryKey = theAttribute.isPrimaryKey();
            bool isNullable = theAttribute.getNullable();
            Keywords theKeyWord = theAttribute.getType();
            printAttribute(theKeyWord, name, type, size, isPrimaryKey, isAutoIncr, isNullable, aStream);
        }

        return StatusResult{true};
    }
    
    
}
