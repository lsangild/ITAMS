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

union Neo7_Ack
{
	struct{
	uint8_t header[2];
	uint8_t acknowledge[2];
	uint16_t lenght;
	uint8_t cmdClass;
	uint8_t cmdID;
	uint8_t ck_a;
	uint8_t ck_b;
	uint8_t	footer[2];
	} Neo7_Ack_T;
	
	uint8_t data[12];
};


extern struct uart_t gpsUart;
uint8_t gpsRxBuffer[GPS_UART_BUFFER_SIZE];

// Setup GPS on sercom5
void GPS_Init();
// Adds header, checksum and CRLF to the message and writes it into packetBuffer. Returns complete message length.
uint8_t GPS_ConstructMessage(uint8_t class, uint8_t ID, uint16_t lenght, uint8_t* payload, uint8_t* packetBuffer);
// Calculates checksum and writes values into ck_a and ck_b
void GPS_CalculateChecksum(uint16_t length, uint8_t* payload, uint8_t* ck_a, uint8_t* ck_b);
// Sends and receives GPS data. Answer is sritten into answer. Returns number of bytes received.
uint8_t GPS_send(uint8_t class, uint8_t ID, uint16_t length, uint8_t* payload, uint8_t* answer);
// Used for sending setup commands. Checks for ack/nack and returns appropriate true/false
uint8_t GPS_setup(uint8_t class, uint8_t ID, uint16_t length, uint8_t* payload);
// Polls the GPS for data and returns it in the GPS_data_t struct
struct GPS_data_t GPS_Poll();

uint8_t GPS_CheckAcknowledge(uint16_t length, uint8_t class, uint8_t id, uint8_t* buffer);





#endif /* GPS_DRIVER_H_ */
