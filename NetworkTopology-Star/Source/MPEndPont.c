/**************************************************************************************************
  Filename:       DemoSensor.c

  Description:    Sensor application for the sensor demo utilizing the Simple API.

                  The sensor application binds to a gateway and will periodically 
                  read temperature and supply voltage from the ADC and send report   
                  towards the gateway node.  


  Copyright 2009 Texas Instruments Incorporated. All rights reserved.

  IMPORTANT: Your use of this Software is limited to those specific rights
  granted under the terms of a software license agreement between the user
  who downloaded the software, his/her employer (which must be your employer)
  and Texas Instruments Incorporated (the "License").  You may not use this
  Software unless you agree to abide by the terms of the License. The License
  limits your use, and you acknowledge, that the Software may not be modified,
  copied or distributed unless embedded on a Texas Instruments microcontroller
  or used solely and exclusively in conjunction with a Texas Instruments radio
  frequency transceiver, which is integrated into your product.  Other than for
  the foregoing purpose, you may not use, reproduce, copy, prepare derivative
  works of, modify, distribute, perform, display or sell this Software and/or
  its documentation for any purpose.

  YOU FURTHER ACKNOWLEDGE AND AGREE THAT THE SOFTWARE AND DOCUMENTATION ARE
  PROVIDED 揂S IS?WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED,
  INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF MERCHANTABILITY, TITLE,
  NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT SHALL
  TEXAS INSTRUMENTS OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER CONTRACT,
  NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR OTHER
  LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
  INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE
  OR CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT
  OF SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
  (INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.

  Should you have any questions regarding your right to use this Software,
  contact Texas Instruments Incorporated at www.TI.com.
**************************************************************************************************/

/******************************************************************************
 * INCLUDES
 */

#include "ZComDef.h"
#include "OSAL.h"
#include "sapi.h"
#include "hal_key.h"
#include "hal_lcd.h"
#include "hal_led.h"
#include "hal_adc.h"
#include "hal_mcu.h"
#include "hal_uart.h"
#include "info.h"
#include "protocol.h"

/******************************************************************************
 * CONSTANTS
 */
#define REPORT_FAILURE_LIMIT                4
#define ACK_REQ_INTERVAL                    5 // each 5th packet is sent with ACK request

// Application States
#define APP_INIT                            0    // Initial state
#define APP_START                           1    // Sensor has joined network
#define APP_BIND                            2    // Sensor is in process of binding
#define APP_REPORT                          4    // Sensor is in reporting state

// Application osal event identifiers
// Bit mask of events ( from 0x0000 to 0x00FF )
#define MY_START_EVT                        0x0001
#define MY_REPORT_EVT                       0x0002
#define MY_FIND_COLLECTOR_EVT               0x0004

#define MY_PROFILE_ID                     0x0F20
#define MY_ENDPOINT_ID                    0x02

// Define devices
#define DEV_ID_SENSOR                     1
#define DEV_ID_COLLECTOR                  2

#define DEVICE_VERSION_SENSOR             1
#define DEVICE_VERSION_COLLECTOR          1

// Define the Command ID's used in this application
#define SENSOR_REPORT_CMD_ID              2
#define DUMMY_REPORT_CMD_ID               3

// Sensor report data format
#define SENSOR_TEMP_OFFSET                0
#define SENSOR_VOLTAGE_OFFSET             1
#define SENSOR_PARENT_OFFSET              2
#define SENSOR_REPORT_LENGTH              4

#define RX_BUF_LEN                        128

#define PACKET_SENSOR_TYPE PACKET_COMBUSTIBLEGAS       //PACKET_DHT11 温湿度传感器
                                                        //PACKET_PHOTORESUSTANCE 光照传感器
                                                        //PACKET_COMBUSTIBLEGAS 烟雾传感器

#if PACKET_SENSOR_TYPE == PACKET_DHT11
    #include "dht11.h"
#endif


/******************************************************************************
 * TYPEDEFS
 */

/******************************************************************************
 * LOCAL VARIABLES
 */

#if PACKET_SENSOR_TYPE == PACKET_PHOTORESUSTANCE
    static void photose_init(void)
    {
      P0SEL |= 0x02;
      P0DIR |= 0x02;
    }
