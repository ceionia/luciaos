[BITS 16]
[SECTION .v86]
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
mov ax, 0xb814
mov es, ax
mov di, 20
mov ax, sp
call real_printword
add di, 2
mov ax, ds
call real_printword
add di, 2
mov ax, cs
call real_printword
int 3
int 0x30 ; exit
jmp $

global v86GfxMode
v86GfxMode:
mov ax, 0x13
int 0x10
int 0x30
jmp $

global v86TextMode
v86TextMode:
mov ax, 0x3
int 0x10
int 0x30
jmp $

global v86DiskRead
v86DiskRead:
xor ax, ax ; TODO fix assuming we're in first 64k
mov ds, ax
mov ah, 0x42
mov dl, 0x80 ; TODO get this from BIOS or something
mov si, v86disk_addr_packet ; ds:si
int 0x13
int 0x30
jmp $
global v86disk_addr_packet
v86disk_addr_packet:
db 0x10, 0x00 ; size, reserved
dw 0x1 ; blocks
dd 0x23000000 ; transfer buffer 0x23000
dq 0x1 ; start block
