/*
The MIT License (MIT)

Copyright (c) 2016-2017 Himanshu Goel

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include "interrupt_man.h"
#include "common/common.h"
#include "interrupts.h"
#include "managers.h"

#define IRQ_COUNT 0xFF
#define MAX_SUBSCRIBERS 4

static UID irq_subscriber_pids[IRQ_COUNT][MAX_SUBSCRIBERS];
static char irq_sub_slot_count[IRQ_COUNT];

static void
InterruptMan_InterruptHandler(uint32_t int_no,
                              uint32_t UNUSED(err_code)) {

    CREATE_NEW_MESSAGE_PTR_TYPE(InterruptMessage, m);
    m->m.MsgType = CardinalMsgType_Interrupt;
    m->m.MsgID = int_no;
    m->vector = int_no;


    for(int i = 0; i < MAX_SUBSCRIBERS; i++) {


        if(irq_subscriber_pids[int_no][i] != 0) {

            PostMessage(irq_subscriber_pids[int_no][i], (Message*)m);
        }
    }
}

void
InterruptMan_Initialize(void) {

    for(int i = 34; i < IRQ_COUNT; i++) {
        memset(irq_subscriber_pids[i], 0, MAX_SUBSCRIBERS * sizeof(UID));
        RegisterInterruptHandler(i, InterruptMan_InterruptHandler);
        irq_sub_slot_count[i] = MAX_SUBSCRIBERS;
    }
}

int
InterruptMan_RegisterProcess(UID pid,
                             int irq,
                             int cnt) {

    ProcessInfo *pInfo = NULL;
    if(GetProcessReference(pid, &pInfo) != ProcessErrors_None)
        return -1;

    if(LockSpinlock(pInfo->lock) == NULL)
        return 0;

    //Mark that this process is using interrupts
    pInfo->InterruptsUsed = 1;

    for(int i = irq; i < irq + cnt; i++) {

        bool irq_filled = FALSE;
        for(uint32_t j = 0; j < MAX_SUBSCRIBERS; j++) {
            if(irq_subscriber_pids[i][j] == 0) {
                irq_subscriber_pids[i][j] = pid;
                irq_filled = TRUE;
                SetInterruptEnableMode(i, TRUE);
                break;
            }
        }

        ASSERT(irq_filled != FALSE, "IRQ Allocation Failure.");

    }

    UnlockSpinlock(pInfo->lock);
    return 0;
}

void
InterruptMan_UnregisterProcess(UID pid) {
    for(uint32_t i = 0; i < IRQ_COUNT; i++) {
        for(uint32_t j = 0; j < MAX_SUBSCRIBERS; j++) {
            if(irq_subscriber_pids[i][j] == pid)
                irq_subscriber_pids[i][j] = 0;
        }
    }
}

int
InterruptMan_AllocateBlock(int cnt) {

    int pos = 0;
    int score = 0;

    for(int i = 0; i < IRQ_COUNT && score < cnt; i++) {
        if(irq_sub_slot_count[i] > 0)score++;
        else {
            pos = i + 1;
            score = 0;
        }
    }

    if((pos + cnt) >= IRQ_COUNT)
        return -1;

    return pos;
}