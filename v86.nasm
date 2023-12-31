[BITS 16]
[SECTION .v86]
global v86Interrupt
v86Interrupt:
int 0x00
int 0x30
ud2

global v86TransFlag
v86TransFlag:
push cs
pop es
mov ax, 0x13
int 0x10
mov ax,0x1012
xor bx,bx
mov cx,5
mov dx,.c
int 0x10
push 0xa000
pop es
xor di,di
xor ax,ax
.loop:
mov cx, 12800
rep stosb
inc ax
cmp ax,5
jl .loop
int 0x30
ud2
.c: db `\0263>=*.\?\?\?=*.\0263>`

global v86VideoInt
v86VideoInt:
int 0x10
int 0x30
ud2

global v86DiskOp
v86DiskOp:
xor bx, bx ; TODO fix assuming we're in first 64k
mov ds, bx
mov dl, 0x80 ; TODO get this from BIOS or something
mov si, v86disk_addr_packet ; ds:si
int 0x13
jc .err
int 0x30
.err:
ud2
global v86disk_addr_packet
v86disk_addr_packet:
db 0x10, 0x00 ; size, reserved
dw 0x1 ; blocks
dd 0x23000000 ; transfer buffer 0x23000
dq 0x1 ; start block

global v86DiskGetGeometry
v86DiskGetGeometry:
mov ah, 8
mov dl, 0x80
int 0x13
movzx eax, ch
shl eax, 16
mov al, cl
mov ah, dh
int 0x30
ud2

global v86DiskReadCHS
v86DiskReadCHS:
push 0x2000
pop es
int 0x13
int 0x30
ud2

global v86TextMode
v86TextMode:
mov ax, 0x3
int 0x10
int 0x30
ud2

real_hexprint:
xor cx, cx
mov bl, al
shr al, 4
jmp .donibble
.nibble2:
mov al, bl
inc cx
.donibble:
and al, 0x0F
cmp al, 0x0A
jl .noadjust
add al, 'A' - '0' - 10
.noadjust:
add al, '0'
mov ah, 0x1f
stosw
test cx, cx
jz .nibble2
ret
real_printword:
mov dx, ax
mov al, ah
call real_hexprint
mov ax, dx
call real_hexprint
ret

global v86Test
v86Test:
mov ax, (0xB8000 + (80*2)) >> 4
mov es, ax
mov di, 0
mov word es:[di+0], 0x1f00 | 'S'
mov word es:[di+2], 0x1f00 | 'S'
mov word es:[di+4], 0x1f00 | ':'
add di, 6
mov ax, ss
call real_printword
add di, 2
mov word es:[di+0], 0x1f00 | 'S'
mov word es:[di+2], 0x1f00 | 'P'
mov word es:[di+4], 0x1f00 | ':'
add di, 6
mov ax, sp
call real_printword
add di, 2
mov word es:[di+0], 0x1f00 | 'D'
mov word es:[di+2], 0x1f00 | 'S'
mov word es:[di+4], 0x1f00 | ':'
add di, 6
mov ax, ds
call real_printword
add di, 2
mov word es:[di+0], 0x1f00 | 'C'
mov word es:[di+2], 0x1f00 | 'S'
mov word es:[di+4], 0x1f00 | ':'
add di, 6
mov ax, cs
call real_printword
add di, 2
mov ax, cs
mov ds, ax
mov si, .testStr
mov ah, 0x1f
mov cx, .testStr_end - .testStr
.print:
lodsb
stosw
loop .print
int 3 ; wait for key
int 0x30 ; exit
ud2
.testStr: db "PRESS ANY KEY"
.testStr_end:
