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
#include "libs/timer.h"

//files located in avr/cpu folder of contiki
#include "dev/leds.h"
#include "dev/rs232.h"
#include "dev/uart1.h"
#include "dev/serial-line.h"
#include "dev/button-sensor.h"
//#include "lib/sensors.h"

/*---------------------------------------------------------------------------*/
PROCESS(example_process, "Main process");
PROCESS(button_process, "Button process");

//process_event_t serial_line_event_message; //do NOT redefine this event here

AUTOSTART_PROCESSES(&example_process, &button_process);

/*---------------------------------------------------------------------------*/

PROCESS_THREAD(example_process, ev, data)
{
	//rs232_set_input(RS232_PORT_0, serial_line_input_byte);
	//rs232_set_input(RS232_PORT_1, serial_line_input_byte);

	serial_line_init();

	PROCESS_BEGIN();

	watchdog_stop();

	rs232_print(0, "ATMega128RFA1 Serial Relay\n\r");
	rs232_print(1, "mac get adr\n\r");

	volatile int running = 1;

    while (running)
    {
    	rs232_print(0, "In loop\n\r");
     	PROCESS_YIELD();  	
     	rs232_print(0, "Past yield!\n\r");
     	if(ev == serial_line_event_message) {
       		//TODO: Instead of doing this, it should pass on the message to the other board
       		//Is there a way to know where its coming from?
       		rs232_print(RS232_PORT_0, "received line:\n\r");
       		rs232_print(RS232_PORT_0, (char *)data);
       		rs232_print(RS232_PORT_0, "\n\r");

       		rs232_print(RS232_PORT_1, (char *)data);
       		//rs232_print(RS232_PORT_1, "\n\r");
    	}
   	}
   	PROCESS_END();
}

PROCESS_THREAD(button_process, ev, data)
{
	PROCESS_BEGIN();
	
	SENSORS_ACTIVATE(button_sensor);

	//AUTOSTART_PROCESSES(&sensors_process);

	watchdog_stop();

	rs232_set_input(RS232_PORT_0, serial_line_input_byte);

	leds_init();

  leds_on(LEDS_GREEN);
  leds_on(LEDS_YELLOW);
  leds_on(LEDS_RED);
  
	volatile int on = 1;
	
	while(1) {

		rs232_print(0, "waiting for button press\n\r");
    	PROCESS_WAIT_EVENT_UNTIL(ev == sensors_event && data == &button_sensor);
    	rs232_print(0, "flipping leds\n\r");
    	if(on)
    	{
			leds_off(LEDS_GREEN);
            leds_off(LEDS_YELLOW);
            leds_off(LEDS_RED);
    	}
    	else
    	{
    		leds_on(LEDS_GREEN);
            leds_on(LEDS_YELLOW);
            leds_on(LEDS_RED);
    	}
    }
    PROCESS_END();
}
/*---------------------------------------------------------------------------*/
