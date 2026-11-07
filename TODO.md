# KFS-1: Grub, boot and screen

## General instructions

### EMULATION ### LANGUAGE ### COMPILER ### COMPILER FLAGS ### LINKING

- KVM nasm cc
- -fno-builtin -fno-exception -fno-stack-protector -fno-rtti -nostdlib -nodefaultlibs
- GNU linker script + LD

## Mandatory part

### Base

#### Done

- GRUB on image
- ASM boot code: multiboot header, init and call main the kernel
- bootable kernel
- Max size 10 MB.
- "42" on the screen.
- interface kernel <-> screen.

#### WIP

- helpers like kernel types or basic functions (strlen, strcmp, ...)

## Bonus part

- scroll and cursor support to I/O interface.
- colors support to I/O interface.
- helpers like printf / printk.
- Handle keyboard entries and print them.
- Handle different screens, and keyboard shortcuts to switch easily between then.

# KFS-2: GDT & Stack

## Mandatory part

- You must create a Global Descriptor Table.

- Your GDT must contain:
  - Kernel Code
  - Kernel Data
  - Kernel stack
  - User code
  - User data
  - User stack
  - Your work should not exceed 10 MB.

- declare your GDT to the BIOS.

- GDT address 0x00000800

- Tool to print the kernel stack.

## Bonus part

- shell
- print-kernel-stack
- reboot
- halt

# KFS-3: Memory

## Mandatory part

- enable memory paging
- memory structure that handle paging and memory rights
- define Kernel and User space
- implement a function to create / get memory pages
- implement functions to allocate, free and get size of a variable. (virtual and physical memory)
- handle "kernel panics" (Print, stop the kernel)
- Your work should not exceed 10 MB.

## Bonus part

- memory dumping and debug
