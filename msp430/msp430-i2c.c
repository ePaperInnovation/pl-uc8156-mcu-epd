/*
 * msp430-i2c.c
 *
 *  Created on: 26.10.2021
 *      Author: zhongke.dai
 */


#include <stdio.h>
#include <stdlib.h>

#include <msp430.h>

#include "types.h"
#include "assert.h"
#include "utils.h"

#include "msp430-defs.h"
#include "msp430-i2c.h"
#include "msp430-gpio.h"



/* We only support a single SPI bus and that bus is defined at compile
 * time.
 */


signed char byteCtr;
unsigned char *TI_receive_field;
unsigned char *TI_transmit_field;


void i2c_ini(void)
{
    gpio_request(I2C_INT, PIN_GPIO | PIN_OUTPUT | PIN_INIT_HIGH);
    gpio_request(I2C_SCL,   PIN_SPECIAL | PIN_OUTPUT);
    gpio_request(I2C_SDA,   PIN_SPECIAL | PIN_OUTPUT | PIN_INPUT);
}





void i2c_close(void)
{
  //  UCxnCTL1 |= UCSWRST;                      // Put state machine in reset
}




void TI_USCI_I2C_receiveinit(unsigned char slave_address,
                             unsigned char prescale){

  UCB1CTL1 = UCSWRST;                        // Enable SW reset
  UCB1CTL0 = UCMST + UCMODE_3 + UCSYNC;       // I2C Master, synchronous mode
  UCB1CTL1 = UCSSEL_2 + UCSWRST;              // Use SMCLK, keep SW reset
  UCB1BR0 = prescale;                         // set prescaler
  UCB1BR1 = 0;
  UCB1I2CSA = slave_address;                  // set slave address
  UCB1CTL1 &= ~UCSWRST;                       // Clear SW reset, resume operation
  UCB1ICTL = UCNACKIE;
  UCB1IE = I2C_INT;                            // Enable RX interrupt
}



void TI_USCI_I2C_transmitinit(unsigned char slave_address,
                          unsigned char prescale){

  UCB1CTL1 = UCSWRST;                        // Enable SW reset
  UCB1CTL0 = UCMST + UCMODE_3 + UCSYNC;       // I2C Master, synchronous mode
  UCB1CTL1 = UCSSEL_2 + UCSWRST;              // Use SMCLK, keep SW reset
  UCB1BR0 = prescale;                         // set prescaler
  UCB1BR1 = 0;
  UCB1I2CSA = slave_address;                  // Set slave address
  UCB1CTL1 &= ~UCSWRST;                       // Clear SW reset, resume operation
  UCB1ICTL = UCNACKIE;
  UCB1IE = I2C_INT;                              // Enable TX ready interrupt
}




void TI_USCI_I2C_receive(unsigned char byteCount, unsigned char *field){
  TI_receive_field = field;
  if ( byteCount == 1 ){
    byteCtr = 0 ;
    __disable_interrupt();
    UCB1CTL1 |= UCTXSTT;                      // I2C start condition
    while (UCB1CTL1 & UCTXSTT);               // Start condition sent?
    UCB1CTL1 |= UCTXSTP;                      // I2C stop condition
    __enable_interrupt();
  } else if ( byteCount > 1 ) {
    byteCtr = byteCount - 2 ;
    UCB1CTL1 |= UCTXSTT;                      // I2C start condition
  } else
    while (1);                                // illegal parameter
}

void TI_USCI_I2C_transmit(unsigned char byteCount, unsigned char *field){
  TI_transmit_field = field;
  byteCtr = byteCount;
  UCB1CTL1 |= UCTR + UCTXSTT;                 // I2C TX, start condition
}


//unsigned char TI_USCI_I2C_slave_present(unsigned char slave_address){
//  unsigned char ie2_bak, slaveadr_bak, ucb0i2cie, returnValue;
//  ucb0i2cie = UCB0I2CIE;                      // restore old UCB0I2CIE
//  ie2_bak = IE2;                              // store IE2 register
//  slaveadr_bak = UCB0I2CSA;                   // store old slave address
//  UCB0I2CIE &= ~ UCNACKIE;                    // no NACK interrupt
//  UCB0I2CSA = slave_address;                  // set slave address
//  IE2 &= ~(UCB0TXIE + UCB0RXIE);              // no RX or TX interrupts
//  __disable_interrupt();
//  UCB0CTL1 |= UCTR + UCTXSTT + UCTXSTP;       // I2C TX, start condition
//  while (UCB0CTL1 & UCTXSTP);                 // wait for STOP condition
//
//  returnValue = !(UCB0STAT & UCNACKIFG);
//  __enable_interrupt();
//  IE2 = ie2_bak;                              // restore IE2
//  UCB0I2CSA = slaveadr_bak;                   // restore old slave address
//  UCB0I2CIE = ucb0i2cie;                      // restore old UCB0CTL1
//  return returnValue;                         // return whether or not
//                                              // a NACK occured
//}
//
//
//
unsigned char TI_USCI_I2C_notready(){
  return (UCB1STAT & UCBBUSY);
}
//
//
//#pragma vector = USCI_B1_VECTOR
//__interrupt void USCIAB1RX_ISR(void)
//{
//  if (UCB1STAT & UCNACKIFG){            // send STOP if slave sends NACK
//    UCB1CTL1 |= UCTXSTP;
//    UCB1STAT &= ~UCNACKIFG;
//  }
//
//}
////
//
//#pragma vector = USCI_B1_VECTOR
//__interrupt void USCIAB1TX_ISR(void)
//{
//  if (IFG2 & UCB0RXIFG){
//    if ( byteCtr == 0 ){
//      UCB0CTL1 |= UCTXSTP;                    // I2C stop condition
//      *TI_receive_field = UCB0RXBUF;
//      TI_receive_field++;
//    }
//    else {
//      *TI_receive_field = UCB0RXBUF;
//      TI_receive_field++;
//      byteCtr--;
//    }
//  }
//  else {
//    if (byteCtr == 0){
//      UCB0CTL1 |= UCTXSTP;                    // I2C stop condition
//      IFG2 &= ~UCB0TXIFG;                     // Clear USCI_B0 TX int flag
//    }
//    else {
//      UCB0TXBUF = *TI_transmit_field;
//      TI_transmit_field++;
//      byteCtr--;
//    }
//  }
//}
























