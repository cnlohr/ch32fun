ENTRY( InterruptVector )

#define KB_HELPER(X) X ## K
#define KB(X) KB_HELPER(X)

MEMORY
{
	FLASH (rx) : ORIGIN = 0x00000000, LENGTH = KB(FLASH_SIZE_KB)
	RAM (xrw)  : ORIGIN = 0x20000000, LENGTH = KB(RAM_SIZE_KB)
#if EXT_ORIGIN
	EXT (rx) : ORIGIN = EXT_ORIGIN, LENGTH = KB(EXT_SIZE_KB)
#endif
#if HAS_COUPLED_RAM
	/* ITCM V5F only */
	ITCM (xrw) : ORIGIN = 0x200A0000, LENGTH = 128K
	/* DTCM V5F only, used for V5F stack  */
	DTCM (xrw) : ORIGIN = 0x200C0000, LENGTH = 256K
#endif
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
		} >FLASH AT>FLASH

#if HAS_HIGHCODE
		.highcodelalign : 
		{       
			. = ALIGN(4);
			PROVIDE(_highcode_lma = .); 
		} >FLASH AT>FLASH 
		
		.highcode :
		{
			. = ALIGN(4);
			PROVIDE(_highcode_vma_start = .);
			*(.highcode*)
			. = ALIGN(4);
			PROVIDE(_highcode_vma_end = .);
		} >RAM AT>FLASH
#endif

#if HAS_COUPLED_RAM
		.itcmalign :
		{
			. = ALIGN(4);
			PROVIDE(_itcm_lma = .);
		} >FLASH AT>FLASH

		.itcm :
		{
			. = ALIGN(4);
			PROVIDE(_itcm_vma_start = .);
			*(.itcm*)
			. = ALIGN(4);
			PROVIDE(_itcm_vma_end = .);
		} >ITCM AT>FLASH

		.dtcmalign :
		{
			. = ALIGN(4);
			PROVIDE(_dtcm_lma = .);
		} >FLASH AT>FLASH

		.dtcm :
		{
			. = ALIGN(4);
			PROVIDE(_dtcm_vma_start = .);
			*(.dtcm*)
			. = ALIGN(4);
			PROVIDE(_dtcm_vma_end = .);
		} >DTCM AT>FLASH
