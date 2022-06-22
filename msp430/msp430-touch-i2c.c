/*
 * msp430-touch-i2c.c
 *
 *
 *
 *
 * * msp430-touch-i2c.h
 *  only for the touch sample from goodix gt1x
 *
 *
 *
 *  Created on: 15.03.2022
 *      Author: zhongke.dai
 */




#include <stddef.h>
#include <types.h>
#include <stdint.h>
#include "msp430-touch-i2c.h"
#include "msp430-i2c.h"








void TOUCH_SENSOR_SendI2cRaw(uint8_t *data, uint8_t size)
{
 // I2C_Master_Transmit(&hi2c3, 0x28, data, size, 1000);
    msp430_i2c_write(0x28,  data, size);
}


void TOUCH_SENSOR_ReadI2cRaw(uint8_t *data, uint8_t size)
{
  //I2C_Master_Receive(&hi2c3, 0x28, data, size, 1000);
    msp430_i2c_read(0x28,  data, size, 1 );
}



void TOUCH_SENSOR_WriteRegister(uint16_t reg, uint8_t *data, uint8_t size)
{
  uint8_t buf[50];                            // size for *data max 48?
  buf[0] = (reg & 0xFF00) >> 8;                     // change the uint16 address to unit8, address_unint8_h
  buf[1] = (reg & 0x00FF);                          // change the uint16 address to unit8, address_unint8_l
  uint16_t bufSize = 2 + size;

  if (size && data)
  {
    memcpy(buf + 2, data, size);
  }

  TOUCH_SENSOR_SendI2cRaw(buf, bufSize);          // send buffer to MCU
}


void TOUCH_SENSOR_ReadRegister(uint16_t reg, uint8_t *out, uint16_t size)
{
  TOUCH_SENSOR_WriteRegister(reg, NULL, 0);
  TOUCH_SENSOR_ReadI2cRaw(out, size);
}

uint8_t TOUCH_SENSOR_ReadRegisterSimple(uint16_t reg)
{
  uint8_t buf = 0xFF;
  TOUCH_SENSOR_ReadRegister(reg, &buf, 1);
  return buf;
}


void TOUCH_SENSOR_WriteRegisterSimple(uint16_t reg, uint8_t data)
{
  TOUCH_SENSOR_WriteRegister(reg, &data, 1);
}

void TOUCH_SENSOR_TriggerNewReading()
{
  // Reset status
  TOUCH_SENSOR_WriteRegisterSimple(REG_STATUS, 0);
  // Send a read cmd
  TOUCH_SENSOR_WriteRegisterSimple(REG_CMD, 1);
}


void TOUCH_SENSOR_ReadCoordinate(uint8_t touchNb, uint16_t *x, uint16_t *y)
{

  uint8_t coord[4];

  // Read xy
  TOUCH_SENSOR_ReadRegister(REG_COORD_DATA + 8 * touchNb, coord, 4);
  *x = coord[0] + (coord[1] << 8);
  *y = coord[2] + (coord[3] << 8);
}


void TOUCH_SENSOR_Reset()
{

}
























