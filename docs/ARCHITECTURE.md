# Source tree — layout, conventions and rationale

This file explains **why** the tree looks the way it does, so every part of it
(including the mechanical parts) can be explained and changed with intent.
For "what depends on what" and "what can I start now", see `docs/ROADMAP.md`.

## 1. Overview

```
kfs/
├── Makefile            build: .c/.s → build/*.o → build/kfs.elf → kfs.iso
├── link/linker.ld      memory layout of the kernel image (loaded at 1 MiB)
├── grub/grub.cfg       GRUB menu entry: multiboot2 /boot/kfs.elf
├── include/            headers, same subdirectories as src/
├── src/                all sources, one directory per subsystem
│   ├── boot/           first code executed + boot information
│   ├── kernel/         kernel core: entry point and kernel-wide services
│   ├── console/        text console logic (screens, scrollback)
│   ├── lib/            freestanding helpers (no libc available)
│   ├── drivers/        hardware devices behind I/O ports or MMIO
│   ├── cpu/            x86 CPU structures: GDT, IDT, exceptions
│   ├── debug/          inspection tools (stack, memory dumps)
│   ├── shell/          interactive shell (KFS-2 bonus)
│   ├── mm/             memory management (KFS-3)
│   └── proc/           processes and multitasking (KFS-5)
├── build/              generated, ignored by git
└── kfs.iso             generated, ignored by git
```

## 2. Conventions

### One directory per subsystem in `src/`

A directory groups files that share **one reason to change**. `mm/` changes
when the memory model changes; `drivers/` when a device is added. The
directory name answers "where would I look for X?" without opening files.

### `include/` mirrors `src/`

```
src/mm/paging.c     ↔   include/mm/paging.h     #include "mm/paging.h"
src/lib/string.c    ↔   include/lib/string.h    #include "lib/string.h"
```

- The Makefile passes `-I include`, so every include is written as a path
  relative to `include/`, whatever the directory of the including file.
- The path tells where a declaration comes from: reading
  `#include "mm/paging.h"` in `sched.c` shows the dependency on `mm/`.
- The path also acts as a namespace: `lib/string.h` and `kernel/signal.h`
  cannot be confused with the libc `<string.h>` / `<signal.h>` (which the
  kernel never uses: no libc in a freestanding kernel). Quotes `"..."` search
  the `-I` directories; `<...>` would mean a system header.
- Only cross-cutting headers stay at the root of `include/`:
  `types.h` (fixed-size types, used everywhere) and `io.h` (port I/O, used by
  several drivers).

### Include what you use (no umbrella header)

Each `.c` includes exactly the headers it uses something from, and each `.h`
includes what it needs to compile on its own (`console/console.h` includes
`drivers/vga.h` for `t_vga`). There is deliberately no header that
includes everything:

- with `-MMD`, an umbrella header included everywhere would make every object
  depend on every header: touching `mm/paging.h` would rebuild all files;
- the include list of a file is its dependency list: `sched.c` including
  `mm/paging.h` shows that `proc/` depends on `mm/`;
- it avoids include-order and circular-include problems.

`kmain` has its own header, `kernel/kernel.h`, holding only its prototype;
`main.c` includes the subsystems it initialises directly.

### `-Wmissing-prototypes`

GCC errors (with `-Werror`) when a non-`static` function is defined without a
previous prototype. It catches two real mistakes:

- a `.c` that does not include its own header: the header's prototype and the
  definition can then disagree (different parameters, return type) without
  any diagnostic, and callers use the wrong ABI;
- a helper meant to be private but not declared `static`.

Consequence: every non-`static` function is declared in its module's header,
and the `.c` includes that header first. `kmain` is the edge case: only
`boot.s` calls it, but it still gets a prototype in `kernel/kernel.h`.

- Include guards are derived from the path: `include/mm/paging.h` →
  `MM_PAGING_H`. Two headers with the same file name in different
  directories then still get distinct guards.

### One `.c` ↔ one `.h`

Each module is a pair `src/<dir>/<name>.c` + `include/<dir>/<name>.h`. The header
is the module's public interface (what other modules may call); anything not
declared there should be `static` in the `.c`. Exception: `io.h` is
header-only, because port I/O helpers are tiny `static inline` functions with
inline assembly that should be inlined at each call site.

