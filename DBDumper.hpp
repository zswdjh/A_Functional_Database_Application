//
//  DBDumper.hpp
//  SFDatabase
//
//  Created by Vladi Iotov on 6/12/18.
//  Copyright © 2018 vladi. All rights reserved.
//

#ifndef DBDumper_hpp
#define DBDumper_hpp

#include <stdio.h>
#include <iostream>

namespace SF {
    
    class DBDumper  {
    protected:
        std::ostream  &stream;
        int           count;
        
    public:
        DBDumper(std::ostream &aStream) : stream(aStream), count(0) {
            aStream << "Block#  Type    TableID\n";
        }
        
        ~DBDumper() {
            stream << std::endl;
        }
        
        DBDumper& writeBlock(unsigned int aBlkNum, std::string& aBlkType, unsigned int aTableID) {
            if(count++) stream << std::endl;//count++ evaluate before increments, ++count evaluate after increments
            //0       Master  0
            
            std::string blkStr = std::to_string(aBlkNum);
            stream << blkStr;
            //padding
            for(int spaces = blkStr.length(); spaces < 8; spaces++){
                stream << " ";
            }
            
            stream << aBlkType;
            //padding
            for(int spaces = aBlkType.length(); spaces < 8; spaces++){
                stream << " ";
            }
            
            std::string tIDStr = std::to_string(aTableID);
            stream << tIDStr;
            //padding
            for(int spaces = tIDStr.length(); spaces < 8; spaces++){
                stream << " ";
            }

            return *this;
        }
        
    };
}

#endif /* DBDumper_hpp */
