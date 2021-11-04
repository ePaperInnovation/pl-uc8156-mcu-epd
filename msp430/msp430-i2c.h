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


#define SDA_PIN 0x02                                  // msp430x261x UCB0SDA pin
#define SCL_PIN 0x04                                  // msp430x261x UCB0SCL pin


#define I2C_INT         GPIO(1,0)    // I2C Interrupt test  // BB_PMIC_POK
#define I2C_SDA         GPIO(3,7)    // BB_I2C_SDA
#define I2C_SCL         GPIO(5,4)    // BB_I2C_SCL



void i2c_ini(void);
void i2c_close(void);

//void TI_USCI_I2C_receiveinit(unsigned char slave_address, unsigned char prescale);
void TI_USCI_I2C_transmitinit(unsigned char slave_address, unsigned char prescale);


void TI_USCI_I2C_receive(unsigned char byteCount, unsigned char *field);
void TI_USCI_I2C_transmit(unsigned char byteCount, unsigned char *field);


//unsigned char TI_USCI_I2C_slave_present(unsigned char slave_address);
unsigned char TI_USCI_I2C_notready();











#endif /* MSP430_MSP430_I2C_H_ */



