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
