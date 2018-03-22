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

int main(void)
{
    /* Initialize the SAM system */
    SystemInit();
	
	InitPorts();
	
	struct SERUSART_CTRLB_T controlB;
	
	*(int*)((void*)&controlB) = REG_SERCOM5_USART_CTRLB;
	controlB.CHSIZE = 5;
	controlB.TXEN = 1;
	controlB.RXEN = 1;
	
	REG_SERCOM5_USART_CTRLB = *(int*)((void*)&controlB);
	
	struct SERUSART_CTRLA_T controlA;
	
	*(int*)((void*)&controlA) = REG_SERCOM5_USART_CTRLA;

	controlA.CMODE = 1;
	controlA.ENABLE = 1;
	
	REG_SERCOM5_USART_CTRLA = *(int*)((void*)&controlA);
	
    /* Replace with your application code */
    while (1) 
    {		
		REG_PORT_OUTSET0 =  PORT_PA20;		
		
		REG_PORT_OUTCLR0 = PORT_PA20;
    }
}