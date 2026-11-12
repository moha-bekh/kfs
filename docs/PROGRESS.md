# KFS — Progress

Updated November 12, 2026. Current work: completing KFS-1 keyboard integration.
Remaining objectives are in [TODO.md](../TODO.md).

## Implemented

- i386 kernel in C and NASM, loaded by BIOS GRUB using Multiboot 2.
  The entry point sets up a 16 KiB stack and calls kmain.
- VGA text output, colors, hardware cursor movement and visibility.
- A private console instance with five independent screens, each containing
  250 history lines of 80 cells and a 25-line view. History wraps, preserves
  the viewed lines while output continues, and follows output again at the bottom.
- String, memory, ASCII classification and integer-conversion helpers.
- printk: %%, %c, %s, %d, %i, %u, %x, %X and %p; character count returned.
  It writes to the active console and renders once at the end of each call.
- PS/2 polling in kmain, with byte decoding in kb_ps2_decode.
  Set 1 US QWERTY and Shift maps; text emitted on press only.
- A modifier mask tracks left/right Shift, Ctrl, Alt and Super.
  E0 sequences are separated from ordinary keys; keypad Enter and slash emit text.
  Complete Pause and Print Screen sequences are ignored without modifier changes.
- Normal and debug builds, separate debug ISO, source-level GDB support.

## Verified

- Learner observed boot, VGA output, cursor visibility, history navigation,
  colors and screen switching in QEMU.
- The 21 output/printk scenarios passed in QEMU/GDB before keyboard integration.
- After adapting the console tests to the private API, all 38 scenarios
  (21 output + 17 console) passed in a temporary 32-bit harness with simulated VGA.
- String/character/integer helpers passed temporary 32-bit checks without libc.
- Learner observed raw keyboard press/release bytes, lowercase, Shift uppercase,
  shifted punctuation and return to lowercase in QEMU.
- The extended decoder passed 12 temporary 32-bit checks with captured printk:
  modifiers, simultaneous Shifts, independent left/right Ctrl, keypad keys,
  ignored navigation, prefix consumption, Pause and Print Screen.

## Current limits

- The latest extended-key behavior has not yet been validated interactively.
  Keyboard checks are not yet stored as a persistent suite.
- Caps Lock, Num Lock and Scroll Lock are declared but not implemented.
  Backspace and Tab are mapped but have no console editing behavior.
- Navigation keys have no action, and screen switching has no keyboard shortcut.
  The private live console has no public scroll API yet.
- PS/2 input assumes the current QEMU setup and set 1 stream; controller setup,
  response/error handling, auxiliary-data filtering and malformed-stream recovery
  are not complete. Pause handling assumes all six bytes arrive.
- printk has no defined policy for unsupported formats, trailing % or null %s.
  Integer helpers require values representable in their return types.
- KFS-2 through KFS-5 modules remain skeletons. Final subject compliance is not
  established; the original subject PDFs are not present in the repository.
