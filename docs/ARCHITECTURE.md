# KFS — Architecture

This document describes the code organization. See [PROGRESS.md](PROGRESS.md)
for implemented behavior and [ROADMAP.md](ROADMAP.md) for planned dependencies.

## Source layout

Each subsystem lives in src/ with public headers under the matching include/
path. For example, src/console/console.c includes "console/console.h".
The compiler receives -I include.

| Directory | Responsibility |
| --- | --- |
| boot/ | Multiboot header, assembly entry and future boot-information parsing |
| kernel/ | kmain and future panic, signal and syscall services |
| console/ | Text buffers, screens, cursor state and history navigation |
| drivers/ | VGA backend, keyboard decoder and future PIC/PIT drivers |
| lib/ | String/memory helpers, ASCII helpers, integer conversion and printk |
| tests/ | Console and output scenarios compiled into the kernel |
| cpu/ | Future GDT, IDT, exception dispatch and supporting assembly |
| debug/ | Future stack and memory inspection |
| shell/ | Future interactive shell |
| mm/ | Future physical/virtual memory management and allocation |
| proc/ | Future processes, signals, sockets and scheduling |

Only types.h and io.h are cross-cutting headers at the root of include/.
The linker script is link/linker.ld; the boot menu is grub/grub.cfg.

## Current runtime

```text
GRUB → _start → establish ESP → kmain
                                 ↓
                          initialize console
                                 ↓
                    poll PS/2 status and data
                                 ↓
                          kb_ps2_decode(byte)
                                 ↓
                              printk
                                 ↓
                         console buffer
                                 ↓
                    console_render_screen → VGA
```

- boot.s establishes a 16 KiB stack before calling C. The image is linked at
  1 MiB as ELF32; NASM sources use bits 32 and elf32 output.
- console.c owns a static t_console. Public output, selection and color
  functions use this instance; screen_* helpers operate on explicit t_screen
  pointers. The large screen buffers are not allocated on the boot stack.
- Each screen keeps its own cells, cursor, view, retained-history metadata
  and current color. Selecting a screen does not reset it or render it.
- console_putc and console_putstr update the buffer. printk formats into the
  active screen and calls console_render_screen once at the end. Thus printk
  depends on kernel console state, unlike the pure string/ASCII helpers.
- The VGA backend owns memory/CRTC accesses. The console uses its interface
  without directly addressing VGA memory or I/O ports.
- Keyboard reception is in kmain; decoding is in keyboard.c. The decoder keeps
  its modifier mask and prefix state private and currently emits text through
  printk. IRQ reception can later feed the same byte-decoding entry point.

## C interfaces

- Include each header actually used. Public headers include their own type
  dependencies; avoid a header that imports every subsystem.
- Declare public functions in the matching header and private helpers/data
  static in the implementation. Header-only port helpers are static inline;
  the inline keyword does not guarantee inlining.
- Header guards prevent duplicate inclusion per translation unit.
- kmain has a prototype in kernel/kernel.h even though assembly calls it.
- -Wmissing-prototypes and -Werror catch missing declarations and warnings.
- GCC inline assembly uses Intel syntax because the build passes -masm=intel.
  -mgeneral-regs-only avoids generated SIMD/FPU register use before that state
  is configured. Compiler-provided headers such as stdarg.h do not imply libc.

## Build organization

C_SRC and ASM_SRC explicitly list the files by KFS. Pattern rules mirror
src/ paths under BUILD_DIR and link them with GNU ld -m elf_i386.
A GRUB staging directory is then turned into an ISO with grub-mkrescue.

```make
$(BUILD_DIR)/%.o: src/%.c
	mkdir -p $(dir $@)
	cc $(CFLAGS) -I include -MMD -MP -c $< -o $@
```

- GCC -MMD -MP and NASM -MD/-MP generate header dependency files.
- -include $(DEP) appears after the default target, so generated rules cannot
  replace all as the default. It need not be the last line of the Makefile.
- Changing a header rebuilds the objects that depend on it; that set evolves
  as modules and includes change.
- Different sources in one directory must have distinct stems: gdt.c and
  gdt_flush.s produce different objects, avoiding a gdt.o collision.
- Normal output goes to build/ and kfs.iso. Debug output goes to build-debug/,
  with its own ISO and -g -O0. Debug builds force recompilation.

For adding a module: create its implementation/header, list it in Makefile,
update dependencies if needed, then verify make build. Commands and test
instructions are maintained in [README.md](../README.md).
