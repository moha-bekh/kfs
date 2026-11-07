/*
 * KFS-5
 *
 * Role: Process structure (PID, status run/zombie/thread, father/children, stack/heap, signal queue, owner uid) and helpers: fork (copy an entire process), wait, exit, getuid. Process memory functions.
 * Depends on: kmalloc/vmalloc, paging
 */

#include "process.h"
