
#ifndef _CARDINAL_ELF_SERVER_H_
#define _CARDINAL_ELF_SERVER_H_

#include <stddef.h>
#include <stdint.h>
#include <cardinal/ipc.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum ElfLoaderError {
    ElfLoaderError_Success,
    ElfLoaderError_IncompatibleBinary,
    ElfLoaderError_NotElf,
    ElfLoaderError_UnknownABI,
    ElfLoaderError_UnknownClass,
    ElfLoaderError_OutOfMemory,
} ElfLoaderError;

typedef enum ElfLimitations {
    ElfLimitations_ArchBits = 1,
    ElfLimitations_32Bit = 1,
    ElfLimitations_64Bit = 0,
    ElfLimitations_LSB = (0 << 1),
    ElfLimitations_MSB = (1 << 1)
} ElfLimitations;

typedef struct ElfInformation {
    void *entry_point;
    void *phdr_data;
    uint64_t phdr_num;
    uint64_t phdr_ent_size;
} ElfInformation;

struct ElfSetupParameters {
    uint16_t ver;
    uint16_t size;
    uint16_t page_size;
    uint16_t argc;
    uint64_t rnd_seed;
    uint64_t elf_entry_point;
    char argv[1];
};

ElfLoaderError
LoadElf(void *loc,
        uint64_t size,
        ElfLimitations limits,
        UID pid,
        ElfInformation *elfData
       );

int
RequestLoadElf(void *loc,
			 uint64_t size,
			 ElfLimitations limits,
			 UID pid,
			 uint32_t *key);

int
IsElfLoaded(uint32_t id,
			ElfLoaderError *error,
			ElfInformation *elfData);

typedef enum {
	ElfLoaderMessageType_LoadRequest
} ElfLoaderMessageType;

typedef struct {
    Message m;
    ElfLoaderMessageType MsgType;
    ElfLimitations Limits;
    void *Source;
    uint64_t Size;
    UID TargetPID;
} ElfLoadRequest;

typedef struct {
	Message m;
	ElfLoaderError result;
	ElfInformation info;
} ElfLoadResponse;

#ifdef __cplusplus
}
#endif

#endif