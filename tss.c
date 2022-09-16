#include <stdint.h>

struct __attribute__((__packed__)) tss_entry_struct {
    uint32_t prev_tss;
    uint32_t esp0;
    uint32_t ss0;
    uint32_t esp1;
    uint32_t ss1;
    uint32_t esp2;
    uint32_t ss2;
    uint32_t cr3;
    uint32_t eip;
    uint32_t eflags;
    uint32_t eax;
    uint32_t ecx;
    uint32_t edx;
    uint32_t ebx;
    uint32_t esp;
    uint32_t ebp;
    uint32_t esi;
    uint32_t edi;
    uint32_t es;
    uint32_t cs;
    uint32_t ss;
    uint32_t ds;
    uint32_t fs;
    uint32_t gs;
    uint32_t ldt;
    uint32_t trap;
    uint32_t iomap_base;
};
struct tss_entry_struct *tss_data;
void write_tss() {
    tss_data = (struct tss_entry_struct *)0x20000;
    for (int i = 0; i < 0x2080; i++)
        ((uint8_t*)tss_data)[i] = 0;
    tss_data->ss0 = 0x10;
    tss_data->esp0 = 0x400000;
    tss_data->iomap_base = 0x80;
}
extern void flushTSS();

void setup_tss() {
    write_tss();
    flushTSS();
}