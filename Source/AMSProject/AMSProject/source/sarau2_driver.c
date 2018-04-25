/*
* sarau2_driver.c
*
* Created: 19-04-2018 08:13:28
*  Author: Qtra
*/

#include "SaraU2_driver.h"
#include "utility.h"
#include "hw_defines.h"


uint8_t SARAU2_Init()
{
	gsmUart.baseAddress = GSM_UART_BASE;
	gsmUart.sercom		= 2;

	struct uartsetup_t gsmSetup;

	gsmSetup.baudRate = 64281; //Hard coded value for register real 19200
	gsmSetup.dataBits = 8;
	gsmSetup.parity = 0;
	gsmSetup.stopBits = 1;
	gsmSetup.rxBufferAddr = gsmRxBuffer;
	gsmSetup.rxBufferSize = GSM_UART_BUFFER_SIZE;

	UART_Init(gsmUart, gsmSetup);
	
	SARAU2_Reset();
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
	SETREG32(GSM_RESET_PORT_BASE + PORT_OUTSET_OFFSET, PORT_PB08); //Set reset pin as high - Active high
	uint16_t i;
	for (i = 0; i < 400000; i++)
	{
		
	} // wait
	SETREG32(GSM_RESET_PORT_BASE + PORT_OUTCLR_OFFSET, PORT_PB08); //Set reset pin as low - Active high
	for (i = 0; i < 400000; i++)
	{
		
	} // wait ?
	
	//Now send something to set autobaud!
	return SARAU2_TestConnection();
}