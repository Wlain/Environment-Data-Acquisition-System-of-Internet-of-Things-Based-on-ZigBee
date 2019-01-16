#ifndef _PROTOCOL_H_
#define _PROTOCOL_H_

/******************************************************************************
 * 数据包定义
 */

/****数据包标志****/
#define PACKET_H_FLAG 0x00
#define PACKET_L_FLAG 0xff

/****数据包类型****/
#define PACKET_DHT11            0x01
#define PACKET_PHOTORESUSTANCE  0x02
#define PACKET_COMBUSTIBLEGAS   0x03

/****操作类型****/
#define PACKET_ON				0x01
#define PACKET_OFF			0x02
#define PACKET_TOGGLE			0x03
#define PACKET_BLINK			0x04


/****数据包偏移量****/
#define PACKET_FLAG_OFFSET              0
#define PACKET_SENSOR_TYPE_OFFSET       2
#define PACKET_SENSOR_SHORT_ADDR_OFFSET 3
#define PACKET_SENSOR_DATA_OFFSET       5

#define PACKET_OPERATION_TYPE_OFFSET	2


/****数据包大小****/
#define PACKET_BASE_SIZE                        PACKET_SENSOR_DATA_OFFSET
#define PACKET_DHT11_DATA_SIZE                  2
#define PACKET_PHOTORESUSTANCE_DATA_SIZE        1
#define PACKET_COMBUSTIBLEGAS_DATA_SIZE         1

#endif