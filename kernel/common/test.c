/**
 * Copyright (c) 2017 Himanshu Goel
 * 
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

#include "test.h"


extern Test *tests;

void
RunAllTests(void) {

    Test *iter = tests;
    while(iter->mthd != NULL) {
        int retVal = iter->mthd();

        //Print the test name, filename and pass/fail status.
        if(retVal != 0) {
            
        }
    }

}