#endif
		.text :
		{
			. = ALIGN(4);
			*(.text)
			*(.text.*)
			*(.rodata)
			*(.rodata*)
			*(.gnu.linkonce.t.*)
			. = ALIGN(4);
		} >FLASH AT>FLASH 

		.fini :
		{
			KEEP(*(SORT_NONE(.fini)))
			. = ALIGN(4);
		} >FLASH AT>FLASH

		PROVIDE( _etext = . );
		PROVIDE( _eitcm = . );  

		.preinit_array :
		{
			PROVIDE_HIDDEN (__preinit_array_start = .);
			KEEP (*(.preinit_array))
			PROVIDE_HIDDEN (__preinit_array_end = .);
		} >FLASH AT>FLASH 
	
		.init_array :
		{
			PROVIDE_HIDDEN (__init_array_start = .);
			KEEP (*(SORT_BY_INIT_PRIORITY(.init_array.*) SORT_BY_INIT_PRIORITY(.ctors.*)))
			KEEP (*(.init_array EXCLUDE_FILE (*crtbegin.o *crtbegin?.o *crtend.o *crtend?.o ) .ctors))
			PROVIDE_HIDDEN (__init_array_end = .);
		} >FLASH AT>FLASH 
	
		.fini_array :
		{
			PROVIDE_HIDDEN (__fini_array_start = .);
			KEEP (*(SORT_BY_INIT_PRIORITY(.fini_array.*) SORT_BY_INIT_PRIORITY(.dtors.*)))
			KEEP (*(.fini_array EXCLUDE_FILE (*crtbegin.o *crtbegin?.o *crtend.o *crtend?.o ) .dtors))
			PROVIDE_HIDDEN (__fini_array_end = .);
		} >FLASH AT>FLASH 
	
		.ctors :
		{
			/* gcc uses crtbegin.o to find the start of
				 the constructors, so we make sure it is
				 first.  Because this is a wildcard, it
				 doesn't matter if the user does not
				 actually link against crtbegin.o; the
				 linker won't look for a file to match a
				 wildcard.  The wildcard also means that it
				 doesn't matter which directory crtbegin.o
				 is in.  */
			KEEP (*crtbegin.o(.ctors))
			KEEP (*crtbegin?.o(.ctors))
			/* We don't want to include the .ctor section from
				 the crtend.o file until after the sorted ctors.
				 The .ctor section from the crtend file contains the
				 end of ctors marker and it must be last */
			KEEP (*(EXCLUDE_FILE (*crtend.o *crtend?.o ) .ctors))
			KEEP (*(SORT(.ctors.*)))
			KEEP (*(.ctors))
		} >FLASH AT>FLASH 
	
		.dtors :
		{
			KEEP (*crtbegin.o(.dtors))
			KEEP (*crtbegin?.o(.dtors))
			KEEP (*(EXCLUDE_FILE (*crtend.o *crtend?.o ) .dtors))
			KEEP (*(SORT(.dtors.*)))
			KEEP (*(.dtors))
		} >FLASH AT>FLASH 

		.dalign :
		{
			. = ALIGN(4);
			PROVIDE(_data_vma = .);
		} >RAM AT>FLASH  

		.dlalign :
		{
			. = ALIGN(4); 
			PROVIDE(_data_lma = .);
		} >FLASH AT>FLASH

		.data :
		{
			. = ALIGN(4);
			__global_pointer$ = . + 0x3fc; /* This gets set in the startup code.  This allows -mrelax'd code to be smaller by acting as a sort of quick reference in the gp register. */
			*(.gnu.linkonce.r.*)
			*(.data .data.*)
			*(.gnu.linkonce.d.*)
			. = ALIGN(8);
			*(.sdata .sdata.*)
			*(.sdata2*)
			*(.gnu.linkonce.s.*)
			. = ALIGN(8);
			*(.srodata.cst16)
			*(.srodata.cst8)
			*(.srodata.cst4)
			*(.srodata.cst2)
			*(.srodata .srodata.*)
			. = ALIGN(4);
			PROVIDE( _edata = .);
		} >RAM AT>FLASH

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
		} >RAM

#if HAS_DMA_QUIRK
	/* CH571/CH573 DMA quirk
	 	DMA peripherals on the CH571/3 only use the low 14 bits of the address
		The high 18 bits are assumed to be 0x20004000,
		yet RAM spans 0x20003800 - 0x20007FFF (18 kB)
		Thus the lower 2K of RAM are not accessible to the DMA peripherals
		Here we create a special .dma_safe segement guaranteed to be above 0x20004000.
		To avoid unnecessary padding, this should be the last segment.
	*/

	/* Force this to be the higher of 0x20004000 and end of previous */
	/* Mark it NOLOAD so it doesn't occupy any flash space */
		.dma_safe (NOLOAD) :
		{
			. = MAX(ORIGIN(RAM) + 0x800, ALIGN(4));
			PROVIDE( _sdma_safe = .);
			*(.dma_safe*)
			. = ALIGN(4);
			PROVIDE( _edma_safe = .);
		} >RAM

		PROVIDE( _end = _edma);
#else 
		PROVIDE( _end = _ebss);
#endif

		PROVIDE( end = . );

#if HAS_COUPLED_RAM
		PROVIDE( _v3f_stack = ORIGIN(RAM) + LENGTH(RAM));
		PROVIDE( _v5f_stack = ORIGIN(DTCM) + LENGTH(DTCM));
#else
		PROVIDE( _eusrstack = ORIGIN(RAM) + LENGTH(RAM));
#endif

#if EXT_ORIGIN
		.storage :
		{
			. = ALIGN(4);
			*(.storage)
			*(.storage.*)
			*(.external)
			*(.external.*)
			. = ALIGN(4);
		} >EXT AT>EXT
#endif


		/DISCARD/ : {
			*(.note .note.*)
			*(.eh_frame .eh_frame.*)
			*(.comment .comment.*)
			*(.ARM.extab* .gnu.linkonce.armextab.*)
			*(.ARM.exidx*)
		}
}
