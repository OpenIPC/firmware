/* SPDX-License-Identifier: MIT */
OUTPUT_FORMAT("elf32-littlearc")
OUTPUT_ARCH(arc)
ENTRY(arc_vectors)
MEMORY
{
    private (rwx) : ORIGIN = 0xa3f00000, LENGTH = 0xc0000
}
SECTIONS
{
    .vectors ORIGIN(private) : { KEEP(*(.vectors)) } > private
    .text ORIGIN(private) + 0x100 : { *(.text) *(.rodata .rodata.*) } > private
    .resource_table ORIGIN(private) + 0x6000 :
    {
        KEEP(*(.resource_table))
    } > private
    .diagnostics ORIGIN(private) + 0x7000 : { *(.diagnostics) } > private
    .data : { *(.data .data.*) } > private
    .bss (NOLOAD) : { *(.bss .bss.*) *(COMMON) } > private
    /DISCARD/ : { *(.comment) *(.note*) *(.eh_frame*) }
}
