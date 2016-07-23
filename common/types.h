#ifndef _OS_TYPES_H_
#define _OS_TYPES_H_

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

typedef uint64_t UID;
typedef uint64_t SysID;

typedef void (*Callback)(void*);
typedef uint32_t (*Initializer)();
typedef int(*ProcessEntryPoint)(int, char**);

typedef uint64_t off_t;

#define TRUE 1
#define FALSE 0

#define ENABLE 1
#define DISABLE 0

#define ENABLED 1
#define DISABLED 0


#define KiB(x) (x * 1024ull)
#define MiB(x) (KiB(1) * 1024ull * x)
#define GiB(x) (uint64_t)(MiB(1) * 1024ull * x)

#define UNUSED(x) UNUSED_ ## x __attribute__((__unused__))

#if defined(DEBUG)

#if defined(x86_64)
#define ASSERT(x) if(!(x))__asm__ volatile("cli\n\thlt")
#endif


#else

#define ASSET(x)
#endif

#endif /* end of include guard: _OS_TYPES_H_ */
