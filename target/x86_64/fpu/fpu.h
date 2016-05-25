#ifndef _CARDINAL_X86_64_FPU_H_
#define _CARDINAL_X86_64_FPU_H_

#include "types.h"

//! Initialize the VFPU
void
FPU_Initialize(void);

//! Save the complete VFPU state to the target location
uint32_t
FPU_SaveState(void *target,
              uint32_t size);

//! Restore the complete VFPU state from the given location
void
FPU_LoadState(void *source,
              uint32_t size);

//! Enable/Disable VFPU lazy state saving (VFPU state is stored in the TLS)
void
FPU_SetLazyStateSaveEnableMode(bool enableMode);

#endif