#elif PACKET_SENSOR_TYPE  == PACKET_COMBUSTIBLEGAS
    static void gas_init(void)
    {
      P0SEL |= 0x02;
      P0DIR |= 0x02;
    }
#endif

static uint8 appState =           APP_INIT;

static uint8 reportFailureNr =    0;

static uint8 myStartRetryDelay =    10;         // milliseconds
static uint16 myReportPeriod =    1000;         // milliseconds
static uint16 myBindRetryDelay =  2000;         // milliseconds

static uint16 parentShortAddr;

/******************************************************************************
 * GLOBAL VARIABLES
 */

// Inputs and Outputs for Sensor device
#define NUM_OUT_CMD_SENSOR                1
#define NUM_IN_CMD_SENSOR                 0

// List of output and input commands for Sensor device
const cId_t zb_OutCmdList[NUM_OUT_CMD_SENSOR] =
{
  SENSOR_REPORT_CMD_ID
};

// Define SimpleDescriptor for Sensor device
const SimpleDescriptionFormat_t zb_SimpleDesc =
{
  MY_ENDPOINT_ID,             //  Endpoint
  MY_PROFILE_ID,              //  Profile ID
  DEV_ID_SENSOR,              //  Device ID
  DEVICE_VERSION_SENSOR,      //  Device Version
  0,                          //  Reserved
  NUM_IN_CMD_SENSOR,          //  Number of Input Commands
  (cId_t *) NULL,             //  Input Command List
  NUM_OUT_CMD_SENSOR,         //  Number of Output Commands
  (cId_t *) zb_OutCmdList     //  Output Command List
};


/******************************************************************************
 * LOCAL FUNCTIONS
 */

static void sendReport(void);
void zb_HanderMsg(osal_event_hdr_t *pMsg);
/*****************************************************************************
 * @fn          zb_HandleOsalEvent
 *
 * @brief       The zb_HandleOsalEvent function is called by the operating
 *              system when a task event is set
 *
 * @param       event - Bitmask containing the events that have been set
 *
 * @return      none
 */
