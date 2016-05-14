#ifndef _INTEL_HD_AUDIO_DRIVER_H_
#define _INTEL_HD_AUDIO_DRIVER_H_

#include "types.h"
#include "drivers.h"
#include "managers.h"

typedef struct {
    uint32_t Verb : 20;
    uint32_t NID : 7;
    uint32_t IndirectNID : 1;
    uint32_t CAd : 4;
} IHDA_Verb;

uint32_t
IHDA_Initialize(void);

void
IHDA_Reset(void);

void
IHDA_DetectCodecs(void);

void
IHDA_SetupCORB(void);

void
IHDA_SetupRIRB(void);

void
IHDA_WriteVerb(IHDA_Verb* verb);

uint32_t
IHDA_ReadResponse(void);

#endif