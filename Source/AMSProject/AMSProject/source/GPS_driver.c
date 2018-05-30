/*
* GPS_driver.c
*
* Created: 4/5/2018 1:27:11 PM
*  Author: Qtra
*/

#include "GPS_driver.h"
#include "utility.h"

void GPS_Init()
{
	gpsUart.baseAddress = GPS_UART_BASE;
	gpsUart.sercom		= 5;
	
	struct uartsetup_t gpsSetup;
	
	gpsSetup.baudRate = 9600;// Hardcoded for baud 9600 - real should be 9600;
	gpsSetup.dataBits = 8;
	gpsSetup.parity = 0;
	gpsSetup.stopBits = 1;
	gpsSetup.rxBufferAddr = gpsRxBuffer;
	gpsSetup.rxBufferSize = GPS_UART_BUFFER_SIZE;
	
  // Setup µC UART part
	UART_Init(gpsUart, gpsSetup);
	// return -- Comment in for debug intergration test GSM
  // Set GPS UART to 9600 only UBX
  uint8_t cmd_GPSUART[] = {0x01, 0x00, 0x00, 0x00, 0xD0, 0x08, 0x00, 0x00, 0x80, 0x25, 0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00};
  GPS_setup(0x06, 0x00, 20, cmd_GPSUART);
	/*
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
   */
  // Turn off Glonass
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

uint8_t GPS_ConstructMessage(uint8_t class, uint8_t ID, uint16_t length, uint8_t* payload, uint8_t* packetBuffer) {
	//Init
	uint8_t ck_a = 0;
	uint8_t ck_b = 0;
	
	// Expects PacketBuffer to be required size, while max size is max of uart output
	uint8_t msg_length = PREPAYLOAD_SIZE + length + CHECK_SIZE;
	
	if(length + 10 > MAX_TX_SIZE){
    return 0;
  }
	
	packetBuffer[2] = class;
	packetBuffer[3] = ID;
  
  if (length <= 256){
    packetBuffer[4] = length;
    packetBuffer[5] = 0x00;
  }
	
	// Fill in payload
	uint16_t i;
	for (i = 0; i < length; i++)
	{
		packetBuffer[i + PREPAYLOAD_SIZE] = payload[i];
	}
	
  // Fill in checksum and CRLF
  GPS_CalculateChecksum(msg_length, packetBuffer, &ck_a, &ck_b);
	
	packetBuffer[PREPAYLOAD_SIZE + length] = ck_a;
	packetBuffer[PREPAYLOAD_SIZE + length + 1] = ck_b;
	
	// Add Sync Chars (Done later to not include into checksum)
	packetBuffer[0] = 0xB5;
	packetBuffer[1] = 0x62;
	
	return msg_length;
}

void GPS_CalculateChecksum(uint16_t length, uint8_t* payload, uint8_t* ck_a, uint8_t* ck_b)
{
	// Calculate checksum
	uint16_t i;
  // Start at 2 to ignore B5 62 identifier
  // length - 2 to not use ck_a, ck_b
	for (i = 2; i < (length - CHECK_SIZE); i++)
	{
		*ck_a = ((*ck_a + payload[i]) & 0xff);
		*ck_b = ((*ck_b + *ck_a) & 0xff);
	}
}

uint8_t GPS_send(uint8_t class, uint8_t ID, uint16_t length, uint8_t* payload, uint8_t* answer)
{
  // Setup
  uint8_t msg[PREPAYLOAD_SIZE + length + CHECK_SIZE];
  char header[] = {0xB5, 0x62};
  int16_t startIndex;
  struct Neo7_MsgHeader* answerHeader = (void*) answer; //Set a struct at the beginning of answer to read header.
  GPS_ConstructMessage(class, ID, length, payload, msg);
  
  do
  {
    // Send
    UART_ResetRXBuffer(gpsUart);
    UART_SendBuffer(gpsUart, msg, PREPAYLOAD_SIZE + length + CHECK_SIZE);
    
    Wait(400000*2);
    // Receive
    startIndex = 0;
    startIndex = UART_ScanRXBuffer(gpsUart, header, HEADER_SIZE) - HEADER_SIZE;
    // Pop excess bytes
    if (startIndex > 0)
    {
      UART_Recieve(gpsUart, answer, startIndex);
      startIndex = 0; //Header should be at 0 now
    }
    if (startIndex == 0)
    {
      // Load the header
      UART_Recieve(gpsUart, answer, PREPAYLOAD_SIZE);
   
     // Get number of bytes in payload, according to preamble
      //inLen = (uint16_t)((answer[5] << 8) | (answer[4] & 0xFF));
      UART_Recieve(gpsUart, &answer[PREPAYLOAD_SIZE], answerHeader->lenght + CHECK_SIZE);
    }
    //FX Create Checksum test / Checksum function
    
  } while (startIndex != 0);
    
  // Return number of bytes read
  return PREPAYLOAD_SIZE + answerHeader->lenght + CHECK_SIZE;
}

uint8_t GPS_setup(uint8_t cmdClass, uint8_t cmdID, uint16_t length, uint8_t* payload)
{
  // Send data
  //uint8_t answer[GPS_ACK_LENGTH];
  union Neo7_Ack answer; //Declares a array of GPS_ACK_LENGTH

  uint8_t bytesReceived = 0;
    
  // Only check if ack/nack if correct amount of data is received.
  do
  {
    bytesReceived = GPS_send(cmdClass, cmdID, length, payload, answer.data);
    // Check if acked
  //} while ((bytesReceived != GPS_ACK_LENGTH) || !GPS_CheckAcknowledge(length, cmdClass, cmdID, answer));
  } while (GPS_CheckAcknowledge(cmdClass, cmdID, answer.data));
}

void GPS_Poll(struct GPS_data_t* data)
{   
  // Create message and send it
  uint8_t msg[GPS_POLL_MSG_LENGTH+100];
  uint8_t bytesReceived = GPS_send(0x01, 0x07, 0, 0x00, msg);
  // Setup struct for data
  data->valid = 1;

  // If data has correct length, flag for valid fix is set and , copy into struct
  if ((bytesReceived == GPS_POLL_MSG_LENGTH) && ((msg[27] & 0x01) == 0x01) && (msg[26] > 0x00))
	{
    memcpy(&data->year, &msg[10], 2);
    memcpy(&data->month, &msg[12], 1);
    memcpy(&data->date, &msg[13], 1);
    memcpy(&data->hour, &msg[14], 1);
    memcpy(&data->minute, &msg[15], 1);
    memcpy(&data->second, &msg[16], 1);
    memcpy(&data->lon, &msg[30], 4);
    memcpy(&data->lat, &msg[34], 4);
    data->valid = 0;
  }
    
  return;
}


uint8_t GPS_CheckAcknowledge(uint8_t cmdClass, uint8_t cmdID, uint8_t* buffer)
{
	// Create expected answer
  
	//uint8_t expectedData[] = {0xB5, 0x62, 0x05, 0x01, 0x02, 0x00, cmdClass, cmdID, 0x00, 0x00};
	//union Neo7_Ack* expected = (void*) expectedData;
	//
	//GPS_CalculateChecksum(GPS_ACK_LENGTH, expectedData, &expected->Neo7_Ack_T.ck_a, &expected->Neo7_Ack_T.ck_b);
	//Set stucture data
	union Neo7_Ack* received;
	received = (void*) buffer;// Set union pointer to correct address
  
	if(received->Neo7_Ack_T.acknowledge[0] != 0x05)
		return 2;	
	
	if(received->Neo7_Ack_T.cmdClass == cmdClass && received->Neo7_Ack_T.cmdID == cmdID)
	{
		
		
		if(received->Neo7_Ack_T.acknowledge[1] == 0x01)
			return 0; //Success
		else
			return 1; //Not Acknowledge
	}
	
	return 3;
}
