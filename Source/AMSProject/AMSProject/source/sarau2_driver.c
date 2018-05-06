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

	gsmSetup.baudRate = 64307; //Hardcoded balue for 9600 baud - 64281; //Hard coded value for register real 19200
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
	UART_ResetRXBuffer(gsmUart);
	
	uint8_t* msg = (uint8_t*)"AT+CCID?\r\n";
	
	UART_SendBuffer(gsmUart, msg, 13);
	
	//uint16_t countToBreak = 0;
	//while (countToBreak == 0)
	//{
		//countToBreak = UART_ScanRXBuffer(gsmUart, '\n');
		////Error! Create timeout!
	//}
	//uint8_t input[countToBreak];
	//UART_Recieve(gsmUart, input, countToBreak);
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

uint8_t SARAU2_SetBaudRate()
{
	uint8_t* msg = "AT+IPR=9600\r\n";
	
	UART_SendBuffer(gsmUart, msg, 14);
	uint16_t i;
}

uint8_t SARAU2_Reset()
{
	SETREG32(GSM_RESET_PORT_BASE + PORT_OUTSET_OFFSET, PORT_PB08); //Set reset pin as high - Active high
	uint32_t i;
	for (i = 0; i < 40000; i++)
	{
		
	} // wait
	SETREG32(GSM_RESET_PORT_BASE + PORT_OUTCLR_OFFSET, PORT_PB08); //Set reset pin as low - Active high
	for (i = 0; i < 400000; i++)
	{
		
	} // wait ?
	
	//Now set baudrate to 19200
	SARAU2_SetBaudRate();
	
	return 0;
}

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
		dataLength = UART_ScanRXBuffer(gsmUart, data);
		if(dataLength == 0)
			return 2;
		//Test for cmd?
		if(ScanString(data, dataLength, "OK", 2))
			return 0;
		else if(ScanString(data, dataLength, "ERROR", 5))
			return 1;
		
	} while (1);
}