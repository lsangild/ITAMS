/*
 * utility.h
 *
 * Created: 4/2/2018 6:07:05 PM
 *  Author: Qtra
 */ 

#ifndef UTILITY_H_
#define UTILITY_H_
#include <stdint.h>

//Set one byte data at address
void SETREG8(uint32_t address, uint8_t data);
//Set two byte data at address
void SETREG16(uint32_t address, uint16_t data);
//Set four byte data at address
void SETREG32(uint32_t address, uint32_t data);


//Read one byte data at address
uint8_t READREG8(uint32_t address);
//Read two byte data at address
uint16_t READREG16(uint32_t address);
//Read four byte data at address
uint32_t READREG32(uint32_t address);

uint8_t ScanString(uint8_t* buffer, uint16_t bufferSize, uint8_t* needle, uint8_t needleSize);

#endif /* UTILITY_H_ */