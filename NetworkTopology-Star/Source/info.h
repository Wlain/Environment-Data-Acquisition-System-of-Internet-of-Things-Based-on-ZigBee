#ifndef _INFO_H
#define _INFO_H
#include "hal_uart.h"
void initUart(halUARTCBack_t pf);
void lcd_dis(void);
void halMcuWaitUs(uint16 usec);
void halMcuWaitMs(uint16 msec);
void Uart_Send_char(char ch);
void Uart_Send_String(char *Data);
void uart0_init(unsigned char StopBits,unsigned char Parity);
#endif