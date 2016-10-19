ENTRY(_start)

SECTIONS
{
	. = 4M;

	.text ALIGN(4K) : AT(ADDR(.text))
	{
		*(.entry_point)
		*(.text)
	}

	/* Read-only data. */
	.rodata ALIGN(8) : AT(ADDR(.rodata))
	{
		*(.rodata)
	}

	/* Read-write data (initialized) */
	.data ALIGN(8) : AT(ADDR(.data))
	{
		*(.data)
	}

	/* Read-write data (uninitialized) and stack */
	.bss ALIGN(8) : AT(ADDR(.bss))
	{
		*(COMMON)
		*(.bss)
	}
}
