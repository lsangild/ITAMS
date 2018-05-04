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
	
	return; //FIXME To Disable init og GPS
	
    // Disable unwanted messages
	uint8_t cmd_DisableMSG[] = {0xF0, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    GPS_setup(0x06, 0x01, 8, cmd_DisableMSG); //
	
	cmd_DisableMSG[1] = 0x05;
	GPS_setup(0x06, 0x01, 8, cmd_DisableMSG);
	
	cmd_DisableMSG[1] = 0x00;
    GPS_setup(0x06, 0x01, 8, cmd_DisableMSG);
	
	cmd_DisableMSG[1] = 0x02;
    GPS_setup(0x06, 0x01, 8, cmd_DisableMSG);
	
	cmd_DisableMSG[1] = 0x03;
    GPS_setup(0x06, 0x01, 8, cmd_DisableMSG);
	
	cmd_DisableMSG[1] = 0x01;
    GPS_setup(0x06, 0x01, 8, cmd_DisableMSG);
	
	cmd_DisableMSG[1] = 0x41;
    GPS_setup(0x06, 0x01, 8, cmd_DisableMSG);
    
    // Turn of Glonass
    uint8_t cmd_DisableGlonass[] = {0x00, 0x16, 0x16, 0x04, 0x00, 0x04, 0xFF, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x01, 0x03, 0x00, 0x01,
		0x00, 0x00, 0x00, 0x05, 0x00, 0x03, 0x00, 0x01, 0x00, 0x00, 0x00, 0x06, 0x08, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00};
    GPS_setup(0x06, 0x3E, 36, cmd_DisableGlonass);
    
    // Set Navigation Configuration to pedestrian
    uint8_t cmd_SetPedestrian[36] = {0};
	cmd_SetPedestrian[0] = 0x01;
	cmd_SetPedestrian[2] = 0x03;
    GPS_setup(0x06, 0x24, 36, cmd_SetPedestrian);
    
    // Power management ON/OFF mode
    uint8_t cmd_PowerManagement[44] = {0};
    cmd_PowerManagement[0] = 0x01;
    cmd_PowerManagement[6] = 0x28;
    GPS_setup(0x06, 0x3B, 44, cmd_PowerManagement);
    
    // Apply Power save mode
	uint8_t cmd_PowerSave[] = {0x08, 0x01};
    GPS_setup(0x06, 0x11, 2, cmd_PowerSave);
}

uint8_t GPS_ConstructMessage(uint8_t class, uint8_t ID, uint16_t length, uint8_t* payload, uint8_t* packetBufffer) {
	//Init
	uint8_t ck_a;
	uint8_t ck_b;
	
	// Expects PacketBuffer to be required size, while max size is max of uart output
	uint8_t msg_length = 6 + length + 4;
	
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
	GPS_CalculateChecksum(msg_length, packetBufffer, &ck_a, &ck_b);
	
	packetBufffer[6 + length] = ck_a;
	packetBufffer[6 + length + 1] = ck_b;
	packetBufffer[6 + length + 2] = '\r';
	packetBufffer[6 + length + 3] = '\n';
	
	// Add Sync Chars (Done later to not include into checksum)
	packetBufffer[0] = 0xB5;
	packetBufffer[1] = 0x62;
	
	return msg_length;
}

void GPS_CalculateChecksum(uint16_t length, uint8_t* payload, uint8_t* ck_a, uint8_t* ck_b)
{
	// Calculate checksum
	uint16_t i;
	for (i = 0; i < length; i++)
	{
		*ck_a = (*ck_a + payload[i]) &0xff;
		*ck_b = (*ck_b + *ck_a)& 0xff;
	}
}

uint8_t GPS_send(uint8_t class, uint8_t ID, uint16_t length, uint8_t* payload, uint8_t* answer)
{
    // Send data
    uint8_t msg[4 + length + 6];
    GPS_ConstructMessage(class, ID, length, payload, msg);
    UART_SendBuffer(gpsUart, msg, 4 + length + 6);
    
    // Receive
    uint16_t countToBreak = 0;
    while (countToBreak == 0)
    {
        countToBreak = UART_ScanRXBuffer(gpsUart, '\n');
    }
    UART_Recieve(gpsUart, answer, countToBreak);
    
    // Return number of bytes read
    return countToBreak;
}

uint8_t GPS_setup(uint8_t cmdClass, uint8_t cmdID, uint16_t length, uint8_t* payload)
{
    // Send data
    uint8_t answer[GPS_ACK_LENGTH];
    uint8_t bytesReceived = GPS_send(cmdClass, cmdID, length, payload, answer);
    
    // Only check if ack/nack if correct amount of data is received.
    if (bytesReceived == GPS_ACK_LENGTH)
    {
        // Check if acked
        if (GPS_CheckAcknowledge(length, cmdClass, cmdID, answer))
        {
	        return 1;
        }
        else
        {
	        return 0;
        }
    }
    else
    {
        return 0;
    }
}

struct GPS_data_t GPS_Poll()
{   
    // Create message and send it
    uint8_t msg[GPS_POLL_MSG_LENGTH];
    uint8_t bytesReceived = GPS_send(0x01, 0x07, 0, 0x00, msg);
    
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


uint8_t GPS_CheckAcknowledge(uint16_t length, uint8_t cmdClass, uint8_t cmdID, uint8_t* buffer)
{
	//Set stucture data
	union Neo7_Ack* recieved;
	recieved = (void*) buffer;// Set union pointer to correct address
	
	if(recieved->Neo7_Ack_T.acknowledge[0] != 0x05)
		return 2;	
	
	if(recieved->Neo7_Ack_T.cmdClass == cmdClass && recieved->Neo7_Ack_T.cmdID == cmdID)
	{
		//Create Checksum test / Checksum function
		
		if(recieved->Neo7_Ack_T.acknowledge[1] == 0x01)
			return 0; //Success
		else
			return 1; //Not Acknowledge
	}
	
	return 3;
}
