/*
  Written by J. Finnegan.
*/

#include <stdio.h>
#include <string.h>
#include "contiki.h"
#include "watchdog.h"
#include "libs/timer.h"

//files located in cpu/avr folder of contiki
#include "dev/leds.h"
#include "dev/rs232.h"
#include "dev/uart1.h"
#include "serial-line-dual.h"
#include "dev/button-sensor.h"
#include "sys/etimer.h"
#include "radio/rf230bb/rf230bb.c"

//#include "libs/hw_timer.h" 
//#include "libs/radio.h"  

/*---------------------------------------------------------------------------*/
PROCESS(relay_process, "Relay process");
PROCESS(button_process, "Button process");
PROCESS(timer_process, "Timer process");

AUTOSTART_PROCESSES(&relay_process, &button_process, &timer_process);

/*---------------------------------------------------------------------------*/
/*process which shows example of use of serial relay*/
PROCESS_THREAD(relay_process, ev, data)
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
          rs232_print(RS232_PORT_1, buf);
          
    	}
      else if(ev == serial_line_event_message_1) 
      {
          // data coming from RN2483, send it on to PC.
          rs232_print(RS232_PORT_0, (char *)data);
          rs232_print(RS232_PORT_0, "\r\n");
      }
   	}
   	PROCESS_END();
}

/*---------------------------------------------------------------------------*/
/*process which should example of use of button*/
PROCESS_THREAD(button_process, ev, data)
{
	PROCESS_BEGIN();
	
  process_start(&sensors_process, NULL);

	SENSORS_ACTIVATE(button_sensor);

	watchdog_stop();

	leds_init();

  leds_on(LEDS_GREEN);
  leds_on(LEDS_YELLOW);
  leds_on(LEDS_RED);
	
	while(1) {

    	PROCESS_WAIT_EVENT_UNTIL((ev==sensors_event) && (data == &button_sensor));

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
/*Process which shows example of use of timers, and of radio send*/
PROCESS_THREAD(timer_process, ev, data)
{
  static struct etimer et;
  PROCESS_BEGIN();
  
  serial_line_init();

  rs232_set_input(RS232_PORT_0,serial_line_input_byte_0);

  /* Delay 10 seconds */
  etimer_set(&et, CLOCK_SECOND*10);

  //radio_init(0x01, false);
  //radio_set_power(15);
  //radio_start();

  rf230_init();

  //set_txpower(15);

  while(1) {

      PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
     
      rs232_print(RS232_PORT_0, "Timer expired!\r\n");

      int succ = rf230_send("test",4);

      if(succ)
      {
        rs232_print(RS232_PORT_0, "Transmit successful!\r\n");
      }
      else
      {
        rs232_print(RS232_PORT_0, "Transmit failed!\r\n");
      }
      
      etimer_reset(&et);

    }
    PROCESS_END();
}
/*---------------------------------------------------------------------------*/
