SOURCES=main.o \
		common/common.o common/sprintf.o common/list.o common/heap.o common/btree.o common/initrd.o common/unicode.o \
		common/test.o \
		managers/bootstrap_mem_pool/bootstrap_mem_pool.o \
		boot_information/multiboot2/parseMultiboot2.o \
		acpi/acpi_tables.o kmalloc/kmalloc.o kmalloc/block_alloc.o \
		managers/thread_manager/thread.o \
		managers/syscall_manager/syscall.o \
		managers/syscall_manager/syscalls/syscall_main.o \
		managers/syscall_manager/syscalls/time.o \
		managers/syscall_manager/syscalls/thread.o \
		managers/syscall_manager/syscalls/memory.o \
		managers/syscall_manager/syscalls/ipc.o \
		managers/syscall_manager/syscalls/property.o \
		managers/syscall_manager/syscalls/errno.o \
		managers/syscall_manager/syscalls/boot_info.o \
		managers/syscall_manager/syscalls/shared_memory.o \
		managers/syscall_manager/syscalls/interrupts.o \
		managers/syscall_manager/syscalls/key.o \
		managers/elf_loader/elf.o \
		managers/key_manager/key_manager.o \
		managers/key_systems/key_systems.o \
		managers/key_systems/shared_memory/shared_memory.o \
		managers/key_systems/signal/signal.o \
		managers/interrupt_manager/interrupt_man.o