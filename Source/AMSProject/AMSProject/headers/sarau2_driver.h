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
uint8_t gsmRxBuffer[GPS_UART_BUFFER_SIZE];

uint8_t SARAU2_Init();
uint8_t SARAU2_TestConnection();
uint8_t SARAU2_SetupProfile();
uint8_t SARAU2_OpenConnection();
uint8_t SARAU2_Reset();



#endif /* SARAU2_DRIVER_H_ */