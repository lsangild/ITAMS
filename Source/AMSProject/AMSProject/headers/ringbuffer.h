/*
* ringbuffer.h
*
* Created: 3/6/2018 9:53:24 AM
*  Author: Qtra
*/

#ifndef RINGBUFFER_H_
#define RINGBUFFER_H_
#include <stdint.h>

typedef struct{
	uint8_t* buffer;
	uint16_t head;
	uint16_t tail;
	uint16_t maxLen;
}ringbuffer_t;

uint8_t RB_PopElement(uint8_t* element, uint8_t elementSize, ringbuffer_t* buffer);
uint8_t RB_PopByte(ringbuffer_t* buffer, uint8_t* data);
uint8_t RB_PopBytes(ringbuffer_t* buffer, uint8_t *data, uint8_t *count);
uint8_t RB_PushElement(uint8_t* element, uint8_t elementSize, ringbuffer_t* buffer);
uint8_t RB_PushByte(uint8_t data, ringbuffer_t* buffer);
void	RB_ClearBuffer(ringbuffer_t* buffer);
uint8_t RB_IsEmpty(ringbuffer_t* buffer);
uint8_t RB_IsFull(ringbuffer_t* buffer);
uint8_t RB_ScanBuffer(ringbuffer_t* buffer, char data);



#endif /* RINGBUFFER_H_ */