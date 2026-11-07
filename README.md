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

For debugging, run `make gdb-server`, then in another terminal:

```sh
gdb build/kfs.elf
```

In GDB: `target remote localhost:1234`, then for example `break _start` and `continue`.

## Layout

One directory per module in `src/`, headers flat in `include/` (one per module).
Every planned module of KFS-1 to KFS-5 exists as a skeleton already listed in the
Makefile; each file's header comment states its KFS, role and dependencies.

```
src/
  boot/      boot.s (entry, Multiboot 2 header), multiboot.c     KFS-1, 3
  kfs/       main.c, panic.c, signal.c, syscall.c                KFS-1, 3, 4
  console/   console.c (screens, scrollback)                     KFS-1
  lib/       kstring.c, printk.c                                 KFS-1
  drivers/   vga.c, keyboard.c, pic.c, pit.c                     KFS-1, 4
  cpu/       gdt.c, gdt_flush.s, idt.c, isr.s, exceptions.c      KFS-2, 4
  debug/     stack.c, memdump.c                                  KFS-2, 3
  shell/     shell.c                                             KFS-2 bonus
  mm/        pmm.c, paging.c, kmalloc.c, vmalloc.c               KFS-3
  proc/      process.c, proc_signal.c, socket.c, sched.c,
             exec.c, switch.s                                    KFS-5
include/     module headers, types.h, io.h (port I/O)
link/        linker.ld
grub/        grub.cfg
```

- `TODO.md`: objectives per KFS (extracted from the subjects)
- `docs/ROADMAP.md`: module dependency graph and what can be started now
- `docs/PROGRESS.md`, `docs/LEARNING.md`: progress and learning guidelines
- `docs/MAKEFILE_EDIT.md`: history of Makefile changes
- `build/`: generated intermediate files; `kfs.iso`: generated QEMU image
