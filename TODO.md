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

# KFS-4: Interrupts

## Mandatory part

- Create an Interrupt Descriptor Table (IDT), fill it and register it.
  - CPU exceptions 0x00-0x13 (division by zero, debug, NMI, breakpoint, overflow,
    bounds, invalid opcode, coprocessor N/A, double fault, coprocessor segment
    overrun, invalid TSS, segment not present, stack fault, GPF, page fault,
    reserved, math fault, alignment check, machine check, SIMD FP).
  - Hardware and software interrupts.
- Signal-callback system on the kernel API.
- Interface to schedule signals.
- Interface to clean registers before a panic / halt.
- Interface to save the stack before a panic.
- Global panic fault handling.
- Then: keyboard handling through the IDT (IRQ).

## Bonus part

- Syscall base functions (handled by the IDT; no processes yet).
- Keyboard: multiple layouts (qwerty, azerty).
- Keyboard: `get_line` (like `read`: waits for characters, returns on `\n`).

# KFS-5: Processes

## Mandatory part

### Data

- Process structure:
  - PID
  - Status (run, zombie, thread)
  - Pointers to father and children
  - Stack and heap of the process
  - Current signals (queue list)
  - Owner id (user)
- Functions:
  - Queue a signal to a process, delivered on the next CPU tick
  - Socket communication helpers between processes
  - Functions to work on the memory of a process
  - Copy an entire process (fork)
- Syscall helpers: wait, exit, getuid, signal, kill.
- Everything must work like any UNIX system.

### Memory

- Use the KFS-3 paging: set a page size, assign one page to each new process.
- Kernel-side: track each process's virtual memory (its pages) and their
  position in the whole memory (needed for future allocation syscalls).

### Execution

- Multitasking (free choice of method; the kernel handles few processes).

### Testing

- No binary execution yet: test from inside the kernel with a function like
  `exec_fn(addr, function, size)` that loads a function as a process and runs it
  alongside the others. Prove the work through it.

## Bonus part

- `mmap`-like functions so a process can get its virtual memory.
- Link the IDT and processes (for future syscall signals).
- BSS and data sections in the process structure.
