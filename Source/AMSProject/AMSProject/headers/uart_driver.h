/*
 * uart_driver.h
 *
 * Created: 3/9/2018 9:24:15 AM
 *  Author: Qtra
 */ 


#ifndef UART_DRIVER_H_
#define UART_DRIVER_H_
#include "sam.h"

struct uartsetup_t{
	unsigned long baudRate;
	uint8_t dataBits;
	uint8_t stopBits;
	uint8_t parity;
};

struct CTRLB_T{
	unsigned int CHSIZE : 3;
	unsigned : 3;
	unsigned int SBMODE : 1;
	unsigned : 1;
	unsigned int COLDEN : 1;
	unsigned int SFDE : 1;
	unsigned int ENC : 1;
	unsigned : 2;
	unsigned int PMODE : 1;
	unsigned : 2;
	unsigned int TXEN : 1;
	unsigned int RXEN : 1;
	unsigned : 14;
};

#include <stdint.h>

void UART_Init(unsigned long baudRate, unsigned char dataBit, uint8_t stopBits, uint8_t parity);
void UART_EnableInt();
void UART_DisableInt();
void UART_ISR();
void UART_SendBuffer(unsigned char* buffer, uint16_t size);
void UART_Recieve(uint8_t* data, uint16_t* size);

/**************************************/



#endif /* UART_DRIVER_H_ */