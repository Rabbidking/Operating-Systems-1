
CYGWIN?=C:\cygwin64
CC_EXE?="gcc.exe"
LD_EXE?="ld.exe"
OBJCOPY?="objcopy.exe"
#elf_i386 or i386pe
LDPLATFORM?=i386pe
QEMU?=c:/program files/qemu/qemu-system-i386
FOOL?=./fool.exe
ZIPPER?=./zipper.exe
OPTIMIZE?=-O0
INTERRUPT_TEST?=3


############################################
#Don't change these definitions
CFLAGS:=$(OPTIMIZE) \
	-masm=intel \
	-m32 \
	-Wall -Werror \
	-fno-builtin \
	-mgeneral-regs-only \
	-nostdinc -nostdlib -fleading-underscore \
	-ffreestanding
CC=$(CC_EXE) $(CFLAGS)
LD:=$(LD_EXE) -m$(LDPLATFORM)
############################################

all: build run

build:
	$(CC) -c kernelasm.s
	$(CC) -c kernelc.c
	$(CC) -c util.c
	$(CC) -c kprintf.c
	$(CC) -c console.c
	$(CC) -c interrupts.c
	$(CC) -c -DINTERRUPT_TEST=$(INTERRUPT_TEST) testsuite.c
	$(LD) -Map kernelmap.txt -T linkerscript.txt -o kernel.tmp \
		kernelasm.o kernelc.o util.o kprintf.o console.o interrupts.o testsuite.o
	$(OBJCOPY) -Obinary kernel.tmp kernel.bin
	
	$(FOOL) hd.img create 400
	$(FOOL) hd.img cp kernel.bin KERNEL.BIN
	
run:
	"$(QEMU)" \
		-drive file=hd.img,index=0,media=disk,format=raw  \
		-soundhw pcspk \
		-serial mon:stdio -echr 27

objdump:
	objdump --adjust-vma=0x100000 -D -m i386 -M intel -b binary kernel.bin 

clean: 
	-/bin/rm $(wildcard *.o) $(wildcard *.exe) $(wildcard *.img) kernelmap.txt $(wildcard *.tmp) $(wildcard *.bin)

zip:
	$(ZIPPER) lab.zip \
		$(wildcard *.s) \
		$(wildcard *.c) \
		$(wildcard *.h) \
		$(wildcard *.txt) \
		Makefile
