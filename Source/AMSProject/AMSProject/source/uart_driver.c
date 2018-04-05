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
	//Setup clock
	REG_PM_APBCMASK |= 1 << (2 + uartBase.sercom);// Shift Clock power bit in
	GCLK->CLKCTRL.reg = GCLK_CLKCTRL_ID(0x14+uartBase.sercom) | GCLK_CLKCTRL_GEN_GCLK0 | GCLK_CLKCTRL_CLKEN; //Select clock, and enable
	//Write more elegant! Enables the specific clock, uses same reg for all generics
	
	//Setup Baud Rate
	uint16_t tmpBaudRate = 65536*(1-16*((double)uartSetup.baudRate/F_CPU));
	SETREG16(uartBase.baseAddress + SERCOM_USART_BAUD_OFFSET, tmpBaudRate);
	
	//Setup Control A Register
	struct SERUSART_CTRLA_T tmpCtrA;
	*(int*)((void*)&tmpCtrA) = REG_SERCOM5_I2CM_CTRLA;
	
	tmpCtrA.FORM = 0;
	tmpCtrA.CMODE = 1;
	tmpCtrA.MODE = 1;
	tmpCtrA.ENABLE = 1;
	
	SETREG32(uartBase.baseAddress + SERCOM_USART_CTRLA_OFFSET, *(int*)((void*)&tmpCtrA));
	
	//Setup Control B Register
	struct SERUSART_CTRLB_T tmpCtrB;
	*(int*)((void*)&tmpCtrB) = REG_SERCOM5_I2CM_CTRLA;
	
	tmpCtrB.TXEN = 1; //TX enable
	tmpCtrB.RXEN = 1; //RX enable
	tmpCtrB.PMODE = uartSetup.parity; //Parity 0 = equal, 1 = odd
	tmpCtrB.SBMODE = uartSetup.stopBits-1; //Stop bits 0 = 1, 1 = 2
	tmpCtrB.CHSIZE = (uartSetup.dataBits > 7) ? (uartSetup.dataBits == 9 ? 1 : 0) : (uartSetup.dataBits > 4) ? uartSetup.dataBits : 5; // If databit is 8 | 9 set to 0 or 1. Else if 5-7 set to that.
	
	SETREG32(uartBase.baseAddress + SERCOM_USART_CTRLB_OFFSET, *(int*)((void*)&tmpCtrB));
	
	//Setup RX Buffer
	serComRxBuffers[uartBase.sercom].buffer = uartSetup.rxBufferAddr;
	serComRxBuffers[uartBase.sercom].maxLen = uartSetup.rxBufferSize;
	RB_ClearBuffer(&serComRxBuffers[uartBase.sercom]);
	
	//Setup Int
	UART_EnableInt(uartBase);
	
	UART_PadInit(uartBase);
	
	UART_ClearInt(uartBase);
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
			*(int*)((void*)&tmpCtrA) = REG_SERCOM5_I2CM_CTRLA;
			
			tmpCtrA.RXPO = 0x1;
			
			SETREG32(uartBase.baseAddress + SERCOM_USART_CTRLA_OFFSET, *(int*)((void*)&tmpCtrA));
			break;
		case 3:
			break;
		case 4:
			break;
		case 5:
			//Setup Control A Register
			*(int*)((void*)&tmpCtrA) = REG_SERCOM5_I2CM_CTRLA;
		
			tmpCtrA.RXPO = 0x3;
			tmpCtrA.TXPO = 0x1;
		
			SETREG32(uartBase.baseAddress + SERCOM_USART_CTRLA_OFFSET, *(int*)((void*)&tmpCtrA));
			break;
		default:
			break;
	}
}

void UART_EnableInt(struct uart_t uartBase)
{
	SETREG8(uartBase.baseAddress + SERCOM_USART_INTENSET_OFFSET, 0x0f);
}

