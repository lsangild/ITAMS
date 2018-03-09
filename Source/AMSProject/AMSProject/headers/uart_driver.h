/*
 * uart_driver.h
 *
 * Created: 3/9/2018 9:24:15 AM
 *  Author: Qtra
 */ 


#ifndef UART_DRIVER_H_
#define UART_DRIVER_H_

void UART_Init(unsigned long baudRate, unsigned char dataBit, uint8_t stopBits, uint8_t parity);
void UART_EnableInt();
void UART_DisableInt();
void UART_ISR();
void UART_SendBuffer(uint8_t* buffer, uint16_t size);
void UART_Recieve(uint8_t* data, uint16_t* size);

/**************************************/



#endif /* UART_DRIVER_H_ */