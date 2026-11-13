# KFS — Kernel From Scratch

Educational i386 kernel in C and NASM, booted by BIOS GRUB with Multiboot 2.
KFS-1 console and printk are implemented; keyboard integration is in progress.
The current kmain initializes the console and polls PS/2 input.

## Environment

The development setup uses macOS for editing and the amd64 OrbStack VM
`arch-kfs` for compiling and running QEMU. The project directory is shared:
no project-copy step is needed between edits and builds.

Build dependencies: make, an i386-capable C compiler, NASM, GNU ld, GRUB
(grub-mkrescue or grub2-mkrescue, i386-pc modules, grub-file), xorriso and mtools.
Running/debugging also uses qemu-system-i386, GDB and telnet for the monitor.
The default display is curses; GTK is optional and needs a graphical session.

## Build and run

Run these commands inside Linux, or prefix them with `orb -m arch-kfs` from
the project directory on macOS.

```sh
make build            # compile/link build/kfs.elf
make                  # build the ISO and start QEMU
make run              # update the ISO if needed and start QEMU
make check-multiboot  # check the existing ELF's Multiboot 2 header
make monitor          # connect to the running QEMU monitor on port 4444
make clean            # remove build/
make fclean           # also remove kfs.iso
```

Select a display with `make run QEMU_DISPLAY=curses`, `QEMU_DISPLAY=gtk`
or `QEMU_DISPLAY=vnc=:0`. In a second terminal, `make monitor` then `quit`
stops QEMU. To inject keys that curses or the host terminal does not deliver,
use `sendkey shift-a` or `sendkey ctrl` in that monitor.

Normal clean targets do not remove build-debug/ or other custom build folders.
The current .gitignore ignores build/, *.o and *.iso; custom build directories
can still contain untracked ELF and dependency files. Do not commit generated
artifacts.

## Source debugging

Stop the previous QEMU before rebuilding. From macOS, use two terminals:

```sh
orb -m arch-kfs make debug-run  # rebuild with -g -O0 and boot paused
orb -m arch-kfs make debug-gdb  # connect and stop in kmain
```

Inside Linux, use `make debug-run` and `make debug-gdb` directly.
`make debug-build` only builds; debug output and its ISO live in build-debug/.
`make gdb-server` uses the regular build, which has no C debug information by
default. See [Makefile notes](docs/MAKEFILE_EDIT.md).

In GDB, `n` executes the displayed line without entering its calls, `s` enters
calls, `finish` runs until the current function returns, and `bt` shows frames.
Inspect local results only after initialization and in their owning frame.
Ctrl+C interrupts the guest and restores the prompt. To reboot the same image:

```gdb
monitor system_reset
continue
```

## Tests

All three suites run in kmain before keyboard polling: 17 console cases,
21 output/printk cases and 24 keyboard/console integration cases. The keyboard
suite feeds set 1 bytes directly to the decoder, then checks rendered VGA cells.
It covers modifiers, locks and repeats, keypad, prefixes, screen shortcuts,
history navigation, Backspace and Tab. It resets the decoder and console afterward.

Run all 62 cases automatically in a separate headless QEMU guest:

```sh
orb -m arch-kfs make test
```

This builds a temporary debug ISO, checks Multiboot 2, runs the suites through
GDB and exits unsuccessfully if a suite fails. It leaves the interactive VM and
existing build directories alone. Python 3 is required in addition to the build,
QEMU and GDB tools.

For interactive debugging, use the debug targets above. In kmain, step past the
three suite calls and inspect `console_results`, `output_results` and
`keyboard_results`; every structure has an `all_passed` field.
Use `break keyboard_test_case` (or `console_test_case` / `output_test_case`),
then `p name`, `finish` and `continue` to inspect individual cases.

For visual screen switching, call `test_console_screens_visual()` and set
`break console_visual_checkpoint`. At each stop the screen has already rendered;
inspect `stage`, then continue. Tests inject bytes directly: physical keyboard
transport and terminal key interception still need manual `sendkey` validation.
Scroll Lock currently has no visible effect; its case checks that its events do
not emit text, rather than observing its private state.

## Files

- src/ and include/: matching subsystem implementations and public interfaces.
  lib/ contains string, ctype, stdlib and printk; tests/ contains both suites.
- link/linker.ld: kernel image layout; grub/grub.cfg: boot menu.
- [TODO.md](TODO.md): remaining objectives per KFS.
- [PROGRESS.md](docs/PROGRESS.md): concrete implementation and validation state.
- [ARCHITECTURE.md](docs/ARCHITECTURE.md): structure and current runtime.
- [ROADMAP.md](docs/ROADMAP.md): subsystem dependencies and later KFS sequence.
- [LEARNING.md](docs/LEARNING.md): tutoring approach; [AGENTS.md](AGENTS.md): agent rules.

Later KFS subsystems exist as skeletons listed in Makefile. The original subject
PDFs are not present; TODO.md is a working summary, not the authoritative subject.
