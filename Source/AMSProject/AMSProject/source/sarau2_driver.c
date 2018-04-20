/*
* sarau2_driver.c
*
* Created: 19-04-2018 08:13:28
*  Author: Qtra
*/

#include "SaraU2_driver.h"


uint8_t SARAU2_Init()
{
	gsmUart.baseAddress = GSM_UART_BASE;
	gsmUart.sercom		= 2;

	struct uartsetup_t gsmSetup;

	gsmSetup.baudRate = 19200;
	gsmSetup.dataBits = 8;
	gsmSetup.parity = 0;
	gsmSetup.stopBits = 1;
	gsmSetup.rxBufferAddr = gsmRxBuffer;
	gsmSetup.rxBufferSize = GSM_UART_BUFFER_SIZE;

	UART_Init(gsmUart, gsmSetup);
	return 0;
}

uint8_t SARAU2_TestConnection()
{
	char* msg = "AT+USIMSTAT?\r";
	UART_SendBuffer(gsmUart, msg, 13);
	
	uint16_t countToBreak = 0;
	while (countToBreak == 0)
	{
		countToBreak = UART_ScanRXBuffer(gsmUart, '\n');
	}
	uint8_t input[countToBreak];
	UART_Recieve(gsmUart, input, countToBreak);
	return 0;
}

uint8_t SARAU2_SetupProfile()
{
	return 0;
}

uint8_t SARAU2_OpenConnection()
{
	return 0;
}

uint8_t SARAU2_Reset()
{
	
}