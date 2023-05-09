
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
	$(CC) -c disk.c
	$(CC) -c file.c
	$(LD) -Map kernelmap.txt -T linkerscript.txt -o kernel.tmp \
		kernelasm.o kernelc.o util.o kprintf.o console.o disk.o file.o
	$(OBJCOPY) -Obinary kernel.tmp kernel.bin
	
	$(FOOL) hd.img create 400
	$(FOOL) hd.img cp kernel.bin KERNEL.BIN
	$(FOOL) hd.img cp article1.txt article1.txt
	$(FOOL) hd.img cp article2.txt article2.txt
	$(FOOL) hd.img cp article3.txt article3.txt
	$(FOOL) hd.img cp article4.txt article4.txt
	$(FOOL) hd.img cp article5.txt article5.txt
	$(FOOL) hd.img cp article6.txt article6.txt
	$(FOOL) hd.img cp article2.txt "my fancy filename.txt"
	$(FOOL) hd.img cp article3.txt "this is a really long filename.c"
	$(FOOL) hd.img cp billofrights.txt BILL.TXT
	
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
