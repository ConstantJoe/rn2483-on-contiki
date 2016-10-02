/*
 * SerialRelay.c
 *
 * Created: 20/06/2016 11:10:49
 *  Author: sbrown
 */ 

//
// ATMega128RFA1 program to relay data between uart0 and uart1
//
// - LEDs
// - Serial Line (UART)
// - Timers
//
// AVR Project Configuration Options
//  General
//   - device=atmega128rfa1
//   - 
//   - optimization=-O0
// Include Directories
//   - "./"
//
// CPU setup (connect to AVR PRogrammer/Fuses)
//   Speed=8MHz
//    - SUT_CKSEL is set to "Internal RC OScillator"
//    - CKDIV8 not selected
//

//#include <avr/io.h>
#include <stdio.h>
#include "contiki.h"
#include "watchdog.h"

#include "libs/serial.h"
#include "libs/timer.h"
//#include "libs/leds.h"
//#include "libs/button.h"

//files located in avr/cpu folder of contiki
#include "dev/leds.h"
#include "dev/button.h"
#include "dev/rs232.h"

#define true 1

/*---------------------------------------------------------------------------*/
PROCESS(example_process, "Main process");
AUTOSTART_PROCESSES(&example_process);
/*---------------------------------------------------------------------------*/

PROCESS_THREAD(example_process, ev, data)
{
	PROCESS_BEGIN();

	//SENSORS_ACTIVATE(button_sensor);

	watchdog_stop(); // stop contiki rebooting


	unsigned char ch;
	bool echo=false;

	// setup the hardware
	//serial_init(0, 57600);
	//serial_init(1, 57600);

	rs232_init(0, USART_BAUD_57600, USART_DATA_BITS_8);
	rs232_init(1, USART_BAUD_57600, USART_DATA_BITS_8);



	leds_init();
	timer_init();
	button_init();
	timer_init();

	// say hello
	rs232_print(0, "ATMega128RFA1 Serial Relay\n\r");
	rs232_print(0, " Note: adds linefeed to return on output to UART1 \n\r");
	//serial_puts(0, "ATMega128RFA1 Serial Relay\n\r");
	//serial_puts(0," Note: adds linefeed to return on output to UART1 \n\r");


	volatile int running=1;

	while (running) {

		if(button_pressed())
		{
			rs232_print(0, "button pressed\n\r");
			while (!button_released());
		}

		//have to write a function that processes incoming data from rs232
		//then link it using rs232_set_input(uint8_t port, int (* f)(unsigned char));
		//see platform/avr-atmega128rfa1/apps/raven-lcd-interface for an example

		if (rs232_ready(0))  //something is wrong with the ready function - we're never getting in here
		{
			ch = rs232_get(0);
			char ch_signed = (char) ch;
			rs232_print(1,&ch_signed);			
			if (echo)
			{
				rs232_print(0,&ch_signed);
				rs232_print(0,"Should be echoing\n\r");
			}
			if (ch=='\r')
				rs232_print(1,"\n"); // Automatically add a linefeed to every return - required by the RN2483 command interface
		}
		if (rs232_ready(1)) 
		{
			ch = rs232_get(1);
			char ch_signed = (char) ch;
			rs232_print(0,&ch_signed);
		}

		//PROCESS_WAIT_EVENT();

		//if(ev == sensors_event && data == &button_sensor) {
		if(button_pressed())
		{
			if (echo) {
				rs232_print(0, "\n\rEcho disabled\n\r");
				echo = false;
                leds_on(LEDS_GREEN);
                leds_on(LEDS_YELLOW);
                leds_on(LEDS_RED);
			}
			else {
				rs232_print(0, "\n\rEcho enabled\n\r");
				echo = true;
				leds_off(LEDS_GREEN);
                leds_off(LEDS_YELLOW);
                leds_off(LEDS_RED);
			}				
			while (!button_released());
		}	

		/*if (serial_ready(0)) 
		{
			ch = serial_get(0);
			serial_put(1,ch);			
			if (echo)
				serial_put(0,ch);
			if (ch=='\r')
				serial_put(1,'\n'); // Automatically add a linefeed to every return - required by the RN2483 command interface
		}
		if (serial_ready(1)) 
		{
			ch = serial_get(1);
			serial_put(0,ch);
		}

		//PROCESS_WAIT_EVENT();

		//if(ev == sensors_event && data == &button_sensor) {
		if(button_pressed())
		{
			if (echo) {
				serial_puts(0, "\n\rEcho disabled\n\r");
				echo = false;
                leds_on(LEDS_GREEN);
                leds_on(LEDS_YELLOW);
                leds_on(LEDS_RED);
			}
			else {
				serial_puts(0, "\n\rEcho enabled\n\r");
				echo = true;
				leds_off(LEDS_GREEN);
                leds_off(LEDS_YELLOW);
                leds_off(LEDS_RED);
			}				
			while (!button_released());
		}*/			

	}

	PROCESS_END();
}
/*---------------------------------------------------------------------------*/