### Header skeleton

```c
#ifndef MM_PAGING_H     /* include guard: the file's content is seen once */
#define MM_PAGING_H     /* per translation unit, even if included twice   */

#include "types.h"      /* fixed-size types (uint32_t...), no libc stdint */

#endif /* MM_PAGING_H */
```

### File header comment

Each skeleton file starts with: the KFS it belongs to, its role, what it
depends on, what it unlocks. Keep it up to date as the design changes; it is
the cheapest documentation.

### Assembly files

- NASM syntax, assembled with `nasm -f elf32`, each file starts with
  `bits 32` (32-bit protected mode code).
- Only what C cannot express goes in `.s`:
  - `boot.s`: there is no stack before it sets `esp`, so C cannot run yet.
  - `gdt_flush.s`: after `lgdt`, `CS` can only be reloaded by a far jump.
    The data segment registers must also be reloaded.
  - `isr.s`: the CPU jumps to the handler with its own frame on the stack
    (and an error code only for some vectors). The handler must preserve every
    register and return with `iret`, not `ret`.
  - `switch.s`: a context switch swaps `esp` (and registers) between two
    stacks, which no C function can do from inside itself.

## 3. Directory by directory

| Directory | Files | Why it is separate |
| --- | --- | --- |
| `boot/` | `boot.s`, `multiboot.c` | Everything that depends on *how* the kernel was booted (Multiboot 2 header, info structure from GRUB). Changing bootloader = changing only this. |
| `kernel/` | `main.c` (`kmain`), `panic.c`, `signal.c`, `syscall.c` | Kernel-wide services that do not belong to a single subsystem. |
| `console/` | `console.c` | Text logic: screens, cursor, scrollback. Knows nothing about VGA memory. |
| `lib/` | `string.c`, `printk.c` | Replacements for libc pieces. No hardware, no kernel state; usable from anywhere. |
| `drivers/` | `vga.c`, `keyboard.c`, `pic.c`, `pit.c` | One file per device. Each one talks to hardware (ports `0x60/0x64`, `0x20/0xA0`, `0x40-0x43`, MMIO `0xB8000`). |
| `cpu/` | `gdt.c`, `gdt_flush.s`, `idt.c`, `isr.s`, `exceptions.c` | CPU-defined data structures and mechanisms (segmentation, interrupt dispatch), not devices. |
| `debug/` | `stack.c`, `memdump.c` | Read-only inspection tools, used by `panic` and the shell. |
| `shell/` | `shell.c` | Consumer of keyboard + console: pure "application" on top of the kernel. |
| `mm/` | `pmm.c`, `paging.c`, `kmalloc.c`, `vmalloc.c` | Layered (see below). |
| `proc/` | `process.c`, `proc_signal.c`, `socket.c`, `sched.c`, `exec.c`, `switch.s` | Everything about processes; depends on `mm/` and the timer. |

### Notable splits and why

- **`vga` vs `console`**: the backend (write a cell to `0xB8000`) is separated
  from the logic (where is the cursor, which line is visible). The console can
  be tested or re-targeted (e.g. serial port) without touching VGA code.
- **`keyboard`: decoder vs reception**: decoding scancodes is identical whether
  the byte comes from a polling loop (KFS-1) or from the IRQ1 handler (KFS-4).
  Keep a function that takes one byte and knows nothing about where it came
  from; only the caller changes between KFS-1 and KFS-4.
- **`mm/` in four layers**:

  ```
  vmalloc / kmalloc     allocate, free, size        (what callers use)
        │
     paging             page tables, rights, CR3    (virtual → physical)
        │
       pmm              which physical frames are free
        │
    multiboot           memory map given by GRUB
  ```

  Each layer only uses the one below. A bug is then located by layer.
- **`pic` vs `idt`**: the IDT is a CPU table; the PIC is an external chip
  that turns device lines into interrupt vectors. Different documentation,
  different failure modes.
- **`pit` and `multiboot`**: not named by the subjects, but required by them:
  KFS-5's "delivered on the next CPU tick" and multitasking need a timer;
  KFS-3's physical memory needs the memory map.
