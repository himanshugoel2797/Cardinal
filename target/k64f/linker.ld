MEMORY
{
  VECTORS (rx)          : ORIGIN = 0x00000000, LENGTH = 0x00000400
  FLASH_PROTECTION (rx) : ORIGIN = 0x00000400, LENGTH = 0x00000010
  FLASH (rx)            : ORIGIN = 0x00000410, LENGTH = 0x00100000 - 0x00000410
  RAM (rwx)             : ORIGIN = 0x1FFF0200, LENGTH = 0x00040000 - 0x200
}

ENTRY(_sys_init)
SECTIONS
{
	.interrupt_descriptors :
	{
		*(.interrupt_descriptors)
	} > VECTORS

	.flash_config :
	{
		*(.flash_config)
	} > FLASH_PROTECTION

	.text :
	{
		*(.text)
	} > FLASH

    .ARM.exidx :
    {
      *(.ARM.exidx* .gnu.linkonce.armexidx.*)
    } > FLASH

    .eh_frame :
    {
        *(.eh_frame)
    } > FLASH

	__d = .;
	.data : AT(__d)
	{
		*(.data)
	} > RAM

	.bss :
	{
		*(.bss)
		*(.COMMON)
	} > RAM

	.stack_info :
	{
		*(.stack_info)
	} > RAM
}
