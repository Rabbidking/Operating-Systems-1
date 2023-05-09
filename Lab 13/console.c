#include "console.h"
#include "serial.c"
#include "font.h"
#include "util.h"

static unsigned char* framebuffer;
static unsigned pitch, framebufferSize;
static int row, col;
int cc = 0, cr = 0;
static int color1, color2, color3, color4;

void RGBToBytes(unsigned color)
{
	//foreground color
	unsigned int temp = color >> 4;
	unsigned int I = temp & 8;
	
	unsigned int R = temp & 4;
	
	if(R)
	{
		//R on
		if(I)
			R = 31;
		else
			R = 21;
	}
	else
	{
		//R off
		if(I)
			R = 10;
		else
			R = 0;
	}
	
	unsigned int G = temp & 2;
	
	if(G)
	{
		//G on
		if(I)
			G = 63;
		else
			G = 42;
	}
	else
	{
		//G off
		if(I)
			G = 20;
		else
			G = 0;
	}
	
	unsigned int B = temp & 1;
	if(B)
	{
		//B on
		if(I)
		{
			B = 31;
		}
		else
		{
			B = 21;
		}
	}
	else
	{
		//B off
		if(I)
		{
			B = 10;
		}
		else
		{
			B = 0;
		}
	}
	
	color1 = (R << 3) + (G >> 3);	//should be 0s
	color3 = B + (G << 5);
//===========================================================================================================================================================
	//background color
	I = color & 8;
	
	R = color & 4;
	
	if(R)
	{
		//R on
		if(I)
			//if I is on, different color than if I is off
			R = 31;
		else
			R = 21;
	}
	else
	{
		//R off
		if(I)
			R = 10;
		else
			R = 0;
	}
	
	G = color & 2;
	
	if(G)
	{
		//G on
		if(I)
			G = 63;
		else
			G = 42;
	}
	else
	{
		//G off
		if(I)
			G = 20;
		else
			G = 0;
	}
	
	B = color & 1;
	if(B)
	{
		//B on
		if(I)
		{
			B = 31;
		}
		else
		{
			B = 21;
		}
	}
	else
	{
		//B off
		if(I)
		{
			B = 10;
		}
		else
		{
			B = 0;
		}
	}
	
	color2 = (R << 3) + (G >> 3);
	color4 = B + (G << 5);
	
	//kprintf("%d\n", color2);
	//kprintf("%d\n", color3);
	//kprintf("%d\n", color4);
}

void console_init(struct BootInfo* bis)
{
	// first reduce size from 64 -> 32 bits
	unsigned tmp = (unsigned)(bis->framebufferAddress);
	
	// and then change type to pointer
	framebuffer = (unsigned char*)tmp;
	pitch = bis->framebufferPitch;
	
	//height * width of screen
	framebufferSize = bis->framebufferHeight * pitch;
}

void drawCharacter(char ch, int x, int y)
{
	RGBToBytes(0x17);
	
	for(row = 0; row < CHAR_HEIGHT; row++)
	{
		for(col = 0; col < CHAR_WIDTH; col++)
		{
			unsigned q = MASK_VALUE >> col;
			if(font_data[(unsigned) ch][row] & q)
				setPixel(x+col, y+row, color4, color2);
			else
				setPixel(x+col, y+row, color3, color1);
		}
	}
}

void setPixel(int x, int y, unsigned char c1, unsigned char c2)
{		
	unsigned idx = (y * pitch) + (x * 2);
	framebuffer[idx] = c1;
	framebuffer[idx+1] = c2;
}

void clearScreen()
{
	RGBToBytes(0x17);
	for(int i = 0; i < framebufferSize; i += 2)
	{
		framebuffer[i] = color3;
		framebuffer[i + 1] = color1;
	}
}

void scroll()
{
    if(cr >= 37)
    {
        //save the last row into the framebuffer
        kmemcpy(framebuffer, framebuffer + pitch * CHAR_HEIGHT, 1600 * (600 - CHAR_HEIGHT));

        //clear that row, move it up one, start a new row
		for(int i = 0; i <= 79; i++)
		{
			drawCharacter(' ', i * CHAR_WIDTH, 36 * CHAR_HEIGHT);
		}
        cr = 36, cc = 0;
    }
}

void console_putc(char ch)
{
	serialOut(ch);
	
	switch(ch)
    {
        case '\x7f':
            // move back one character (rubout previous character)
            if(cc == 0)
            {
                if(cr == 0)
                {
                    cc = 0;
                    cr = 0;
                }
                else
                {
                    cr--;
                    cc = 79;
                }
            }
            else
            {
                cc--;
            }
            drawCharacter(' ', cc * CHAR_WIDTH, cr * CHAR_HEIGHT);
            break;

        case '\t':
            // move to next column divisible by 8
			if(cc % 8 == 0)
				cc += 8;
			else
				cc += (8 - (cc % 8));
			
			if(cc > 79)
			{
				cr++;
				cc = 0;
				scroll();
			}
						
            break;

        case '\f':
            // clear screen
			clearScreen();
            cr = 0;
            cc = 0;
            break;

        case '\r':
            // back to the start of the current line
            cc = 0;
            break;

        case '\n':
            // move down to next line, back to left side
            cr++;
            cc = 0;
			scroll();
            break;
			
		case '\e':
			//escape character, change character attributes
			break;

        default:
            drawCharacter(ch, cc * CHAR_WIDTH, cr * CHAR_HEIGHT);
            cc++;
			if(cc > 79)
            {
                cr++;
                cc = 0;
				scroll();
            }
            break;
    }
}


