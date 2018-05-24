/*
 * sarau2_driver.h
 *
 * Created: 19-04-2018 08:13:03
 *  Author: Qtra
 */ 


#ifndef SARAU2_DRIVER_H_
#define SARAU2_DRIVER_H_

#include "uart_driver.h"
#include "sw_defines.h"

extern struct uart_t gsmUart;
uint8_t gsmRxBuffer[GSM_UART_BUFFER_SIZE];
uint8_t gsmResponseBuffer[GSM_UART_BUFFER_SIZE]; //Use as container for data from ringbuffer, when running through it.
uint8_t cmdData[GSM_UART_BUFFER_SIZE]; //Use to combine send data
uint8_t socketID;

uint8_t SARAU2_Init();
uint8_t SARAU2_CREG();
uint8_t SARAU2_SetupProfile();
uint8_t SARAU2_OpenConnection();
uint8_t SARAU2_OpenSocket();
uint8_t SARAU2_SendData(char* ip, uint16_t port, uint8_t* data, uint16_t length);
uint16_t SARAU2_ReadData(uint8_t* data, uint16_t readCount);
uint8_t SARAU2_SetBaudRate();
uint8_t SARAU2_SendCmd(struct uart_t serCom, uint8_t* buffer, uint16_t size);
uint8_t SARAU2_Reset();
uint8_t SARA2_CheckOK(uint8_t* cmd, uint8_t cmdLength);
uint8_t SARA2_CheckOKReturnMsg(uint8_t* cmd, uint8_t cmdLength, uint8_t* response, uint16_t* responseLength);



#endif /* SARAU2_DRIVER_H_ */