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

struct GPS_data_t{
	uint16_t year;
	uint8_t month;
	uint8_t date;
	uint8_t hour;
	uint8_t minute;
	uint8_t second;
	uint32_t lat;
	uint32_t lon;
	// error = 0, means data is valid.
	uint8_t error;
};

extern struct uart_t gpsUart;
uint8_t gpsRxBuffer[GPS_UART_BUFFER_SIZE];

void GPS_Init();
uint8_t GPS_ConstructMessage(uint8_t class, uint8_t ID, uint16_t lenght, char* payload, char* packetBuffer);
void GPS_CalculateChecksum(uint16_t lenght, char* payload, uint8_t* ck_a, uint8_t* ck_b);
struct GPS_data_t GPS_Poll();




#endif /* GPS_DRIVER_H_ */