void UART_ClearInt(struct uart_t uartBase)
{
	SETREG8(uartBase.baseAddress + SERCOM_USART_INTFLAG_OFFSET, 0xff);
}

void UART_DisableInt(struct uart_t uartBase)
{
	SETREG8(uartBase.baseAddress + SERCOM_USART_INTENCLR_OFFSET, 0x0f);
}

void UART_ISR(struct uart_t uartBase)
{
	uint8_t vector = 0;
	do 
	{
		switch (vector)
		{
			case SERCOM_USART_INTFLAG_RXC:
				break;
			case SERCOM_USART_INTFLAG_RXS:
				break;
			case SERCOM_USART_INTFLAG_DRE:
				if(!serComTransfers[uartBase.sercom].isEmpty && serComTransfers[uartBase.sercom].index < serComTransfers[uartBase.sercom].count)
				{
					SETREG8(uartBase.baseAddress + SERCOM_USART_DATA_OFFSET, serComTransfers[uartBase.sercom].buffer[serComTransfers->index]);
				}
				break;
			case SERCOM_USART_INTFLAG_TXC:
				if(!serComTransfers[uartBase.sercom].isEmpty && serComTransfers[uartBase.sercom].index == serComTransfers[uartBase.sercom].count)
				{
					serComTransfers[uartBase.sercom].isEmpty = 1;
				}
				break;
			default:
				break;
		}
	} while ((vector = UART_INTVECTOR(uartBase)) != 0);
}

//Get interrupt vector from register
uint8_t UART_INTVECTOR(struct uart_t uartBase)
{
	if(READREG16(uartBase.baseAddress + SERCOM_USART_INTFLAG_OFFSET) & SERCOM_USART_INTFLAG_RXC) 
	{
		SETREG8(uartBase.baseAddress + SERCOM_USART_INTFLAG_OFFSET, SERCOM_USART_INTFLAG_RXC);
		return SERCOM_USART_INTFLAG_RXC;
	}
	if(READREG16(uartBase.baseAddress + SERCOM_USART_INTFLAG_OFFSET) & SERCOM_USART_INTFLAG_RXS) 
	{
		SETREG8(uartBase.baseAddress + SERCOM_USART_INTFLAG_OFFSET, SERCOM_USART_INTFLAG_RXS);
		return SERCOM_USART_INTFLAG_RXS;
	}
	if(READREG16(uartBase.baseAddress + SERCOM_USART_INTFLAG_OFFSET) & SERCOM_USART_INTFLAG_DRE)
	{
		SETREG8(uartBase.baseAddress + SERCOM_USART_INTFLAG_OFFSET, SERCOM_USART_INTFLAG_DRE);
		return SERCOM_USART_INTFLAG_DRE;
	}	
	if(READREG16(uartBase.baseAddress + SERCOM_USART_INTFLAG_OFFSET) & SERCOM_USART_INTFLAG_TXC)
	{
		SETREG8(uartBase.baseAddress + SERCOM_USART_INTFLAG_OFFSET, SERCOM_USART_INTFLAG_TXC);
		return SERCOM_USART_INTFLAG_TXC;		
	}
	return 0;
}

uint8_t UART_SendBuffer(struct uart_t serCom, uint8_t* buffer, uint16_t size)
{
	if(serComTransfers[serCom.sercom].isEmpty && size <= MAX_TX_SIZE)
	{
		UART_ClearInt(serCom);
		
		serComTransfers[serCom.sercom].index = 0;
		serComTransfers[serCom.sercom].count = size;
		memcpy(serComTransfers[serCom.sercom].buffer, buffer, size);
		serComTransfers[serCom.sercom].isEmpty = 0;		
		SETREG8(serCom.baseAddress + SERCOM_USART_DATA_OFFSET, serComTransfers[serCom.sercom].buffer[serComTransfers[serCom.sercom].index++]);
		return 1;
	}
	return 0;
}
