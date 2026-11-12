# Makefile notes

This file summarizes the current build choices. Commands are maintained in
[README.md](../README.md); it is not a log of past verification runs.

## Tool selection

- GRUB_MKRESCUE and GRUB_FILE detect grub-mkrescue/grub-file, falling back to
  grub2-mkrescue/grub2-file. Both can be overridden on the make command line.
- GRUB's module directory is currently fixed at /usr/lib/grub/i386-pc.
  Tool-name detection alone does not guarantee compatibility with every distro.
- The C rule invokes cc directly. Setting CC does not currently change that rule;
  CFLAGS is the variable used to adjust compilation options.

## Compilation and dependencies

- C is compiled for 32-bit x86 with -m32, -ffreestanding, -fno-builtin,
  -fno-stack-protector and -fno-pie. Inline assembly uses -masm=intel.
- -mgeneral-regs-only prevents compiler-generated SIMD/FPU register use before
  the kernel configures that state. A struct return previously generated SSE
  instructions and caused an invalid-opcode exception followed by a triple fault.
- -Wall -Wextra -Werror -Wmissing-prototypes enforce warnings and public prototypes.
- NASM emits elf32; GNU ld links with -m elf_i386 and link/linker.ld.
- GCC -MMD -MP and NASM -MD/-MP produce per-object .d files.
  -include $(DEP) is after the default target, before the debug targets.

## QEMU

- QEMU_DISPLAY defaults to curses, with command-line overrides for gtk, none
  or vnc=:0. The original GTK default is no longer used.
- run and gdb-server share QEMU_FLAGS. The monitor listens on
  127.0.0.1:4444; override QEMU_MONITOR_PORT for another port.
- gdb-server adds -S -s: start paused and expose GDB on port 1234.
  A different monitor port alone does not avoid a second GDB-port conflict.
- make monitor connects using telnet. Monitor commands include sendkey and quit.

## Debug builds and cleanup

- debug-build forces a complete rebuild with -g -O0 in build-debug/ and writes
  build-debug/kfs.iso, separate from the regular ELF/ISO.
- debug-run builds that image and starts QEMU paused.
- debug-gdb loads the matching ELF, selects Intel disassembly, connects to
  localhost:1234, breaks at kmain and continues. It does not rebuild.
- Stop QEMU before rebuilding its image; GDB must use the ELF from that image.
- clean removes only BUILD_DIR (build/ by default). fclean also removes ISO
  (kfs.iso by default). Debug/custom directories require explicit cleanup.