void zb_HandleOsalEvent( uint16 event )
{
    uint8 logicalType;
#if PACKET_SENSOR_TYPE == PACKET_DHT11
  uint8 pData[PACKET_BASE_SIZE + PACKET_DHT11_DATA_SIZE];
#elif PACKET_SENSOR_TYPE == PACKET_PHOTORESUSTANCE
  uint8 pData[PACKET_BASE_SIZE + PACKET_PHOTORESUSTANCE_DATA_SIZE];
#else
  uint8 pData[PACKET_BASE_SIZE + PACKET_COMBUSTIBLEGAS_DATA_SIZE];
#endif
  if(event & SYS_EVENT_MSG)
  {
    
  }
  
  if( event & ZB_ENTRY_EVENT )          //zigbee入网事件
  { 
    // blind LED 2 to indicate joining a network
    HalLedSet( HAL_LED_2, HAL_LED_MODE_OFF );
    HalLedBlink ( HAL_LED_2, 0, 50, 500 );
     
    logicalType = ZG_DEVICETYPE_ENDDEVICE;
    zb_WriteConfiguration(ZCD_NV_LOGICAL_TYPE, sizeof(uint8), &logicalType);
#ifdef LCD_USE
    uart0_init(0,0);
    lcd_dis();                                              //显示先关信息     
    //Uart_Send_String("{data=LINK: off                 This is a endpoint}");          //显示在线
#endif
    // Start the device 
    zb_StartRequest();
    
#if PACKET_SENSOR_TYPE == PACKET_DHT11
  dht11_io_init();
#elif PACKET_SENSOR_TYPE == PACKET_PHOTORESUSTANCE
  photose_init();
#else
  gas_init();
#endif
  }
  
  if ( event & MY_REPORT_EVT )      // MY_REPORT_EVT事件触发处理
  {
    if ( appState == APP_REPORT ) 
    {
      pData[PACKET_FLAG_OFFSET] = PACKET_L_FLAG;
      pData[PACKET_FLAG_OFFSET + 1] = PACKET_H_FLAG;
      pData[PACKET_SENSOR_SHORT_ADDR_OFFSET] = (NLME_GetShortAddr() & 0xff);
      pData[PACKET_SENSOR_SHORT_ADDR_OFFSET + 1] = (NLME_GetShortAddr() >> 8);
#if PACKET_SENSOR_TYPE == PACKET_DHT11
    dht11_update(); 
    pData[PACKET_SENSOR_TYPE_OFFSET] = PACKET_DHT11;
    pData[PACKET_SENSOR_DATA_OFFSET] = dht11_temp();
    pData[PACKET_SENSOR_DATA_OFFSET + 1] = dht11_humidity();
#elif PACKET_SENSOR_TYPE == PACKET_PHOTORESUSTANCE
    pData[PACKET_SENSOR_TYPE_OFFSET] = PACKET_PHOTORESUSTANCE;
    pData[PACKET_SENSOR_DATA_OFFSET] = HalAdcRead (HAL_ADC_CHN_AIN1,HAL_ADC_RESOLUTION_8);
#else
    pData[PACKET_SENSOR_TYPE_OFFSET] = PACKET_COMBUSTIBLEGAS;
    pData[PACKET_SENSOR_DATA_OFFSET] = HalAdcRead (HAL_ADC_CHN_AIN1,HAL_ADC_RESOLUTION_8);
#endif
    
#if PACKET_SENSOR_TYPE == PACKET_DHT11
      zb_SendDataRequest( 0xFFFE, SENSOR_REPORT_CMD_ID, PACKET_BASE_SIZE + PACKET_DHT11_DATA_SIZE, pData, 0, AF_TX_OPTIONS_NONE, 0 );
#elif PACKET_SENSOR_TYPE == PACKET_PHOTORESUSTANCE
      zb_SendDataRequest( 0xFFFE, SENSOR_REPORT_CMD_ID, PACKET_BASE_SIZE + PACKET_PHOTORESUSTANCE_DATA_SIZE, pData, 0, AF_TX_OPTIONS_NONE, 0 );
#else
      zb_SendDataRequest( 0xFFFE, SENSOR_REPORT_CMD_ID, PACKET_BASE_SIZE + PACKET_COMBUSTIBLEGAS_DATA_SIZE, pData, 0, AF_TX_OPTIONS_NONE, 0 );
#endif
      osal_start_timerEx( sapi_TaskID, MY_REPORT_EVT, myReportPeriod );
    }
  }
  if ( event & MY_FIND_COLLECTOR_EVT )
  {
    // Delete previous binding
    if ( appState==APP_REPORT ) 
    {
      zb_BindDevice( FALSE, SENSOR_REPORT_CMD_ID, (uint8 *)NULL );
    }
    
    appState = APP_BIND;
    
    // Find and bind to a collector device
    zb_BindDevice( TRUE, SENSOR_REPORT_CMD_ID, (uint8 *)NULL );
  }
}

void zb_HanderMsg(osal_event_hdr_t *msg)
{

}
/******************************************************************************
 * @fn      zb_HandleKeys
 *
 * @brief   Handles all key events for this device.
 *
 * @param   shift - true if in shift/alt.
 * @param   keys - bit field for key events. Valid entries:
 *                 EVAL_SW4
 *                 EVAL_SW3
 *                 EVAL_SW2
 *                 EVAL_SW1
 *
 * @return  none
 */
void zb_HandleKeys( uint8 shift, uint8 keys )
{
  // Shift is used to make each button/switch dual purpose.
  if ( shift )
  {
    if ( keys & HAL_KEY_SW_1 )
    {
    }
    if ( keys & HAL_KEY_SW_2 )
    {
    }
    if ( keys & HAL_KEY_SW_3 )
    {
    }
    if ( keys & HAL_KEY_SW_4 )
    {
    }
  }
  else
  {
    if ( keys & HAL_KEY_SW_1 )
    {
    }
    if ( keys & HAL_KEY_SW_2 )
    {
    }
    if ( keys & HAL_KEY_SW_3 )
    {
    }
    if ( keys & HAL_KEY_SW_4 )
    {
    }
  }
}

/******************************************************************************
 * @fn          zb_StartConfirm
 *
 * @brief       The zb_StartConfirm callback is called by the ZigBee stack
 *              after a start request operation completes
 *
 * @param       status - The status of the start operation.  Status of
 *                       ZB_SUCCESS indicates the start operation completed
 *                       successfully.  Else the status is an error code.
 *
 * @return      none
 */
