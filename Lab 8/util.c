#include "util.h"

unsigned char inb(unsigned short addr)
{
	unsigned tmp;
	asm volatile ( "in al,dx" : "=a"(tmp) : "d"(addr) );
	return tmp;
}

void outb(unsigned short addr, unsigned char value)
{
	asm volatile ( "out dx, al" : : "a" (value), "d" (addr) : "memory" );
}

void kmemset(void * x, char v, int count)
{
	char * p = (char*) x;
	while(count--)
		*p++ = v;
}

void kmemcpy(void * d, void * s, int count)
{
	char * dp = (char*) d, *sp = (char*) s;
	for(int i = 0; i < count; i++)
		*dp++ = *sp++;
}