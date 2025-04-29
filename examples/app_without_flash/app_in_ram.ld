ENTRY( InterruptVector )

/* Run from RAM example, small RAM footprint so should run on any CH32V's */

MEMORY
{
	RAM (xrw)	: ORIGIN = 0x20000000, LENGTH = 2K /* Most parts have more RAM */
}

SECTIONS
{
	.init :
	{ 
		_sinit = .;
		. = ALIGN(4);
		KEEP(*(SORT_NONE(.init)))
		. = ALIGN(4);
		_einit = .;
	} >RAM AT>RAM

	.boot_firmware :
	{
		. = ALIGN(4);
		_boot_firmware_xor = ABSOLUTE((((ABSOLUTE(.)|0x77C) - (ABSOLUTE(.)&0x77C))<<16)|ABSOLUTE(.));
		KEEP(*(.boot_firmware))
		. = ALIGN(4);
	}	>RAM AT>RAM

	.text :
	{
		. = ALIGN(4);
		*(.text)
		*(.text.*)
		*(.rodata)
		*(.rodata*)
		*(.gnu.linkonce.t.*)
		. = ALIGN(4);
	} >RAM AT>RAM 

	.fini :
	{
		KEEP(*(SORT_NONE(.fini)))
		. = ALIGN(4);
	} >RAM AT>RAM

	PROVIDE( _etext = . );
	PROVIDE( _eitcm = . );	

	.preinit_array :
	{
		PROVIDE_HIDDEN (__preinit_array_start = .);
		KEEP (*(.preinit_array))
		PROVIDE_HIDDEN (__preinit_array_end = .);
	} >RAM AT>RAM 
	
	.init_array :
	{
		PROVIDE_HIDDEN (__init_array_start = .);
		KEEP (*(SORT_BY_INIT_PRIORITY(.init_array.*) SORT_BY_INIT_PRIORITY(.ctors.*)))
		KEEP (*(.init_array EXCLUDE_FILE (*crtbegin.o *crtbegin?.o *crtend.o *crtend?.o ) .ctors))
		PROVIDE_HIDDEN (__init_array_end = .);
	} >RAM AT>RAM 
	
	.fini_array :
	{
		PROVIDE_HIDDEN (__fini_array_start = .);
		KEEP (*(SORT_BY_INIT_PRIORITY(.fini_array.*) SORT_BY_INIT_PRIORITY(.dtors.*)))
		KEEP (*(.fini_array EXCLUDE_FILE (*crtbegin.o *crtbegin?.o *crtend.o *crtend?.o ) .dtors))
		PROVIDE_HIDDEN (__fini_array_end = .);
	} >RAM AT>RAM 
	
	.ctors :
	{
		/* gcc uses crtbegin.o to find the start of
		 the constructors, so we make sure it is
		 first.	Because this is a wildcard, it
		 doesn't matter if the user does not
		 actually link against crtbegin.o; the
		 linker won't look for a file to match a
		 wildcard.	The wildcard also means that it
		 doesn't matter which directory crtbegin.o
		 is in.	*/
		KEEP (*crtbegin.o(.ctors))
		KEEP (*crtbegin?.o(.ctors))
		/* We don't want to include the .ctor section from
		 the crtend.o file until after the sorted ctors.
		 The .ctor section from the crtend file contains the
		 end of ctors marker and it must be last */
		KEEP (*(EXCLUDE_FILE (*crtend.o *crtend?.o ) .ctors))
		KEEP (*(SORT(.ctors.*)))
		KEEP (*(.ctors))
	} >RAM AT>RAM 
	
	.dtors :
	{
		KEEP (*crtbegin.o(.dtors))
		KEEP (*crtbegin?.o(.dtors))
		KEEP (*(EXCLUDE_FILE (*crtend.o *crtend?.o ) .dtors))
		KEEP (*(SORT(.dtors.*)))
		KEEP (*(.dtors))
	} >RAM AT>RAM 

	.dalign :
	{
		. = ALIGN(4);
		PROVIDE(_data_vma = .);
	} >RAM AT>RAM	

	.dlalign :
	{
		. = ALIGN(4); 
		PROVIDE(_data_lma = .);
	} >RAM AT>RAM

	.data :
	{
		. = ALIGN(4);
		__global_pointer$ = . + 0x3fc;
		*(.gnu.linkonce.r.*)
		*(.data .data.*)
		*(.gnu.linkonce.d.*)
		. = ALIGN(4);
		*(.sdata .sdata.*)
		*(.sdata2*)
		*(.gnu.linkonce.s.*)
		. = ALIGN(4);
		*(.srodata.cst16)
		*(.srodata.cst8)
		*(.srodata.cst4)
		*(.srodata.cst2)
		*(.srodata .srodata.*)
		. = ALIGN(4);
		PROVIDE( _edata = .);
	} >RAM AT>RAM

	.bss :
	{
		. = ALIGN(4);
		PROVIDE( _sbss = .);
		*(.sbss*)
		*(.gnu.linkonce.sb.*)
		*(.bss*)
		*(.gnu.linkonce.b.*)	
		*(COMMON*)
		. = ALIGN(4);
		PROVIDE( _ebss = .);
	} >RAM AT>RAM

	PROVIDE( runwordpad = 0x20000180 ); /* Size >= 1kb */
	PROVIDE( scratchpad = 0x20000100 );

	PROVIDE( _end = _ebss);
	PROVIDE( end = . );

	PROVIDE( _eusrstack = ORIGIN(RAM) + LENGTH(RAM));	
}