void zb_StartConfirm( uint8 status )
{
  // If the device sucessfully started, change state to running
  if ( status == ZB_SUCCESS )   //zigbee协议栈启动成功
  {
    // Change application state
    appState = APP_START;
    
    // Set LED 2 to indicate that node is operational on the network
    HalLedSet( HAL_LED_2, HAL_LED_MODE_ON );
   
    // Store parent short address
    zb_GetDeviceInfo(ZB_INFO_PARENT_SHORT_ADDR, &parentShortAddr);
    
    // Set event to bind to a collector
    osal_set_event( sapi_TaskID, MY_FIND_COLLECTOR_EVT ); 
  }
  else            //zigbee协议栈启动失败重新启动
  {
    // Try again later with a delay
    osal_start_timerEx( sapi_TaskID, MY_START_EVT, myStartRetryDelay );
  }
}

/******************************************************************************
 * @fn          zb_SendDataConfirm
 *
 * @brief       The zb_SendDataConfirm callback function is called by the
 *              ZigBee after a send data operation completes
 *
 * @param       handle - The handle identifying the data transmission.
 *              status - The status of the operation.
 *
 * @return      none
 */
void zb_SendDataConfirm( uint8 handle, uint8 status )
{
  if(status != ZB_SUCCESS) 
  {
    if ( ++reportFailureNr >= REPORT_FAILURE_LIMIT ) 
    {
       // Stop reporting
       osal_stop_timerEx( sapi_TaskID, MY_REPORT_EVT );
       
       // Try binding to a new gateway
       osal_set_event( sapi_TaskID, MY_FIND_COLLECTOR_EVT );
       reportFailureNr=0;
    }
  }
  // status == SUCCESS
  else 
  {
    // Reset failure counter
    reportFailureNr=0;
  }
}

/******************************************************************************
 * @fn          zb_BindConfirm
 *
 * @brief       The zb_BindConfirm callback is called by the ZigBee stack
 *              after a bind operation completes.
 *
 * @param       commandId - The command ID of the binding being confirmed.
 *              status - The status of the bind operation.
 *
 * @return      none
 */
void zb_BindConfirm( uint16 commandId, uint8 status )
{
  if( status == ZB_SUCCESS )
  {   
    appState = APP_REPORT;
    
    // Start reporting
    osal_set_event( sapi_TaskID, MY_REPORT_EVT );

  }
  else
  {
    osal_start_timerEx( sapi_TaskID, MY_FIND_COLLECTOR_EVT, myBindRetryDelay );
  }
}

/******************************************************************************
 * @fn          zb_AllowBindConfirm
 *
 * @brief       Indicates when another device attempted to bind to this device
 *
 * @param
 *
 * @return      none
 */
void zb_AllowBindConfirm( uint16 source )
{
}

/******************************************************************************
 * @fn          zb_FindDeviceConfirm
 *
 * @brief       The zb_FindDeviceConfirm callback function is called by the
 *              ZigBee stack when a find device operation completes.
 *
 * @param       searchType - The type of search that was performed.
 *              searchKey - Value that the search was executed on.
 *              result - The result of the search.
 *
 * @return      none
 */
void zb_FindDeviceConfirm( uint8 searchType, uint8 *searchKey, uint8 *result )
{
}

/******************************************************************************
 * @fn          zb_ReceiveDataIndication
 *
 * @brief       The zb_ReceiveDataIndication callback function is called
 *              asynchronously by the ZigBee stack to notify the application
 *              when data is received from a peer device.
 *
 * @param       source - The short address of the peer device that sent the data
 *              command - The commandId associated with the data
 *              len - The number of bytes in the pData parameter
 *              pData - The data sent by the peer device
 *
 * @return      none
 */
