// Copyright (c) 2017 Himanshu Goel
// 
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef CARDINAL_CMN_TEST_H_
#define CARDINAL_CMN_TEST_H_

#include "types.h"

#define TEST_DEF(test_nm, f, fail_str) {&f, test_nm, fail_str, __FILE__ },

typedef int (*TestMethod)(void);

typedef struct {
    TestMethod mthd;
    char test_name[256];
    char fail_str[256];
    char filename[64];
} Test;

void
RunAllTests(void);

#endif