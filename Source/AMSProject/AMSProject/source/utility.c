/*
 * utility.c
 *
 * Created: 4/2/2018 6:06:50 PM
 *  Author: Qtra
 */ 
#include "utility.h"

void SETREG8(uint32_t address, uint8_t data)
{
	*(int*)(address) = data;	
}

void SETREG16(uint32_t address, uint16_t data)
{
	*(int*)(address) = data;
}

void SETREG32(uint32_t address, uint32_t data)
{
	*(int*)(address) = data;
}
