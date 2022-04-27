/*
 * msp430-i2c.h
 *
 *  Created on: 26.10.2021
 *      Author: zhongke.dai
 */

#ifndef MSP430_MSP430_I2C_H_
#define MSP430_MSP430_I2C_H_

#include <stddef.h>
#include <types.h>
#include <stdint.h>


//#define SDA_PIN 0x02                                  // msp430x261x UCB0SDA pin
//#define SCL_PIN 0x04                                  // msp430x261x UCB0SCL pin



#define I2C_INT         GPIO(1,0)    // I2C Interrupt test  // BB_PMIC_POK
#define I2C_SDA         GPIO(3,7)    // BB_I2C_SDA
#define I2C_SCL         GPIO(5,4)    // BB_I2C_SCL




enum pl_i2c_flags {
    PL_I2C_NO_STOP  = 1 << 0,       /**< Do not generate stop bit */
    PL_I2C_NO_START = 1 << 1,       /**< Do not generate start bit */
};




int i2c_init(void);


void i2c_close(void);


int msp430_i2c_read(uint8_t i2c_addr, uint8_t *data, uint8_t count, uint8_t flags);
void msp430_i2c_write(uint8_t i2c_addr, const uint8_t *data, uint8_t count);











#endif /* MSP430_MSP430_I2C_H_ */



