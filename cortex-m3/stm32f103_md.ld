/* Linker script to configure memory regions. 
 * Need modifying for a specific board. 
 *   FLASH.ORIGIN: starting address of flash
 *   FLASH.LENGTH: length of flash
 *   RAM.ORIGIN: starting address of RAM bank 0
 *   RAM.LENGTH: length of RAM bank 0
 */
SEARCH_DIR("=/usr/arm-none-eabi/lib"); SEARCH_DIR("=/usr/local/lib"); SEARCH_DIR("=/lib"); SEARCH_DIR("=/usr/lib");

_SRAM_SIZE = 0x00005000;

MEMORY
{
  FLASH (rx) : ORIGIN = 0x08000000, LENGTH = 0x00020000 /* 128K */
  RAM   (rwx): ORIGIN = 0x20000000, LENGTH = _SRAM_SIZE /* 20K */
}

__stack = ORIGIN(RAM) + LENGTH(RAM);
__Main_Stack_Size = 1024 ;
_estack = __stack - __Main_Stack_Size; 	/* STM specific definition */
PROVIDE ( _Main_Stack_Size = __Main_Stack_Size ) ;
__Main_Stack_Limit = __stack  - __Main_Stack_Size ;
PROVIDE ( _Main_Stack_Limit = __Main_Stack_Limit ) ;
_Minimum_Stack_Size = 512 ;
PROVIDE ( _Heap_Begin = _end_noinit ) ;
PROVIDE ( _Heap_Limit = __stack - _Main_Stack_Size ) ;
_RAM_Start = ORIGIN(RAM);
_RAM_Len = _estack - _RAM_Start;

ENTRY(Reset)

/* Linker script to place sections and symbol values. Should be used together
 * with other linker script that defines memory regions FLASH and RAM.
 * It references following symbols, which must be defined in code:
 *   ISR::Reset : Entry of reset handler
 * 
 * It defines following symbols, which code can use without definition:
 *   __exidx_start
 *   __exidx_end
 *   __copy_table_start__
 *   __copy_table_end__
 *   __zero_table_start__
 *   __zero_table_end__
 *   __etext
 *   __data_start__
 *   __preinit_array_start
 *   __preinit_array_end
 *   __init_array_start
 *   __init_array_end
 *   __fini_array_start
 *   __fini_array_end
 *   __data_end__
 *   __bss_start__
 *   __bss_end__
 *   __end__
 *   end
 *   __HeapLimit
 *   __StackLimit
 *   __StackTop
 *   __stack
 */

SECTIONS
{
    .text :
    {
        KEEP(*(.isr_vector))
        *(.text*)

        PROVIDE (_init = .);
        *crti.o(.init)
        KEEP(*(.init))
        *crtn.o(.init)
        PROVIDE (_fini = .);
        KEEP(*(.fini))

        /* .ctors */
        *crtbegin.o(.ctors)
        *crtbegin?.o(.ctors)
        *(EXCLUDE_FILE(*crtend?.o *crtend.o) .ctors)
        *(SORT(.ctors.*))
        *(.ctors)

        /* .dtors */
        *crtbegin.o(.dtors)
        *crtbegin?.o(.dtors)
        *(EXCLUDE_FILE(*crtend?.o *crtend.o) .dtors)
        *(SORT(.dtors.*))
        *(.dtors)

        *(.rodata*)
        
        /* GLOBAL OFFSET TABLE*/
        *(.got*)
        
        *(.eh_frame)    
        *(.rodata)                 /* read-only data (constants) */
        *(.rodata*)
        *(.glue_7)
        *(.glue_7t)

        KEEP(*(.eh_frame*))
    } > FLASH

    .ARM.extab :
    {
        *(.ARM.extab* .gnu.linkonce.armextab.*)
    } > FLASH

    __exidx_start = .;
    .ARM.exidx :
    {
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
    } > FLASH
    __exidx_end = .;

    /* To copy multiple ROM to RAM sections,
     * uncomment .copy.table section and,
     * define __STARTUP_COPY_MULTIPLE in startup_ARMCMx.S */

    .copy.table :
    {
        . = ALIGN(4);
        __copy_table_start__ = .;
        LONG (__etext)
        LONG (__data_start__)
        LONG (__data_end__ - __data_start__)
        __copy_table_end__ = .;
    } > FLASH


    /* To clear multiple BSS sections,
     * uncomment .zero.table section and,
     * define __STARTUP_CLEAR_BSS_MULTIPLE in startup_ARMCMx.S */
    .zero.table :
    {
        . = ALIGN(4);
        __zero_table_start__ = .;
        LONG (__bss_start__)
        LONG (__bss_end__ - __bss_start__)
        __zero_table_end__ = .;
    } > FLASH

    __etext = .;

    _sidata = LOADADDR(.data);

    .data : ALIGN(4)
    {
        FILL(0xFF)
        . = ALIGN(4);
        PROVIDE(__textdata__ = LOADADDR(.data));
        __data_start__ = .;
        *(vtable)
        _sdata = .;        /* create a global symbol at data start */
        *(.data*)
        *(.ramtext)

        . = ALIGN(4);
        /* preinit data */
        __preinit_array_start = .;
        KEEP(*(.preinit_array))
        __preinit_array_end = .;

        . = ALIGN(4);
        /* init data */
        __init_array_start = .;
        KEEP(*(SORT(.init_array.*)))
        KEEP(*(.init_array))
        __init_array_end = .;


        . = ALIGN(4);
        /* finit data */
        PROVIDE_HIDDEN (__fini_array_start = .);
        KEEP(*(SORT(.fini_array.*)))
        KEEP(*(.fini_array))
        PROVIDE_HIDDEN (__fini_array_end = .);

        KEEP(*(.jcr*))
        . = ALIGN(4);
        *(.ramfunctions)
        /* All data end */
        _edata = .;        /* define a global symbol at data end */
        __data_end__ = .;

    } > RAM AT > FLASH

    .bss (NOLOAD) : ALIGN(4)
    {
        . = ALIGN(4);
        _sbss = .;         /* define a global symbol at bss start */
        __bss_start__ = _sbss;
        *(.bss*)
        *(.bss.*)
        *(COMMON)
        . = ALIGN(4);
        PROVIDE(_bss_end = .);
        _ebss = .;
        __bss_end__ = _ebss;
    } > RAM

    .heap (COPY):
    {
        __end__ = .;
        PROVIDE(end = .);
        *(.heap*)
        __HeapLimit = .;
    } > RAM

    /* .stack_dummy section doesn't contains any symbols. It is only
     * used for linker to calculate size of stack sections, and assign
     * values to stack symbols later */
    .stack_dummy (COPY):
    {
        *(.stack*)
    } > RAM

    .RamFunc (COPY):
    {
        *(.RamFunc*)
    } > RAM

    /* Set stack top to end of RAM, and stack limit move down by
     * size of stack_dummy section */
    __StackTop = ORIGIN(RAM) + LENGTH(RAM);
    __StackLimit = __StackTop - SIZEOF(.stack_dummy);
    PROVIDE(__stack = __StackTop);

    /* Check if data + heap + stack exceeds RAM limit */
    ASSERT(__StackLimit >= __HeapLimit, "region RAM overflowed with stack")
}
