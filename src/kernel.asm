[BITS 32]
global _start
extern kernel_main

CODE_SEG equ 0x08 
DATA_SEG equ 0x10 

_start:
mov ax, DATA_SEG
mov ds, ax
mov es, ax
mov fs, ax
mov gs, ax
mov ss, ax
mov ebp, 0x00200000
mov esp, ebp

; enable a20 line
in al, 0x92
or al, 2
out 0x92, al

; Remap the master PIC
mov al, 00010001b
out 0x20, al; tell the master PIC

mov al, 0x20 ;interrup 0x20 is the place master ISR is starting
out 0x21, al

mov al, 00000001b
out 0x21, al
;End master pic

call kernel_main

jmp $

times 512-($ - $$) db 0