- **`kernel/signal.c` vs `proc/proc_signal.c`**: KFS-4 asks for a kernel signal
  callback system with no processes; KFS-5 adds per-process signal queues on
  top of it.

## 4. How the Makefile maps the tree

```make
$(BUILD_DIR)/%.o: src/%.c        # src/mm/paging.c → build/mm/paging.o
	mkdir -p $(dir $@)           # create build/mm/ if missing
	cc $(CFLAGS) -I include -c $< -o $@
```

- The tree under `build/` mirrors `src/`; `$(dir $@)` is the directory part
  of the target, `$<` the first prerequisite (the source).
- `C_SRC` / `ASM_SRC` are explicit lists, grouped per KFS with `+=`. Explicit
  lists (instead of `$(wildcard ...)`) make the build fail loudly if a file is
  missing or misnamed, and show in one place what is part of the kernel.
- An empty `.c` (only an `#include`) is a valid translation unit and produces
  an empty `.o`: this is why the skeleton builds.

### Header dependencies (`-MMD -MP`, `DEP`)

Without extra work, an object depends only on its source file: editing a
header (e.g. a struct layout) would not rebuild the objects that include it,
and they would silently keep the old layout. The compilers therefore write,
next to each object, a `.d` file listing what it really depends on:

```make
DEP = $(OBJ:.o=.d)              # build/mm/paging.o -> build/mm/paging.d
cc   ... -MMD -MP -c $< -o $@                      # C
nasm ... -MD $(@:.o=.d) -MP $< -o $@               # assembly
-include $(DEP)                 # at the end of the Makefile
```

Example generated for `vga.c`:

```make
build/drivers/vga.o: src/drivers/vga.c include/drivers/vga.h \
 include/console/console.h include/types.h
include/drivers/vga.h:
include/console/console.h:
include/types.h:
```

- `-MMD`: write the `.d` while compiling, listing user headers only (not
  system headers; `-MD` would list them too).
- `-MP`: add an empty rule per header (`include/types.h:`). Without it,
  deleting or renaming a header makes `make` fail with "No rule to make
  target" because the old `.d` still references it.
- `-include`: the `-` ignores missing files (first build, after `clean`).
  It is placed at the end so that the first rule read from a `.d` never
  becomes the default goal (`all` stays the default).
- NASM has the same mechanism (`-MD <file>`, `-MP`). No `.s` uses
  `%include` yet, so their `.d` only lists the source today; it starts
  mattering as soon as one does.

Verified: after `touch include/console/console.h`, `make build` recompiles
exactly `main.o`, `console.o`, `vga.o` (the three includers) and relinks.

### Mechanical trap: same stem, same directory

`src/cpu/gdt.c` and `src/cpu/gdt.s` would both produce `build/cpu/gdt.o`
(and `gdt.d`). This is why the assembly file is named `gdt_flush.s`. Keep
`.c` and `.s` names distinct within a directory.

## 5. Adding a module

1. Create `src/<dir>/<name>.c` and `include/<dir>/<name>.h` (guard
   `<DIR>_<NAME>_H` + `types.h`).
2. Add the `.c` (or `.s`) to the right KFS group in the Makefile.
3. Write the header comment: KFS, role, depends on, unlocks.
4. Add the edge(s) to the graph in `docs/ROADMAP.md`.
5. `make build` must still pass.

## 6. Trade-offs and alternatives

| Option | Example | Pros | Cons |
| --- | --- | --- | --- |
| Flat `include/` (previous layout) | `#include "paging.h"` | Simple, short includes | Long flat list, globally unique names needed (`kstring.h`, `ksignal.h`) |
| **Current**: `include/` mirrors `src/` | `#include "mm/paging.h"` | Grouping visible, path namespaces names, close to Linux (`include/linux/...`) | Longer includes; moving a module touches its includers |
| Headers next to sources | `src/mm/paging.h` | Module = one directory | Include paths become relative or need many `-I` |
| `arch/x86/` split (Linux-style) | `arch/x86/cpu/gdt.c` | Separates portable code from x86 code | Useless here: i386 is mandatory, single architecture |

History: headers were first flat with `k` prefixes, then mirrored by
subsystem while they were still empty (cheapest moment); `src/kfs/` was
renamed `src/kernel/` ("kfs" is the project name, not a role).
