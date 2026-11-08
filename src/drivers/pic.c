/*
 * KFS-4
 *
 * Role: 8259 PIC: remap IRQ 0-15 away from CPU exception vectors 0x00-0x1F, mask/unmask lines, send EOI.
 * Depends on: io.h
 * Unlocks: hardware IRQs (keyboard, timer). Can be written early, only testable once the IDT exists.
 */

#include "drivers/pic.h"
