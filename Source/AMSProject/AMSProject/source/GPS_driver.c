/*
* GPS_driver.c
*
* Created: 4/5/2018 1:27:11 PM
*  Author: Qtra
*/

#include "GPS_driver.h"

void GPS_Init()
{
	gpsUart.baseAddress = GPS_UART_BASE;
	gpsUart.sercom		= 5;
	
	struct uartsetup_t gpsSetup;
	
	gpsSetup.baudRate = 19200;
	gpsSetup.dataBits = 8;
	gpsSetup.parity = 0;
	gpsSetup.stopBits = 1;
	gpsSetup.rxBufferAddr = gpsRxBuffer;
	gpsSetup.rxBufferSize = GPS_UART_BUFFER_SIZE;
	
	UART_Init(gpsUart, gpsSetup);
}

uint8_t GPS_ConstructMessage(uint8_t class, uint8_t ID, uint16_t length, char* payload, char* packetBufffer) {
	//Init
	uint8_t ck_a;
	uint8_t ck_b;
	
	// Expects PacketBuffer to be required size, while max size is max of uart output
	uint8_t checksum_length = 6 + length + 4;
	
	if(length + 10 > MAX_TX_SIZE)
	return 0;
	
	packetBufffer[2] = class;
	packetBufffer[3] = ID;
	
	// Fill in payload
	uint16_t i;
	for (i = 0; i < length; i++)
	{
		packetBufffer[i+4] = payload[i];
	}
	GPS_CalculateChecksum(checksum_length, packetBufffer, &ck_a, &ck_b);
	
	packetBufffer[6 + length] = ck_a;
	packetBufffer[6 + length + 1] = ck_b;
	packetBufffer[6 + length + 2] = '\r';
	packetBufffer[6 + length + 3] = '\n';
	
	// Add Sync Chars (Done later to not include into checksum)
	packetBufffer[0] = 0xB5;
	packetBufffer[1] = 0x62;
	
	return checksum_length;
}

void GPS_CalculateChecksum(uint16_t lenght, char* payload, uint8_t* ck_a, uint8_t* ck_b)
{
	// Calculate checksum
	uint16_t i;
	for (i = 0; i < lenght; i++)
	{
		*ck_a = (*ck_a + payload[i]) &0xff;
		*ck_b = (*ck_b + *ck_a)& 0xff;
	}
}

void GPS_Poll()
{
	char* msg = GPS_POLL_MSG;
	UART_SendBuffer(gpsUart,msg, GPS_POLL_MSG_LENGTH);
	
	uint16_t countToBreak = 0;
	while (countToBreak == 0)
	{
		countToBreak = UART_ScanRXBuffer(gpsUart, '\n');
	}
	uint8_t input[countToBreak];
	UART_Recieve(gpsUart,input, countToBreak);
}