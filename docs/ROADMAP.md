# KFS — Module dependencies

Current implementation and validation: [PROGRESS.md](PROGRESS.md).
Remaining objectives per KFS: [TODO.md](../TODO.md).
This file describes dependencies, not a second task list.

## Current foundation

Boot, port I/O, VGA, console, helpers and printk are implemented.
The PS/2 polling decoder is implemented for ordinary set 1 keys and selected
extended keys; keyboard integration is still being completed.
The later subsystems exist as skeletons in src/ and include/, listed in Makefile.

## Dependencies

An arrow means the target uses or builds on the source. Later-module edges
show the planned design, not implemented behavior.

```mermaid
graph LR
  io[io.h] --> vga
  vga --> console
  console --> printk
  io --> polling[PS/2 polling]
  polling --> keyboard[Keyboard decoder]
  keyboard --> console
  printk --> stack[Stack inspection]
  printk --> panic
  printk --> memdump
  keyboard --> shell
  stack --> shell
  gdt --> idt
  isr --> idt
  idt --> exceptions
  panic --> exceptions
  io --> pic
  io --> pit
  idt --> irq[IRQ handling]
  pic --> irq
  irq --> keyboard_irq[Keyboard IRQ reception]
  keyboard_irq --> keyboard
  irq --> pit
  idt --> syscall
  multiboot[Multiboot memory map] --> pmm
  pmm --> paging
  paging --> kmalloc
  paging --> vmalloc
  kmalloc --> process
  vmalloc --> process
  pit --> sched
  process --> sched
  switch[Assembly context switch] --> sched
  process --> proc_signal[Process signals]
  ksignal[Kernel signal callbacks] --> proc_signal
  process --> socket
  sched --> exec
```

## Sequence after KFS-1

1. KFS-2: GDT and stack inspection, then the optional shell.
2. KFS-3: preserve Multiboot information, discover physical memory, then paging
   and allocators.
3. KFS-4: IDT, exceptions, PIC/IRQs and timer; reuse the keyboard decoder with
   IRQ reception. Add kernel signals and syscall entry.
4. KFS-5: processes and scheduling on top of memory management, interrupts
   and context switching.

The exact requirements remain those of the original subjects.
