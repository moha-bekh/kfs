/*
 * KFS-4
 *
 * Role: IDT: fill the 256 gates (exceptions 0x00-0x13, IRQs after PIC remap, syscall vector), build the IDTR and load it. Common C-level dispatch of interrupts.
 * Depends on: gdt (code selector), isr.s
 * Unlocks: exceptions, IRQs, syscalls
 */

#include "cpu/idt.h"
