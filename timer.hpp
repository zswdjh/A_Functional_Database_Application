//
//  timer.hpp
//  SFDatabase
//
//  Created by Vladi Iotov on 6/12/18.
//  Copyright © 2018 vladi. All rights reserved.
//

#ifndef timer_hpp
#define timer_hpp

#include <stdio.h>

//Timer class to be added to your project...
class Timer {
    typedef std::chrono::high_resolution_clock high_resolution_clock;
    typedef std::chrono::milliseconds milliseconds;
    
public:
    explicit Timer(bool run = false) {
        if (run) Reset();
    }
    
    void Reset() {
        _start = high_resolution_clock::now();
    }
    
    milliseconds Elapsed() const {
        return std::chrono::duration_cast<milliseconds>
        (high_resolution_clock::now() - _start);
    }
    
    template <typename T, typename Traits>
    friend std::basic_ostream<T, Traits>& operator <<
    (std::basic_ostream<T, Traits>& out, const Timer& timer) {
        return out << timer.Elapsed().count();
    }
    
private:
    high_resolution_clock::time_point _start;
};
#endif /* timer_hpp */
