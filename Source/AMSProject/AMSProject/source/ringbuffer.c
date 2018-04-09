/*
* ringbuffer.c
*
* Created: 3/6/2018 9:53:32 AM
*  Author: Qtra
*/

#include "ringbuffer.h"

uint8_t RB_PopElement(uint8_t* element, uint8_t elementSize, ringbuffer_t* buffer)
{
	uint8_t loopCount = 0;
	uint8_t data;
	for (loopCount = 0; loopCount < elementSize; loopCount++)
	{
		if(RB_PopByte(buffer, &data) != 0)
		{
			buffer->tail -= loopCount;
			return 1;
		}
		element[loopCount] = data;
	}
	return 0;
}

uint8_t RB_PopByte(ringbuffer_t *buffer, uint8_t *data)
{
	// if the head isn't ahead of the tail, we don't have any characters
	if (buffer->head == buffer->tail) // check if circular buffer is empty
		return 1;          // and return with an error

	// next is where tail will point to after this read.
	int next = buffer->tail + 1;
	if(next >= buffer->maxLen)
		next = 0;

	*data = buffer->buffer[buffer->tail]; // Read data and then move
	buffer->tail = next;             // tail to next data offset.
	return 0;  // return success to indicate successful push.
}

uint8_t RB_PopBytes(ringbuffer_t *buffer, uint8_t *data, uint8_t *count)
{
	uint8_t index;
	for (index = 0; index < *count; index++)
	{
		if(RB_PopByte(buffer, &data[index]))
			return 0;
	}
	*count = index+1;
	
	if (index)
		return 0;
		
	return 1;
}

uint8_t RB_PushElement(uint8_t* element, uint8_t elementSize, ringbuffer_t* buffer)
{
	uint8_t loopCount = 0;
	for (loopCount = 0; loopCount < elementSize; loopCount++)
	{
		if(RB_PushByte(element[loopCount], buffer) != 0)
		{
			buffer->head -= loopCount;
			return 1;
		}
	}
	return 0;
}

uint8_t RB_PushByte(uint8_t data, ringbuffer_t* buffer)
{
	// next is where head will point to after this write.
	int next = buffer->head + 1;
	if (next >= buffer->maxLen)
		next = 0;

	if (next == buffer->tail) // check if circular buffer is full
		return 1;       // and return with an error.

	buffer->buffer[buffer->head] = data; // Load data and then move
	buffer->head = next;            // head to next data offset.
	return 0;  // return success to indicate successful push.
}


void RB_ClearBuffer(ringbuffer_t* buffer)
{
	buffer->head = 0;
	buffer->tail = 0;
}


uint8_t RB_IsEmpty(ringbuffer_t* buffer)
{	
	return buffer->head == buffer->tail;
}

uint8_t RB_IsFull(ringbuffer_t* buffer)
{	
	return buffer->head+1 == buffer->tail;
}

uint8_t RB_ScanBuffer(ringbuffer_t* buffer, char data)
{
	if(RB_IsEmpty(buffer))
		return 0;
		
	uint16_t IndexOffset = 0;
	uint16_t loopOffset = 0;
	
	do 
	{
		if(buffer->buffer[buffer->tail+IndexOffset-loopOffset] == data)
			return IndexOffset+1;
			
		IndexOffset++;
	} while (buffer->tail != buffer->tail + IndexOffset - loopOffset && buffer->tail + IndexOffset - loopOffset - 1 != buffer->head); //buffer->tail + IndexOffset + loopOffset = 0
	 
	return 0;
}