/*
 * uart_driver.h
 *
 * Created: 3/9/2018 9:24:15 AM
 *  Author: Qtra
 */ 

#ifndef UART_DRIVER_H_
#define UART_DRIVER_H_

#define MAX_TX_SIZE 256

#include "sam.h"
#include <stdint.h>
#include "sam21d18a_register.h"
#include "ringbuffer.h"

struct uartsetup_t{
	unsigned long baudRate;
	uint8_t dataBits;
	uint8_t stopBits;
	uint8_t parity;
	uint8_t* rxBufferAddr;
	uint16_t rxBufferSize;
};

struct uart_t{
	uint32_t baseAddress;
	uint8_t sercom;
};

struct uarttransfer_t{
	uint8_t index;
	uint8_t count;
	uint8_t buffer[MAX_TX_SIZE];
	uint8_t isEmpty;	
};

struct uarttransfer_t serComTransfers[6];
ringbuffer_t serComRxBuffers[6];

void UART_Init(struct uart_t uartBase, struct uartsetup_t uartSetup);
void UART_SetupBuffer(struct uart_t uartBase, struct uartsetup_t uartSetup);
void UART_PadInit(struct uart_t uartBase);
void UART_EnableSerCom(struct uart_t uartBase);
void UART_EnableInt(struct uart_t uartBase);
void UART_ClearInt(struct uart_t uartBase);
void UART_DisableInt(struct uart_t uartBase);
void UART_ISR(struct uart_t uartBase);
uint16_t UART_ScanRXBuffer(struct uart_t serCom, uint8_t data);
uint16_t UART_Recieve(struct uart_t serCom, uint8_t* data, uint16_t count);
uint8_t UART_SendBuffer(struct uart_t serCom, uint8_t* buffer, uint16_t size);
uint8_t UART_ResetRXBuffer(struct uart_t serCom);

/**************************************/



#endif /* UART_DRIVER_H_ */