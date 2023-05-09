#include "console.h"
#include "BootInfo.h"
#include "testsuite.c"
#include "disk.h"
#include "DirEntry.h"

extern struct VBR vbr;
struct DirEntry* dir;
//static char clusterBuffer[65536];

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

	disk_init();
	console_init(bis);
	clearScreen();
	sweet();
	asm("hlt");
}
