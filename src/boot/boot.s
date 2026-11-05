bits 32

global _start

extern main

section .multiboot align=8

header:
    ; magic number
    dd 0xE85250D6
    ; architecture
    dd 0x00
    ; header_length
    dd .header_end - header
    ; checksum
    dd -(0xE85250D6 + 0x00 + (.header_end - header))
    ; type
    dw 0
    ; flags
    dw 0
    ; total tag size
    dd 8
  .header_end:

section .text

_start:

  mov esp, stack_top

  call main

  .loop:
    hlt
    jmp .loop

section .bss align=16

stack_bot:
  resb 0x4000
stack_top:

; LDFLAGS = -m elf_i386 -T link/linker.ld -nostdlib
