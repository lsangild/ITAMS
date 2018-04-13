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
#include "sw_defines.h"

extern struct uart_t gpsUart;
uint8_t gpsRxBuffer[GPS_UART_BUFFER_SIZE];

void GPS_Init();
uint8_t GPS_ConstructMessage(uint8_t class, uint8_t ID, uint16_t lenght, char* payload, char* packetBufffer);
void GPS_CalculateChecksum(uint16_t lenght, char* payload, uint8_t* ck_a, uint8_t* ck_b);
void GPS_Poll();




#endif /* GPS_DRIVER_H_ */
