//
//  ShowDatabaseView.hpp
//  database
//
//  Created by Vladi Iotov on 5/14/18.
//  Copyright © 2018 vladi. All rights reserved.
//

#ifndef ShowDatabaseView_hpp
#define ShowDatabaseView_hpp

#include <stdio.h>
#include <string>
#include <iostream>
#include <dirent.h>
#include <sys/types.h>
#include "Keywords.hpp"

namespace SF{
    
    class ShowDatabaseView{
        private:
            std::string thePath;
            StatusResult traverseDirectoryByExtension(std::string& aExt, std::ostream& aStream);
            void printFileName(std::string& aFileName, std::ostream& aStream);
        
        public:
            ShowDatabaseView(std::string& aPath) : thePath(aPath){};
            StatusResult show(std::string& anExtension, std::ostream& aStream);
        
    };
    
}

#endif /* ShowDatabaseView_hpp */
