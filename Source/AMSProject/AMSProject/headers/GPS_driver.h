/*
 * GPS_driver.h
 *
 * Created: 4/5/2018 1:26:24 PM
 *  Author: Qtra
 */ 


#ifndef GPS_DRIVER_H_
#define GPS_DRIVER_H_

#include <string.h>
#include "uart_driver.h"

extern struct uart_t gpsUart;

void InitGPS();

uint8_t ConstructGPSMessage(uint8_t class, uint8_t ID, uint16_t lenght, char* payload, char* packet);
void CalculateChecksum(uint16_t lenght, char* payload, uint8_t ck_a, ck_b);



#endif /* GPS_DRIVER_H_ */
