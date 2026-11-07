/*
 * KFS-2
 *
 * Role: GDT at 0x00000800 with kernel code/data/stack and user code/data/stack descriptors; build the GDTR and load it.
 * Depends on: nothing (GRUB's GDT is used until then)
 * Unlocks: IDT (gate selectors point into the GDT), user space (KFS-3/5)
 */

#include "gdt.h"
