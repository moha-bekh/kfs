# KFS-1: Grub, boot and screen

Progress updated: November 12, 2026. Completed items below describe the
implemented behavior and observed checks; they are not a full subject evaluation.

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
- Bootable kernel
- "42" on the screen.
- Interface kernel / screen: VGA backend and console buffers.
- Basic helpers imported and adapted from libft:
  - Strings and memory: strlen, strcmp, strncmp, memset, memcpy, memmove,
    memcmp, strchr, strrchr, strlcpy, strlcat, memchr.
  - ASCII characters: isalpha, isdigit, isalnum, isspace, isprint, isascii,
    tolower, toupper.
  - Integer conversions: atoi, atol and the project helper atoi_base.
  - Helper checks executed in a temporary 32-bit program without libc.
    Integer conversions require representable input; overflow detection is pending.

#### Remaining checks

- Verify the final kernel size against the 10 MB limit.
- Review the final submission against the original subject and allowed headers.

## Bonus part

### Done and checked

- Port I/O helpers: inb / outb.
- Hardware cursor movement, visibility and synchronization with the rendered view.
- Circular history: 250 lines per screen, with a 25-line view.
- Scroll up/down, history limits, wraparound and preservation during new output.
- Resume following output after scrolling back to the bottom.
- Five independent screens: contents, cursor, history and view preserved.
- Screen selection by index, with invalid indices rejected.
- Full screen-buffer initialization with blank grey-on-black cells.
- VGA character attributes; green and red output checked in the visual scenario.
- 17 console scenarios passed in QEMU/GDB before the latest helper imports.
- Visual cursor/navigation and screen-switching scenarios checked by the learner.
- Console instance is private to console.c; public output functions no longer
  require a console argument. Color selection is stored per screen.
- printk supports literal text, %%, %c, %s, %d, %i, %u, %x, %X and %p,
  using stdarg and returning the emitted character count.
- 21 current output scenarios passed in QEMU/GDB after the console refactoring:
  initialization, colors, screen selection, history navigation, formatting,
  INT_MIN, unsigned limits, pointers, mixed arguments and return count.
- Earlier console test sources are retained as commented reference; the active
  suite is test_output_all in src/tests/output.c, called from kmain.

### Next

- Check a combined printk message visually. Decide and test behavior for
  unsupported formats, a trailing percent sign and null string arguments.
- Implement PS/2 keyboard input in polling mode: controller status/data,
  scancodes, press/release events, modifiers and character conversion.
- Echo keyboard input through the console.
- Connect keyboard shortcuts to history navigation and screen selection.
- Re-run the console suite after integration and validate interactive behavior.

### Debugging support

- Separate debug build with symbols and QEMU/GDB Makefile targets.
- Visual scenarios use console_visual_checkpoint as a GDB breakpoint.
- Current output tests can be stepped through with a breakpoint on
  output_test_case; inspect name and use finish to see each result.
- Compile C code with -mgeneral-regs-only while SIMD state is not configured.

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

- Declare your GDT to the BIOS (kernel with lgdt).

- GDT address 0x00000800

- Print the kernel stack.

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
