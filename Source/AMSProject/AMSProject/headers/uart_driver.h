/*
 * uart_driver.h
 *
 * Created: 3/9/2018 9:24:15 AM
 *  Author: Qtra
 */ 


#ifndef UART_DRIVER_H_
#define UART_DRIVER_H_
#include "sam.h"
#include <stdint.h>
#include "sam21d18a_register.h"

struct uartsetup_t{
	unsigned long baudRate;
	uint8_t dataBits;
	uint8_t stopBits;
	uint8_t parity;
};

struct UART_t{
	int* baseAddress;
};

void UART_Init(unsigned long baudRate, unsigned char dataBit, uint8_t stopBits, uint8_t parity);
void UART_EnableInt();
void UART_DisableInt();
void UART_ISR();
void UART_SendBuffer(struct UART_t serCom, unsigned char* buffer, uint16_t size);
void UART_Recieve(struct UART_t serCom, uint8_t* data, uint16_t* size);

/**************************************/



#endif /* UART_DRIVER_H_ */