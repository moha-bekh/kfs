; KFS-4
;
; Role: Interrupt entry stubs: save state, push vector/error code, call the C dispatcher, restore, iret.
; Used by: idt.c

bits 32
