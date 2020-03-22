#include <wiringPi.h>
#define SHIFT 0x12
#define ENTER 0xA
#define K_PRESS 2
#define K_RELEASE 3
int kb_scan();
void init_keyboard();
