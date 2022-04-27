/*
 * msp430-i2c.c
 *
 *  Created on: 26.10.2021
 *      Author: zhongke.dai
 */







/* Note: From MSP430F5438x Errata sheet
 * USCI35 - USCI Module Function
 * Violation of setup and hold times for (repeated) start in I2C master mode
 * In I2C master mode, the setup and hold times for a (repeated) START, tSU,STA
 * and tHD,STA respectively, can be violated if SCL clock frequency is greater
 * than 50kHz in standard mode (100kbps). As a result, a slave can receive incorrect
 * data or the I2C bus can be stalled due to clock stretching by the slave.
 *
 * Workaround:
 * If using repeated start, ensure SCL clock frequencies is <50kHz in I2C standard
 * mode (100kbps)
 *
 * The implication being that clock frequencies from 50Khz to 100Khz are not reliable.
 */









#include <stdio.h>
#include <stdlib.h>

#include <msp430.h>

#include "types.h"
#include "assert.h"
#include "utils.h"

#include "msp430-defs.h"
#include "msp430-i2c.h"
#include "msp430-touch-i2c.h"
#include "msp430-gpio.h"



/* We only support a single SPI bus and that bus is defined at compile
 * time.
 */




uint8_t TXData;
uint8_t TXByteCtr;



#define SCL_CLOCK_DIV 50                    // SCL clock divider

#define USCI_UNIT   B
#define USCI_CHAN   1



//struct pl_i2c {
//    /**
//       read some data on the I2C bus
//       @param[in] i2c this pl_i2c instance
//       @param[in] addr 8-bit I2C address
//       @param[out] data buffer to receive the data being read
//       @param[in] count number of bytes to read
//       @param[in] flags flags bitmask using pl_i2c_flags
//       @return -1 if error, 0 otherwise
//     */
//    int (*read)(struct pl_i2c *i2c, uint8_t addr,
//            uint8_t *data, uint8_t count, uint8_t flags);
//
//    /**
//       write some data on the I2C bus
//       @param[in] i2c this pl_i2c instance
//       @param[in] addr 8-bit I2C address
//       @param[in] data buffer with data to be written
//       @param[in] count number of bytes to write
//       @param[in] flags flags bitmask using pl_i2c_flags
//       @return -1 if error, 0 otherwise
//     */
//    int (*write)(struct pl_i2c *i2c, uint8_t addr,
//             const uint8_t *data, uint8_t count, uint8_t flags);
//
//    /**
//       free the resources associated with this instance
//       @param i2c this pl_i2c instance
//     */
//    void (*free)(struct pl_i2c *i2c);
//
//    /**
//       private data specific to this instance
//     */
//    void *priv;
//};


