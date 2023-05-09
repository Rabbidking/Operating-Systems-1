#pragma once

#define READ_BIT 1		//1 <--- (1 << 0) <--- 00....01
#define WRITE_BIT 2 	//2 <--- (1 << 1) <--- 00....10
#define APPEND_BIT 4 	//4 <--- (1 << 2) <--- 000..100

struct File{
	/*
	size
	location in SSD/HD/HW
	permissions (read-only, write-only, read-write, append)
	file position
	*/
	
	int in_use;
	unsigned size;
	int start;	//Placeholder for now. FIX ME!! (later)
	int flags;	//individual bit positions represent our permissions (....sawr)
	unsigned pos;
	unsigned cluster;
	
};
#define MAX_FILES 16 //real OS's use something like 1000 or so...
struct File fileTable[MAX_FILES];

int file_open(const char * fname, int flags);
int file_close(int fd);