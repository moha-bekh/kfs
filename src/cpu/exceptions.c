/*
 * KFS-4
 *
 * Role: Handlers for CPU exceptions 0x00-0x13 (division by zero ... SIMD FP), routed to the global panic fault handling.
 * Depends on: idt, panic
 * Unlocks: global panic fault handling
 */

#include "exceptions.h"
