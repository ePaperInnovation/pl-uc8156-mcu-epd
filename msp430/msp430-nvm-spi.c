/*
 * msp430-nvm-spi.c
 *
 *  Created on: 03.01.2022
 *      Author: zhongke.dai
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <msp430.h>

#include "types.h"
#include "assert.h"
#include "utils.h"

#include "msp430-defs.h"
#include "msp430-spi.h"
#include "msp430-gpio.h"

#include "msp430-nvm-spi.h"



uint8_t send_cmd(uint8_t cmd)
{

    uint8_t stat = spi_write_read_byte(cmd);
    return stat;
}




uint8_t read_para()
{
    uint8_t stat = spi_write_read_byte(0x00);
    return stat;
}




uint8_t write_pgm(u8 *data, int transferSize)
{

    int i = transferSize;
    while(i > 0)
     {
          spi_write_read_byte(*data);
          data++;
          i--;
     }
    return 0;
}




 int MX25U4033E_check_prod_code()
{
    uint8_t rdid[3];
    gpio_set_value_hi(SPI_CS_SLAVE);
    gpio_set_value_lo(SPI_CS_SLAVE);
    send_cmd(MX25U4033E_RDID);
    rdid[0] = spi_write_read_byte(0x00);
    rdid[1] = spi_write_read_byte(0x00);
    rdid[2] = spi_write_read_byte(0x00);

    gpio_set_value_hi(SPI_CS_SLAVE);

 //   printf("Manufacture/Device ID: 0x%x 0x%x 0x%x \n", rdid[0], rdid[1], rdid[2]);

//    uint32_t id = rdid[0] << 16;
//    id |= rdid[1] << 8;
//    id |= rdid[2];
//
    if ( ((rdid[0] == 0xff) && (rdid[1] == 0xff) && (rdid[2] == 0xff))    || ((rdid[0] == 0x00) && (rdid[1] == 0x00) && (rdid[2] == 0x00)) ) {
        printf("Invalid Manufacture/Device ID. \n");
        return -1;
    }

    return 0;
}


int nvm_MX25U4033E_spi_pgm(uint32_t addr, uint8_t * blob, int len,  uint8_t *data)     // write the program to the external flash
{
     int stat = -1;
     uint32_t register_address = addr;
     uint8_t reg[3];
     int chunkSize = 64;
     uint32_t byte_offset = 0;
     int bytes_to_transfer = len;


     uint8_t cmp_blob[len];
     uint8_t data_read[256];


     // check product code
     if(MX25U4033E_check_prod_code())
     {
         printf("NVM product code check failed.");

         return -1;
     }


//     // send write enable
//     gpio_set_value_lo(SPI_CS_SLAVE);
//     stat = send_cmd(MX25U4033E_WREN);
//     gpio_set_value_hi(SPI_CS_SLAVE);
//
//     // send chip erase
//     gpio_set_value_lo(SPI_CS_SLAVE);
//     stat = send_cmd(MX25U4033E_CE);
//     //uint8_t reg_[3] = {0x00, 0x00, 0x00};
//     //stat = spi->write_bytes(spi, reg_, 3);
//     gpio_set_value_hi(SPI_CS_SLAVE);

 //    mdelay(5000);

     // poll for chip erase has finished (WEL=0, WIP=0)
     if(MX25U4033E_wait_idle())
     {
         printf("Wait for idle after chip erase failed.");
         return -1;
     }

     // write new data to the flash
     while(bytes_to_transfer > 0){

         // transfer chunkSize or bytes to transfer
         size_t transferChunkSize = (bytes_to_transfer >= chunkSize) ? chunkSize : bytes_to_transfer;

         reg[0] = (register_address >> 16) & 0xff;
         reg[1] = (register_address >> 8) & 0xff;
         reg[2] = (uint8_t) register_address;

         data = &(blob[byte_offset]);

         // send write enable
         gpio_set_value_lo(SPI_CS_SLAVE);
         stat = send_cmd(MX25U4033E_WREN);
         gpio_set_value_hi(SPI_CS_SLAVE);

         // write data
         gpio_set_value_lo(SPI_CS_SLAVE);
         stat = send_cmd( MX25U4033E_PP);                        // write page program command
         stat |=  send_cmd(reg[0]);
         stat |=  send_cmd(reg[1]);
         stat |=  send_cmd(reg[2]);                     // write 3-byte address


         stat =   write_pgm(data, transferChunkSize);      //  stat = spi->write_bytes(spi, data, transferChunkSize);      // write data
         gpio_set_value_hi(SPI_CS_SLAVE);




         // poll for page programming has finished (WEL=0, WIP=0)
         if(MX25U4033E_wait_idle())
         {
            printf("Wait for idle after page programming failed.");
            return -1;
         }

         byte_offset += transferChunkSize;
         register_address += transferChunkSize;
         bytes_to_transfer -= transferChunkSize;
     }

     nvm_MX25U4033E_spi_read(addr, cmp_blob, len, data_read);

         if (memcmp(blob, cmp_blob, len) != 0)
         {

             printf("blob[0] = %X, %X \n", blob[0], cmp_blob[0]);
             printf("blob[1] = %X, %X \n", blob[1], cmp_blob[1]);
             printf("blob[2] = %X, %X \n", blob[2], cmp_blob[2]);


             //printf("NVM programming finished: verify after pgm failed!\n");

             return -1;
         }
//         else
//         {
//             printf("blob[0] = %X, %X \n", blob[0], cmp_blob[0]);
//             printf("blob[1] = %X, %X \n", blob[1], cmp_blob[1]);
//             printf("blob[2] = %X, %X \n", blob[2], cmp_blob[2]);
//          //   printf("NVM programming finished: verification successful!\n");
//         }

         return stat;
}





int nvm_MX25U4033E_spi_read(uint32_t addr, uint8_t * blob, int len ,  uint8_t *data_read)    // read from the external flash
{
         int stat = 0;
         uint32_t register_address = addr;
         uint8_t reg[3];
         static const int chunkSize = 256;
         uint32_t byte_offset = 0;
         int bytes_to_transfer = len;
       //  uint8_t *data;


         // open spi
//         stat = spi->open(spi);
//         spi->msh = 12000000; //SPI_TRANSFER_RATE_IN_HZ --> max. 12Mhz for long cable;

         // check product code
         if(MX25U4033E_check_prod_code())
         {
             printf("NVM product code check failed.");
             return -1;
         }

         while(bytes_to_transfer > 0){

             // transfer chunkSize or bytes to transfer
             size_t transferChunkSize = (bytes_to_transfer >= chunkSize) ? chunkSize : bytes_to_transfer;   // choose the bigger one from chunkSize and bytes_to_transfer

             reg[0] = (register_address >> 16) & 0xff;
             reg[1] = (register_address >> 8) & 0xff;
             reg[2] = (uint8_t) register_address;

             data_read = &(blob[byte_offset]);

             gpio_set_value_lo(SPI_CS_SLAVE);


             stat |=  send_cmd(MX25U4033E_READ);         // read command
             stat |=  send_cmd(reg[0]);
             stat |=  send_cmd(reg[1]);
             stat |=  send_cmd(reg[2]);



          //   stat |= spi->read_bytes(spi, data, transferChunkSize);      // read data

            int i = transferChunkSize;
            while(i > 0)
             {
                 *data_read = read_para();
                 data_read++;
                  i--;
             }
             gpio_set_value_hi(SPI_CS_SLAVE);
             if(stat < 0)
             {
                 printf("Read from NVM failed.");

                 return -1;
             }

             byte_offset += transferChunkSize;
             register_address += transferChunkSize;
             bytes_to_transfer -= transferChunkSize;
         }

         if(MX25U4033E_wait_idle())
         {
             printf("Wait for idle after chip erase failed.");
             return -1;
         }

         return 0;
}



int MX25U4033E_wait_idle()
{
     uint8_t buf = 1;

         unsigned long timeout = 60000; // ca. 6s


         while (buf >= MX25U4033E_STATUS_WIP)
         {

             gpio_set_value_lo(SPI_CS_SLAVE);
             send_cmd(MX25U4033E_RDSR);
             buf = read_para();
             gpio_set_value_hi(SPI_CS_SLAVE);
             mdelay(1);
             --timeout;

             if (timeout == 0)
             {
                 printf("WIP timeout.");
                 return -1;

             }
         }


         return 0;
}


int MX25U4033E_chip_erase()
{
         // send write enable
         gpio_set_value_lo(SPI_CS_SLAVE);
         send_cmd(MX25U4033E_WREN);
         gpio_set_value_hi(SPI_CS_SLAVE);

         // send chip erase
         gpio_set_value_lo(SPI_CS_SLAVE);
         send_cmd(MX25U4033E_CE);
         //uint8_t reg_[3] = {0x00, 0x00, 0x00};
         //stat = spi->write_bytes(spi, reg_, 3);
         gpio_set_value_hi(SPI_CS_SLAVE);

      //   mdelay(5000);

         // poll for chip erase has finished (WEL=0, WIP=0)
         if(MX25U4033E_wait_idle())
         {
             printf("Wait for idle after chip erase failed. /n");
             return -1;
         }

         return 0;
}


int check_waveform_length()
{

    return 0;
}


int nvm_MX25U4033E_spi_pgm_no_check(uint32_t addr, uint8_t * blob, int len , uint8_t * data)     // write the program to the external flash
{
     int stat = -1;
     uint32_t register_address = addr;
     uint8_t reg[3];
     int chunkSize = 64;
     uint32_t byte_offset = 0;
     int bytes_to_transfer = len;


     // check product code
     if(MX25U4033E_check_prod_code())
     {
         printf("NVM product code check failed.");

         return -1;
     }


     if(MX25U4033E_wait_idle())
     {
         printf("Wait for idle after chip erase failed.");
         return -1;
     }

     // write new data to the flash
     while(bytes_to_transfer > 0){

         // transfer chunkSize or bytes to transfer
         size_t transferChunkSize = (bytes_to_transfer >= chunkSize) ? chunkSize : bytes_to_transfer;

         reg[0] = (register_address >> 16) & 0xff;
         reg[1] = (register_address >> 8) & 0xff;
         reg[2] = (uint8_t) register_address;

         data = &(blob[byte_offset]);

         // send write enable
         gpio_set_value_lo(SPI_CS_SLAVE);
         stat = send_cmd(MX25U4033E_WREN);
         gpio_set_value_hi(SPI_CS_SLAVE);

         // write data
         gpio_set_value_lo(SPI_CS_SLAVE);
         stat = send_cmd( MX25U4033E_PP);                        // write page program command
         stat |=  send_cmd(reg[0]);
         stat |=  send_cmd(reg[1]);
         stat |=  send_cmd(reg[2]);                     // write 3-byte address


         stat =   write_pgm(data, transferChunkSize);      //  stat = spi->write_bytes(spi, data, transferChunkSize);      // write data
         gpio_set_value_hi(SPI_CS_SLAVE);




         // poll for page programming has finished (WEL=0, WIP=0)
         if(MX25U4033E_wait_idle())
         {
            printf("Wait for idle after page programming failed.");
            return -1;
         }

         byte_offset += transferChunkSize;
         register_address += transferChunkSize;
         bytes_to_transfer -= transferChunkSize;
     }


         return stat;
}




