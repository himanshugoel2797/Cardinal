TARGET_SOURCES=boot.o bootstrap.o gdt/gdt.o idt/idt.o cpuid/cpuid.o fpu/fpu.o rtc/rtc.o debug_gfx.o \
				hal/memory.o hal/synchronization.o hal/thread.o \
				hal/interrupts/interrupt_man.o hal/timer.o \
				hal/syscall.o \
				pit/pit.o \
				hpet/hpet.o \
				apic/apic.o apic/io_apic/io_apic.o apic/apic_init.o \
				trampoline.o \
				page_manager/phys_mem_manager.o virt_mem_manager/virt_mem_manager.o \
				smp/smp.o \
				syscalls/syscalls.o syscalls/arch_prctl.o \
