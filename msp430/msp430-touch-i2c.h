/*
 * msp430-touch-i2c.h
 *  only for the touch sample from goodix gt1x
 *  Created on: 15.03.2022
 *      Author: zhongke.dai
 */



#ifndef MSP430_MSP430_TOUCH_I2C_H_
#define MSP430_MSP430_TOUCH_I2C_H_

#include <stddef.h>
#include <types.h>
#include <stdint.h>




// Touch parameters

#define MAX_NB_TOUCH 10
#define RESOLUTION_TOUCH_X 670   // for UC8177
#define RESOLUTION_TOUCH_Y 496   // for UC8177





// I2c registers
#define REG_TRIGGER 0x8056          // REG_CONFIC_TRIGGER
#define REG_RESOLUTION 0x8051       // REG_CONFIC_RESOLUTION
#define REG_COORD_DATA 0x8150       // REG_CONFIG_DATA
#define REG_STATUS 0x814E           // REG_READ_COOR_ADDR
#define REG_CMD 0x8040              // REG_CMD




static const char KEYBOARD_LINE_0[] = {'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p'};
static const char KEYBOARD_LINE_1[] = {'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l'};
static const char KEYBOARD_LINE_2[] = {'z', 'x', 'c', 'v', 'b', 'n', 'm'};




#define KEY_X_SIZE 48
#define KEY_Y_SIZE 74
#define KEYBOARD_Y_BASE 293
#define KEYBOARD_LINE_0_X_BASE 0
#define KEYBOARD_LINE_0_Y_BASE (KEYBOARD_Y_BASE)
#define KEYBOARD_LINE_1_X_BASE 23
#define KEYBOARD_LINE_1_Y_BASE (KEYBOARD_LINE_0_Y_BASE + KEY_Y_SIZE)
#define KEYBOARD_LINE_2_X_BASE 31
#define KEYBOARD_LINE_2_Y_BASE (KEYBOARD_LINE_1_Y_BASE + KEY_Y_SIZE)
#define KEYBOARD_LINE_3_Y_BASE (KEYBOARD_LINE_2_Y_BASE + KEY_Y_SIZE)
#define KEYBOARD_Y_LIMIT (KEYBOARD_LINE_3_Y_BASE + KEY_Y_SIZE)


void TOUCH_SENSOR_SendI2cRaw(uint8_t *data, uint8_t size);
void TOUCH_SENSOR_ReadI2cRaw(uint8_t *data, uint8_t size);
void TOUCH_SENSOR_WriteRegister(uint16_t reg, uint8_t *data, uint8_t size);
void TOUCH_SENSOR_ReadRegister(uint16_t reg, uint8_t *out, uint16_t size);
uint8_t TOUCH_SENSOR_ReadRegisterSimple(uint16_t reg);
void TOUCH_SENSOR_WriteRegisterSimple(uint16_t reg, uint8_t data);
void TOUCH_SENSOR_TriggerNewReading();
void TOUCH_SENSOR_ReadCoordinate(uint8_t touchNb, uint16_t *x, uint16_t *y);
void TOUCH_SENSOR_Reset();



#endif /* MSP430_MSP430_I2C_H_ */
