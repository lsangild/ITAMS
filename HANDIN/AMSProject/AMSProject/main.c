/*
* AMSProject.c
*
* Created: 3/9/2018 9:21:56 AM
* Author : Qtra
*/


#include "sam.h"
#include "defines.h"
#include "sam21d18a_register.h"
#include "hw_defines.h"
#include "sw_defines.h"
#include "uart_driver.h"
#include "ringbuffer.h"
#include "utility.h"
#include <string.h>
#include "GPS_driver.h"
#include "sarau2_driver.h"
#include "Device_Startup/startup.h"

struct uart_t gpsUart; //Name for specifics
struct uart_t gsmUart;

long end = 0x20008000;

void InitPorts()
{
	//REG_PORT_DIRSET0 = PORT_PA20; //LED
	SETREG32(GSM_RESET_PORT_BASE + PORT_DIRSET_OFFSET, PORT_PB08); //Set reset pin as output
	SETREG32(GSM_RESET_PORT_BASE + PORT_OUTCLR_OFFSET, PORT_PB08); //Set reset pin as low - Active high
	
	//*(int*) To indicate that it is a position to be writen at
	//Set UART Pins for GPS
	SETREG32(SERCOM5_UART_PORT_BASE + PORT_DIRSET_OFFSET, SERCOM5_UART_PIN_TX); //Set transmit pin to output
	SETREG32(SERCOM5_UART_PORT_BASE + PORT_DIRCLR_OFFSET, SERCOM5_UART_PIN_RX); //Set recieve pin to input
	
	//Set UART Pins for GSM
	SETREG32(SERCOM2_UART_PORT_BASE + PORT_DIRSET_OFFSET, SERCOM2_UART_PIN_TX); //Set transmit pin to output
	SETREG32(SERCOM2_UART_PORT_BASE + PORT_DIRCLR_OFFSET, SERCOM2_UART_PIN_RX); //Set recieve pin to input
	
	//Set Multiplexer
	SETREG8(SERCOM5_UART_PORT_BASE + PORT_PMUX_OFFSET + SERCOM5_PORT_PMUX_OFFSET, 0x33); //Set pmu functiontion D -
	SETREG8(SERCOM5_UART_PORT_BASE + PORT_PINCFG_OFFSET + 22, PORT_PINCFG_PMUXEN);
	SETREG8(SERCOM5_UART_PORT_BASE + PORT_PINCFG_OFFSET + 23, PORT_PINCFG_PMUXEN + PORT_PINCFG_INEN);
	
	SETREG8(SERCOM2_UART_PORT_BASE + PORT_PMUX_OFFSET + SERCOM2_PORT_PMUX_OFFSET, 0x22); //Set pmu functiontion C -
	SETREG8(SERCOM2_UART_PORT_BASE + PORT_PINCFG_OFFSET + 12, PORT_PINCFG_PMUXEN);
	SETREG8(SERCOM2_UART_PORT_BASE + PORT_PINCFG_OFFSET + 13, PORT_PINCFG_PMUXEN + PORT_PINCFG_INEN);
}

void InitInterrupts()
{
	NVIC_EnableIRQ(SERCOM5_IRQn);
	NVIC_EnableIRQ(SERCOM2_IRQn);
}

uint8_t InitModules()
{
	GPS_Init();
	SARAU2_Init();
	
	while(SARAU2_OpenConnection()){}
	
	uint8_t error = SARAU2_OpenSocket();
	if(!error)
	{
		return 0;
	}
		
	return 1;
}

void LoopThrough()
{
	
	/* Replace with your application code */
	uint8_t pcData[1024];
	while (1)
	{
		uint8_t pcCount = UART_Recieve(gpsUart, pcData, 255);
		if(pcCount > 0)
		{
			if(pcData[0] == 0xAA)
			{
				SARAU2_Reset();
			}
			else if (pcData[0] == 0xBB)
			{
				UART_SendBuffer(gsmUart, (uint8_t*)"Sending Hello World\r\n", pcCount);
				
				uint8_t error = SARAU2_OpenConnection();
				if(!error)
				{
					
					error = SARAU2_OpenSocket();
					if(!error)
					{
						
						error = SARAU2_SendData("188.114.136.5", 30000, (uint8_t*)"Hello World", 11);
						if(!error)
						{
							int16_t count = SARAU2_ReadData(pcData, 32);
							if( count > 0)
								UART_SendBuffer(gsmUart, pcData, count);
						}
					}
				}
			}
			else if (pcData[0] == 0xCC)
			{
				uint8_t error = SARAU2_SendData("188.114.136.5", 30000, (uint8_t*)"Hello World", 11);
				if(!error)
				{
					int16_t count = SARAU2_ReadData(pcData, 32);
					if( count > 0)
						UART_SendBuffer(gsmUart, pcData, count);
				}
			
		}
			else
			{
				UART_SendBuffer(gsmUart, pcData, pcCount);
			}
		}
		
		Wait(400000);
		
		uint8_t emCount = UART_Recieve(gsmUart, pcData, 255);
		if(emCount > 0)
		{
			UART_SendBuffer(gpsUart, pcData, emCount);
		}
		
		Wait(400000);
	}
}

void TestGPS()
{
	struct GPS_data_t GPSdata;
	while(1)
	{
		GPS_Poll(&GPSdata);
		while (GPSdata.valid != 0){
			GPS_Poll(&GPSdata);
		}
		SARAU2_SendData("188.114.136.5", 30000, (uint8_t*) &GPSdata, sizeof(struct GPS_data_t));
		Wait(48000000);
	}
	//writeGPStoSD(GPSdata);
}

int main(void)
{
	/* Initialize the SAM system */
	SystemInit();
	
	SystemInit_Premade();
	
	InitPorts();
	
	InitInterrupts();
	
	if(!InitModules())
	{
		//LoopThrough();
		TestGPS();
	}	
}

void SERCOM5_Handler()
{
	UART_ISR(gpsUart);
}

void SERCOM2_Handler()
{
	UART_ISR(gsmUart);
}