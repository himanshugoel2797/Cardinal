SOURCES=main.o \
		common/common.o common/sprintf.o common/list.o common/elf.o common/initrd.o common/unicode.o \
		managers/bootstrap_mem_pool/bootstrap_mem_pool.o \
		boot_information/multiboot2/parseMultiboot2.o \
		acpi/acpi_tables.o kmalloc/kmalloc.o kmalloc/block_alloc.o \
		managers/process_manager/process.o \
		managers/thread_manager/thread.o \
		managers/power_manager/pwm.o \
		managers/device_manager/device_manager.o \
		managers/syscall_manager/syscall.o \
		managers/syscall_manager/syscalls/syscall_main.o \
		managers/syscall_manager/syscalls/time.o \
		managers/syscall_manager/syscalls/thread.o \
		managers/syscall_manager/syscalls/memory.o \
		managers/syscall_manager/syscalls/signal.o \
		managers/syscall_manager/syscalls/file_server.o \
		managers/syscall_manager/syscalls/ipc.o \
		sys/pipe.o \
		drivers/usb/controller.o \
		drivers/usb/uhci/uhci_controller.o \
		Sibyl/sec_mon.o
