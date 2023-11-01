ORG 0x7c00
BITS 16

CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start

_start:
  jmp short start
  nop

  times 33 db 0

start:
  jmp 0:step2

step2:
  cli ; clear interrupts
  mov ax, 0x00
  mov ds, ax
  mov es, ax
  mov ss, ax
  mov sp, 0x7c00
  sti ; Enables interrupts

.load_protected:
  cli
  lgdt[gdt_descriptor]
  mov eax,cr0
  or eax,0x1
  mov cr0,eax
  jmp CODE_SEG:load32

; GDT
gdt_start:
gdt_null:
  dd 0x0
  dd 0x0

gdt_code: ; cs hsould point to thid
  dw 0xffff; segment limiy first 015bits
  dw 0
  db 0
  db 0x9a
  db 11001111b
  db 0

gdt_data:
  dw 0xffff; segment limiy first 015bits
  dw 0
  db 0
  db 0x92
  db 11001111b
  db 0

gdt_end:

gdt_descriptor:
  dw gdt_end - gdt_start-1
  dd gdt_start

[BITS 32]
load32:
  mov eax, 1
  mov ecx, 100
  mov edi, 0x0100000
  call ata_lib_read
  jmp CODE_SEG: 0x0100000

ata_lib_read:
  mov ebx, eax,
  shr eax, 24
  or eax, 0xe0
  mov dx, 0x1f6
  out dx, al
  ; finisges sending highest 8bit
  mov eax, ecx
  mov dx, 0x1f2
  out dx, al

  ;send more bits to the lba
  mov eax, ebx
  mov dx, 0x1f3
  out dx, al
  ;finshed sending

  ;send more bits to the lba
  mov dx, 0x1f4
  mov eax, ebx
  shr eax, 8
  out dx, al
  ;finshed sending

  ;sending upper 16 bit
  mov dx, 0x1f5
  mov eax, ebx
  shr eax, 16
  out dx, al
  ;finshed sending
  
  mov dx, 0x1f7
  mov al, 0x20
  out dx, al

  ;read all sectors into the memory
.next_sector:
  push ecx

; check if we need to read
.try_again:
  mov dx, 0x1f7
  in al, dx
  test al, 8
  jz .try_again

; we need to read 256 words atr a tiem
  mov ecx, 256
  mov dx, 0x1f0
  rep insw
  pop ecx
  loop .next_sector
  ; end of reading sector
  ret

times 510-($ - $$) db 0
dw 0xAA55
