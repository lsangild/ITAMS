/*
* sarau2_driver.c
*
* Created: 19-04-2018 08:13:28
*  Author: Qtra
*/

#include "SaraU2_driver.h"
#include "utility.h"
#include "hw_defines.h"

extern struct uart_t gpsUart;

uint8_t SARAU2_Init()
{
	gsmUart.baseAddress = GSM_UART_BASE;
	gsmUart.sercom		= 2;

	struct uartsetup_t gsmSetup;

	gsmSetup.baudRate = 9600; //Hardcoded balue for 9600 baud - 64281; //Hard coded value for register real 19200
	gsmSetup.dataBits = 8;
	gsmSetup.parity = 0;
	gsmSetup.stopBits = 1;
	gsmSetup.rxBufferAddr = gsmRxBuffer;
	gsmSetup.rxBufferSize = GSM_UART_BUFFER_SIZE;

	UART_Init(gsmUart, gsmSetup);
	
	SARAU2_Reset();
	
	Wait(400000);
	
	uint8_t regStatus = SARAU2_CREG();
		
	return 0;
}

uint8_t SARAU2_CREG()
{	
	uint8_t* msg = (uint8_t*)"AT+CREG?\r\n";
	
	SARAU2_SendCmd(gsmUart, msg, 10);
	
	while(1)
	{
		Wait(400000);
		uint16_t rspLength;
		uint8_t rsp = SARA2_CheckOKReturnMsg(msg, 10, gsmResponseBuffer, &rspLength);
		
		if (rsp == 0) //CREG responded OK
		{
			int16_t cregIndex = IndexOfString(gsmResponseBuffer, rspLength, (uint8_t*)"+CREG: 0,", 9);
			
			UART_SendBuffer(gpsUart, gsmResponseBuffer, rspLength);
			if(cregIndex > 0)
			{
				uint8_t regStatus = gsmResponseBuffer[cregIndex+9];
				switch (regStatus)
				{
					case '0': // Error on registration
						return 2;
					case '1': //Registered home network. This should be correct status
						return 0;
					case '2': //Searching for network. Idle state
						break;
					case '3': //Registration denied. Not enough signal?
						return 1;
					case '4': //Unknown state
						return 3;
					case '5': //Registered roaming network. This should be correct status
						return 0;
					default: // SMS only and other states. Keep checking
						break;					
				}
			}
		}
		else if (rsp == 1) //Should not be able to return this
		{
			SARAU2_SendCmd(gsmUart, msg, 10);
			
		}
		else if (rsp == 2)
		{
			SARAU2_SendCmd(gsmUart, msg, 10);
		}
	}
	
	return 0;
}

uint8_t SARAU2_SetupProfile()
{
	uint8_t* cmd = (uint8_t*)"AT+CGDCONT=1,\"IP\",\"internet\"\r\n";
	uint8_t errorStatus = SARAU2_SendCmd(gsmUart, cmd, 11);
	if(errorStatus)//Error happened! Could not send!
		return errorStatus;
		
	Wait(400000);
	
	errorStatus = SARA2_CheckOK(cmd, 11);
	if(errorStatus)//Error happened! Could not attach to GPRS service
		return errorStatus;
		
	return 0;
}

uint8_t SARAU2_OpenConnection()
{
	uint8_t errorStatus = SARAU2_CREG();
	if(errorStatus)//Error happened! Not registered
		return 1;
	
	uint8_t* cmd = (uint8_t*)"AT+CGATT?\r\n";	
	errorStatus = SARAU2_SendCmd(gsmUart, cmd, 11);	
	if(errorStatus)//Error happened! Could not send!
		return 1;
	
	Wait(400000);
	
	errorStatus = SARA2_CheckOK(cmd, 11);	
	if(errorStatus)//Error happened! Could not attach to GPRS service
		return 1;
		
	errorStatus = SARAU2_SetupProfile();
	if(errorStatus)//Error happened! Could not setup profile
		return 1;
	
	cmd = (uint8_t*)"AT+CGACT=1,1\r\n";
	errorStatus = SARAU2_SendCmd(gsmUart, cmd, 14);
	if(errorStatus)//Error happened! Could not send!
		return 1;
	
	Wait(400000);
	
	errorStatus = SARA2_CheckOK(cmd, 11);
	if(errorStatus)//Error happened! Could not attach to GPRS service
		return 1;
	
	return 0;
}

