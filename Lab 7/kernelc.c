#include "console.h"
#include "BootInfo.h"
#include "testsuite.c"
#include "disk.h"
#include "DirEntry.h"

extern struct VBR vbr;
struct DirEntry* dir;
static char clusterBuffer[65536];

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
	
	//list root directory here
	kprintf("START%c", '\n');
	readCluster(vbr.root_cluster, &clusterBuffer);
	struct DirEntry * ents = (struct DirEntry*) clusterBuffer;
	
	for(int a = 0; ents[a].base[0] != 0; a++)
	{
		if(ents[a].base[0] == (char)0xe5)
			continue;
		
		if(ents[a].attributes == 15)
			continue;	//handle LFN
		
		else if(ents[a].base[0] == (char)0x00)
			break;	//EOF
		
		for (int i = 0; i < 8; i++)
		{
			if(ents[a].base[i] == ' ')
				break;
			else
				kprintf("%c", ents[a].base[i]);
		}
		
		kprintf("%c", '.');
		
		for(int j = 0; j < 3; j++)
			kprintf("%c", ents[a].ext[j]);
		
		/*kprintf(" ");
		
		for(int k = 0; k < 11; k++)
			kprintf("%c ", ents[a].creationDate[k]);
		
		for(int t = 0; t < 9; t++)
			kprintf("%c ", ents[a].creationTime[t]);*/
		
		kprintf("%c", '\n');
	}
	kprintf("DONE%c", '\n');
	asm("hlt");
}
