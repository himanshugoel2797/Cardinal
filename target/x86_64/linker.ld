/* The bootloader will look at this image and start execution at the symbol
   designated as the entry point. */
ENTRY(_start)

/* Tell where the various sections of the object files will be put in the final
   kernel image. */

SECTIONS
{
	KERNEL_VADDR = 0xFFFFFFFF80000000;
	. = 1M;

	.multiboot ALIGN(4K) : AT(ADDR(.multiboot))
	{
		*(.multiboot)
    	*(.bootstrap_stack)
    	*(.bootstrap_code)
	}

	. = 0x0f000;
	.trampoline_code ALIGN(4K) : AT(ADDR(.trampoline_code))
	{
		*(.trampoline_code)
	  	*(.trampoline_data)
		*(.trampoline_stack)
	}
	. = KERNEL_VADDR + 2M;
  	_region_kernel_start_ = .;
	.text ALIGN(4K) : AT(ADDR(.text) - KERNEL_VADDR)
	{
		*(.entry_point)
		*(.tramp_handler)
		*(.text)
	}

	/* Read-only data. */
	.rodata ALIGN(4K) : AT(ADDR(.rodata) - KERNEL_VADDR)
	{
		*(.rodata)
	}

	/* Read-write data (initialized) */
	.data ALIGN(4K) : AT(ADDR(.data) - KERNEL_VADDR)
	{
		*(.data)
	}

	/* Read-write data (uninitialized) and stack */
	.bss ALIGN(4K) : AT(ADDR(.bss) - KERNEL_VADDR)
	{
		*(COMMON)
		*(.bss)
	}

	/* The compiler may produce other sections, by default it will put them in
	   a segment with the same name. Simply add stuff here as needed. */
	_region_kernel_end_ = .; 
}
