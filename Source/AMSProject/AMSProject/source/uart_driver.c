/*
* uart_driver.c
*
* Created: 3/9/2018 9:24:24 AM
*  Author: Qtra
*/

#include "uart_driver.h"
#include "defines.h"
#include "utility.h"
#include <string.h>

void UART_Init(struct uart_t uartBase, struct uartsetup_t uartSetup)
{
	REG_PM_APBCMASK |= 1 << (2 + uartBase.sercom);// Shift Clock power bit in
	
	uint16_t tmpBaudRate = 65536*(1-8*((double)uartSetup.baudRate/F_CPU));
	SETREG16(uartBase.baseAddress + SERCOM_USART_BAUD_OFFSET, tmpBaudRate); //Not sure the calc is correct
	
	struct SERUSART_CTRLA_T tmpCtrA;
	
	*(int*)((void*)&tmpCtrA) = REG_SERCOM5_I2CM_CTRLA;
	
	tmpCtrA.FORM = 0;
	tmpCtrA.CMODE = 1;
	tmpCtrA.ENABLE = 1;
	
	SETREG32(uartBase.baseAddress + SERCOM_USART_CTRLA_OFFSET, *(int*)((void*)&tmpCtrA));
	
	struct SERUSART_CTRLB_T tmpCtrB;
	*(int*)((void*)&tmpCtrB) = REG_SERCOM5_I2CM_CTRLA;
	
	tmpCtrB.TXEN = 1; //TX enable
	tmpCtrB.RXEN = 1; //RX enable
	tmpCtrB.PMODE = uartSetup.parity; //Parity 0 = equal, 1 = odd
	tmpCtrB.SBMODE = uartSetup.stopBits-1; //Stop bits 0 = 1, 1 = 2
	tmpCtrB.CHSIZE = (uartSetup.dataBits > 7) ? (uartSetup.dataBits == 9 ? 1 : 0) : (uartSetup.dataBits > 4) ? uartSetup.dataBits : 5; // If databit is 8 | 9 set to 0 or 1. Else if 5-7 set to that.
	
	SETREG32(uartBase.baseAddress + SERCOM_USART_CTRLB_OFFSET, *(int*)((void*)&tmpCtrB));
}

void UART_EnableInt(struct uart_t uartBase)
{
	SETREG8(uartBase.baseAddress + SERCOM_USART_INTENSET_OFFSET, 0x0f);
}

void UART_DisableInt(struct uart_t uartBase)
{
	SETREG8(uartBase.baseAddress + SERCOM_USART_INTENCLR_OFFSET, 0x0f);
}

void UART_ISR(struct uart_t uartBase, ringbuffer_t rxBuffer)
{
	
}

uint8_t UART_SendBuffer(struct uart_t serCom, uint8_t* buffer, uint16_t size)
{
	if(serComTransfers[serCom.sercom].isEmpty && size <= MAX_TX_SIZE)
	{
		serComTransfers[serCom.sercom].index = 0;
		serComTransfers[serCom.sercom].count = size;
		memcpy(serComTransfers[serCom.sercom].buffer, buffer, size);
		serComTransfers[serCom.sercom].isEmpty = 0;		
		return 1;
	}
	return 0;
}
