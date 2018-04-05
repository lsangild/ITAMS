/*
 * sw_defines.h
 *
 * Created: 4/2/2018 5:56:56 PM
 *  Author: Qtra
 */ 


#ifndef SW_DEFINES_H_
#define SW_DEFINES_H_

#define GPS_UART_Base				0x42001C00 //Set to ctrl a reg
#define GPS_UART_BUFFER_SIZE		256

#define GSM_UART_Base				0x42001000 //Set to ctrl a reg

#define UART_INTVECTOR_DRE				1
#define UART_INTVECTOR_TXC				2
#define UART_INTVECTOR_RXC				4
#define UART_INTVECTOR_RXS				8




#endif /* SW_DEFINES_H_ */