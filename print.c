#include "print.h"

char nibbleToHex(uint8_t n) {
    return n > 9 ? (n - 10) + 'A' : n + '0';
}
uintptr_t printByte(uint8_t v, uint16_t *buff) {
    *(char *)&buff[0] = nibbleToHex((v >> 4) & 0xF);
    *(char *)&buff[1] = nibbleToHex(v & 0xF);
    return 2;
}
uintptr_t printWord(uint16_t v, uint16_t *buff) {
    printByte(v >> 8, buff);
    printByte(v, &buff[2]);
    return 4;
}
uintptr_t printDword(uint32_t v, uint16_t *buff) {
    printWord(v >> 16, buff);
    printWord(v, &buff[4]);
    return 8;
}

uintptr_t printStr(char *v, uint16_t *buff) {
    char *s;
    for (s = v;*s;s++,buff++)
        *(char*)buff = *s;
    return s - v;
}
