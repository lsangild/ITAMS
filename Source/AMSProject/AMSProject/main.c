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

Sercom *test = SERCOM2;

struct uart_t gpsUart; //Name for specifics

uint8_t gpsRxBuffer[GPS_UART_BUFFER_SIZE];


void InitPorts()
{
	REG_PORT_DIRSET0 = PORT_PA20;
	
	//*(int*) To indicate that it is a position to be writen at
	//Set UART Pins for GPS
	SETREG32(SERCOM5_UART_PORT_BASE + PORT_DIRSET_OFFSET, SERCOM5_UART_PIN_TX); //Set transmit pin to output
	SETREG32(SERCOM5_UART_PORT_BASE + PORT_DIRCLR_OFFSET, SERCOM5_UART_PIN_RX); //Set recieve pin to input
	
	//Set UART Pins for GSM
	SETREG32(SERCOM2_UART_PORT_BASE + PORT_DIRSET_OFFSET, SERCOM2_UART_PIN_TX); //Set transmit pin to output
	SETREG32(SERCOM2_UART_PORT_BASE + PORT_DIRCLR_OFFSET, SERCOM2_UART_PIN_RX); //Set recieve pin to input
		
	//Set Multiplexer
	SETREG8(SERCOM2_UART_PORT_BASE + PORT_PMUX_OFFSET + SERCOM2_PORT_PMUX_OFFSET, 0x22); //Set pmu functiontion C - 
	SETREG8(SERCOM2_UART_PORT_BASE + PORT_PINCFG_OFFSET + 12, PORT_PINCFG_PMUXEN);
	SETREG8(SERCOM2_UART_PORT_BASE + PORT_PINCFG_OFFSET + 13, PORT_PINCFG_PMUXEN + PORT_PINCFG_INEN);
	
	SETREG8(SERCOM5_UART_PORT_BASE + PORT_PMUX_OFFSET + SERCOM5_PORT_PMUX_OFFSET, 0x33); //Set pmu functiontion D -
	SETREG8(SERCOM5_UART_PORT_BASE + PORT_PINCFG_OFFSET + 22, PORT_PINCFG_PMUXEN);
	SETREG8(SERCOM5_UART_PORT_BASE + PORT_PINCFG_OFFSET + 23, PORT_PINCFG_PMUXEN + PORT_PINCFG_INEN);
}

void InitInterrupts()
{
	NVIC_EnableIRQ(SERCOM2_IRQn);
	NVIC_EnableIRQ(SERCOM5_IRQn);		
}

int main(void)
{
    /* Initialize the SAM system */
    SystemInit();
	
	InitPorts();	
		
	InitInterrupts();
	
	gpsUart.baseAddress = GPS_UART_Base;
	gpsUart.sercom		= 5;
	
	struct uartsetup_t gpsSetup;
	
	gpsSetup.baudRate = 19200;
	gpsSetup.dataBits = 8;
	gpsSetup.parity = 0;
	gpsSetup.stopBits = 1;
	gpsSetup.rxBufferAddr = gpsRxBuffer;
	gpsSetup.rxBufferSize = GPS_UART_BUFFER_SIZE;
		
	UART_Init(gpsUart, gpsSetup);

	uint8_t buffer[] = {"Hello World"};
	
	UART_SendBuffer(gpsUart, buffer, 11);
	
    /* Replace with your application code */
    while (1) 
    {		
		REG_PORT_OUTTGL0 =  PORT_PA20;		
		
		//UART_SendBuffer(gpsUart, buffer, 11);
    }
}

void SERCOM5_Handler()
{
	UART_ISR(gpsUart);
}

void SERCOM2_Handler()
{
	
}