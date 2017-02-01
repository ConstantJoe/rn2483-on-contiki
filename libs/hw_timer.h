#include <stdbool.h>

void hw_timer_init(void);
void hw_timer_start(unsigned short value);
bool hw_timer_overflow();
unsigned short hw_timer_now();
unsigned long hw_timer_now_us();
void hw_timer_wait_micro(unsigned short delay);
void hw_timer_wait_milli(unsigned short delay);

