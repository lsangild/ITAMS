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

extern struct uart_t gsmUart;
extern struct uart_t gpsUart;

uint8_t UART_CheckBase(struct uart_t uartBase)
{
	if(uartBase.baseAddress & 0xff)
		return 1;
	return 0;
}

void UART_Init(struct uart_t uartBase, struct uartsetup_t uartSetup)
{	
	//Setup clock
	REG_PM_APBCMASK |= 1 << (2 + uartBase.sercom);// Shift Clock power bit in
	GCLK->CLKCTRL.reg = GCLK_CLKCTRL_ID(0x14+uartBase.sercom) | GCLK_CLKCTRL_GEN_GCLK0 | GCLK_CLKCTRL_CLKEN; //Select clock, and enable
	//Write more elegant! Enables the specific clock, uses same reg for all generics
	
	UART_PadInit(uartBase);	
	
	//Setup Control A Register
	SERCOM_USART_CTRLA_Type tmpCtrA;
	tmpCtrA.reg = READREG32(uartBase.baseAddress + SERCOM_USART_CTRLA_OFFSET);
	
	tmpCtrA.bit.FORM = 0;
	tmpCtrA.bit.CMODE = 1;
	tmpCtrA.bit.MODE = 1;
	tmpCtrA.bit.SAMPR = 0;
	tmpCtrA.bit.DORD = 1;
	
	SETREG32(uartBase.baseAddress + SERCOM_USART_CTRLA_OFFSET, tmpCtrA.reg);
	
	//Setup Control B Register
	struct SERUSART_CTRLB_T tmpCtrB;
	*(int*)((void*)&tmpCtrB) = READREG32(uartBase.baseAddress + SERCOM_USART_CTRLB_OFFSET);
	tmpCtrB.TXEN = 1; //TX enable
	tmpCtrB.RXEN = 1; //RX enable
	tmpCtrB.PMODE = uartSetup.parity; //Parity 0 = equal, 1 = odd
	tmpCtrB.SBMODE = uartSetup.stopBits-1; //Stop bits 0 = 1, 1 = 2
	tmpCtrB.CHSIZE = (uartSetup.dataBits > 7) ? (uartSetup.dataBits == 9 ? 1 : 0) : (uartSetup.dataBits > 4) ? uartSetup.dataBits : 5; // If databit is 8 | 9 set to 0 or 1. Else if 5-7 set to that.
	
	SETREG32(uartBase.baseAddress + SERCOM_USART_CTRLB_OFFSET, *(int*)((void*)&tmpCtrB));
	
	//Setup Baud Rate
	double scale = (double)uartSetup.baudRate/(double)F_CPU;
	uint16_t tmpBaudRate = 65536.0f*(1.0f-(16.0f*scale));
	SETREG16(uartBase.baseAddress + SERCOM_USART_BAUD_OFFSET, uartSetup.baudRate);//tmpBaudRate);// 64281); // Hard Coded 19200 BAUD Rate - Calculation is random

	UART_SetupBuffer(uartBase, uartSetup);	

	//Setup Int
	UART_ClearInt(uartBase);
	
	UART_EnableInt(uartBase);
	
	UART_EnableSerCom(uartBase);
}

void UART_SetupBuffer(struct uart_t uartBase, struct uartsetup_t uartSetup)
{
	//Setup RX Buffer
	serComRxBuffers[uartBase.sercom].buffer = uartSetup.rxBufferAddr;
	serComRxBuffers[uartBase.sercom].maxLen = uartSetup.rxBufferSize;
	RB_ClearBuffer(&serComRxBuffers[uartBase.sercom]);

	//Setup RX buffer
	serComTransfers[uartBase.sercom].count = 0;
	serComTransfers[uartBase.sercom].index = 0;
	serComTransfers[uartBase.sercom].isEmpty = 1;
}

void UART_PadInit(struct uart_t uartBase)
{
	struct SERUSART_CTRLA_T tmpCtrA;
	switch(uartBase.sercom)
	{
		case 0:
			break;
		case 1:
			break;
		case 2:
			//Setup Control A Register
			*(int*)((void*)&tmpCtrA) = READREG32(uartBase.baseAddress + SERCOM_USART_CTRLB_OFFSET);
		
			tmpCtrA.RXPO = 0x1;
		
			SETREG32(uartBase.baseAddress + SERCOM_USART_CTRLA_OFFSET, *(int*)((void*)&tmpCtrA));
			break;
		case 3:
			break;
		case 4:
			break;
		case 5:
			//Setup Control A Register
			*(int*)((void*)&tmpCtrA) = READREG32(uartBase.baseAddress + SERCOM_USART_CTRLB_OFFSET);
		
			tmpCtrA.RXPO = 0x3;
			tmpCtrA.TXPO = 0x1;
		
			SETREG32(uartBase.baseAddress + SERCOM_USART_CTRLA_OFFSET, *(int*)((void*)&tmpCtrA));
			break;
		default:
			break;
	}
}

