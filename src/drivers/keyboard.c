/*
 * KFS-1 (bonus) / KFS-4
 *
 * Role: PS/2 keyboard: scancode set 1 decoding (make/break, 0xE0 prefix, modifiers, layout table, input buffer). Keep the decoder independent from how the byte is received (polling on 0x64/0x60 now, IRQ1 handler in KFS-4). Bonus KFS-4: multiple layouts (qwerty/azerty), get_line.
 * Depends on: io.h (port I/O); console to echo
 * Unlocks: shell (KFS-2 bonus), screen switching shortcuts (KFS-1 bonus)
 */

#include "keyboard.h"
