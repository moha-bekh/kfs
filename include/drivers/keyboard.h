#ifndef DRIVERS_KEYBOARD_H
#define DRIVERS_KEYBOARD_H

/* KFS-1 (bonus) / KFS-4 — see src/drivers/keyboard.c */

#include "lib/printk.h"
#include "types.h"

#define PS2_DATA 0x60   // RW
#define PS2_STATUS 0x64 // R
#define PS2_CMD 0x64    // W

#define PS2_KEYMAP_SIZE 128

/* Set 1 prefixes and masks. Check prefixes before interpreting the break bit.
 */
#define PS2_SC_EXTENDED 0xE0
#define PS2_SC_PAUSE_PREFIX 0xE1
#define PS2_SC_RELEASE_MASK 0x80
#define PS2_SC_KEY_MASK 0x7F

/* Simple make codes. Break codes use the same code with bit 7 set. */
#define PS2_SC_L_SHIFT 0x2A
#define PS2_SC_R_SHIFT 0x36
#define PS2_SC_L_CTRL 0x1D
#define PS2_SC_L_ALT 0x38
#define PS2_SC_CAPS_LOCK 0x3A

/* Second byte of E0-prefixed make sequences, not standalone scancodes.
 * Example: right Ctrl press = E0 1D, release = E0 9D.
 */
#define PS2_SC_E0_R_CTRL 0x1D
#define PS2_SC_E0_R_ALT 0x38
#define PS2_SC_E0_L_SUPER 0x5B
#define PS2_SC_E0_R_SUPER 0x5C
#define PS2_SC_E0_KEYPAD_ENTER 0x1C
#define PS2_SC_E0_KEYPAD_SLASH 0x35
#define PS2_SC_E0_HOME 0x47
#define PS2_SC_E0_UP 0x48
#define PS2_SC_E0_PAGE_UP 0x49
#define PS2_SC_E0_LEFT 0x4B
#define PS2_SC_E0_RIGHT 0x4D
#define PS2_SC_E0_END 0x4F
#define PS2_SC_E0_DOWN 0x50
#define PS2_SC_E0_PAGE_DOWN 0x51
#define PS2_SC_E0_INSERT 0x52
#define PS2_SC_E0_DELETE 0x53

/* US QWERTY, set 1 make codes without E0/E1 prefixes.
 * Zero entries do not produce a character. Release events must not emit text.
 * Keypad digits are omitted until Num Lock is handled.
 */

enum e_mod {
  L_SHIFT = 0x001,
  R_SHIFT = 0x002,
  L_CTRL = 0x004,
  R_CTRL = 0x008,
  L_ALT = 0x010,
  R_ALT = 0x020,
  L_SUPER = 0x040,
  R_SUPER = 0x080,
  LOCK_CAPS = 0x100,
  LOCK_NUM = 0x200,
  LOCK_SCROLL = 0x400
};

void kb_ps2_decode(uint8_t scan_code);

#endif /* DRIVERS_KEYBOARD_H */
