/*
 * KFS-3
 *
 * Role: Parse the Multiboot 2 information structure (memory map tag). Requires boot.s to pass the info pointer (ebx) and magic (eax) to the kernel.
 * Depends on: boot.s
 * Unlocks: physical frame allocator
 */

#include "boot/multiboot.h"
