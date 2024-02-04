#include "bdutil.h"
int getbdlen(char *bd) {
    char *a = bd;
    int count = 0;
    uint8_t flag;
    for(;;) {
        flag = *(uint8_t*)(a + 1);
        a += 16;
        count += 16;
        if(flag >= 0x7) return count;
    };
}

int getnsamplesfrombdlen(int len) {
    return (len / 16) * 28;
}
