/*
The MIT License (MIT)

Copyright (c) 2016-2017 Himanshu Goel

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#ifndef _MANAGERS_H_
#define _MANAGERS_H_

#include "bootstrap_mem_pool/bootstrap_mem_pool.h" //Bootstrap memory pool
#include "process_manager/process.h"			   //Process Manager
#include "thread_manager/thread.h"				   //Thread Manager
#include "power_manager/pwm.h"					   //Power Manager
#include "user_manager/user_manager.h"			   //User Manager
#include "syscall_manager/syscall.h"			   //Syscall Manager
#include "key_manager/key_manager.h"			   //Key Manager
#include "interrupt_manager/interrupt_man.h"

#endif /* end of include guard: _MANAGERS_H_ */
