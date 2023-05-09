#include "kprintf.h"

extern unsigned getUptime();

void sweet(){
    unsigned last=0;
    while(1){
        unsigned uptime = getUptime();
        if( uptime != last ){
            int msec = uptime % 1000;
            uptime /= 1000;
            int sec = uptime % 60;
            int min = uptime / 60;
            kprintf("System uptime: %02d:%02d.%03d\n",min,sec,msec);
            last = uptime;
        }
        asm volatile ("hlt" ::: "memory");
    }
}
