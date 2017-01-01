/*
The MIT License (MIT)

Copyright (c) 2016-2017 Himanshu Goel

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include "device_manager.h"
#include "common.h"
#include "list.h"
#include "kmalloc.h"

typedef struct DeviceEntry {
    char name[MAX_DEVICE_NAME_LEN];
    DeviceType dev_type;
    DeviceActivationStateChangeHandler changeState;
} DeviceEntry;

typedef struct BusEntry {
    char name[MAX_BUS_NAME_LEN];
    List *devices;
} BusEntry;

static List *buses;

void
DeviceManager_Initialize(void) {
    Spinlock s = CreateSpinlock();
    buses = List_Create(s);
}

DeviceManagerError
RegisterBus(const char *name) {
    for(uint64_t i = 0; i < List_Length(buses); i++) {
        BusEntry *bEntry = List_EntryAt(buses, i);
        if(strncmp(bEntry->name, name, strlen(bEntry->name)) == 0) {
            return DeviceManagerError_BusExists;
        }
    }

    BusEntry *b = kmalloc(sizeof(BusEntry));
    if(!b)return DeviceManagerError_Unknown;

    strcpy_s(b->name, MAX_BUS_NAME_LEN, name, strlen(name));
    b->devices = List_Create(CreateSpinlock());
    if(!b->devices) {
        kfree(b);
        return DeviceManagerError_Unknown;
    }

    if(List_AddEntry(buses, b) != ListError_None) {
        kfree(b);
        return DeviceManagerError_Unknown;
    }

    return DeviceManagerError_Success;
}

DeviceManagerError
RegisterDevice(const char *name,
               const char *bus_name,
               DeviceType dev_type,
               DeviceActivationStateChangeHandler handler) {
    if(handler == NULL)return DeviceManagerError_HandlerNotPresent;

    for(uint64_t i = 0; i < List_Length(buses); i++) {
        BusEntry *bEntry = List_EntryAt(buses, i);
        if(strncmp(bEntry->name, bus_name, strlen(bEntry->name)) == 0) {

            for(uint64_t j = 0; j < List_Length(bEntry->devices); j++) {
                DeviceEntry *dEntry = List_EntryAt(bEntry->devices, j);
                if(strncmp(dEntry->name, name, strlen(dEntry->name)) == 0)
                    return DeviceManagerError_DeviceExists;
            }

            DeviceEntry *d = kmalloc(sizeof(DeviceEntry));
            if(!d)return DeviceManagerError_Unknown;

            strcpy_s(d->name, MAX_DEVICE_NAME_LEN, name, strlen(name));
            d->dev_type = dev_type;
            d->changeState = handler;

            if(List_AddEntry(bEntry->devices, d) != ListError_None) {
                kfree(d);
                return DeviceManagerError_Unknown;
            }

            return DeviceManagerError_Success;
        }
    }
    return DeviceManagerError_UnknownBus;
}