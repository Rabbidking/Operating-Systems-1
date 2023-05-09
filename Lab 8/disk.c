#include "disk.h"
#include "errno.h"
#include "util.h"
#include "vbr.h"
#include "LFNEntry.h"
#include "kprintf.h"

struct VBR vbr;

void disk_init()
{
	disk_read_sector(0, &vbr);
}

void outw(unsigned short port, unsigned short value)
{
	asm volatile("out dx,ax" : : "a"(value), "d"(port) : "memory");
}

unsigned short inw(unsigned short port)
{
	unsigned value;
	asm volatile("in ax,dx" : "=a"(value): "d"(port) );
	return (unsigned short) value;
}

void waitForControllerReady()
{
	//check controller's busy bit
	while(inb(0x1f7) & 0x80)
		;
}

int disk_read_sector(unsigned sector_number, void * address)
{
	//If this function returns 0, success. Otherwise, error.
		
	if(sector_number & 0xf0000000)
	{
		//if this is a nonzero, we have an error
		return EINVAL;
	}
	
	//wait for controller to be ready to receive commands
	waitForControllerReady();
	
	//select device and write top 4 bytes to 0x1f6
	outb(0x1f6, 0xe0 | (sector_number >> 24));
	
	//write 2 to port 0x3f6 (tells disk controller to use PIO)
	outb(0x3f6, 0x2);
	
	//write sector count to 0x1f2
	outb(0x1f2, 0x1);
	
	//write the sector number (If you ask for multiple sectors, you must read all the data from the device before issuing another request)
	//low bits to 0x1f3
	outb(0x1f3, 0xff & sector_number);
	
	//next 8 bits to 0x1f4
	outb(0x1f4, sector_number >> 8);
	
	//next 8 bits to 0x1f5
	outb(0x1f5, sector_number >> 16);
	
	//initiate the READ
	outb(0x1f7, 0x20);
	
	//poll "drive-ready" bit between each sector transferred
	waitForControllerReady();
		
	//wait for data to be ready
	while(1)
	{
		unsigned char c = inb(0x1f7);
		//kprintf("%x\n", c);
		if ((c >> 0) & 1 || (c >> 5) & 1)
		{
			return EIO;
		}
		else if((c >> 3) & 1)
		{
			//data is ready
			//READ data
			int i;
			unsigned short * ptr = (unsigned short*)address;
			for(i=0;i<256;++i)
			{
				unsigned short d = inw(0x1f0);
				*ptr = d; //ptr = pointer to unsigned short
				ptr++;
			}
			return SUCCESS;
		}
	}	
}

int disk_write_sector(unsigned sector_number, const void * address)
{
	//If this function returns 0, success. Otherwise, error.
		
	if(sector_number & 0xf0000000)
	{
		//if this is a nonzero, we have an error
		return EINVAL;
	}
	
	//wait for controller to be ready to receive commands
	waitForControllerReady();
	
	//select device and write top 4 bytes to 0x1f6
	outb(0x1f6, 0xe0 | (sector_number >> 24));
	
	//write 2 to port 0x3f6 (tells disk controller to use PIO)
	outb(0x3f6, 0x2);
	
	//write sector count to 0x1f2
	outb(0x1f2, 0x1);
	
	//write the sector number (If you ask for multiple sectors, you must read all the data from the device before issuing another request)
	//low bits to 0x1f3
	outb(0x1f3, 0xff & sector_number);
	
	//next 8 bits to 0x1f4
	outb(0x1f4, sector_number >> 8);
	
	//next 8 bits to 0x1f5
	outb(0x1f5, sector_number >> 16);
	
	//initiate the WRITE
	outb(0x1f7, 0x30);
	
	//poll "drive-ready" bit between each sector transferred
	waitForControllerReady();
	
	//wait for data to be ready
	while(1)
	{
		unsigned char c = inb(0x1f7);
		if ((c >> 0) & 1 || (c >> 5) & 1)
			return EIO;
		
		else if((c >> 3) & 1)
		{
			//data is ready
			//WRITE data
			int i;
			for(i=0;i<256;++i)
			{
				outw(0x1f0, *(unsigned short*) address);
				address++;
			}
			return SUCCESS;
		}
	}
}

unsigned clusterNumberToSectorNumber(unsigned clnum)
{
	if(clnum < 2)
		return -EINVAL;
	
	//start of data area (clnum * 8 sectors per cluster) + (sector where cluster 2's data starts)
	unsigned dataStart = ((clnum - 2) * vbr.sectors_per_cluster) + (vbr.first_sector + vbr.reserved_sectors + vbr.num_fats * vbr.sectors_per_fat);
	
	return dataStart;
}

void readCluster(unsigned clnum, void * p)
{
	unsigned sector_number = clusterNumberToSectorNumber(clnum);
	for(int i = 0; i < vbr.sectors_per_cluster; i++)
	{
		if(disk_read_sector(sector_number + i, p) != 0)
		{
			kprintf("Error!");
			break;
		}
		p += 512;
	}
}