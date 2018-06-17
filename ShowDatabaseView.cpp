//
//  ShowDatabaseView.cpp
//  database
//
//  Created by Vladi Iotov on 5/14/18.
//  Copyright © 2018 vladi. All rights reserved.
//

#include "ShowDatabaseView.hpp"

namespace SF {
    
    //basic callback function...can be made more thorough easily...
    void ShowDatabaseView::printFileName(std::string& aFileName, std::ostream& aStream){
        aStream << "\t" << aFileName << "\n";
    }
    
    
    StatusResult ShowDatabaseView::traverseDirectoryByExtension(std::string& aExt, std::ostream& aStream){
        DIR *dp;
        struct dirent *dirp;
        
        StatusResult result{false, gFilePathDNE};
        
        if((dp  = opendir(thePath.c_str())) == NULL) {
            return result;
        }
        
        size_t extLen = aExt.length();
        while ((dirp = readdir(dp)) != NULL) {
            std::string currFile = std::string(dirp->d_name);
            
            //only print out filenames with valid extension
            //at a minimum filename is as long as extension
            if(currFile.length() > extLen){
                //check if current file's extension matches requested
                std::string fileExt = currFile.substr(currFile.length()-extLen);
                
                if(fileExt == aExt){
                    currFile = currFile.substr(0, currFile.length()-extLen); //get rid of extension when printing
                    printFileName(currFile, aStream);
                    result = StatusResult{true};
                }
            }
        }
        
        closedir(dp);
        return result;
    }

    
    StatusResult ShowDatabaseView::show(std::string& anExtension, std::ostream& aStream){
        if(!traverseDirectoryByExtension(anExtension, aStream) ){
            aStream << "\tThere are no available databases!\n";
        }
        
        return StatusResult{true};

    }
    
}