int i2c_init(void)
{
//    static const struct pl_gpio_config gpios[] = {
//        { I2C_SCL, PL_GPIO_SPECIAL },
//        { I2C_SDA, PL_GPIO_SPECIAL },
//    };

    /* we only support one i2c channel on MSP430 */
//    assert(channel == 0);
//
//    if (pl_gpio_config_list(gpio, gpios, ARRAY_SIZE(gpios)))
//        return -1;

    // Recommended initialisation steps of I2C module as shown in User Guide:


    gpio_request(I2C_SCL,  PIN_SPECIAL);
    gpio_request(I2C_SDA,  PIN_SPECIAL);





    UCxnCTL1 |= UCSWRST;                    // Enable SW reset
   // P3SEL |= 0x06;
    UCxnCTL0 = UCMST | UCMODE_3 | UCSYNC;   // I2C Master, synchronous mode
    UCxnCTL1 = UCSSEL_2 | UCTR | UCSWRST;   // Use SMCLK, TX mode, keep SW reset
    UCxnBR0 = SCL_CLOCK_DIV;                // fSCL = SMCLK/N = ~400kHz
    UCxnBR1 = 0;
    UCxnI2COA = 0x01A5;                     // own address (no general call)
    UCxnIE = 0;                             // disable all interrupts
    UCxnCTL1 &= ~UCSWRST;                   // Clear SW reset, resume operation
   // __enable_interrupt();                  // Make this operation atomic
    // if bus not free generate a manual clock pulse.
//    if (UCxnSTAT & UCBBUSY) {
//        if (gpio->config(I2C_SCL, PL_GPIO_OUTPUT | PL_GPIO_INIT_L))
//            return -1;
//
//        if (gpio->config(I2C_SCL, PL_GPIO_SPECIAL))
//            return -1;
//
//        assert(!(UCxnSTAT & UCBBUSY));
//    }

//    i2c->read = msp430_i2c_read;
//    i2c->write = msp430_i2c_write;
//    i2c->priv = NULL;

    return 0;
}




 void msp430_i2c_write(uint8_t i2c_addr, const uint8_t *data, uint8_t count)
{
   // unsigned int gie = __get_SR_register() & GIE; //Store current GIE state

  //  __disable_interrupt();                  // Make this operation atomic
    TXByteCtr = count;

     UCxnIE |= UCTXIE;                         // Enable TX interrupt

     UCxnI2CSA  = i2c_addr;              // set Slave Address



    while (1)
    {
        TXData = *data++;
        UCxnCTL1 |= UCTR;                   // UCTR=1 => Transmit Mode (R/W bit = 0)
        UCxnCTL1 |= UCTXSTT;                // Transmit start


        __bis_SR_register(LPM0_bits +GIE);                 // Restore original GIE state
        __no_operation();

    }
}



 int msp430_i2c_read(uint8_t i2c_addr, uint8_t *data, uint8_t count, uint8_t flags)
{
    int result = -1;
    int remaining = count;
    int send_stop;
    int stop_sent = 0;
    unsigned int gie = __get_SR_register() & GIE; // store current GIE state

    __disable_interrupt();                  // Make this operation atomic

    send_stop = (!(flags & PL_I2C_NO_STOP) && (count == 1));

    if (count == 0)
        goto no_data;

    if (!(flags & PL_I2C_NO_START))
    {
        UCxnI2CSA  = i2c_addr;              // set Slave Address

        UCxnIFG = 0;                        // clear interrupt flags
        UCxnCTL1 &= ~UCTR;                  // UCTR=0 => Receive Mode (R/W bit = 1)
        UCxnCTL1 |= UCTXSTT;                // Transmit start

        while (UCxnCTL1 & UCTXSTT);         // wait for START to complete
        if (UCxnIFG & UCNACKIFG)            // bail if NACK'd
            goto error;

        // special case handling of single byte read in case timing critical
        if (send_stop) {
            UCxnCTL1 |= UCTXSTP;            // generate stop
            while (UCxnCTL1 & UCTXSTP);     // Ensure stop condition got sent

            while ((UCxnIFG & UCRXIFG) == 0);   // Wait for data ready
            *data++ = UCxnRXBUF;                // read the byte (allows collection of next)
            remaining--;
            stop_sent = 1;
        }
    }

    while (remaining)
    {
        remaining--;
        if (remaining == 0 && !(flags & PL_I2C_NO_STOP)) {
            UCxnCTL1 |= UCTXSTP;            // generate stop
            while(UCxnCTL1 & UCTXSTP);      // Ensure stop condition got sent
            stop_sent = 1;
        }
        while ((UCxnIFG & UCRXIFG) == 0);   // Wait for data ready
        *data++ = UCxnRXBUF;                // read the byte (allows collection of next)
    }

no_data:
    result = 0;

    // suppress stop if requested or not already sent
    if (!(flags & PL_I2C_NO_STOP) && !stop_sent)
    {
error:
        UCxnCTL1 |= UCTXSTP;                // Transmit Stop
        while(UCxnCTL1 & UCTXSTP);          // Ensure stop condition got sent
    }
    __bis_SR_register(gie);                 // Restore original GIE state

    return result;
}



 unsigned char TI_USCI_I2C_notready(){
   return (UCxnSTAT & UCBBUSY);
 }



#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = USCI_B1_VECTOR
__interrupt void USCI_B1_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(USCI_B1_VECTOR))) USCI_B1_ISR (void)
#else
#error Compiler not supported!
#endif
{
  switch(__even_in_range(UCB1IV,12))
  {
  case  0: break;                           // Vector  0: No interrupts
  case  2: break;                           // Vector  2: ALIFG
  case  4: break;                           // Vector  4: NACKIFG
  case  6: break;                           // Vector  6: STTIFG
  case  8: break;                           // Vector  8: STPIFG
  case 10: break;                           // Vector 10: RXIFG
  case 12:                                  // Vector 12: TXIFG
    if (TXByteCtr)                          // Check TX byte counter
    {
      UCB1TXBUF = TXData;                   // Load TX buffer
      TXByteCtr--;                          // Decrement TX byte counter
    }
    else
    {
      UCB1CTL1 |= UCTXSTP;                  // I2C stop condition
      UCB1IFG &= ~UCTXIFG;                  // Clear USCI_B1 TX int flag
      __bic_SR_register_on_exit(LPM0_bits); // Exit LPM0
    }
    break;
  default: break;
  }
}












