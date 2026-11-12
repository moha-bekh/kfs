# KFS — Kernel From Scratch

An educational i386 kernel project written in C and assembly. It boots using GRUB and Multiboot 2; the VGA text display and console are under development.

## Dependencies

- `make`, a C compiler capable of producing i386 code, `nasm`, and GNU `ld`
- GRUB (`grub-mkrescue`, `i386-pc` modules, `grub-file`), `xorriso`, and `mtools`
- QEMU x86 with a GTK interface; GDB for debugging

On Arch Linux, the packages used are `make`, `gcc`, `binutils`, `nasm`, `grub`, `xorriso`, `mtools`, `qemu-system-x86`, `qemu-ui-gtk`, and `gdb`.

## Commands

```sh
make build            # compile and link build/kfs.elf
make                  # build the image and run QEMU
make run              # build the image if needed and run QEMU
make check-multiboot  # check the existing ELF
make gdb-server       # run QEMU paused for a GDB connection
make monitor          # open the QEMU monitor of a running instance (telnet :4444)
make clean            # remove build/
make fclean           # also remove kfs.iso
```

Without a graphical session (e.g. a VM), pick another QEMU display:
`make run QEMU_DISPLAY=curses` (VGA text in the terminal). Quit from another
terminal with `make monitor` then `quit`. See `docs/MAKEFILE_EDIT.md`.

For C source debugging, use two terminals from the project directory. With
OrbStack, run these commands from macOS:

```sh
orb -m arch-kfs make debug-run  # terminal 1: rebuild with -g -O0, start paused
orb -m arch-kfs make debug-gdb  # terminal 2: connect and stop in kmain
```

Inside Linux, use `make debug-run` and `make debug-gdb` directly. Debug files
are stored in `build-debug/`, including a separate ISO. `make debug-build`
only builds that image; it forces recompilation to ensure debug symbols exist.
Stop QEMU before rebuilding. In GDB, use `list` and `next`. The current
`kmain` runs `test_output_all`; after the call returns, inspect `print res`
or `print res.all_passed`. The suite contains 21 scenarios for the console
and printk. `make gdb-server` remains available for the regular build,
which does not enable C debug information by default.

To inspect each scenario separately, set `break output_test_case` before
running the suite. At each stop, `print name` identifies the scenario and
`finish` executes it and reports its result. Use `continue` for the next case.
Use Ctrl+C if execution does not return; `bt` shows the current call stack.

## Layout

Active output scenarios live in `src/tests/output.c`, with declarations in
`include/tests/output.h`. Each case initializes the private console instance.
Tests check rendered VGA cells, formatting and return counts. Cursor appearance
still requires observation in QEMU. The earlier console scenarios in
`src/tests/console.c` are retained as commented reference for the old API.

One directory per subsystem in `src/`, mirrored in `include/` (`src/mm/paging.c`
↔ `include/mm/paging.h`, included as `"mm/paging.h"`). See `docs/ARCHITECTURE.md`.
Every planned module of KFS-1 to KFS-5 exists as a skeleton already listed in the
Makefile; each file's header comment states its KFS, role and dependencies.

```
src/
  boot/      boot.s (entry, Multiboot 2 header), multiboot.c     KFS-1, 3
  kernel/    main.c (kmain), panic.c, signal.c, syscall.c        KFS-1, 3, 4
  console/   console.c (screens, scrollback)                     KFS-1
  lib/       string.c, printk.c                                  KFS-1
  drivers/   vga.c, keyboard.c, pic.c, pit.c                     KFS-1, 4
  cpu/       gdt.c, gdt_flush.s, idt.c, isr.s, exceptions.c      KFS-2, 4
  debug/     stack.c, memdump.c                                  KFS-2, 3
  shell/     shell.c                                             KFS-2 bonus
  mm/        pmm.c, paging.c, kmalloc.c, vmalloc.c               KFS-3
  proc/      process.c, proc_signal.c, socket.c, sched.c,
             exec.c, switch.s                                    KFS-5
include/     <subsystem>/<module>.h mirroring src/; at the root only the
             cross-cutting types.h and io.h (port I/O)
link/        linker.ld
grub/        grub.cfg
```

- `TODO.md`: objectives per KFS (extracted from the subjects)
- `docs/ARCHITECTURE.md`: tree layout, conventions and rationale
- `docs/ROADMAP.md`: module dependency graph and what can be started now
- `docs/PROGRESS.md`, `docs/LEARNING.md`: progress and learning guidelines
- `docs/MAKEFILE_EDIT.md`: history of Makefile changes
- `build/`: generated intermediate files; `kfs.iso`: generated QEMU image
