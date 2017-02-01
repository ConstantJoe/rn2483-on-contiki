void radio_init(unsigned short macaddr);
void radio_start();
unsigned char radio_tx_done();
void radio_send(unsigned char *buffer, unsigned char len, unsigned short dst);
unsigned char radio_rxed();
unsigned char radio_recv(unsigned char *buffer, unsigned char len,
   unsigned short *dst, unsigned short *src,
   unsigned char *valid_crc, unsigned char *energy, unsigned char *lqi);

void radio_set_power(unsigned char level);

unsigned char radio_rssi();
char radio_max_rssi();

unsigned char radio_latest_energy();
unsigned char radio_latest_lqi();

void radio_shutdown();
