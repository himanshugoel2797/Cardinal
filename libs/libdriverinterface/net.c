/**
 * Copyright (c) 2017 Himanshu Goel
 *
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */
#include "net.h"

#include <string.h>
#include <cardinal/namespace/server.h>
#include <cardinal/io/server.h>

#define NetDeviceDataString "Name: %s MAC: %.3x:%.3x:%.3x:%.3x:%.3x:%.3x DeviceType: %.8x DeviceFeatures: %.8x TransmitKey: %s ReceiveKey: %s"

static void NetDeviceDataToString(const NetDeviceData *const data, char *buf) {

    char tx_key[KEY_LENGTH];
    char rx_key[KEY_LENGTH];

    KeyToString(data->TransmitBuffer, tx_key);
    KeyToString(data->ReceiveBuffer, rx_key);

    sprintf(buf, NetDeviceDataString, data->Name,
            data->MAC[0],
            data->MAC[1],
            data->MAC[2],
            data->MAC[3],
            data->MAC[4],
            data->MAC[5],
            data->DeviceType,
            data->DeviceFeatures,
            tx_key,
            rx_key
           );

}

void
RegisterNetDevice(const NetDeviceData *const data) {

}

void
GetCurrentNetDeviceFile(void) {

}

NetDeviceFeature
GetEnabledFeatures(void) {

}