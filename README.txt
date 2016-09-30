TO BUILD AND UPLOAD A PURE C PROGRAM TO THE ATMEGA128RFA1

full post see here:

http://maxembedded.com/2015/06/setting-up-avr-gcc-toolchain-on-linux-and-mac-os-x/
and
http://ladyada.net/learn/avr/avrdude.html

but basically, to make the program using the avr toolchain instead of Atmel Studio, you have to:

create an object file for each .c file:
avr-gcc -g -Os -mmcu=atmega128rfa1 -c leds.c
avr-gcc -g -Os -mmcu=atmega128rfa1 -c serial.c
avr-gcc -g -Os -mmcu=atmega128rfa1 -c button.c
avr-gcc -g -Os -mmcu=atmega128rfa1 -c timer.c
avr-gcc -g -Os -mmcu=atmega128rfa1 -c SerialRelay.c

create a .elf file from all of those object files:
avr-gcc -g -mmcu=atmega128rfa1 -o SerialRelay.elf SerialRelay.o leds.o serial.o button.o radio.o

generate a .hex file from that elf file:
avr-objcopy -j .text -j .data -O ihex SerialRelay.elf SerialRelay.hex

obviously, all of the above can be done with a simple Makefile.

then, write it to the rfa1 using an stk600 and jtagicev3 by:
sudo avrdude -p m128rfa1 -U flash:w:SerialRelay.hex -c jtag3 -v 

then, go to here: http://www.engbedded.com/fusecalc/
and use it to calculate the AVRDUDE arguments you want to use for the fuses.
for example, I chose atmega128rfa1 as the part number
Then changed the clock source to Transceiver Oscillator; Start-up time: 16K CK + 0ms
Turned off "Divide clock by 8 internally"
and turned on "On-Chip Debug Enabled"
the avrdude arguments you then need are displayed on the bottom right of the page.
then, program them in all at once e.g. by
sudo avrdude -p m128rfa1 -U lfuse:w:0xd7:m -U hfuse:w:0x19:m -U efuse:w:0xff:m  -c jtag3 -v 


Still figuring out how to see extra constants (e.g. NODE_ID) via the command line.


TO BUILD AND UPLOAD A CONTIKI PROGRAM TO THE ATMEGA128RFA1

make the project using:
make TARGET=avr-atmega128rfa1

upload using avrdude:
sudo avrdude -p m128rfa1 -U flash:w:SerialRelay.avr-atmega128rfa1 -c jtag3 -v

