#ifndef _NETWORK_INTERFACE_DRIV_H_
#define _NETWORK_INTERFACE_DRIV_H_

#include "types.h"
#include "rtl8139/rtl8139.h"

//* Provides a means for a network device to be interfaced without regard for which device it is
typedef struct
{
    bool (*detect)(uint32_t);				//*< Check if this network device is present
    uint32_t (*init)(uint32_t);				//*< Initialize the network device
    uint32_t (*enable)(uint32_t);			//*< Enable the network device
    uint32_t (*disable)(uint32_t);			//*< Disable the network device
    uint32_t (*set_power_state)(uint32_t, 	//*< Set the power state for the network device
                                uint32_t);
    void (*transmit)(void*,					//*< Transmit a packet of data
                     uint16_t);

    bool active;
    bool present;
    int pci_index;
} NI_DriverInterface;


//* Initialize the network interface

//* \sa NI_RequestTransmitPointer(), NI_RequestRecievePointer()
void
NI_Initialize(void);

//* Start the network interface

//* \sa NI_Initialize()
void
NI_Start(void);


//* Used by drivers to notify the OS that a packet has been recieved, the OS will copy the packet into another buffer
//* allowing the driver to keep recieving


void
NI_NotifyPacketRecieved(void *packet,
                        uint16_t len);

#endif