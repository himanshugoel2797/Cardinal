/*
The MIT License (MIT)

Copyright (c) 2016-2017 Himanshu Goel

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#ifndef _LIB_CARDINAL_PROPERTY_H_
#define _LIB_CARDINAL_PROPERTY_H_

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "syscall_property.h"
#include "syscall.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \defgroup property_syscall Property Syscalls
 * @{
 */

/**
 * The properties that can be manipulated using GetProperty/SetProperty.
 */
typedef enum {
    CardinalProperty_TID,               //!< Get Thread ID.
    CardinalProperty_PID,               //!< Get Process ID.
    CardinalProperty_SetTidAddress,     //!< Set the TID write address.
    CardinalProperty_Time,              //!< Get the time.
    CardinalProperty_GroupID,           //!< Get/Set the Group ID, can increase it.
    CardinalProperty_R0_GroupID,        //!< Get the Group ID for any process.
#ifdef __x86_64__
    CardinalProperty_ArchPrctl,         //!< Perform architecture specific operations.
    CardinalProperty_IOPL               //!< Change the IO privilege level.
#endif
} CardinalProperties;

#ifndef _KERNEL_

/**
 * @brief      Gets a property value.
 *
 * @param[in]  prop  The property
 * @param[in]  type  The subtype of the property
 * @param[out] val   The value
 *
 * @return     Error code on failure, 0 on success.
 */
static __inline uint64_t
GetProperty(CardinalProperties prop, uint64_t type, uint64_t *val) {
    if(val != NULL) {
        *val = Syscall2(Syscall_GetProperty, prop, type);
        return GetErrno();
    }
    return -EINVAL;
}

/**
 * @brief      Sets a property value.
 *
 * @param[in]  prop  The property
 * @param[in]  type  The subtype of the property
 * @param[in]  val   The value to set.
 *
 * @return     Error code on failure, 0 on success.
 */
static __inline bool
SetProperty(CardinalProperties prop, uint64_t type, uint64_t val) {
    return Syscall3(Syscall_SetProperty, prop, type, val);
}

#endif

/**@}*/

#ifdef __cplusplus
}
#endif

#endif
