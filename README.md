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
make clean            # remove build/
make fclean           # also remove kfs.iso
```

For debugging, run `make gdb-server`, then in another terminal:

```sh
gdb build/kfs.elf
```

In GDB: `target remote localhost:1234`, then for example `break _start` and `continue`.

## Layout

- `src/boot/`: assembly entry point and Multiboot 2 header
- `src/kfs/`, `src/console/`, `src/drivers/`: kernel, console, and VGA backend
- `include/`: headers; `link/`: linker script; `grub/`: GRUB configuration
- `TODO.md`: objectives; `docs/`: progress and learning guidelines
- `build/`: generated intermediate files; `kfs.iso`: generated QEMU image
