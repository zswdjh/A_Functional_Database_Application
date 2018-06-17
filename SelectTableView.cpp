//
//  SelectTableView.cpp
//  SFDatabase
//
//  Created by Vladi Iotov on 5/15/18.
//  Copyright © 2018 vladi. All rights reserved.
//

#include "SelectTableView.hpp"
#include <iomanip>
namespace SF {
    
#define decimalSize 15
#define varcharSize 50
    
    bool SelectTableView::describeTabular() {
        std::ostringstream theStringStream;
        
        std::string hLine;
        std::vector<Attribute> attributes = (*theSchema).getAttributes();
        for(int i = 0; i < attributes.size();i++){
            int columnSize = decimalSize;
            if(attributes[i].getType() == Keywords::varchar_kw)
                columnSize = varcharSize;
            theStringStream << "| "<<attributes[i].getName()<<std::string(columnSize-2-attributes[i].getName().size(),' ');
            hLine = hLine+ "+" +std::string(columnSize-1,'-');
        }
        theStringStream<<"|";
        hLine = hLine + "+" + "\n";
        
        
        std::cout<<hLine;
        std::cout<<theStringStream.str()<<std::endl;
        std::cout<<hLine;
        
        
        return true;
    }
    
    
    StatusResult SelectTableView::display(RowCollection& aCollection, std::ostream& aStream){
        
         describeTabular();
         std::string newLine = "\n";
         std::string empty;
         RowList theRows = aCollection.getRows();
         
         for(auto& rowPt: theRows){
             KeyValueList theKvList = rowPt->getColumns();
             std::string hLine;
             for(auto& theKvPair:theKvList){
                std::string aString = " "+theKvPair.second.getValue();
                int aFieldLength = decimalSize;
                if(theKvPair.second.getDataType() == Keywords::varchar_kw){
                aFieldLength = varcharSize;
                }
                 
                printfield_formatted('|', aString, ' ', aFieldLength, aStream);
                hLine += printfield_formatted('+','-', aFieldLength-1);
             }
             printfield_formatted('|', newLine ,' ', (int)newLine.length(),aStream);
             hLine += printfield_formatted('+','\n', (int)newLine.length());
             aStream<<hLine;
        }
        
        return StatusResult{true};
    }
    
    void SelectTableView::printfield_formatted(char aFirstChar, std::string& aString, char aPaddingChar, int aFieldLength,std::ostream& aStream){
        aStream << aFirstChar << aString;
        aStream << std::setfill(aPaddingChar) << std::setw((int)(aFieldLength-aString.length()));
    }
    
    std::string SelectTableView::printfield_formatted(char aFirstChar, char aPaddingChar, int aFieldLength){
        std::string theFirst(1,aFirstChar);
        std::string thePadding(aFieldLength,aPaddingChar);
        return theFirst+thePadding;
    }
}
