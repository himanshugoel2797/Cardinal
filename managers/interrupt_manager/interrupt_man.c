#include "interrupt_man.h"
#include "common/common.h"
#include "interrupts.h"
#include "managers.h"

#define IRQ_COUNT 0xFF
#define MAX_SUBSCRIBERS 4

static UID irq_subscriber_pids[IRQ_COUNT][MAX_SUBSCRIBERS];

static void
InterruptMan_InterruptHandler(uint32_t int_no,
							  uint32_t UNUSED(err_code)) {

	CREATE_NEW_MESSAGE_PTR_TYPE(InterruptMessage, m);
	m->m.MsgType = CardinalMsgType_Interrupt;
	m->m.MsgID = int_no;
	m->vector = int_no;

	for(int i = 0; i < MAX_SUBSCRIBERS; i++) {
		if(irq_subscriber_pids[int_no][i] != 0) {
			PostMessages(irq_subscriber_pids[int_no][i], (Message**)&m, 1);
		}
	}	
}

void
InterruptMan_Initialize(void) {

	for(int i = 0x10; i < IRQ_COUNT; i++){
		memset(irq_subscriber_pids[i], 0, MAX_SUBSCRIBERS * sizeof(UID));
		RegisterInterruptHandler(i, InterruptMan_InterruptHandler);
		SetInterruptEnableMode(i, TRUE);
	}
}

void
InterruptMan_RegisterProcess(UID pid, 
							 uint32_t irq, 
							 uint32_t cnt) {

	for(uint32_t i = irq; i < cnt; i++) {

		bool irq_filled = FALSE;
		for(uint32_t j = 0; j < MAX_SUBSCRIBERS; j++) {
			if(irq_subscriber_pids[i][j] == 0) {
				irq_subscriber_pids[i][j] = pid;
				irq_filled = TRUE;
				break;
			}
		}

		if(irq_filled == FALSE)
			__asm__ ("cli\n\thlt");

	}
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