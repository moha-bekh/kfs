/*
 * KFS-4 / KFS-5
 *
 * Role: 8253/8254 PIT: periodic timer on IRQ0 (the 'CPU tick').
 * Depends on: io.h, pic, idt
 * Unlocks: signal scheduling (KFS-4), signal delivery 'on next tick' and preemptive multitasking (KFS-5)
 */

#include "drivers/pit.h"
