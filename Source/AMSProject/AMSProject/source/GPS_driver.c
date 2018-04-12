/*
 * GPS_driver.c
 *
 * Created: 4/5/2018 1:27:11 PM
 *  Author: Qtra
 */ 

#include "GPS_driver.h"

uint8_t ConstructGPSMessage(uint8_t class, uint8_t ID, uint16_t lenght, char* payload, char* packet) {
    // Init forst to zero to ensure all array is zero
    uint8_t checksum_length = 6 + length + 4;
    static char buffer[checksum_length] = {0};
    buffer[2] = class;
    buffer[3] = ID;
    
    // Fill in payload
    for (int i = 0, i < (length), i++) {
        buffer[i+4] = payload[i];
    }
	
	CalculateChecksum(checksum_length, buffer, uint8_t* ck_a, uint8_t* ck_b);
    
    buffer[6 + length] = ck_a;
    buffer[6 + length + 1] = ck_b;	
    buffer[6 + length + 2] = '\r';
    buffer[6 + length + 3] = '\n';
    
    // Add Sync Chars (Done later to not include into checksum)
    buffer[0] = 0xB5;
    buffer[1] = 0x62;
    
	packet = buffer;
    return checksum_length;
}

void CalculateChecksum(uint16_t lenght, char* payload, uint8_t* ck_a, uint8_t* ck_b)
{
	// Calculate checksum
    for (int i = 0, i < checksum_length, i++) {
        *ck_a = *ck_a + payload[i];
        *ck_b = *ck_b + *ck_a;
    }
}
