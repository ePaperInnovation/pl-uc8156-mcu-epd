/*
 * touchscreen.c
 *
 *  Created on: 27.10.2021
 *      Author: zhongke.dai
 */

#include <msp430.h>
#include "msp430/msp430-i2c.h"


unsigned char timercounter;
unsigned char array[40] = { 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xa, 0xb };
unsigned char store[40] = { 13, 13, 13, 13, 13, 13, 13, 0, 0, 0};



void touchscreen_test(void)
{
    WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT



    _EINT();

    TI_USCI_I2C_transmitinit(0x28,0x12);  // init transmitting with USCI
      while ( TI_USCI_I2C_notready() );
    TI_USCI_I2C_transmit(4,array);       // start transmitting
//    while ( TI_USCI_I2C_notready() );         // wait for bus to be free
//    if ( TI_USCI_I2C_slave_present(0x50) )    // slave address may differ from
//    {                                         // initialization
//      TI_USCI_I2C_receiveinit(0x50,0x12);   // init receiving with USCI
//      while ( TI_USCI_I2C_notready() );         // wait for bus to be free
//      TI_USCI_I2C_receive(4,store);
//      while ( TI_USCI_I2C_notready() );         // wait for bus to be free
//
//      TI_USCI_I2C_transmitinit(0x50,0x12);  // init transmitting with
//      while ( TI_USCI_I2C_notready() );         // wait for bus to be free
//      TI_USCI_I2C_transmit(4,array);       // start transmitting
//    }

   // LPM3;


}
