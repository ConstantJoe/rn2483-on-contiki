/*
  Written by J. Finnegan.
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "watchdog.h"
#include "dev/leds.h"
#include "dev/rs232.h"
#include "serial-line-dual.h"
#include "dev/button-sensor.h"
//#include "radio/rf230bb/rf230bb.c"
#include "net/rime/rime.h"
//#include "net/rime/broadcast.h"

/*---------------------------------------------------------------------------*/
PROCESS(relay_process, "Relay process");
PROCESS(button_process, "Button process");
PROCESS(timer_process, "Timer process");

AUTOSTART_PROCESSES(&button_process, &relay_process);

/*from example-broadcast.c*/
static void
broadcast_recv(struct broadcast_conn *c, const linkaddr_t *from)
{
  rs232_print(RS232_PORT_0, "Message received.\r\n");
  rs232_print(RS232_PORT_0, (char *)packetbuf_dataptr());
  //printf("broadcast message received from %d.%d: '%s'\n",
  //       from->u8[0], from->u8[1], (char *)packetbuf_dataptr());
}
static const struct broadcast_callbacks broadcast_call = {broadcast_recv};
static struct broadcast_conn broadcast;

unsigned int sys_sleep(unsigned long length)
{

  if(length > 100)
  {
    char* command = "sys sleep ";

    //convert ulong to string
    const int n = snprintf(NULL, 0, "%lu", length);
    assert(n > 0);
    char len_str[n+1];
    int c = snprintf(len_str, n+1, "%lu", length);
    assert(len_str[n] == '\0');
    assert(c == n);

    char *result = malloc(strlen(command)+strlen(len_str)+3);
    strcpy(result, command);
    strcat(result, len_str);
    strcat(result, "\r\n");

    rs232_print(RS232_PORT_1, result);

    return 1;
  }
  else
  {
    rs232_print(RS232_PORT_0, "ERROR: sleep length not long enough.");
    return 0;
  }
}

unsigned int sys_sleep_response(char* data)
{
  if(strcmp(data,"ok"))
  {
    return 1;
  }
  else
  {
    return 0;
  }
}

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
     	/*if(ev == serial_line_event_message_0) 
      {
          // data coming from PC, send it on to RN2483.        
          // have to append newline to string as per RN2483 requirements.
          char buf[strlen(data)+2];
          strcpy(buf, data);

          buf[strlen(data)]   = '\r';
          buf[strlen(data)+1]   = '\n';
          buf[strlen(data)+2]   = '\0';
          rs232_print(RS232_PORT_1, buf);

          rs232_print(RS232_PORT_0, "sending\r\n");          
          
    	}
      else */if(ev == serial_line_event_message_1) 
      {
          // data coming from RN2483, send it on to PC.
          rs232_print(RS232_PORT_0, (char *)data);
          rs232_print(RS232_PORT_0, "\r\n");


      }
   	}
   	PROCESS_END();
}

/*---------------------------------------------------------------------------*/
/*process which shows example of use of button*/
PROCESS_THREAD(button_process, ev, data)
{
    serial_line_init();
    rs232_set_input(RS232_PORT_0,serial_line_input_byte_0);
    rs232_set_input(RS232_PORT_1,serial_line_input_byte_1);
  PROCESS_EXITHANDLER(broadcast_close(&broadcast);)


  //serial_line_init();

  //rs232_set_input(RS232_PORT_1,serial_line_input_byte_1);

	PROCESS_BEGIN();
	
  broadcast_open(&broadcast, 129, &broadcast_call);

  process_start(&sensors_process, NULL);

	SENSORS_ACTIVATE(button_sensor);

	watchdog_stop();

  rs232_print(RS232_PORT_0, "ATMega128RFA1 Serial Relay on Contiki\r\n");

	leds_init();

	while(1) {

    	PROCESS_WAIT_EVENT_UNTIL((ev==sensors_event) && (data == &button_sensor));

    	if(leds_get())
    	{
			      leds_off(LEDS_GREEN);

            //char* ret_data = mac_get_devaddr();

            //rs232_print(RS232_PORT_0, "return data\r\n");
            //rs232_print(RS232_PORT_0, ret_data);
            //leds_off(LEDS_RED);

            //unsigned long sleep_time = 2000;
            //sys_sleep(sleep_time);

            //Idea: all function calls will be in the format
            //Where all function names are in the same format as in the RN2483 AT commands manual, 
            //all (or at least most) initial calls return an unsigned boolean, and response function to x is x_response.

            unsigned long sleep_time = 2000;
            if(sys_sleep(sleep_time)) 
            {
                PROCESS_WAIT_EVENT_UNTIL(ev==serial_line_event_message_0);
                unsigned int response = sys_sleep_response((char*) data);

                if(response)
                {
                    rs232_print(RS232_PORT_0, "success\r\n");
                }
                else
                {
                    rs232_print(RS232_PORT_0, "failure\r\n"); 
                }
            }
            else
            {
                // "catch" block
                rs232_print(RS232_PORT_0, "sys sleep failed\r\n"); 
            }
    	}
    	else
    	{
            packetbuf_copyfrom("Hello", 6);
            broadcast_send(&broadcast);
            rs232_print(RS232_PORT_0, "Message sent\r\n");
            
            /*int succ = rf230_send("test",4);

            if(succ)
            {*/
            leds_on(LEDS_GREEN);
            /*
            else
            {
                leds_on(LEDS_RED);
            }*/
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

  //rf230_init();

  while(1) {

      PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
     
      rs232_print(RS232_PORT_0, "Timer expired!\r\n");

      /*int succ = rf230_send("test",4);

      if(succ)
      {
        rs232_print(RS232_PORT_0, "Transmit successful!\r\n");
      }
      else
      {
        rs232_print(RS232_PORT_0, "Transmit failed!\r\n");
      }*/
      
      etimer_reset(&et);

    }
    PROCESS_END();
}
/*---------------------------------------------------------------------------*/
