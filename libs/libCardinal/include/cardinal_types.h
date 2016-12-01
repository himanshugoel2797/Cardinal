#ifndef _LIB_CARDINAL_TYPES_H_
#define _LIB_CARDINAL_TYPES_H_

#include <stddef.h>
#include <stdint.h>

//! Unique Identifier
typedef uint64_t UID;

//! System Identifier
typedef uint64_t SysID;

#define KiB(x) (x * 1024ull)
#define MiB(x) (KiB(1) * 1024ull * x)
#define GiB(x) (uint64_t)(MiB(1) * 1024ull * x)

#endif