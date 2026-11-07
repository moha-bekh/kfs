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
