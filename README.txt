There are two goals for this repo:

1. Add in full support for atmega128rfa1 to Contiki. Though the platform is currently officially supported there are a lot of missing features (e.g. the leds library is just an empty shell).

2. Build an interface for interacting with LoRa boards. This will first be simply sending firmware defined commands over a serial line to a RN2483, and reading and passing on the output over the other available serial line.

Then, the plan is to write software for an SX1272 and have the two boards communicate over the serial line in a more interesting way than is provided by the RN2483's firmware-defined commands. 