uint8_t SARAU2_SetBaudRate()
{
	uint8_t* msg = (uint8_t*)"AT+IPR=9600\r\n";
	
	UART_SendBuffer(gsmUart, msg, 14);
	
	Wait(400000);
	
	return SARA2_CheckOK(msg, 14);
}

uint8_t SARAU2_Reset()
{
	SETREG32(GSM_RESET_PORT_BASE + PORT_OUTSET_OFFSET, PORT_PB08); //Set reset pin as high - Active high
	
	Wait(400000);
	
	SETREG32(GSM_RESET_PORT_BASE + PORT_OUTCLR_OFFSET, PORT_PB08); //Set reset pin as low - Active high
	
	Wait(400000);
	
	//Now set baudrate to 19200
	SARAU2_SetBaudRate();
	
	return 0;
}

//////////////////////////////////////////////////////////////////////////
// Scan RX buffer for OK or ERROR.
//////////////////////////////////////////////////////////////////////////
// Error codes
// 0 - success
// 1 - error
// 2 - Not recognized
uint8_t SARA2_CheckOK(uint8_t* cmd, uint8_t cmdLength)
{
	uint8_t data[256];
	uint16_t dataLength = 0;
	do
	{
		dataLength = UART_ScanRXBuffer(gsmUart, '\n');
		if(dataLength == 0)
			return 2;
		//Test for cmd?
		dataLength = UART_Recieve(gsmUart, data, dataLength);
		
		if(ScanString(data, dataLength, (uint8_t*)"OK", 2))
			return 0;
		else if(ScanString(data, dataLength, (uint8_t*)"ERROR", 5))
			return 1;
		
	} while (1);
}

//////////////////////////////////////////////////////////////////////////
// Scan RX buffer for OK or ERROR and return the response before status.
// FIXME Potential for cutting msg in half
//////////////////////////////////////////////////////////////////////////
// Error codes
// 0 - success
// 1 - error
// 2 - Not recognized
// 3 - Error Out of index
uint8_t SARA2_CheckOKReturnMsg(uint8_t* cmd, uint8_t cmdLength, uint8_t* response, uint16_t* responseLength)
{
	uint16_t dataOffset = 0;
	uint16_t dataLength = 0;
	do
	{
		dataLength = UART_ScanRXBuffer(gsmUart, '\n'); //Insert response line into output
		
		if(dataLength == 0) //No Data
		{
			*responseLength = dataOffset + dataLength;
			return 2;
		}
		
		if( dataOffset + dataLength > GSM_UART_BUFFER_SIZE) // ERROR out of index!
			return 3;
			
		dataLength = UART_Recieve(gsmUart, &response[dataOffset], dataLength);
				
		if(ScanString(&response[dataOffset], dataLength, (uint8_t*)"OK", 2))
		{
			*responseLength = dataOffset + dataLength;
			return 0;
		}
		else if(ScanString(&response[dataOffset], dataLength, (uint8_t*)"ERROR", 5))
		{
			*responseLength = dataOffset + dataLength;
			return 1;
		}			
		
		dataOffset += dataLength;
		
		if( dataOffset > GSM_UART_BUFFER_SIZE) // ERROR out of index!
			return 3; 
		
	} while (1);
}

//////////////////////////////////////////////////////////////////////////
//Send buffer, but reset rx first
//////////////////////////////////////////////////////////////////////////
uint8_t SARAU2_SendCmd(struct uart_t serCom, uint8_t* buffer, uint16_t size)
{
	UART_ResetRXBuffer(serCom);
	return UART_SendBuffer(serCom, buffer, size);	
}