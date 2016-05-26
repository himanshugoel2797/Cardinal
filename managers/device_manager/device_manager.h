#ifndef _CARDINAL_DEVICE_MAN_H_
#define _CARDINAL_DEVICE_MAN_H_

#include "types.h"

#define MAX_BUS_NAME_LEN 256
#define MAX_DEVICE_NAME_LEN 256

typedef enum DeviceType
{
	DeviceType_Unknown,
	DeviceType_MassStorage,
	DeviceType_Display,
	DeviceType_Multimedia,
	DeviceType_Memory,
	DeviceType_Bridge,
	DeviceType_SimpleComm,
	DeviceType_SystemPeripheral,
	DeviceType_InputDevice,
	DeviceType_DockingStation,
	DeviceType_Processor,
	DeviceType_SerialBus,
	DeviceType_Wireless,
	DeviceType_IntelligentIO,
	DeviceType_SatComm,
	DeviceType_CryptoController
}DeviceType;

typedef enum DeviceManagerError{
	DeviceManagerError_Success,
	DeviceManagerError_UnknownBus,
	DeviceManagerError_Unknown,
	DeviceManagerError_HandlerNotPresent,
	DeviceManagerError_DeviceExists,
	DeviceManagerError_BusExists
}DeviceManagerError;

typedef enum DeviceActivationState{
	DeviceActivationState_Active,
	DeviceActivationState_Disabled
}DeviceActivationState;

typedef void (*DeviceActivationStateChangeHandler)(DeviceActivationState state);

void
DeviceManager_Initialize(void);

DeviceManagerError
RegisterBus(const char *name);

DeviceManagerError
RegisterDevice(const char *name, 
			   const char *bus_name,
			   DeviceType dev_type,
			   DeviceActivationStateChangeHandler handler
			   );

#endif