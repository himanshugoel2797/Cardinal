// Copyright (c) 2017 Himanshu Goel
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT
#ifndef CARDINAL_DRIVERINTERFACE_NET_H_
#define CARDINAL_DRIVERINTERFACE_NET_H_

#include <stdint.h>
#include <cardinal/keyman.h>


#define NET_DEVICE_NAME_LEN 256
#define MAC_ADDR_LEN 6
#define NET_NAMESPACE "net"

typedef enum NetDeviceType {
    Ethernet = (1 << 0),
} NetDevceType;

typedef enum NetDeviceFeature {
    NetDeviceFeature_IPChecksum = (1 << 0),
    NetDeviceFeature_UDPChecksum = (1 << 1),
    NetDeviceFeature_TCPChecksum = (1 << 2),
} NetDeviceFeature;

typedef struct NetDeviceData {
    char Name[NET_DEVICE_NAME_LEN];
    uint8_t MAC[MAC_ADDR_LEN];
    NetDeviceType DeviceType;
    NetDeviceFeature DeviceFeatures;
    Key_t TransmitBuffer;
    Key_t ReceiveBuffer;
} NetDeviceData;

void
RegisterNetDevice(const NetDeviceData *const data);

void
GetCurrentNetDeviceFile(void);

NetDeviceFeature
GetEnabledFeatures(void);

#endif