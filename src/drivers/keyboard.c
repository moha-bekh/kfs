/*
 * KFS-1 (bonus) / KFS-4
 *
 * Role: PS/2 keyboard: scancode set 1 decoding (make/break, 0xE0 prefix,
 * modifiers, layout table, input buffer). Keep the decoder independent from how
 * the byte is received (polling on 0x64/0x60 now, IRQ1 handler in KFS-4). Bonus
 * KFS-4: multiple layouts (qwerty/azerty), get_line. Depends on: io.h (port
 * I/O); console to echo Unlocks: shell (KFS-2 bonus), screen switching
 * shortcuts (KFS-1 bonus)
 */

#include "drivers/keyboard.h"
#include "types.h"

/* US QWERTY mapping for simple set 1 make codes; not a scancode decoder. */
const char keyboard_qwerty[PS2_KEYMAP_SIZE] = {
    [0x01] = '\x1B', [0x02] = '1', [0x03] = '2',  [0x04] = '3',  [0x05] = '4',
    [0x06] = '5',    [0x07] = '6', [0x08] = '7',  [0x09] = '8',  [0x0A] = '9',
    [0x0B] = '0',    [0x0C] = '-', [0x0D] = '=',  [0x0E] = '\b', [0x0F] = '\t',
    [0x10] = 'q',    [0x11] = 'w', [0x12] = 'e',  [0x13] = 'r',  [0x14] = 't',
    [0x15] = 'y',    [0x16] = 'u', [0x17] = 'i',  [0x18] = 'o',  [0x19] = 'p',
    [0x1A] = '[',    [0x1B] = ']', [0x1C] = '\n', [0x1E] = 'a',  [0x1F] = 's',
    [0x20] = 'd',    [0x21] = 'f', [0x22] = 'g',  [0x23] = 'h',  [0x24] = 'j',
    [0x25] = 'k',    [0x26] = 'l', [0x27] = ';',  [0x28] = '\'', [0x29] = '`',
    [0x2B] = '\\',   [0x2C] = 'z', [0x2D] = 'x',  [0x2E] = 'c',  [0x2F] = 'v',
    [0x30] = 'b',    [0x31] = 'n', [0x32] = 'm',  [0x33] = ',',  [0x34] = '.',
    [0x35] = '/',    [0x37] = '*', [0x39] = ' ',  [0x4A] = '-',  [0x4E] = '+',
};

/* Shift changes letters and punctuation; Caps Lock is handled by the decoder.
 */
const char keyboard_qwerty_shift[PS2_KEYMAP_SIZE] = {
    [0x01] = '\x1B', [0x02] = '!', [0x03] = '@',  [0x04] = '#',  [0x05] = '$',
    [0x06] = '%',    [0x07] = '^', [0x08] = '&',  [0x09] = '*',  [0x0A] = '(',
    [0x0B] = ')',    [0x0C] = '_', [0x0D] = '+',  [0x0E] = '\b', [0x0F] = '\t',
    [0x10] = 'Q',    [0x11] = 'W', [0x12] = 'E',  [0x13] = 'R',  [0x14] = 'T',
    [0x15] = 'Y',    [0x16] = 'U', [0x17] = 'I',  [0x18] = 'O',  [0x19] = 'P',
    [0x1A] = '{',    [0x1B] = '}', [0x1C] = '\n', [0x1E] = 'A',  [0x1F] = 'S',
    [0x20] = 'D',    [0x21] = 'F', [0x22] = 'G',  [0x23] = 'H',  [0x24] = 'J',
    [0x25] = 'K',    [0x26] = 'L', [0x27] = ':',  [0x28] = '"',  [0x29] = '~',
    [0x2B] = '|',    [0x2C] = 'Z', [0x2D] = 'X',  [0x2E] = 'C',  [0x2F] = 'V',
    [0x30] = 'B',    [0x31] = 'N', [0x32] = 'M',  [0x33] = '<',  [0x34] = '>',
    [0x35] = '?',    [0x37] = '*', [0x39] = ' ',  [0x4A] = '-',  [0x4E] = '+',
};

static uint16_t mods;
static bool_t e0_pending;
static uint8_t pause_remaining;

/* Separate maps: the same base code can name different left/right keys. */
static const uint16_t normal_modifiers[PS2_KEYMAP_SIZE] = {
    [PS2_SC_L_SHIFT] = L_SHIFT,
    [PS2_SC_R_SHIFT] = R_SHIFT,
    [PS2_SC_L_CTRL] = L_CTRL,
    [PS2_SC_L_ALT] = L_ALT,
};

static const uint16_t extended_modifiers[PS2_KEYMAP_SIZE] = {
    [PS2_SC_E0_R_CTRL] = R_CTRL,
    [PS2_SC_E0_R_ALT] = R_ALT,
    [PS2_SC_E0_L_SUPER] = L_SUPER,
    [PS2_SC_E0_R_SUPER] = R_SUPER,
};

static bool_t kb_update_modifier(uint8_t code, bool_t extended,
                                 bool_t released) {
  uint16_t mask = extended ? extended_modifiers[code] : normal_modifiers[code];
  if (!mask)
    return FALSE;
  if (released)
    mods &= ~mask;
  else
    mods |= mask;
  return TRUE;
}

static void kb_decode_extended(uint8_t code, bool_t released) {
  if (released)
    return;

  switch (code) {
  case PS2_SC_E0_KEYPAD_ENTER:
    printk("%c", '\n');
    break;
  case PS2_SC_E0_KEYPAD_SLASH:
    printk("%c", '/');
    break;
  case PS2_SC_E0_HOME:
  case PS2_SC_E0_UP:
  case PS2_SC_E0_PAGE_UP:
  case PS2_SC_E0_LEFT:
  case PS2_SC_E0_RIGHT:
  case PS2_SC_E0_END:
  case PS2_SC_E0_DOWN:
  case PS2_SC_E0_PAGE_DOWN:
  case PS2_SC_E0_INSERT:
  case PS2_SC_E0_DELETE:
    /* Navigation actions will be connected to the console later. */
    break;
  default:
    /* Unsupported extended keys must never fall through to QWERTY.
     * This also ignores the fake Shift bytes in Print Screen sequences.
     */
    break;
  }
}

void kb_ps2_decode(uint8_t scan_code) {
  /* Pause has a six-byte set 1 sequence. Consume it without an action for now.
   * This assumes a complete sequence; malformed-stream recovery is not handled.
   */
  if (pause_remaining) {
    pause_remaining--;
    return;
  }
  if (scan_code == PS2_SC_PAUSE_PREFIX) {
    e0_pending = FALSE;
    pause_remaining = 5;
    return;
  }
  if (scan_code == PS2_SC_EXTENDED) {
    e0_pending = TRUE;
    return;
  }

  /* Consume the prefix once, even for unknown keys and release events. */
  bool_t extended = e0_pending;
  e0_pending = FALSE;
  bool_t released = (scan_code & PS2_SC_RELEASE_MASK) != 0;
  uint8_t code = scan_code & PS2_SC_KEY_MASK;

  if (kb_update_modifier(code, extended, released))
    return;
  if (extended) {
    kb_decode_extended(code, released);
    return;
  }
  if (released)
    return;

  char key = (mods & (L_SHIFT | R_SHIFT)) ? keyboard_qwerty_shift[code]
                                         : keyboard_qwerty[code];
  if (key)
    printk("%c", key);
}
