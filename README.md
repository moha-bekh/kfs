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

The tests are compiled into the kernel but are not run by the current polling
loop. Temporarily place these calls in kmain before that loop:

```c
#include "tests/output.h"
#include "tests/console.h"

/* Inside kmain, before keyboard polling. */
t_output_test_results output = test_output_all();
t_console_test_results screens = test_console_all();
(void)output;
(void)screens;
```

After both calls return, inspect `p output` and `p screens` in GDB.
Each suite has an all_passed field: 21 output/printk cases and 17 console cases.
Output cases inspect rendered VGA cells; history cases exercise screen_*
functions on a static fixture. The live console instance stays private.

For individual cases, use `break output_test_case` or `break console_test_case`.
At each stop, `p name` identifies the case and `finish` reports its result;
`continue` reaches the next one.

For visual screen switching, call `test_console_screens_visual()` and set
`break console_visual_checkpoint`. At each stop the screen has already rendered;
inspect `stage`, then continue. Visual history navigation awaits a public scroll
interface for the live console. Keyboard decoder checks so far were temporary
harnesses, not a persistent repository test suite.

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
