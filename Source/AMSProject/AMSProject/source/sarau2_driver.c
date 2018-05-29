/*
* sarau2_driver.c
*
* Created: 19-04-2018 08:13:28
*  Author: Qtra
*/

#include "SaraU2_driver.h"
#include "utility.h"
#include "hw_defines.h"
#include <string.h>
#include "stdio.h"
#include "stdlib.h"

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

uint8_t SARAU2_SetupExternalContext()
{
	uint8_t* cmd = (uint8_t*)"AT+CGDCONT=1,\"IP\",\"www.internet.metelia.dk\"\r\n";
	uint8_t errorStatus = SARAU2_SendCmd(gsmUart, cmd, 45);
	if(errorStatus)//Error happened! Could not send!
		return errorStatus;
		
	Wait(4000000);
	
	errorStatus = SARA2_CheckOK(cmd, 45);
	if(errorStatus)//Error happened! Could not attach to GPRS service
		return errorStatus;
		
	return 0;
}

uint8_t SARAU2_SetupInternalContext()
{
	uint8_t* cmd = (uint8_t*)"AT+UPSD=0,1,\"www.internet.metelia.dk\"\r\n";
	uint8_t errorStatus = SARAU2_SendCmd(gsmUart, cmd, 39);
	if(errorStatus)//Error happened! Could not send!
		return errorStatus;
	
	Wait(4000000);
	
	errorStatus = SARA2_CheckOK(cmd, 39);
	if(errorStatus)//Error happened! Could not attach to GPRS service
		return errorStatus;
	
	cmd = (uint8_t*)"AT+UPSDA=0,3\r\n";
	errorStatus = SARAU2_SendCmd(gsmUart, cmd, 14);
	if(errorStatus)//Error happened! Could not send!
		return errorStatus;
		
	Wait(4000000);
	
	uint8_t loopCount = 10;
	do
	{
		Wait(40000000);
		
		errorStatus = SARA2_CheckOK(cmd, 11);
		if(!errorStatus)//Error happened! Could not attach to GPRS service
			break;
		
		loopCount--;
	} while (loopCount!= 0);
	
	if(errorStatus)//Error happened! Could not attach to GPRS service
		return errorStatus;
	
	return 0;
}

uint8_t SARAU2_OpenConnection()
{
	uint8_t errorStatus = SARAU2_CREG();
	if(errorStatus)//Error happened! Not registered
		return 1;
	
	uint8_t* cmd = (uint8_t*)"AT+CGATT=1\r\n";	
	errorStatus = SARAU2_SendCmd(gsmUart, cmd, 11);	
	if(errorStatus)//Error happened! Could not send!
		return 1;
	
	Wait(4000000);
	
	errorStatus = SARA2_CheckOK(cmd, 11);	
	if(errorStatus)//Error happened! Could not attach to GPRS service
		return 1;
		
	errorStatus = SARAU2_SetupExternalContext();
	if(errorStatus)//Error happened! Could not setup profile
		return 1;
	
	cmd = (uint8_t*)"AT+CGACT=1,1\r\n";
	errorStatus = SARAU2_SendCmd(gsmUart, cmd, 14);
	if(errorStatus)//Error happened! Could not send!
		return 1;
	
	uint8_t loopCount = 10;
	do 
	{
		Wait(40000000);
		
		errorStatus = SARA2_CheckOK(cmd, 11);
		if(!errorStatus)//Error happened! Could not attach to GPRS service
			break;	
			
		loopCount--;
	} while (loopCount!= 0);
	
	if(errorStatus)//Error happened! Could not attach to GPRS service
		return 1;
	
	errorStatus = SARAU2_SetupInternalContext();
	if(errorStatus)//Error happened! Could not setup profile
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
		dataLength = UART_ScanRXBuffer(gsmUart, '\n', 1);
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
		dataLength = UART_ScanRXBuffer(gsmUart, '\n', 1); //Insert response line into output
		
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

uint8_t SARAU2_OpenSocket()
{
	uint8_t cmd[13] = "AT+USOCR=17\r\n";
	SARAU2_SendCmd(gsmUart, cmd, 13);
	uint16_t rspLength;
	
	Wait(400000);
	
	uint8_t rsp = SARA2_CheckOKReturnMsg(cmd, 10, gsmResponseBuffer, &rspLength);
	if(rsp == 0)
	{
		int16_t cregIndex = IndexOfString(gsmResponseBuffer, rspLength, (uint8_t*)"+USOCR: ", 8);
		if(cregIndex > 0)
			socketID = AsciiNumToInt(gsmResponseBuffer[cregIndex+8]);		
		else
			return 1;
	}
	else
	{
		return 1;
	}
	return 0;
}

uint8_t SARAU2_SendData(char* ip, uint16_t port, uint8_t* data, uint16_t length)
{
	uint8_t cmdData[GSM_SEND_DATA_REQ_LENGTH];
	uint16_t cmdLength = sprintf((char*)cmdData,"AT+USOST=%d,\"%s\",%d,%d\r\n", socketID, ip, port, length);
	
	uint8_t index = 0;
	uint8_t loopcount = 5;
	uint8_t errorCode = 0;
	
	SARAU2_SendCmd(gsmUart, cmdData, cmdLength);
	SARAU2_SendCmd(gpsUart, cmdData, cmdLength);
	
	do 
	{
		Wait(400000);
		index = UART_ScanRXBuffer(gsmUart, '@');
		if(index > 0)
			break;
			
		errorCode = SARA2_CheckOK(cmdData,8);
		if(errorCode != 2)
			return errorCode;
		
		loopcount--;
		if (loopcount == 0)
			return 3;
	} while (1);
	
	
	SARAU2_SendCmd(gsmUart, data, length);
	
	Wait(4000000);
	
	errorCode = SARA2_CheckOK(cmdData,8);
	
	if (errorCode)
		return errorCode;
	else 
		return 0;
}	


int16_t SARAU2_ReadData(uint8_t* data, uint16_t readCount)
{
	uint8_t cmdData[18];
	
	uint16_t cmdLength = sprintf((char*)cmdData,"AT+USORF=%d,%d\r\n", socketID,readCount);
	
	SARAU2_SendCmd(gsmUart, cmdData, cmdLength);
	
	Wait(400000);
		
	uint16_t rspLength = UART_Recieve(gsmUart, gsmResponseBuffer, GSM_UART_BUFFER_SIZE); // GSM_UART_BUFFER_SIZE Can't scan for footer, so get all data in buffer
	
	// FX May give issues! Can't test if returned data is OK or ERROR, That data can be in data before footer
	if(rspLength != 0)
	{
		int16_t lengthIndex = IndexOfNthString(gsmResponseBuffer, rspLength, (uint8_t*)",", 1, 4); 
		int16_t dataIndex = IndexOfNthString(gsmResponseBuffer, rspLength, (uint8_t*)",", 1, 5); 
		if(dataIndex > 0 && lengthIndex > 0)
		{
			lengthIndex +=1; //Offset after ,
			dataIndex +=2; //Offset after , +
			uint8_t lengthSize = dataIndex-lengthIndex-2;
			char lengthStr[lengthSize+1];
			lengthStr[lengthSize-1] = 0;
				
			memcpy(lengthStr, &gsmResponseBuffer[lengthIndex], lengthSize);
			
			uint16_t dataLength = atoi(lengthStr);
			memcpy(data, &gsmResponseBuffer[dataIndex], dataLength);
			
			return dataLength;
		}
		else
		{
			return -2;
		}	
	}
	else
	{
		return -1;
	}
}
