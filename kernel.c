#include <stdint.h>

#include "print.h"
#include "interrupt.h"

#include "tss.h"

typedef unsigned short word;

char check_apic() {
    uint32_t eax, ebx, ecx, edx;
    asm("cpuid" : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx) : "a"(1));
    return (edx & (1 << 9)) != 0;
}
char check_sse() {
    uint32_t eax, ebx, ecx, edx;
    asm("cpuid" : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx) : "a"(1));
    return (edx & (1 << 25)) != 0;
}

void enable_sse() {
    asm volatile(
        "mov %%cr0, %%eax\n"
        "and $0xFFFB, %%ax\n"
        "or $0x2, %%ax\n"
        "mov %%eax, %%cr0\n"
        "mov %%cr4, %%eax\n"
        "or $0x600, %%ax\n"
        "mov %%eax, %%cr4\n"
        : : : "%eax"
    );
}

void print_flags() {
    uint32_t flags;
    asm volatile("pushfd\npop %%eax":"=a"(flags));
    printDword(flags, 0xB8000 + (160*4) + 50);
}
void print_cr0() {
    uint32_t reg;
    asm volatile("mov %%cr0, %%eax":"=a"(reg));
    printDword(reg, 0xB8000 + (160*5) + 50);
}
void print_cr3() {
    uint32_t reg;
    asm volatile("mov %%cr3, %%eax":"=a"(reg));
    printDword(reg, 0xB8000 + (160*5) + 50 + 8*2 + 2);
}
void print_cr4() {
    uint32_t reg;
    asm volatile("mov %%cr4, %%eax":"=a"(reg));
    printDword(reg, 0xB8000 + (160*5) + 50 + 8*4 + 4);
}

__attribute((__no_caller_saved_registers__))
extern void enter_v86(uint32_t ss, uint32_t esp, uint32_t cs, uint32_t eip);
extern void v86Test();
extern void v86GfxMode();
extern void v86TextMode();
extern void v86DiskRead();
__attribute((__no_caller_saved_registers__))
extern char *jmp_usermode_test();
__attribute((__no_caller_saved_registers__))
extern void kbd_wait();

/*
Real Mode Accessible (First MB)
 00000 -  00400 IVT (1kB)
 00400 -  01000 Unused (3kB)
 01000 -  04000 Paging (12kB)
 04000 -  07C00 Free (15kB)
 07C00 -  08000 Boot (512B)
 08000 -  20000 Kernel Code (96kB)
 20000 -  20080 TSS (128B)
 20080 -  22080 TSS IOMAP (8kB)
 22080 -  22400 Unused (896B)
 22400 -  23000 Free (3kB)
 23000 -  30000 Disk Buffer (52kB)
 30000 -  80000 Free (320kB)
 80000 -  90000 Real Mode Stack (64kB)
 90000 -  A0000 Free (64kB)
 A0000 -  FFFFF BIOS Area (384kB)
Protected Only (1MB+)
100000 - 300000 Free (2mB)
300000 - 310000 Task Stack (64kB)
310000 - 320000 Interrupt Stack (64kB)
320000 - 400000 Kernel Stack (896kB)
400000 - 500000 Usermode Stack (1mB)
*/

void start() {
    word *vga_text = (word *)0xb8000;
    char h[] = "LuciaOS";
    for (int i = 0; i < sizeof(h); i++)
        *(char *)&vga_text[i] = h[i];

    uint32_t o;
    asm("mov %%esp, %%eax" : "=a"(o) : :);
    printDword(o, (uint16_t *)&vga_text[80]);
    asm("mov %%ebp, %%eax" : "=a"(o) : :);
    printDword(o, (uint16_t *)&vga_text[160]);

    //char c[] = "APIC support: ";
    //char apic;
    //printByte(apic = check_apic(), (short*)&vga_text[80*3 + sizeof(c) - 1]);
    //for (int i = 0; i < sizeof(c); i++)
    //    *(char *)&vga_text[i+(80*3)] = c[i];
    //if (!apic) return;

    char sse_str[] = "SSE support: ";
    char sse;
    printByte(sse = check_sse(), (uint16_t*)&vga_text[80*4 + sizeof(sse_str) - 1]);
    for (int i = 0; i < sizeof(sse_str); i++)
        *(char *)&vga_text[i+(80*4)] = sse_str[i];
    if (!sse) return;
    enable_sse();

    // edit
    setup_interrupts();
    setup_tss();
    print_flags();
    print_cr0();
    print_cr3();
    print_cr4();
    FARPTR v86_entry = i386LinearToFp(v86Test);
    enter_v86(0x8000, 0xFF00, FP_SEG(v86_entry), FP_OFF(v86_entry));
    v86_entry = i386LinearToFp(v86GfxMode);
    enter_v86(0x8000, 0xFF00, FP_SEG(v86_entry), FP_OFF(v86_entry));
    char *vga = jmp_usermode_test();
    //asm ("xchgw %bx, %bx");

    for (int i = 0; i < 320; i++) {
        vga[i] = i;
    }
    kbd_wait();
    v86_entry = i386LinearToFp(v86TextMode);
    enter_v86(0x8000, 0xFF00, FP_SEG(v86_entry), FP_OFF(v86_entry));
    v86_entry = i386LinearToFp(v86DiskRead);
    enter_v86(0x8000, 0xFF00, FP_SEG(v86_entry), FP_OFF(v86_entry));
    vga_text = (word *)0xb8000;
    char *bootloader = (char *)0x23000;
    for (int i = 0; i < (80*25)/2; i++) {
        printByte(bootloader[i], &vga_text[i*2]);
    }
}

