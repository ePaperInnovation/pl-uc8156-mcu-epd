/*
 * msp430-nvm-spi.h
 *
 *  Created on: 03.01.2022
 *      Author: zhongke.dai
 */

#ifndef MSP430_MSP430_NVM_SPI_H_
#define MSP430_MSP430_NVM_SPI_H_


#define MX25U4033E_SIZE     (0x80000)   // max size

// Register Address
#define MX25U4033E_PP       (0x02)  // page program
#define MX25U4033E_READ     (0x03)  // read
#define MX25U4033E_RDSR     (0x05)  // read status register
#define MX25U4033E_WREN     (0x06)  // write enable
#define MX25U4033E_SE       (0x20)  // sector erase
#define MX25U4033E_CE       (0x60)  // chip erase
#define MX25U4033E_RDID     (0x9f)  // read identification

#define MX25U4033E_MANID_DEVID (0xc22014) // 1xbyte Manufacture id 2xbyte device id

// Status Register Bits
#define MX25U4033E_STATUS_WIP   (1     ) // write in progress
#define MX25U4033E_STATUS_WEL   (1 << 1) // write enable latch


uint8_t send_cmd(uint8_t cmd);
uint8_t read_para();




int MX25U4033E_check_prod_code();
int MX25U4033E_wait_idle();
int MX25U4033E_chip_erase();
int nvm_MX25U4033E_spi_read(uint32_t addr, uint8_t * blob, int len ,  uint8_t *data_read);
int nvm_MX25U4033E_spi_pgm(uint32_t addr, uint8_t * blob, int len ,  uint8_t *data);     // write the program to the external flash
int nvm_MX25U4033E_spi_pgm_no_check(uint32_t addr, uint8_t * blob, int len , uint8_t * data) ;








#endif /* MSP430_MSP430_NVM_SPI_H_ */
