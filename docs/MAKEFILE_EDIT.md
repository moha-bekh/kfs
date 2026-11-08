# Makefile edit — portable GRUB tool detection

## Problem

On Fedora, `make` failed with:

```
make: grub-mkrescue: No such file or directory
```

GRUB tools have different names depending on the distribution:

| Distro          | ISO tool          | Multiboot check | Package(s)                        |
|-----------------|-------------------|-----------------|-----------------------------------|
| Debian / Ubuntu | `grub-mkrescue`   | `grub-file`     | `grub-pc-bin`, `xorriso`, `mtools` |
| Fedora          | `grub2-mkrescue`  | `grub2-file`    | `grub2-tools`, `grub2-pc-modules`, `xorriso`, `mtools` |

The Makefile hardcoded the Debian names, so it broke on Fedora. Hardcoding
the Fedora names would have broken it on Ubuntu (e.g. school machines).

## What changed

1. **Two new variables** at the top of the Makefile:

   ```make
   GRUB_MKRESCUE ?= $(shell command -v grub-mkrescue 2>/dev/null || command -v grub2-mkrescue 2>/dev/null)
   GRUB_FILE     ?= $(shell command -v grub-file     2>/dev/null || command -v grub2-file     2>/dev/null)
   ```

   - `command -v X` prints the full path of `X` if it exists, else fails.
   - `||` falls back to the `grub2-` name.
   - `?=` lets you override from the command line:
     `make GRUB_MKRESCUE=/some/other/grub-mkrescue`.

2. **Recipes use the variables** instead of hardcoded names
   (`$(ISO)` rule and `check-multiboot`).

3. **Clear error if no tool is found**: each recipe starts with a
   `test -n` guard that prints which package to install. The guard sits in
   the recipe (not a global `$(error ...)`) so `make clean` / `make build`
   still work on a machine without GRUB.

## Unchanged

`--directory=/usr/lib/grub/i386-pc` is the same path on Debian/Ubuntu and
Fedora, so it was left as is.

## Verified

On Fedora 44: `GRUB_MKRESCUE` resolves to `/usr/bin/grub2-mkrescue`, the ISO
builds, and `make check-multiboot` passes.

---

# Makefile edit — configurable QEMU display and external monitor

## Problem

1. `run` and `gdb-server` hardcoded `-display gtk`. In a VM without a graphical
   session (e.g. an x86_64 OrbStack VM on an Apple Silicon Mac, where `$DISPLAY`
   is empty) QEMU cannot open a window, so `make run` fails.
2. With `-display curses`, the kernel's VGA text output is drawn in the
   terminal, but QEMU then grabs the keyboard: `Ctrl+C` is forwarded to the
   guest, and the monitor shortcut (`Alt+2`, or `Esc` then `2` typed quickly)
   is easy to miss. Result: no obvious way to quit.

## What changed

1. **New variables** (next to the GRUB ones):

   ```make
   QEMU_DISPLAY ?= gtk
   QEMU_MONITOR_PORT ?= 4444
   QEMU_FLAGS = \
   	-display $(QEMU_DISPLAY) \
   	-cdrom $(ISO) \
   	-monitor telnet:127.0.0.1:$(QEMU_MONITOR_PORT),server,nowait
   ```

   - `QEMU_DISPLAY` keeps `gtk` as default, so nothing changes on a desktop
     machine. `?=` lets you override it: `make run QEMU_DISPLAY=curses`
     (also `none`, `vnc=:0`, ...).
   - `-monitor telnet:...,server,nowait` exposes the QEMU monitor on a local
     TCP port. `server` = QEMU listens, `nowait` = the guest boots without
     waiting for a client to connect. Bound to `127.0.0.1` only, so it is not
     reachable from the network.

2. **`run` and `gdb-server` use `$(QEMU_FLAGS)`** instead of duplicating
   the flags. `gdb-server` only appends `-S -s` (freeze CPU at start, GDB
   stub on `:1234`).

3. **New `monitor` target**: `telnet 127.0.0.1 $(QEMU_MONITOR_PORT)`.

## Usage

