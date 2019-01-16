#include "ZComDef.h"
#include "OSAL.h"
#include "sapi.h"
#include "hal_key.h"
#include "hal_led.h"
#include "DebugTrace.h"
#include "osal_nv.h"
#include "NLMEDE.h"
#include "AF.h"
#include "OnBoard.h"
#include "nwk_util.h"
#include "ZDProfile.h"
#include "ZDObject.h"
#include "hal_led.h"
#include "hal_key.h"
#include "sapi.h"
#include "MT_SAPI.h"

#if defined( MT_TASK )
  #include "osal_nv.h"
#endif
#include "mt_app.h"
#include "mt_uart.h"
#include "mt.h"

#define my_FindDeviceConfirm zb_FindDeviceConfirm

void my_FindDevice(uint8 searchType, uint8 *searchKey);
void my_FindDeviceConfirm( uint8 searchType, uint8 *searchKey, uint8 *result );
void SAPI_SendCback( uint8 event, uint8 status, uint16 data );

static int my_FindDeviceProc( uint16 source, uint16 command, uint16 len, uint8 *pData);

void my_SendDataRequest ( uint8* destination, uint16 commandId, uint8 len,
                          uint8 *pData, uint8 handle, uint8 txOptions, uint8 radius );
#define SAPICB_DATA_CNF   0xE0
void my_SendDataRequest ( uint8* destination, uint16 commandId, uint8 len,
                          uint8 *pData, uint8 handle, uint8 txOptions, uint8 radius )
{
  afStatus_t status;
  afAddrType_t dstAddr;

  txOptions |= AF_DISCV_ROUTE;

  // Set the destination address
  //if (destination == ZB_BINDING_ADDR)
  {
    // Binding
    //dstAddr.addrMode = afAddrNotPresent;
  }
  //else
  {
    // Use short address
    //dstAddr.addr.shortAddr = destination;
    //dstAddr.addrMode = afAddr16Bit;
    osal_memcpy(dstAddr.addr.extAddr, destination, Z_EXTADDR_LEN);
    //MT_ReverseBytes( dstAddr.addr.extAddr, Z_EXTADDR_LEN );
    
    dstAddr.addrMode = afAddr64Bit;

    //if ( ADDR_NOT_BCAST != NLME_IsAddressBroadcast( destination ) )
    //{
     // txOptions &= ~AF_ACK_REQUEST;
    //}
  }

  dstAddr.panId = 0;                                    // Not an inter-pan message.
  dstAddr.endPoint = sapi_epDesc.simpleDesc->EndPoint;  // Set the endpoint.

  // Send the message
  status = AF_DataRequest(&dstAddr, &sapi_epDesc, commandId, len,
                          pData, &handle, txOptions, radius);

  if (status != afStatus_SUCCESS)
  {
    SAPI_SendCback( SAPICB_DATA_CNF, status, handle );
  }
}

void my_FindDevice(uint8 searchType, uint8 *searchKey)
{
  uint16 dAddr;
  uint16 cmd;
  uint8 dlen;
  if (ZB_IEEE_SEARCH == searchType)  {
    dAddr = 0xffff;
    cmd = 0xff01;
    dlen = 8;
    //my_SendDataRequest(searchKey, cmd, dlen, searchKey , 0, 0, AF_DEFAULT_RADIUS );
    zb_SendDataRequest(dAddr, cmd, dlen, searchKey , 0, AF_ACK_REQUEST, AF_DEFAULT_RADIUS );
  }
  if (ZB_NWKA_SEARCH == searchType) {
    dAddr = searchKey[0]<<8 | searchKey[1];
    cmd = 0xff02;
    dlen = 2;
    zb_SendDataRequest(dAddr, cmd, dlen,searchKey , 0, AF_ACK_REQUEST, AF_DEFAULT_RADIUS );
  }
 
}

static int my_FindDeviceProc( uint16 source, uint16 command, uint16 len, uint8 *pData) 
{
  static uint8 dat[10];
  
  if (command == 0xff01) {
    uint8 *pExtAddr = dat;
    ZMacGetReq( ZMacExtAddr, pExtAddr );
    if (len == Z_EXTADDR_LEN && TRUE == osal_memcmp(pData, pExtAddr, Z_EXTADDR_LEN)) {    
      dat[8] = NLME_GetShortAddr();
      dat[9] = NLME_GetShortAddr() >> 8;
      zb_SendDataRequest(source, 0xfe01, 10, dat, 0, AF_ACK_REQUEST, AF_DEFAULT_RADIUS);
    }
    return 1;
  }
  if (command == 0xff02) {
    uint16 sa = NLME_GetShortAddr();
    if (len == 2 && (pData[0]<<8 | pData[1]) == sa) {
      dat[0] = sa;
      dat[1] = sa>>8;
      ZMacGetReq( ZMacExtAddr, &dat[2] );
      zb_SendDataRequest(source, 0xfe02, 10, dat, 0, AF_ACK_REQUEST, AF_DEFAULT_RADIUS);
    }
    return 1;
  }
  if (command == 0xfe01) {
    if (len == 10) {
      my_FindDeviceConfirm(ZB_IEEE_SEARCH, pData, (unsigned char *)&pData[8]);
    }
    return 1;
  }
  if (command == 0xfe02) {
    if (len == 10) {
      my_FindDeviceConfirm(ZB_NWKA_SEARCH, pData, (unsigned char *)&pData[2]);
    }
  }
  return 0;
}
extern void _zb_ReceiveDataIndication( uint16 source, uint16 command, uint16 len, uint8 *pData  );
void _zb_ReceiveDataIndication( uint16 source, uint16 command, uint16 len, uint8 *pData  )
{
  if (my_FindDeviceProc(source, command, len, pData) != 0) return;
  zb_ReceiveDataIndication(source, command, len, pData);
}