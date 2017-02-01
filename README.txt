There are two goals for this repo:

1. Add in full support for atmega128rfa1 to Contiki. Though the platform is currently officially supported there are a lot of missing features (e.g. in the main branch of Contiki the leds library is just an empty shell). This will involve integrating the rfa1-supported libraries I already have (from a very basic OS, see libs folder) into a Contiki branch.

2. Build an interface for interacting with LoRa boards. This will first be simply sending firmware defined commands over a serial line to a RN2483, and reading and passing on the output over the other available serial line.

Then, the plan is to write software for an SX1272 and have the two boards communicate over the serial line in a more interesting way than is provided by the RN2483's firmware-defined commands.

This repo is a work in progress - some things work right now and some don't. See TODO file for details. 
