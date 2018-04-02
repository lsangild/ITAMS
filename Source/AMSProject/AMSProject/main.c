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

struct uart_t gpsUart; //Name for specifics

void InitPorts()
{
	REG_PORT_DIRSET0 = PORT_PA20;
	
	//*(int*) To indicate that it is a position to be writen at
	//Set UART Pins for GPS
	*(int*)(SERCOM5_UART_PORT_BASE + PORT_DIRSET_OFFSET) = SERCOM5_UART_PIN_TX; //Set transmit pin to output
	*(int*)(SERCOM5_UART_PORT_BASE + PORT_DIRCLR_OFFSET) = SERCOM5_UART_PIN_RX; //Set recieve pin to input
	
	//Set UART Pins for GSM
	*(int*)(SERCOM2_UART_PORT_BASE + PORT_DIRSET_OFFSET) = SERCOM2_UART_PIN_TX; //Set transmit pin to output
	*(int*)(SERCOM2_UART_PORT_BASE + PORT_DIRCLR_OFFSET) = SERCOM2_UART_PIN_RX; //Set recieve pin to input
		
	
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
		
	gpsUart.baseAddress = GPS_UART_Base;
	gpsUart.sercom		= 5;
	
	struct uartsetup_t gpsSetup;
	
	gpsSetup.baudRate = 9600;
	gpsSetup.dataBits = 8;
	gpsSetup.parity = 0;
	gpsSetup.stopBits = 1;
	
	UART_Init(gpsUart, gpsSetup);
		
    /* Replace with your application code */
    while (1) 
    {		
		REG_PORT_OUTSET0 =  PORT_PA20;
		
		REG_PORT_OUTCLR0 = PORT_PA20;
    }
}

void SERCOM5_Handler()
{
	
}

void SERCOM2_Handler()
{
	
}