#pragma once

#define SERIALIO 0x3f8
#define SERIALFLAGS 0x3fd

#include "util.h"
#include "kprintf.h"
#include "serial.h"

void serialOut( char c )
{
	while( 0 == (inb(SERIALFLAGS) & (1<<5)) )
		;
	outb( SERIALIO, c );
}