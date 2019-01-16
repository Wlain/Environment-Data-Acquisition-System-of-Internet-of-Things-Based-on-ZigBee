#include <ioCC2530.h>             //引入CC2530所对应的头文件（包含各SFR的定义）

/*----------------------------------------------------------------------------*/
/*                            macro declaration 	                      */
/*----------------------------------------------------------------------------*/
#define D7    P1_0              //定义D7为P1_0口控制
#define D6    P1_1              //定义D6为P1_1口控制

#define uint   unsigned  int
#define uchar  unsigned  char

#define CLKSPD  ( CLKCONCMD & 0x07 )    //getting the clock division factor

void xtal_init(void);
void led_init(void);
void ext_init(void);
void time1_init(void);
void uart0_init(unsigned char StopBits,unsigned char Parity);
void watchdog_init(void);
void sleepTimer_init(void);
void halWait(unsigned char wait);