```sh
# terminal 1
make run QEMU_DISPLAY=curses
# terminal 2
make monitor
(qemu) xp /4bc 0xb8000     # read VGA text memory
(qemu) info registers      # CPU state
(qemu) sendkey shift-a     # inject a real key press/release (keyboard tests)
(qemu) quit
```

Only one QEMU instance at a time can use a given port; run a second one with
`QEMU_MONITOR_PORT=4445`. If the terminal looks broken after QEMU exits
(curses), type `reset`.

## Verified

On Arch Linux x86_64 (OrbStack VM, no display, QEMU 11.1.3):
`make run QEMU_DISPLAY=none`, then via the monitor port
`xp /4bc 0xb8000` returned `'4' 0x07 '2' 0x07` ("42"), and `quit` stopped
QEMU cleanly.

---

# Makefile edit — `src/kernel/`, mirrored `include/`, header dependencies

## Problem

1. `src/kfs/` was renamed `src/kernel/`, and `include/` was reorganised to
   mirror `src/` (`include/mm/paging.h`); `kstring.c` became `lib/string.c`.
   The source lists had to follow.
2. Objects only depended on their source file. After editing a header, `make`
   did not rebuild the objects including it, so they kept the old struct
   layouts / constants: silent, hard-to-explain bugs.

## What changed

1. **Paths**: `src/kfs/*.c` → `src/kernel/*.c`, `src/lib/kstring.c` →
   `src/lib/string.c`. `-I include` is unchanged: includes are now written
   relative to it (`#include "mm/paging.h"`).

2. **Dependency files**:

   ```make
   DEP = $(OBJ:.o=.d)

   nasm -f elf32 -MD $(@:.o=.d) -MP $< -o $@      # .s rule
   cc $(CFLAGS) -I include -MMD -MP -c $< -o $@   # .c rule

   -include $(DEP)                                # last line
   ```

   - `$(OBJ:.o=.d)` is a substitution reference: same as
     `$(patsubst %.o,%.d,$(OBJ))`. Deriving `DEP` from `OBJ` (rather than
     from `C_SRC`/`ASM_SRC` with two more `patsubst`) keeps one source of
     truth: each object gets exactly one `.d` next to it, for C and assembly.
   - `-MMD` (gcc): write `build/x.d` while compiling, user headers only.
     `-MD` (nasm) takes the `.d` path explicitly: `$(@:.o=.d)`.
   - `-MP`: one empty rule per header, so deleting/renaming a header does not
     break the build with "No rule to make target".
   - `-include`: `-` = do not fail when `.d` files do not exist yet. At the end
     of the Makefile so `all` remains the default goal.

## Verified

On Arch Linux x86_64 (gcc 16.2.1, NASM 3.02): `make fclean && make build`
without warnings; `build/drivers/vga.d` lists `vga.h`, `console.h`,
`types.h`; a second `make build` does nothing; after
`touch include/console/console.h`, only `main.o`, `console.o`, `vga.o` are
rebuilt and the ELF relinked; `make -n` still starts with the ISO steps
(`all`); the kernel boots and shows `42`.

---

# Makefile edit — `-Wmissing-prototypes`

## Problem

A `.c` that forgets to include its own header compiles silently: if the
header says `int f(int)` and the definition is `int f(char *)`, callers and
callee disagree on the arguments and nothing reports it. Same for helpers
that should be `static` but are not.

## What changed

```make
CFLAGS = \
	-m32 \
	-Wall -Wextra -Werror -Wmissing-prototypes \
	...
```

Not part of `-Wall`/`-Wextra` in C. With `-Werror`, any non-`static`
function defined without a previous prototype is an error.

Related source change: `include/kfs.h` (umbrella header) was replaced by
`include/kernel/kernel.h`, which only declares `kmain` (required by this
flag); `src/kernel/main.c` now includes `console/console.h` and
`drivers/vga.h` directly. See `docs/ARCHITECTURE.md`, "Include what you use".

## Verified

Experiment: `int helper(int x) { return x; }` in a file without header
compiles without the flag, and fails with
`error: no previous prototype for 'helper' [-Werror=missing-prototypes]`.
Removing `kmain`'s prototype gives the same error for `kmain`.
The whole project builds with the flag, boots and shows `42`.
