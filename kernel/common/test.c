/**
 * Copyright (c) 2017 Himanshu Goel
 * 
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

#include "test.h"
#include "memory.h"

#define TEST_INCLUDE_HDRS_
#include "test_defs.h"
#undef TEST_INCLUDE_HDRS_

Test tests[] = {
    #include "test_defs.h"
    {NULL, "", "", ""}
};

void
RunAllTests(void) {
#ifdef _TEST_
    Test *iter = tests;
    while(iter->mthd != NULL) {
        int retVal = iter->mthd();

        //Print the test name, filename and pass/fail status.
        PrintDebugMessage("[Test] NAME: %s ", iter->test_name);
        if(retVal != 0) {
            PrintDebugMessage("FAILED: %d ", retVal);
            PrintDebugMessage("MSG: %s", iter->fail_str);
            PrintDebugMessage("FROM: %s\r\n", iter->filename);
        }else{
            PrintDebugMessage("SUCCESS\r\n");
        }
    }
#endif
}