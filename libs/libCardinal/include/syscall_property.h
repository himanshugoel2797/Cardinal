#ifndef _LIB_CARDINAL_PROPERTY_H_
#define _LIB_CARDINAL_PROPERTY_H_

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "syscall_property.h"
#include "syscall.h"

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
    CardinalProperty_SpecialDestination,//!< Set a special destination.
    CardinalProperty_Exit,              //!< Exit the process, setting an exit code.
    CardinalProperty_GroupID,           //!< Get the Group ID, can increase it.
    CardinalProperty_R0_BootInfo,       //!< Get the boot information.
    CardinalProperty_R0_PhysicalAddress,//!< Translate a virtual address to a physical address.
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
    if(val != NULL)
        *val = Syscall2(Syscall_GetProperty, prop, type);

    return GetErrno();
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

#endif
