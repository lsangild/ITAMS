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

#define HEADER_SIZE 2
#define CMD_SIZE 2
#define LENGTH_SIZE 2
#define PREPAYLOAD_SIZE (HEADER_SIZE + CMD_SIZE + LENGTH_SIZE)
#define CHECK_SIZE 2

// Data structure containing received GPS data
struct GPS_data_t{
	uint16_t year;
	uint8_t month;
	uint8_t date;
	uint8_t hour;
	uint8_t minute;
	uint8_t second;
	// valid = 0, means data is valid.
	uint8_t valid;
	uint32_t lat;
	uint32_t lon;
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
	} Neo7_Ack_T;
	
	uint8_t data[GPS_ACK_LENGTH];
};

struct Neo7_MsgHeader{
  uint8_t header[2];
  uint8_t class;
  uint8_t ID;
  uint16_t lenght;
};

extern struct uart_t gpsUart;
uint8_t gpsRxBuffer[GPS_UART_BUFFER_SIZE];

// Setup GPS on sercom5
void GPS_Init();
// Adds header, checksum and CRLF to the message and writes it into packetBuffer. Returns complete message length.
uint8_t GPS_ConstructMessage(uint8_t class, uint8_t ID, uint16_t lenght, uint8_t* payload, uint8_t* packetBuffer);
// Calculates checksum and writes values into ck_a and ck_b
void GPS_CalculateChecksum(uint16_t length, uint8_t* payload, uint8_t* ck_a, uint8_t* ck_b);
// Sends and receives GPS data. Answer is written into answer. Returns number of bytes received.
uint8_t GPS_send(uint8_t class, uint8_t ID, uint16_t length, uint8_t* payload, uint8_t* answer);
// Used for sending setup commands. Checks for ack/nack and returns appropriate true/false
uint8_t GPS_setup(uint8_t class, uint8_t ID, uint16_t length, uint8_t* payload);
// Polls the GPS for data and returns it in the GPS_data_t struct
void GPS_Poll(struct GPS_data_t* data);

uint8_t GPS_CheckAcknowledge(uint8_t class, uint8_t id, uint8_t* buffer);


#endif /* GPS_DRIVER_H_ */