void UART_EnableSerCom(struct uart_t uartBase)
{
	switch (uartBase.sercom)
	{
		case 0:
			SERCOM0->USART.CTRLA.bit.ENABLE = 1;
			break;
		case 1:
			SERCOM1->USART.CTRLA.bit.ENABLE = 1;
			break;
		case 2:
			SERCOM2->USART.CTRLA.bit.ENABLE = 1;
			break;
		case 3:
			SERCOM3->USART.CTRLA.bit.ENABLE = 1;
			break;
		case 4:
			SERCOM4->USART.CTRLA.bit.ENABLE = 1;
			break;
		case 5:
			SERCOM5->USART.CTRLA.bit.ENABLE = 1;
			break;
		default:
		/* Your code here */
		break;
	}
}

void UART_EnableInt(struct uart_t uartBase)
{
	SETREG8(uartBase.baseAddress + SERCOM_USART_INTENSET_OFFSET, 0x0E);
}

void UART_ClearInt(struct uart_t uartBase)
{
	SETREG8(uartBase.baseAddress + SERCOM_USART_INTFLAG_OFFSET, 0xff);
}

void UART_DisableInt(struct uart_t uartBase)
{
	SETREG8(uartBase.baseAddress + SERCOM_USART_INTENCLR_OFFSET, 0x0E);
}

void UART_ISR(struct uart_t uartBase)
{
	UART_CheckBase(uartBase);
	
	uint8_t flags = READREG8(uartBase.baseAddress + SERCOM_USART_INTFLAG_OFFSET);
	
	if(flags & SERCOM_USART_INTFLAG_RXC)
	{
		uint8_t data = READREG8(uartBase.baseAddress + SERCOM_USART_DATA_OFFSET);
		RB_PushByte(data, &serComRxBuffers[uartBase.sercom]);
		UART_CheckBase(gsmUart);
		UART_CheckBase(gpsUart);
	}
	if(flags & SERCOM_USART_INTFLAG_RXS)
	{
	}
	if(flags & SERCOM_USART_INTFLAG_TXC)
	{
		if(!serComTransfers[uartBase.sercom].isEmpty && serComTransfers[uartBase.sercom].index < serComTransfers[uartBase.sercom].count)
		{
			SETREG8(uartBase.baseAddress + SERCOM_USART_DATA_OFFSET, serComTransfers[uartBase.sercom].buffer[serComTransfers[uartBase.sercom].index++]);
		}
		else if(!serComTransfers[uartBase.sercom].isEmpty)
		{
			serComTransfers[uartBase.sercom].isEmpty = 1;
			SETREG8(uartBase.baseAddress + SERCOM_USART_INTFLAG_OFFSET, SERCOM_USART_INTFLAG_TXC);
		}
	}
}

uint8_t UART_ScanRXBuffer(struct uart_t serCom, char data)
{
	return RB_ScanBuffer(&serComRxBuffers[serCom.sercom], data);
}

uint8_t UART_Receive(struct uart_t serCom, uint8_t* data, uint8_t count)
{
	uint8_t index;
	if (RB_IsEmpty(&serComRxBuffers[serCom.sercom]))
	{
		return 0;
	}
	
	for (index = 0; index < count; index++)
	{
		if(RB_PopByte(&serComRxBuffers[serCom.sercom], &data[index]))
			return index;
	}
	return index;
}

uint8_t UART_SendBuffer(struct uart_t serCom, uint8_t* buffer, uint16_t size)
{
	UART_CheckBase(serCom);
	if(serComTransfers[serCom.sercom].isEmpty && size <= MAX_TX_SIZE)
	{
		UART_ClearInt(serCom);
		serComTransfers[serCom.sercom].index = 0;
		serComTransfers[serCom.sercom].count = size;
		memcpy(serComTransfers[serCom.sercom].buffer, buffer, size);
		serComTransfers[serCom.sercom].isEmpty = (uint8_t)0;
		SETREG8(serCom.baseAddress + SERCOM_USART_DATA_OFFSET, serComTransfers[serCom.sercom].buffer[serComTransfers[serCom.sercom].index++]);
		return 1;
	}
	return 0;
}

uint8_t UART_ResetRXBuffer(struct uart_t uartBase)
{	
	RB_ClearBuffer(&serComRxBuffers[uartBase.sercom]);
	return 0;
}
