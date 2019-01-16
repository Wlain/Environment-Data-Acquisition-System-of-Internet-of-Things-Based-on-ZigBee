/*********************************************************************************************
* 文件：info.c
* 作者：liutong 2016.7.20
* 说明：通过串口控制LCD显示的相关程序
* 修改：
* 注释：
*********************************************************************************************/
#include <ioCC2530.h>            
#include <stdio.h>
#include "info.h"
#include "string.h"
#include "OnBoard.h"
#define HAL_INFOP_IEEE_OSET        0xC                          //mac地址偏移量
#define halWait(x) halMcuWaitMs(x)

/*********************************************************************************************
* 名称：lcd_dis
* 功能：通过串口控制LCD显示实验内容和节点的MAC
* 参数：无
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void lcd_dis(void){
  for(unsigned char i = 0;i<2;i++){                              //发送TYPE,发2遍
    Uart_Send_String("{TYPE=06205}");                            //星状网实验
    halWait(250);
    halWait(250);
  }
  
  halWait(250);
  halWait(250);
  char CC2530_MAC[30] = {0};                                     //存放MAC
  char devmacaddr[8];
  unsigned char *macaddrptr = (unsigned char *)(P_INFOPAGE+HAL_INFOP_IEEE_OSET);
  for(int i=0;i<8;i++) {
    devmacaddr[i] = macaddrptr[i];                              //获取mac地址
  }
  strcat(CC2530_MAC,"{MAC=");
  sprintf(&CC2530_MAC[5],"%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X",
                          devmacaddr[7],devmacaddr[6],devmacaddr[5],
                          devmacaddr[4],devmacaddr[3],devmacaddr[2],
                          devmacaddr[1],devmacaddr[0]);
  CC2530_MAC[28]='}';
   for(unsigned char i = 0;i<2;i++){                            //发送MAC，发2遍
   Uart_Send_String(CC2530_MAC);  
   halWait(250);
   halWait(250);
  }
}
/*********************************************************************************************
* 名称：halMcuWaitMs
* 功能：毫秒延时函数
* 参数：无
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
#pragma optimize=none
void halMcuWaitMs(uint16 msec){
  while(msec--)
    halMcuWaitUs(1000);

}
/*********************************************************************************************
* 名称：halMcuWaitUs
* 功能：微秒延时函数
* 参数：无
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
#pragma optimize=none
void halMcuWaitUs(uint16 usec)
{
    usec>>= 1;
    while(usec--)
    {
        asm("NOP");
        asm("NOP");
        asm("NOP");
        asm("NOP");
        asm("NOP");
        asm("NOP");
        asm("NOP");
        asm("NOP");
        asm("NOP");
        asm("NOP");
        asm("NOP");
        asm("NOP");
        asm("NOP");
        asm("NOP");
        asm("NOP");
        asm("NOP");
        asm("NOP");
    }
}
/*uart0初始化
-------------------------------------------------------*/
void uart0_init(unsigned char StopBits,unsigned char Parity)
{
  P0SEL |=  0x0C;                 //初始化UART0端口
  PERCFG&= ~0x01;                 //选择UART0为可选位置一
  P2DIR &= ~0xC0;                 //P0优先作为串口0
  U0CSR = 0xC0;                   //设置为UART模式,而且使能接受器
   
  U0GCR = 0x0A;                  
  U0BAUD = 0x3B;                  //波特率设置为38400
  
  U0UCR |= StopBits|Parity;       //设置停止位与奇偶校验
}
/*串口发送字节函数
-------------------------------------------------------*/
void Uart_Send_char(char ch)
{
  U0DBUF = ch;
  while(UTX0IF == 0);
  UTX0IF = 0;
}

/*串口发送字符串函数
-------------------------------------------------------*/
void Uart_Send_String(char *Data)
{  
  while (*Data != '\0')
  {
    Uart_Send_char(*Data++);
  }
}