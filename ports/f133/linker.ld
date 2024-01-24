/*
 * linker.ld
 *
 * Copyright(c) 2007-2022 Jianjun Jiang <8192542@qq.com>
 * Copyright(c) 2024 Victor Suarez Rovere <suarezvictor@gmail.com>
 * Official site: http://xboot.org
 * Mobile phone: +86-18665388956
 * QQ: 8192542
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

OUTPUT_FORMAT("elf64-littleriscv", "elf64-littleriscv", "elf64-littleriscv")
OUTPUT_ARCH(riscv)
ENTRY(_start)

STACK_SIZE = 0x100000;

MEMORY
{
	ram  : org = 0x40000000, len = 48M
}

SECTIONS
{
	.text :
	{
		PROVIDE(__image_start = .);
		PROVIDE(__text_start = .);
		PROVIDE(__spl_start = .);
		build/crt0.o (.text*)
		. = ALIGN(8);
		PROVIDE(__spl_end = .);
		*(.text*)
		*(.init.text)
		*(.exit.text)
		PROVIDE(__text_end = .);
	} > ram
	PROVIDE(__spl_size = __spl_end - __spl_start);

	.initcall ALIGN(8) :
	{
		PROVIDE(__init_array_start = .);
		PROVIDE(__initcall_start = .);
		KEEP(*(.initcall_0.text))
		KEEP(*(.initcall_1.text))
		KEEP(*(.initcall_2.text))
		KEEP(*(.initcall_3.text))
		KEEP(*(.initcall_4.text))
		KEEP(*(.initcall_5.text))
		KEEP(*(.initcall_6.text))
		KEEP(*(.initcall_7.text))
		KEEP(*(.initcall_8.text))
		KEEP(*(.initcall_9.text))
		PROVIDE(__init_array_end = .);
		PROVIDE(__initcall_end = .);
	} > ram

	.exitcall ALIGN(8) :
	{
		PROVIDE(__exitcall_start = .);
		KEEP(*(.exitcall_9.text))
		KEEP(*(.exitcall_8.text))
		KEEP(*(.exitcall_7.text))
		KEEP(*(.exitcall_6.text))
		KEEP(*(.exitcall_5.text))
		KEEP(*(.exitcall_4.text))
		KEEP(*(.exitcall_3.text))
		KEEP(*(.exitcall_2.text))
		KEEP(*(.exitcall_1.text))
		KEEP(*(.exitcall_0.text))
		PROVIDE(__exitcall_end = .);
	} > ram

	.ksymtab ALIGN(16) :
	{
		PROVIDE(__ksymtab_start = .);
		KEEP(*(.ksymtab.text))
		PROVIDE(__ksymtab_end = .);
	} > ram

	.rodata ALIGN(8) :
	{
		PROVIDE(__rodata_start = .);
		*(.rodata*)
		*(.srodata*)
		PROVIDE(__rodata_end = .);
	} > ram

	.data ALIGN(8) :
	{
		PROVIDE(__data_start = .);
		PROVIDE(__global_pointer$ = . + 0x800);
		*(.sdata*)
		*(.data*)
		. = ALIGN(8);
  		PROVIDE(__data_end = .);
		PROVIDE(__image_end = .);
	} > ram

	.bss ALIGN(8) (NOLOAD) :
	{
		PROVIDE(__bss_start = .);
		*(.bss*)
		*(.sbss*)
		*(COMMON)
		. = ALIGN(8);
		PROVIDE(__bss_end = .);
	} > ram

	.stack ALIGN(16) (NOLOAD) :
	{
		PROVIDE(__stack_start = .);
		. += STACK_SIZE;
		. = ALIGN(16);
		PROVIDE(__stack_end = .);
	} > ram

	/DISCARD/ : { *(.dynsym) }
	/DISCARD/ : { *(.dynstr*) }
	/DISCARD/ : { *(.dynamic*) }
	/DISCARD/ : { *(.interp*) }
}
