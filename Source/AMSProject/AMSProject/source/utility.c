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


//////////////////////////////////////////////////////////////////////////
// Scan array of chars for containing string. Return 1 if needle exists.
//////////////////////////////////////////////////////////////////////////
uint8_t ScanString(uint8_t* buffer, uint16_t bufferSize, uint8_t* needle, uint8_t needleSize)
{
	uint16_t i;
	uint8_t matched = 0;
	for (i = 0; i < bufferSize; i++)
	{
		if(buffer[i] == needle[matched])
		{
			matched++;
			if (matched == needleSize)
				return 1;
		}
		else
		{
			matched = 0;
		}
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////////
// Scan array of chars for containing string. Returns index of first symbol in needle or -1
//////////////////////////////////////////////////////////////////////////
int16_t ScanString(uint8_t* buffer, uint16_t bufferSize, uint8_t* needle, uint8_t needleSize)
{
	uint16_t i;
	uint8_t matched = 0;
	for (i = 0; i < bufferSize; i++)
	{
		if(buffer[i] == needle[matched])
		{
			matched++;
			if (matched == needleSize)
				return i - needleSize + 1; // Return index of first symbol in needle
		}
		else
		{
			matched = 0;
		}
	}
	return -1;
}