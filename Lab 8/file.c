#include "file.h"
#include "disk.h"
#include "util.h"
#include "console.h"
#include "errno.h"
#include "DirEntry.h"
#include "vbr.h"
#include "kprintf.h"

extern struct VBR vbr;
struct DirEntry* dir;
static char clusterBuffer[65536];

char capitalization(char ascii)
{
	char newChar = ascii;
	if (ascii >= 97 && ascii <= 122)
		newChar = ascii - 32;
	return newChar;
}

int file_open(const char * fname, int flags)
{
	//returns descriptor (index to file table entry) or error code (negative values)
	
	int fd;
	int offset;
	int isGood = 1;	//0 is false, 1 is true
	
	readCluster(vbr.root_cluster, &clusterBuffer);
	struct DirEntry * ents = (struct DirEntry*) clusterBuffer;
	
	//kprintf("%s\n", fname);
	for(int a = 0; ents[a].base[0] != 0; a++)
	{
		isGood = 1;
		offset = 0;
		if(ents[a].attributes != 15 && ents[a].base[0] != (char)0xe5)
		{
			//kprintf("Entry name: %s\n", ents[a].base);
			//kprintf("File name: %s\n", fname);
			//kprintf("\n");
			
			for (int b = 0; b < 8; b++)
			{
				capitalization(fname[b]);
				//kprintf("%d\n", capitalization(fname[b]));
				if(ents[a].base[b] == 32)
					break;
				if(capitalization(fname[b]) != ents[a].base[b])
				{
					//kprintf("Are we good? %d\n", isGood);
					isGood = 0;
				}
				offset++;
				/*kprintf("Current entry char: %c\n", ents[a].base[b]);
				kprintf("Current filename char: %c\n", capitalization(fname[b]));
				kprintf("offset = %d\n", offset);
				kprintf("isGood = %d\n", isGood);
				kprintf("\n");*/
				
			}
			
			//kprintf("I AM HERE!\n");
			offset++;
			
			for (int c = 0; c < 3; c++)
			{
				capitalization(fname[c]);
				if(ents[a].ext[c] == 32)
					break;
				if(capitalization(fname[c + offset]) != ents[a].ext[c])
					isGood = 0;
				/*kprintf("Current entry char: %c\n", ents[a].ext[c]);
				kprintf("Current filename char: %c\n", capitalization(fname[c + offset]));
				kprintf("offset = %d\n", c + offset);
				kprintf("isGood = %d\n", isGood);
				kprintf("\n");*/
			}
			
			if(isGood == 1)
			{
				//find vacant entry, set it to fd. If none, return -EMFILE
				
				for(int i = 0; i < MAX_FILES; i++)
				{
					//if(fileTable[i].size == 0)
						//return -EMFILE;
					
					if (fileTable[i].in_use == 0)
					{
						fd = i;
						fileTable[fd].in_use = 1;
						fileTable[fd].pos = 0;
						fileTable[fd].size = ents[fd].size;
						fileTable[fd].cluster = (ents[fd].clusterHigh << 16) + ents[fd].clusterLow;
						return fd;
					}
					
				}
			}
		}
	}
	return -ENOENT;
}

int file_close(int fd)
{
	//return 0 for success, non-zero for error

	if(fd < 0 || fd >= MAX_FILES || fileTable[fd].in_use == 0)
		//MUST check if index is valid first, BEFORE the in_use check
		return EINVAL;

	fileTable[fd].in_use = 0;

	return SUCCESS;
}
