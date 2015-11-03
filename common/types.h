#ifndef _OS_TYPES_H_
#define _OS_TYPES_H_

#include <stddef.h>
#include <stdint.h>

typedef uint64_t UID;
typedef UID SysID;

typedef void (*Callback)(void*);
typedef uint32_t (*Initializer)();
typedef int(*ProcessEntryPoint)(int, char**);
typedef uint8_t bool;

#define TRUE 1
#define FALSE 0

#define ENABLE 1
#define DISABLE 0

#define ENABLED 1
#define DISABLED 0


#define KiB(x) (x * 1024)
#define MiB(x) (KiB(1) * 1024 * x)
#define GiB(x) (uint64_t)(MiB(1) * 1024 * x)

#endif /* end of include guard: _OS_TYPES_H_ */
