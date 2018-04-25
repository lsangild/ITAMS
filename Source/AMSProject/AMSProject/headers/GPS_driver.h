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

// Data structure containing received GPS data
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

// Setup GPS on sercom5
void GPS_Init();
// Adds header, checksum and CRLF to the message and writes it into packetBuffer. Returns complete message length.
uint8_t GPS_ConstructMessage(uint8_t class, uint8_t ID, uint16_t lenght, char* payload, char* packetBuffer);
// Calculates checksum and writes values into ck_a and ck_b
void GPS_CalculateChecksum(uint16_t length, char* payload, uint8_t* ck_a, uint8_t* ck_b);
// Sends and receives GPS data. Answer is sritten into answer. Returns number of bytes received.
uint8_t GPS_send(uint8_t class, uint8_t ID, uint16_t length, char* payload, char* answer);
// Used for sending setup commands. Checks for ack/nack and returns appropriate true/false
uint8_t GPS_setup(uint8_t class, uint8_t ID, uint16_t length, char* payload);
// Polls the GPS for data and returns it in the GPS_data_t struct
struct GPS_data_t GPS_Poll();





#endif /* GPS_DRIVER_H_ */
