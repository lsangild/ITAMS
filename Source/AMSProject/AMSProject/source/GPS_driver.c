/*
 * GPS_driver.c
 *
 * Created: 4/5/2018 1:27:11 PM
 *  Author: Qtra
 */ 

#include "GPS_driver.h"

const char* ConstructUBXMessage(uint8_t class, uint8_t ID, int length, char * payload) {
    // Setup
    uint8_t CK_A = 0;
    uint8_t CK_B = 0;
    // Init forst to zero to ensure all array is zero
    uint8_t checksum_length = 6 + length + 2;
    char buffer[checksum_length] = {0};
    buffer[2] = class;
    buffer[3] = ID;
    
    // Fill in payload
    for (int i = 4, i < (4 + length), i++) {
        output[i] = &payload[i - 4];
    }
    
    // Calculate checksum
    for (int i = 0, i < checksum_length, i++) {
        CK_A = CK_A + &payload[i];
        CK_B = CK_B + CK_A;
    }
    
    output[6 + length] = CK_A;
    output[6 + length + 1] = CK_B;
    
    // Add Sync Chars (Done later to not include into checksum)
    buffer[0] = 0xB5;
    buffer[1] = 0x62;
    
    return *buffer;
}
