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
#include <string.h>
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


int  append(char*s, size_t size, char c) {
     if(strlen(s) + 1 >= size) {
          return 0;
     }
     int len = strlen(s);
     s[len] = c;
     s[len+1] = '\0';
     return 1;
}

/*---------------------------------------------------------------------------*/
PROCESS(example_process, "Main process");
PROCESS(button_process, "Button process");

AUTOSTART_PROCESSES(&example_process, &button_process);

/*---------------------------------------------------------------------------*/

PROCESS_THREAD(example_process, ev, data)
{

	serial_line_init();

  rs232_set_input(RS232_PORT_0,serial_line_input_byte_0);
  rs232_set_input(RS232_PORT_1,serial_line_input_byte_1);

	PROCESS_BEGIN();

	watchdog_stop();

	rs232_print(RS232_PORT_0, "ATMega128RFA1 Serial Relay on Contiki\r\n");

	volatile int running = 1;

    while (running)
    {
     	PROCESS_YIELD();  	//this should wait for a specific event
     	if(ev == serial_line_event_message_0) 
      {
          // data coming from PC, send it on to RN2483.        
          // have to append newline to string as per RN2483 requirements.
          char buf[strlen(data)+2];
          strcpy(buf, data);

          buf[strlen(data)]   = '\r';
          buf[strlen(data)+1]   = '\n';
          buf[strlen(data)+2]   = '\0';
          rs232_print(RS232_PORT_0, buf);
          rs232_print(RS232_PORT_1, buf);
          
    	}
      else if(ev == serial_line_event_message_1) 
      {
          // data coming from RN2483, send it on to PC.
          rs232_print(RS232_PORT_0, "received line from uart1:\r\n");
          rs232_print(RS232_PORT_0, (char *)data);
          rs232_print(RS232_PORT_0, "\r\n");
      }
   	}
   	PROCESS_END();
}

PROCESS_THREAD(button_process, ev, data)
{
	PROCESS_BEGIN();
	
  process_start(&sensors_process, NULL);

	SENSORS_ACTIVATE(button_sensor);

	//AUTOSTART_PROCESSES(&sensors_process);

	watchdog_stop();

	leds_init();

  leds_on(LEDS_GREEN);
  leds_on(LEDS_YELLOW);
  leds_on(LEDS_RED);
  
	//volatile int on = 1;
	
	while(1) {

		  //rs232_print(0, "waiting for button press\n\r");
    	PROCESS_WAIT_EVENT_UNTIL((ev==sensors_event) && (data == &button_sensor));
    	//rs232_print(0, "flipping leds\n\r");
    	if(leds_get())
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
