#ifndef CHIPSET_DRIVERS_H_
#define CHIPSET_DRIVERS_H_

#include "types.h"
#include "ich9/chipset_driver.h"

//Determine a means to identify chipsets, there should be two ways, first a PCI device check and then a check for some value that can be reliably determined from the configuration space, thus checking is in two rounds, the chipset is determined appropriately, the driver may also check for a configuration file on disk

//! Detect the current chipset and if a driver is available, register the driver for it

void
Chipset_IdentifyAndRegister(void);

#endif