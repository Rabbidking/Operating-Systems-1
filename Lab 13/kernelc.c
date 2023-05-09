#include "console.h"
#include "BootInfo.h"
#include "interrupts.h"
#include "kprintf.h"

void sweet();

void clearBss(char* start, char* end) {
	//fill BSS with zeroes
	char* p = start;
	while(p != end)
	{
		*p=0;
		++p;
	}
}

void kmain(struct BootInfo* bis){

	clearBss(0, (char*) 16384);
	console_init(bis);
	clearScreen();
	interrupt_init();
	//kprintf("Are we going to interrupt?");
	sweet();
	while(1)
		asm("hlt");
}
