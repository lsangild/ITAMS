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
	
	gpsSetup.baudRate = 64307;// Hardcoded for baud 9600 - real should be 9600;
	gpsSetup.dataBits = 8;
	gpsSetup.parity = 0;
	gpsSetup.stopBits = 1;
	gpsSetup.rxBufferAddr = gpsRxBuffer;
	gpsSetup.rxBufferSize = GPS_UART_BUFFER_SIZE;
	
	UART_Init(gpsUart, gpsSetup);
	
	UART_SendBuffer(gpsUart, 0xB56206010800F004000000000000033F0D0A, 18);
}

uint8_t GPS_ConstructMessage(uint8_t class, uint8_t ID, uint16_t length, char* payload, char* packetBufffer) {
	//Init
	uint8_t ck_a;
	uint8_t ck_b;
	
	// Expects PacketBuffer to be required size, while max size is max of uart output
	uint8_t checksum_length = 6 + length + 4;
	
	if(length + 10 > MAX_TX_SIZE){
        return 0;
    }
	
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

struct GPS_data_t GPS_Poll()
{   
    // Create message and send it
    char msg[GPS_POLL_MSG_LENGTH];
    GPS_ConstructMessage(0x01, 0x07, 0x0000, 0x00, msg);
	UART_SendBuffer(gpsUart, msg, GPS_POLL_MSG_LENGTH);
	
    // Receive GPS data
	uint16_t countToBreak = 0;
	while (countToBreak == 0)
	{
		countToBreak = UART_ScanRXBuffer(gpsUart, '\n');
	}
	uint8_t input[countToBreak];
	UART_Recieve(gpsUart, input, countToBreak);
    
    // Setup struct for data
    struct GPS_data_t data;
    data.error = 1;

    // Copy data to GPS struct
    if ((countToBreak == 95) && (input[29] > 0))
	{
        memcpy(&data.year, input + 10, 2);
        memcpy(&data.month, input + 12, 1);
        memcpy(&data.date, input + 13, 1);
        memcpy(&data.hour, input + 14, 1);
        memcpy(&data.minute, input + 15, 1);
        memcpy(&data.second, input + 16, 1);
        memcpy(&data.lat, input + 34, 4);
        memcpy(&data.lon, input + 30, 4);
        data.error = 0;
    }
    
    return data;
}
