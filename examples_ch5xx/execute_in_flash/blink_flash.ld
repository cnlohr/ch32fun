ENTRY(blink_from_flash)

SECTIONS
{
	. = 0x00070000;
	.text : {
		*(.text*)
		*(.rodata*)
		*(.srodata*)
	}

	ASSERT(SIZEOF(.text) <= 0x100, "blink image exceeds its 256-byte DataFlash slot")
}
