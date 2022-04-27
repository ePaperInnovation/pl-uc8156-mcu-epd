/*
 * touchscreen.c
 *
 *  Created on: 27.10.2021
 *      Author: zhongke.dai
 */

#include <msp430.h>
#include <stddef.h>
#include <types.h>
#include <stdint.h>
#include "msp430-touch-i2c.h"
#include "msp430-i2c.h"



unsigned char timercounter;
uint8_t array[10] = { 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xa };
uint8_t store[40] = { 13, 13, 13, 13, 13, 13, 13, 0, 0, 0};



void touchscreen_test(void)
{
    TOUCH_SENSOR_SendI2cRaw(array, 10);
    TOUCH_SENSOR_SendI2cRaw(store, 40);
}
