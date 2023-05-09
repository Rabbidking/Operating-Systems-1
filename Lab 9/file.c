#include "file.h"
#include "disk.h"
#include "util.h"
#include "console.h"
#include "errno.h"
#include "DirEntry.h"
#include "vbr.h"
#include "kprintf.h"

#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2

extern struct VBR vbr;
static char clusterBuffer[65536];	//DO NOT MAKE THIS A LOCAL!
struct DirEntry* dir;
//readCluster(vbr.rootCluster, clusterBuffer);
//struct file
//struct DirEntry * ents = (struct DirEntry*) clusterBuffer;
/*de[0];
de[1];
de[2];
de[3];*/

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
			for (int b = 0; b < 8; b++)
			{
				capitalization(fname[b]);
				if(ents[a].base[b] == 32)
					break;
				if(capitalization(fname[b]) != ents[a].base[b])
				{
					isGood = 0;
				}
				offset++;
			}
			
			offset++;
			
			for (int c = 0; c < 3; c++)
			{
				capitalization(fname[c]);
				if(ents[a].ext[c] == 32)
					break;
				if(capitalization(fname[c + offset]) != ents[a].ext[c])
					isGood = 0;
			}
			
			if(isGood == 1)
			{			
				for(int i = 0; i < MAX_FILES; i++)
				{	
					if (fileTable[i].in_use == 0)
					{
						fd = i;
						fileTable[fd].in_use = 1;
						fileTable[fd].pos = 0;
						fileTable[fd].size = ents[a].size;
						fileTable[fd].flags = ents[a].attributes;
						fileTable[fd].firstCluster = (ents[a].clusterHigh << 16) + ents[a].clusterLow;
						return fd;
					}
					
				}
			}
			isGood = 1;
			offset = 0;
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

int file_read(int fd, void* buf, unsigned count)
{
	static char someBuffer[4096];
	int final = 0;
	
	if(fd < 0 || fd > MAX_FILES)
		return -EINVAL;
	
	struct File* fp = &fileTable[fd];
	
	if(fp->in_use != 0)
	{
		if(count <= 0)
			return 0;
		
		if(fp->pos >= fp->size)
			return 0;
		
		readCluster(fp->firstCluster, someBuffer);
		
		unsigned offsetInBuffer = fp->pos % 4096;
		unsigned b_size = 4096 - offsetInBuffer;
		final = count;
		unsigned remaining = fp->size - fp->pos;
		
		if(final > b_size)
			final = b_size;
		
		if(final > remaining)
			final = remaining;
		
		kmemcpy(buf, someBuffer + offsetInBuffer, final);
		
		fp->pos += final;
		return final;
	}
	return -EIO;
}

int file_write(int fd, const void* buf, unsigned count)
{
	return -ENOSYS; //no such system call
}

int file_seek(int fd, int offset, int whence)
{
	struct File* fp = &fileTable[fd];
	if(whence == SEEK_SET)
	{
		if(offset < 0)
			return -EINVAL;
		else
			fp->pos = offset;
	}
	
	else if(whence == SEEK_CUR)
	{
		if(fp->pos + offset < 0)
			return -EINVAL;
		else
			fp->pos += offset;
	}
	
	else if(whence == SEEK_END)
	{
		if(offset + fp->size < 0)
			return -EINVAL;
		else
			fp->pos = fp->size + offset;
	}
	
	else
		return -EINVAL;
	
	return fp->pos;
}