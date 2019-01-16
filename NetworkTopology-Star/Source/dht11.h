#ifndef __DHT11_H__
#define __DHT11_H__

void dht11_io_init(void);

void dht11_update(void);

unsigned char dht11_humidity(void);

unsigned char dht11_temp(void);

#endif
