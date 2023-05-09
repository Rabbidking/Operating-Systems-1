#include "util.h"
#include "kprintf.h"
#include "interrupts.h"

unsigned uptime = 0;

// Interrupt handlers for default/unknown cases.
__attribute__((__interrupt__))
void unknownInterrupt(struct InterruptFrame* fr){
    kprintf("\nFatal exception at eip=%x\n",fr->eip);
    haltForever();
}

__attribute__((__interrupt__))
void unknownInterruptWithCode(struct InterruptFrame* fr, unsigned code){
    kprintf("Fatal exception: Code=%x eip=%x\n",code,fr->eip);
    haltForever();
}

// Creates first entry in GDT (must be all zeroes)
struct GDTEntry gdt[] = {
    { 0,0,0,0,0,0 }, //zeros
    { 0xffff, 0,0,0, 0xcf92, 0}, //data
    { 0xffff, 0,0,0, 0xcf9a, 0} //code
};

// Interrupt handlers
__attribute__((__interrupt__))
void divide_by_zero(struct InterruptFrame* fr)
{
    kprintf("Fatal error: divide by zero!");
    haltForever();
}

__attribute__((__interrupt__))
void debug_trap(struct InterruptFrame* fr)
{
    kprintf("Fatal exception: debug trap");
    haltForever();
}

__attribute__((__interrupt__))
void undefined_opcode(struct InterruptFrame* fr)
{
    kprintf("Fatal exception: undefined opcode");
    haltForever();
}

//Framework for interrupt handlers 0-7
__attribute__((__interrupt__))
void firstPicInterrupt(struct InterruptFrame* fr)
{
	outb( 0x20, 32 );
}

//Framework for interrupt handlers: 8-15
__attribute__((__interrupt__))
void secondPicInterrupt(struct InterruptFrame* fr){
	outb( 0x20, 32 );
	outb( 0xa0, 32 );
}

__attribute__((__interrupt__))
void timerInterrupt(struct InterruptFrame* fr) {
	uptime++;
	outb(0x70, 0xc);
	inb(0x71);
	outb(0x20, 32); 
	outb(0xa0, 32);
}

void haltForever(void){
    while(1){
        asm volatile("hlt" ::: "memory");
    }
}

struct IDTEntry idt[48];
// Utility function to write the IDT
void table(int i, void* func){
    unsigned x = (unsigned)func;
    idt[i].addrLow = x&0xffff;
    idt[i].selector = 2 << 3;
    idt[i].zero = 0;
    idt[i].flags = 0x8e;
    idt[i].addrHigh = x>>16;
}

// Initialize GDT via LGDT
void interrupt_init(){
    struct LGDT lgdt;
    lgdt.size = sizeof(gdt);
    lgdt.addr = &gdt[0];
    asm volatile( "lgdt [eax]" : : "a"(&lgdt) : "memory" );

    /*
    Time to set up the table.
    * For each of the 47 entries of idt:
        * If this is an interrupt that pushes a code:
        table( num , unknownInterruptWithCode )
        * Else:
        table( num, unknownInterrupt);
		
		*calling table before it's initialized = bad
		*do a for loop, initialize table for all 47 entries, THEN define specific cases
		
	*/
	
	for(int i = 0; i < 48; i++)
	{
		if(i == 8 || i == 10 || i == 11 || i == 12 || i == 13 || i == 14 || i == 17)
			table(i, unknownInterruptWithCode);
		else if (i >= 32 && i <= 39)
			table(i, firstPicInterrupt);
		else if (i > 40 && i <= 47)
			table(i, secondPicInterrupt);
		else
			table(i, unknownInterrupt);
		
		
		switch(i){
        case 0:
            table(0, divide_by_zero);
			break;
        case 3:
            table(3, debug_trap);
			break;
        case 6:
            table(6, undefined_opcode);
			break;
		case 40:
			//real-time clock
			table(40, timerInterrupt);
		}
				
	}

	
    // Interrupt initialization
    struct LIDT tmp;
    tmp.size = sizeof(idt);
    tmp.addr = &idt[0];
    asm volatile("lidt [eax]" : : "a"(&tmp) : "memory" );
	
	//================================================================================================
	//LAB 13 STUFF

	///Map PICs
	//PIC #1
	outb(0x20, 0x11);	//tell PIC 1 to do setup
	outb(0x21, 32);		//set base for interrupts
	outb(0x21, 4);		//PIC 1 is primary
	outb(0x21, 1);		//use 8066 conventions
	outb(0x21, 0);		//Enable all 8 IRQ lines (inhibit bits are all 0)

	//PIC #2
	outb(0xa0, 0x11);
	outb(0xa1, 40) ;	//Base for interrupts
	outb(0xa1, 2);		//Secondary PIC
	outb(0xa1, 1);
	outb(0xa1, 0);

	//Communicate with RTC chip
	outb(0x70, 0xa);					//Tell RTC we want to access rate register
	unsigned temp = inb(0x71);			//get old value
	outb(0x70, 0xa);					//access rate register (again)
	outb(0x71, 10 | (temp & 0xf0));		//Write the rate (3…15) selected previously (slide 15)

	//Tell the RTC chip to generate the interrupts
	outb(0x70,11);						//Access register 11 of rtc chip, reset value
	unsigned sv = inb(0x71);			//Read previous value
	outb(0x70,11);						//Select register 11
	outb(0x71, sv | 0x40);				//Enable interrupts
	
	//Interrupt response
	outb(0x20, 0xc);
	inb(0x21);
	
	outb(0xa0, 0xc);
	inb(0xa1);
	
	outb(0x70, 0xc);					//Tell chip we’re reading its status register
	inb(0x71);							//Discard value

	asm volatile("sti" : : : "memory");	//unsets the "interrupt inhibit" bit. Hardware interrupts will be fired continuously from here on out
	
	//================================================================================================
	
}

unsigned getUptime()
{
	//returns the number of milliseconds since the system was booted
	//CPU usage should be close to 0
	//display should count up in (mostly) accurate wall-clock time
	
	//depends on interrupt timer, update every 1/2 second or so (interrupt period)

	//kprintf("%d\n", uptime);
	
	return uptime * 1000 / 64;	//10=64Hz
}