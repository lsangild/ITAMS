/*
 * uart_driver.c
 *
 * Created: 3/9/2018 9:24:24 AM
 *  Author: Qtra
 */ 

#include "uart_driver.h"

/**************************************************
* "uart.c":                                       *
* Implementation file for the Mega32 UART driver. *
*  STK500 setup:                                  *
*  Header "RS232 spare" connected to RXD/TXD:     *
*  RXD = PORTD, bit0                              *
*  TXD = PORTD, bit1                              *
*                                                 *
* Henning Hargaard, 1/11 2011                     *
***************************************************/
#include "uart_driver.h"
#include "defines.h"

void UART_Init(unsigned long baudRate, uint8_t dataBit, uint8_t stopBits, uint8_t parity)
{
	//Set Clock power !!!!!!
	
	uint16_t tmpBaudRate = 65536*(1-8*(baudRate/F_CPU));
	REG_SERCOM5_USART_BAUD = tmpBaudRate;
	
	struct SERUSART_CTRLB_T tmpCtrB;	
	tmpCtrB.TXEN = 1; //TX enable
	tmpCtrB.RXEN = 1; //RX enable
	tmpCtrB.PMODE = parity; //Parity 0 = equal, 1 = odd
	tmpCtrB.SBMODE = stopBits-1; //Stop bits 0 = 1, 1 = 2
	tmpCtrB.CHSIZE = (dataBit > 7) ? (dataBit == 9 ? 1 : 0) : (dataBit > 4) ? dataBit : 5; // If databit is 8 | 9 set to 0 or 1. Else if 5-7 set to that.
	
	REG_SERCOM5_USART_CTRLB |= *(int*)(void*)&tmpCtrB; 
}

void UART_EnableInt()
{
	
}

void UART_DisableInt()
{
	
}

void UART_ISR()
{
	
}

void UART_SendBuffer(struct UART_t serCom, uint8_t* buffer, uint16_t size)
{
	
}

void UART_Recieve(struct UART_t serCom, uint8_t* data, uint16_t* size)
{
	
}


///*************************************************************************
//USART initialization.
    //Asynchronous mode.
	//RX and TX enabled.
	//No interrupts enabled.
	//Number of Stop Bits = 1.
	//No Parity.
	//Baud rate = Parameter.
	//Data bits = Parameter.
//Parameters:
	//BaudRate: Wanted Baud Rate.
	//Databits: Wanted number of Data Bits.
//*************************************************************************/
//void InitUART(unsigned long BaudRate, unsigned char DataBit)
//{
//unsigned int TempUBRR;
//
  //if ((BaudRate >= 110) && (BaudRate <= 115200) && (DataBit >=5) && (DataBit <= 8))
  //{ 
    //// "Normal" clock, no multiprocessor mode (= default)
    //UCSRA = 0b00100000;
    //// No interrupts enabled
    //// Receiver enabled
    //// Transmitter enabled
    //// No 9 bit operation
    //UCSRB = 0b00011000;	
    //// Asynchronous operation, 1 stop bit, no parity
    //// Bit7 always has to be 1
    //// Bit 2 and bit 1 controls the number of databits
    //UCSRC = 0b10000000 | (DataBit-5)<<1;
    //// Set Baud Rate according to the parameter BaudRate:
    //// Select Baud Rate (first store "UBRRH--UBRRL" in local 16-bit variable,
    ////                   then write the two 8-bit registers separately):
    //TempUBRR = XTAL/(16*BaudRate) - 1;
    //// Write upper part of UBRR
    //UBRRH = TempUBRR >> 8;
    //// Write lower part of UBRR
    //UBRRL = TempUBRR;
  //}  
//}

