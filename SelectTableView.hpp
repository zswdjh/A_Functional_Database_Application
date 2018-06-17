//
//  SelectTableView.hpp
//  SFDatabase
//
//  Created by Vladi Iotov on 5/15/18.
//  Copyright © 2018 vladi. All rights reserved.
//

#ifndef SelectTableView_hpp
#define SelectTableView_hpp

#include <stdio.h>
#include <iostream>
#include <sstream>

#include "Keywords.hpp"
#include "Row.hpp"
#include "Schema.hpp"

namespace SF {
    
    class SelectTableView{
        
    private:
        Schema* theSchema;
        bool describeTabular();
        void printfield_formatted(char aFirstChar, std::string& aString, char aPaddingChar, int aFieldLength,std::ostream& aStream);
        std::string printfield_formatted(char aFirstChar, char aPaddingChar, int aFieldLength);
        
    public:
        SelectTableView(Schema* aSchema) : theSchema(aSchema){};
        StatusResult display(RowCollection& aRowCollection, std::ostream& aStream);
        
    };
    
}
#endif /* SelectTableView_hpp */
