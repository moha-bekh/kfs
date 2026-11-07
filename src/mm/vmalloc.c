/*
 * KFS-3
 *
 * Role: Virtual-side allocation: allocate, free, get size of an allocation.
 * Depends on: paging
 * Unlocks: process memory (KFS-5), mmap bonus
 */

#include "vmalloc.h"