void zb_ReceiveDataIndication( uint16 source, uint16 command, uint16 len, uint8 *pData  )
{
  if(len >= PACKET_BASE_SIZE 
     && pData[PACKET_FLAG_OFFSET] == PACKET_L_FLAG 
     && pData[PACKET_FLAG_OFFSET + 1] == PACKET_H_FLAG
     && *(uint16 *)(pData + PACKET_SENSOR_SHORT_ADDR_OFFSET) == NLME_GetShortAddr())
  {
    switch(pData[PACKET_OPERATION_TYPE_OFFSET])
    {
      case PACKET_ON:
        HalLedSet( HAL_LED_1, HAL_LED_MODE_ON );
        break;
      case PACKET_OFF:
        HalLedSet( HAL_LED_1, HAL_LED_MODE_OFF );
        break;
      case PACKET_TOGGLE:
        HalLedSet( HAL_LED_1, HAL_LED_MODE_TOGGLE );
        break;
      case PACKET_BLINK:
        HalLedBlink(HAL_LED_1, 0, 50, 500);
        break;
      default:
        break;
      }
  }
}

/******************************************************************************
 * @fn          sendReport
 *
 * @brief       Send sensor report
 *
 * @param       none
 *              
 * @return      none
 */
static void sendReport(void)
{
 /* static uint8 reportNr=0;
  uint8 txOptions;
  //上报过程中LED灯闪烁一次
  HalLedSet( HAL_LED_1, HAL_LED_MODE_OFF );
  HalLedSet( HAL_LED_1, HAL_LED_MODE_BLINK );  
  
  // Read and report temperature value
  /*pData[SENSOR_TEMP_OFFSET] =  0x30;     //任意选取的常量
  
  // Read and report voltage value
  pData[SENSOR_VOLTAGE_OFFSET] = 0x03;   //任意选取的常量
    
  pData[SENSOR_PARENT_OFFSET] =  HI_UINT16(parentShortAddr);
  pData[SENSOR_PARENT_OFFSET + 1] =  LO_UINT16(parentShortAddr);
  
  // Set ACK request on each ACK_INTERVAL report
  // If a report failed, set ACK request on next report
  if ( ++reportNr<ACK_REQ_INTERVAL && reportFailureNr==0 ) 
  {
    txOptions = AF_TX_OPTIONS_NONE;
  }
  else 
  {
    txOptions = AF_MSG_ACK_REQUEST;
    reportNr = 0;
  }
  // Destination address 0xFFFE: Destination address is sent to previously
  // established binding for the commandId.
  //将数据包发送给协调器（协调器的地址为0xFFFE）
  //zb_SendDataRequest( 0xFFFE, SENSOR_REPORT_CMD_ID, SENSOR_REPORT_LENGTH, pData, 0, txOptions, 0 );
  pPacket[PACKET_FLAG_OFFSET] = 0xff;
  pPacket[PACKET_SELF_SHORT_ADDR_OFFSET] = NLME_GetShortAddr();
  pPacket[PACKET_PARENT_SHORT_ADDR_OFFSET] = NLME_GetCoordShortAddr();
  #if PACKET_SENSOR_TYPE == PACKET_DHT11
    pPacket[PACKET_PACKET_SENSOR_TYPE_OFFSET] = PROTOCOL_HUMITEMP;
    //dht11_init();
    dht11_start();
    pPacket[PACKET_DATA_OFFSET] = dht11_shidu[0];
    pPacket[PACKET_DATA_OFFSET + 1] = dht11_wendu[0];
  #elif PACKET_SENSOR_TYPE == PACKET_PHOTORESUSTANCE
    pPacket[PACKET_PACKET_SENSOR_TYPE_OFFSET] = PROTOCOL_PHOTORESISTANCE;
    //photoresistance_init();
    pPacket[PACKET_DATA_OFFSET] = (photoresistance_data() & 0xff);
    pPacket[PACKET_DATA_OFFSET + 1] = (photoresistance_data() >> 8);
  #elif PACKET_SENSOR_TYPE  == PACKET_COMBUSTIBLEGAS
    pPacket[PACKET_PACKET_SENSOR_TYPE_OFFSET] = PROTOCOL_COMBUSTIBLEGAS;
    //combustibleGas_init();
    pPacket[PACKET_DATA_OFFSET] = (combustibleGas_data() & 0xff);
    pPacket[PACKET_DATA_OFFSET + 1] = (combustibleGas_data() >> 8);
  #endif
  zb_SendDataRequest( 0xFFFE, SENSOR_REPORT_CMD_ID, 4, "123", 0, txOptions, 0 );*/
}