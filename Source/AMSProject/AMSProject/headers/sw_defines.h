/*
 * sw_defines.h
 *
 * Created: 4/2/2018 5:56:56 PM
 *  Author: Qtra
 */ 


#ifndef SW_DEFINES_H_
#define SW_DEFINES_H_


//GPS Defines
#define GPS_UART_BASE				0x42001C00 //Set to ctrl a reg
#define GPS_UART_BUFFER_SIZE		256
#define GPS_POLL_MSG_LENGTH			90
#define GPS_ACK_LENGTH              12

//GSM Defines
#define GSM_UART_BASE				0x42001000 //Set to ctrl a reg
#define GSM_UART_BUFFER_SIZE		1024
#define GSM_AT_START				"AT"
#define GSM_AT_START_LENGTH			2

#define UART_INTVECTOR_DRE				1
#define UART_INTVECTOR_TXC				2
#define UART_INTVECTOR_RXC				4
#define UART_INTVECTOR_RXS				8




#endif /* SW_DEFINES_H_ */
