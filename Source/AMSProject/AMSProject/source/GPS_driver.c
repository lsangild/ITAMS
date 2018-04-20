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
	
    // Disable unwanted messages
    GPS_setup(0x06, 0x01, 8, 0xF004000000000000);
	GPS_setup(0x06, 0x01, 8, 0xF005000000000000);
    GPS_setup(0x06, 0x01, 8, 0xF000000000000000);
    GPS_setup(0x06, 0x01, 8, 0xF002000000000000);
    GPS_setup(0x06, 0x01, 8, 0xF003000000000000);
    GPS_setup(0x06, 0x01, 8, 0xF001000000000000);
    GPS_setup(0x06, 0x01, 8, 0xF041000000000000);
    
    // Turn of Glonass
    GPS_setup(0x06, 0x3E, 36, 0x001616040004FF0001000000010103000100000005000300010000000608FF0000000000);
    
    // Set Navigation Configuration to pedestrian
    GPS_setup(0x06, 0x24, 36, 0x010003000000000000000000000000000000000000000000000000000000000000000000);
    
    // Power management ON/OFF mode
    GPS_setup(0x06, 0x3B, 44, 0x0100000000002800000000000000000000000000000000000000000000000000000000000000000000000000);
    
    // Apply Power save mode
    GPS_setup(0x06, 0x11, 2, 0x0801);
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

void GPS_CalculateChecksum(uint16_t length, char* payload, uint8_t* ck_a, uint8_t* ck_b)
{
	// Calculate checksum
	uint16_t i;
	for (i = 0; i < length; i++)
	{
		*ck_a = (*ck_a + payload[i]) &0xff;
		*ck_b = (*ck_b + *ck_a)& 0xff;
	}
}

uint8_t GPS_send(uint8_t class, uint8_t ID, uint16_t length, char* payload, char* answer)
{
    // Send data
    char msg[4 + length + 6];
    GPS_ConstructMessage(class, ID, length, payload, msg);
    UART_SendBuffer(gpsUart, msg, 4 + length + 6);
    
    // Receive
    uint16_t countToBreak = 0;
    while (countToBreak == 0)
    {
        countToBreak = UART_ScanRXBuffer(gpsUart, '\n');
    }
    uint8_t input[countToBreak];
    UART_Recieve(gpsUart, answer, countToBreak);
    
    // Return number of bytes read
    return countToBreak;
}

uint8_t GPS_setup(uint8_t class, uint8_t ID, uint16_t length, char* payload)
{
    // Send data
    char answer[10];
    GPS_send(class, ID, length, payload, answer);
    
    // Create expected answer
    uint8_t ck_a;
    uint8_t ck_b;
    GPS_CalculateChecksum(length, payload, &ck_a, &ck_b)
    char expected[10] = {0xB5, 0x62, class, ID, (uint8_t) length, (uint8_t) length >> 8, ck_a, ck_b, 0x0D, 0x0A};
    
    // Check if acked
    if (answer == expected)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

struct GPS_data_t GPS_Poll()
{   
    // Create message and send it
    char msg[GPS_POLL_MSG_LENGTH];
    bytesReceived = GPS_send(0x01, 0x07, 0, 0x00, char* msg)
    
    // Setup struct for data
    struct GPS_data_t data;
    data.error = 1;

    // Copy data to GPS struct
    if ((bytesReceived == 95) && (msg[29] > 0))
	{
        memcpy(&data.year, msg + 10, 2);
        memcpy(&data.month, msg + 12, 1);
        memcpy(&data.date, msg + 13, 1);
        memcpy(&data.hour, msg + 14, 1);
        memcpy(&data.minute, msg + 15, 1);
        memcpy(&data.second, msg + 16, 1);
        memcpy(&data.lat, msg + 34, 4);
        memcpy(&data.lon, msg + 30, 4);
        data.error = 0;
    }
    
    return data;
}
