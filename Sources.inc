SOURCES=main.o \
		common/common.o common/sprintf.o common/list.o \
		managers/bootstrap_mem_pool/bootstrap_mem_pool.o \
		boot_information/multiboot2/parseMultiboot2.o \
		acpi/acpi_tables.o kmalloc/kmalloc.o managers/process_manager/process.o \
		managers/thread_manager/thread.o
