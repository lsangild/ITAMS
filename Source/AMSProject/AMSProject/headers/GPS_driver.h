/*
 * GPS_driver.h
 *
 * Created: 4/5/2018 1:26:24 PM
 *  Author: Qtra
 */ 


#ifndef GPS_DRIVER_H_
#define GPS_DRIVER_H_

#include "uart_driver.h"

extern struct uart_t gpsUart;

void InitGPS();


#endif /* GPS_DRIVER_H_ */