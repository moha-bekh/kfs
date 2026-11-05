# KFS-1: Grub, boot and screen

## General instructions

### EMULATION

- KVM

### LANGUAGE

- ASM
- C

### COMPILER

- nasm
- cc

### COMPILER FLAGS

- -fno-builtin
- -fno-exception
- -fno-stack-protector
- -fno-rtti
- -nostdlib
- -nodefaultlibs

### LINKING

- GNU linker script + LD

## Mandatory part

### Base

- Install GRUB on an virtual image
- Write an ASM boot code that handles multiboot header, and use GRUB to init and
call main function of the kernel itself.
- Write basic kernel code of the choosen language.
- Compile it with correct flags, and link it to make it bootable.
- Once all of those steps above are done, you can write some helpers like kernel types
or basic functions (strlen, strcmp, ...)
- Your work must not exceed 10 MB.
- Code the interface between your kernel and the screen.
- Display "42" on the screen.

### Makefile

...

## Bonus part

- Add scroll and cursor support to your I/O interface.
- Add colors support to your I/O interface.
- Add helpers like printf / printk in order to print information / debug easily.
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

- You must declare your GDT to the BIOS.

- The GDT must be set at address 0x00000800

When this is done, you have to code a tool to print the kernel stack, in a human-friendly
way. (Tip: If you haven’t made a printk yet, now is a good time !)

## Bonus part

Assuming your keyboard work correctly in your Kernel, and you able to catch an entry,
let’s code a Shell !
Not a POSIX Shell, just a minimalistic shell with a few commands, for debugging pur-
poses.
For example, you could implement the print-kernel-stack-thing in this shell, and some
other things like reboot, halt and such.
