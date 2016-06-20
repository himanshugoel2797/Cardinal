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