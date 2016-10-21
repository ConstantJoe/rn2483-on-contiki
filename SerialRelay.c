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
#include "serial-line.h"

//#include "libs/serial.h"
#include "libs/timer.h"
//#include "libs/leds.h"
//#include "libs/button.h"

//files located in avr/cpu folder of contiki
#include "dev/leds.h"
#include "dev/button.h"
#include "dev/rs232.h"
#include "dev/uart1.h"
//#include "dev/serial-line.h"

#define true 1

/*---------------------------------------------------------------------------*/
PROCESS(example_process, "Main process");
PROCESS(button_process, "Button process");
AUTOSTART_PROCESSES(&example_process/*, &button_process*/);
/*---------------------------------------------------------------------------*/

//watchdog_stop();


PROCESS_THREAD(example_process, ev, data)
{
	//PROCESS_BEGIN();

	//SENSORS_ACTIVATE(button_sensor);

	rs232_set_input(RS232_PORT_0, serial_line_input_byte);
	process_init();
	serial_line_init();

	PROCESS_BEGIN();

	watchdog_stop();
	///process_init();
	//rs232_init(0, USART_BAUD_57600, USART_DATA_BITS_8);
	//rs232_set_input(RS232_PORT_0, serial_line_input_byte);
	//uart1_set_input(serial_line_input_byte);
	//rs232_set_input(RS232_PORT_1, serial_line_input_byte);
	//process_init();
	//serial_line_init();

	rs232_print(0, "ATMega128RFA1 Serial Relay\n\r");
	//printf("ATMega128RFA1 Serial Relay end\n\r");

	volatile int running=1;
    while (running)
    {
    	rs232_print(0, "In loop\n\r");
     	PROCESS_YIELD();
     	rs232_print(0, "Past yield!\n\r");
     	if(ev == serial_line_event_message) {
       		//printf("received line: %s\n", (char *)data);
       		rs232_print(0, "received line:\n\r");
       		rs232_print(0, (char *)data);
       		rs232_print(0, "\n\r");
    	}
    	else
    	{
    		//printf("no received line: %s\n", (char *)data);
       		rs232_print(0, "no received line:\n\r");
       		rs232_print(0, (char *)data);
       		rs232_print(0, "\n\r");
       		rs232_print(0, "ev char\n\r");
       		rs232_print(0, (char *)ev);
       		rs232_print(0, "\n\r");
    	}
   	}

   	rs232_print(0, "ATMega128RFA1 Serial Relay end\n\r");
   	//printf("ATMega128RFA1 Serial Relay end\n\r");

   	PROCESS_END();
}


//TODO: change button to work using SENSORS_ACTIVATE function
PROCESS_THREAD(button_process, ev, data)
{
	PROCESS_BEGIN();
	
	
	bool echo=false;
	leds_init();
	button_init();


	volatile int running=1;

	while (running) {

		if(button_pressed())
		{
			rs232_print(0, "button pressed\n\r");
			while (!button_released());
			process_poll(&serial_line_process);
		}

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
	}
	PROCESS_END();	
}
	/*watchdog_stop(); // stop contiki rebooting


	unsigned char ch;
	bool echo=false;

	// setup the hardware
	//serial_init(0, 57600);
	//serial_init(1, 57600);

	rs232_init(0, USART_BAUD_57600, USART_DATA_BITS_8);
	rs232_init(1, USART_BAUD_57600, USART_DATA_BITS_8);

	rs232_redirect_stdout(0); // generic "print function gets sent to UART0"
	rs232_set_input(0, serial_line_input_byte); // set function that gets called whenever input comes from UART0 - use generic serial function predefined in Contiki.
	rs232_set_input(1, serial_line_input_byte);
	//serial_line_init();

	serial_line_init();
	//serial_shell_init();

	leds_init();
	timer_init();
	button_init();
	timer_init();

	// say hello
	//rs232_print(0, "ATMega128RFA1 Serial Relay\n\r");
	//rs232_print(0, " Note: adds linefeed to return on output to UART1 \n\r");
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

		PROCESS_YIELD();
     	if(ev == serial_line_event_message) {
     		printf("received line: %s\n", (char *)data);
     		rs232_print(0, "received line\n\r");
     	}
     	//else
     	//{
     		//printf("no received line print: %s\n", (char *)data);
     		//rs232_print(0, "no received line rs232\n\r");
     	//}

		//like this, but not using rs232_ready and rs232_get
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

		if (serial_ready(0)) 
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
		}	

	}

	PROCESS_END();
}*/
/*---------------------------------------------------------------------------*/
