/*
 * utility.c
 *
 * Created: 4/2/2018 6:06:50 PM
 *  Author: Qtra
 */ 
#include "utility.h"

void SETREG8(uint32_t address, uint8_t data)
{
	*(uint8_t*)(address) = data;	
}

void SETREG16(uint32_t address, uint16_t data)
{
	*(uint16_t*)(address) = data;
}

void SETREG32(uint32_t address, uint32_t data)
{
	*(uint32_t*)(address) = data;
}

uint8_t READREG8(uint32_t address)
{
	return *(uint8_t*)(address);
}

uint16_t READREG16(uint32_t address)
{
	return *(uint16_t*)(address);	
}

uint32_t READREG32(uint32_t address)
{
	return *(uint32_t*)(address);	
}
