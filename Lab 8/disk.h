#pragma once

void waitForControllerReady();
void outw(unsigned short port, unsigned short value);
unsigned short inw(unsigned short port);
int disk_read_sector(unsigned sector_number, void * address);
int disk_write_sector(unsigned sector_number, const void * address);
unsigned clusterNumberToSectorNumber(unsigned clnum);
void readCluster(unsigned clnum, void * p);
void disk_init();