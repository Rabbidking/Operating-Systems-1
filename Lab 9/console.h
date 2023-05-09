#pragma once
#include "BootInfo.h"

void console_putc(char ch);
void setPixel(int x, int y, unsigned char c1, unsigned char c2);
void console_init(struct BootInfo* bis);
void drawCharacter(char ch, int x, int y);
void clearScreen();
void RGBToBytes(unsigned color);
