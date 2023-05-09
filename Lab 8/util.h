#pragma once

unsigned char inb(unsigned short port);
void outb(unsigned short port, unsigned char value);
void kmemset(void * x, char v, int count);
void kmemcpy(void * d, void * s, int count);