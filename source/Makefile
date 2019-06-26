
DEVICE	= atmega48
CFLAGS  = -DDEBUG_LEVEL=0
F_CPU   = 8000000	# in Hz
COMPILE = avr-gcc -Wall -Os -DF_CPU=$(F_CPU) $(CFLAGS) -mmcu=$(DEVICE)


all: 
	$(COMPILE) main.c BD3460.c BK1080.c IOI2C.c -o main.elf
	avr-objcopy -j .text -j .data -O ihex main.elf main.hex
	avr-objdump -h -S main.elf > main.lss
	avr-size main.hex


