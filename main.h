#ifndef _KERNEL_MAIN_H_
#define _KERNEL_MAIN_H_

void
kernel_main_init(void);

void
kernel_main(void);

void
smp_core_main(int coreID,
              int (*getCoreData)(void));

void
smp_unlock_cores(void);

#endif
