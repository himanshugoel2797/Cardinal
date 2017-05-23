/**
 * Copyright (c) 2017 Himanshu Goel
 *
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */
#include "signal.h"

uint64_t KeySys_WaitSignal(Key_t* signalVal) {}

uint64_t KeySys_HandleSignal(Key_t* signalVal) {}

uint64_t KeySys_Signal(Key_t* signalVal, void* param) {}

uint64_t KeySys_Signal_Read(Key_t* key, void* buff, uint32_t flags) {}

uint64_t KeySys_Signal_Write(Key_t* key, void* buff, uint64_t len,
                             uint32_t flags) {}

uint64_t KeySys_Signal_CreateKey(KeyType_t type, uint32_t flags, void* params,
                                 Key_t* key, uint32_t* key_cnt) {}