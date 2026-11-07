/*
 * KFS-3 / KFS-4
 *
 * Role: Kernel panic: print and stop the kernel (KFS-3). KFS-4 adds: interface to clean registers before a panic/halt, interface to save the stack before a panic.
 * Depends on: printk (+ debug/stack for the stack part)
 * Unlocks: exceptions, every fatal error path
 */

#include "panic.h"
