TARGET_SOURCES=boot.o bootstrap.o GDT/gdt.o IDT/idt.o CPUID/cpuid.o fpu/fpu.o rtc/rtc.o debug_gfx.o \
				hal/memory.o hal/synchronization.o hal/thread.o \
				hal/interrupts/interrupt_man.o \
				hal/syscall.o \
				pit/pit.o \
				apic/apic.o utils/native.o apic/io_apic/io_apic.o apic/apic_init.o \
				trampoline.o \
				page_manager/phys_mem_manager.o virt_mem_manager/virt_mem_manager.o \
				smp/smp.o syscalls/syscalls.o \
				pci/pci.o pci/pci_devices.o \
				drivers/uhci/uhci.o \
