/*
 * UC8177C.c
 *
 *  Created on: 01.11.2021
 *      Author: zhongke.dai
 */


#include <msp430.h>
#include <stdio.h>
#include <stdlib.h>
#include "msp430/msp430-spi.h"
#include "msp430/msp430-gpio.h"
#include "types.h"
#include "utils.h"
#include <math.h>
#include <UC8177C.h>



void UC8177_hardware_reset(void)
{
    gpio_set_value_lo(PIN_RESET);
    mdelay(1);
    gpio_set_value_hi(PIN_RESET);
}

unsigned int UC8177_wait_for_BUSY_inactive()
{
    unsigned long counter=0;
    while (gpio_get_value(PIN_BUSY)==0) // BUSY loop
    {
        mdelay(1);
        counter++; // BUSY loop
        if (counter>2000) abort_now("Busy-Loop Timeout", ABORT_UC8177_INIT);
    }
    return counter;
}

void UC8177_wait_for_BUSY_inactive_debug(void)
{
   printf("BUSY loop counter = %d\n", UC8177_wait_for_BUSY_inactive());
}

void UC8177_wait_for_PowerON_ready_debug(void)
 {
    unsigned long counter = UC8177_wait_for_PowerON_ready();
    //printf("PowerOn loop counter = %d\n", counter);
 }


void UC8177_check_RevID(void)
{
    u8 revID = UC8177_spi_read_command_1param(0x70) & 0x0F;
    printf("RevID = %x\n", revID);
}


void UC8177_PSR(u8 param1, u8 param2)    // Panel Setting
{
   UC8177_spi_write_command_2params(0x00, param1, param2);
}


void UC8177_PWR(u8 param1, u8 param2, u8 param3, u8 param4)  // Power Setting
{
    UC8177_spi_write_command_4params(0x01 ,param1, param2, param3, param4);
}


void UC8177_POF(void)    //Power OFF
{
    UC8177_spi_write_only_command(0x02);
}

void UC8177_PFS(u8 param1)  // Power Off Sequence Setting
{
    UC8177_spi_write_command_1param(0x03, param1);
}


void UC8177_PON(void)    //Power On
{
    UC8177_spi_write_only_command(0x04);
}


void UC8177_DPC(u8 param1)  // Dither Process Command
{
    UC8177_spi_write_command_1param(0x05, param1);
}



void UC8177_DSLP(u8 param1)  // Deep Sleep
{
    UC8177_spi_write_command_1param(0x06, param1);
}


void UC8177_BTST(u8 param1, u8 param2, u8 param3)  // Booster Soft Start
{
    UC8177_spi_write_command_3params(0x07, param1, param2, param3);

}


void UC8177_DTM1(u8 param1, u8 *image_data, size_t size)  // Data Start Transmission 1
{
    UC8177_spi_write_command_param_and_bulk_data(0x10, param1, image_data, size);

}

void UC8177_DRF(u8 param1, u8 param2, u8 param3, u8 param4, u8 param5, u8 param6, u8 param7, u8 param8, u8 param9)     // Display Refresh
{
    UC8177_spi_write_command_9params(0x12, param1, param2, param3, param4, param5, param6, param7, param8, param9);
}


void UC8177_DTM2(u8 param1, u8 *image_data, size_t size)  // Data Start Transmission 2
{
    UC8177_spi_write_command_param_and_bulk_data(0x13, param1, image_data, size);
    UC8177_spi_write_only_command(0x11);
}

void UC8177_AWM1(u8 param1)  // Auto Waveform Mode 1
{
    UC8177_spi_write_command_1param(0x15, param1);
}


void UC8177_AWM2(u8 param1, u8 param2, u8 param3, u8 param4) // Auto Waveform Mode 2
{
    UC8177_spi_write_command_4params(0x16, param1, param2, param3, param4);
}

void UC8177_LUTC(u8 *waveform_lutc)     // VCOM LUT(LUTC)
{
    // spi_write_command_and_bulk_data(0x20, waveform_lutc, 64);   // 256/4
}


void UC8177_LUTD(u8 param1, u8 *lutd, size_t size)  // LUT for Frame Data (LUTD)
{
   // spi_write_command_param_and_bulk_data(0x21, param1, lutd, size);
}

bool UC8177_Send_WaveformFile_to_LUTD(char *wf_path)  // LUT for Frame Data (LUTD)
{
    FIL file;
    static int BUFFER_LENGTH = 64;
    u8 *data = (u8 *) malloc(BUFFER_LENGTH);
    uint16_t count;
    uint16_t size;

    if (f_open(&file, wf_path, FA_READ))
        return false;

    gpio_set_value_lo(SPI_CS);

    //command
    u8 command = 0x21;
    spi_write_read_byte(command);

    //frame number
    f_read(&file, data, 1, &count);
    u8 frame_number = *data;
    spi_write_read_byte(frame_number);

    //jump (over LUTC) to LUTD
    f_lseek(&file, 16 + (frame_number/64 + 1) * 16);

    do
    {
        if (f_read(&file, data, BUFFER_LENGTH, &count) != FR_OK)
            return false;

        size = count;

        while (size--)
        {
            spi_write_read_byte(*data);
            data++;
        }

    } while (count);

    gpio_set_value_hi(SPI_CS);

    f_close(&file);

    if (count!=BUFFER_LENGTH)
        return false;

    return(true);
}


void UC8177_LUTR(u8 *lutr)  // LUT for Reagle(lutr)
{
    UC8177_spi_write_command_and_bulk_data(0x22, lutr, 64);
}


void UC8177_PS(u8 param1)  // Power Saving
{
    UC8177_spi_write_command_1param(0x26, param1);
}


void UC8177_MISCS1(u8 param1)  // Miscellaneous setting 1
{
    UC8177_spi_write_command_1param(0x27, param1);
}

void UC8177_PLL(u8 param1)  // PLL Control
{
    UC8177_spi_write_command_1param(0x30, param1);
}


void UC8177_TSC(u8 param1, u8 param2)  // Temperature sensor calibration
{
    UC8177_spi_write_command_2params(0x40, param1, param2);
}


void UC8177_TSE(u8 param1, u8 param2)  // Temperature sensor enable
{
    UC8177_spi_write_command_2params(0x41, param1, param2);
}


void UC8177_CDI(u8 param1, u8 param2)  // Vcom and Data interval setting
{
    UC8177_spi_write_command_2params(0x50, param1, param2);
}


u8 UC8177_LPD(void) // lower power detection
{
    u8 LPD = UC8177_spi_read_command_1param(0x51) & 0x01;
    return LPD;

}


void UC8177_S8S9SPI(void)    //S8 S9 SPI
{
    UC8177_spi_write_only_command(0x54);
}

void UC8177_TCON(u8 param1, u8 param2, u8 param3)  // TCON Setting
{
    UC8177_spi_write_command_3params(0x60, param1, param2, param3);
}


void UC8177_TRES(u8 param1, u8 param2, u8 param3, u8 param4)  // Resolution Setting
{
    UC8177_spi_write_command_4params(0x61, param1, param2, param3, param4);
}


void UC8177_DAM(u8 param1)  // SPI FLASH Control
{
    UC8177_spi_write_command_1param(0x65, param1);
}

void UC8177_FLG(u8 * data)  // get flag status
{
    UC8177_spi_read_command_2params1(0x71, data);
}

//u8* UC8177_PIPE_COL_FLG(void)  // pipe collision status
//{
//    u8 *pipe_flag_state = spi_read_command_2params1(0x72);
//     return pipe_flag_state;
//}
//
//
//
//u8* UC8177_PIPE_BUSY_FLG(void)  // pipe busy status
//{
//    u8 *pipe_busy_flag_state = spi_read_command_2params1(0x73);
//     return pipe_busy_flag_state;
//}


void UC8177_AMV(u8 param1)  // Auto measure Vcom
{
    UC8177_spi_write_command_1param(0x80, param1);
}

u8 UC8177_VV(void) // Vcom Value
{
    u8 VV = UC8177_spi_read_command_1param(0x81);
    return VV;

}


void UC8177_VDCS(u8 param1)  // VCOM DC Setting
{
   UC8177_spi_write_command_1param(0x82, param1);
}

void UC8177_DTMW(u8 param1, u8 param2, u8 param3, u8 param4, u8 param5, u8 param6, u8 param7, u8 param8)     // Data start transmission window
{
    UC8177_spi_write_command_8params(0x83, param1, param2, param3, param4, param5, param6, param7, param8);
}

void UC8177_EDS(u8 param1)  // EPD Discharge Setting
{
    UC8177_spi_write_command_1param(0x84, param1);
}

void UC8177_XONS(u8 param1, u8 param2, u8 param3, u8 param4, u8 param5, u8 param6)     // XON Setting
{
    UC8177_spi_write_command_6params(0x85, param1, param2, param3, param4, param5, param6);
}

void UC8177_LEDDS(u8 param1, u8 param2)  // LED Driver Setting
{
    UC8177_spi_write_command_2params(0x90, param1, param2);
}

void UC8177_PBC(u8 param1)  // Panel Break check
{
    UC8177_spi_write_command_1param(0x91, param1);
}

u8 UC8177_PBCS(void) // Panel break check status
{
    u8 PBCS = UC8177_spi_read_command_1param(0x92) & 0x01;
    return PBCS;

}


void UC8177_GDOS(u8 param1, u8 param2)  // GD Order Setting
{
    UC8177_spi_write_command_2params(0xE0, param1, param2);
}




void UC8177_set_Vcom(int Vcom_mv_value)  // eg: -4V ---> 4000
{
    u8 Vcom_register_value = (u8)Vcom_mv_value/50;
    UC8177_VDCS(Vcom_register_value);
}

//u8 UC8177_BUSY_N_check(void)
//{
//
//    u8 *flag_byte_array = UC8177_FLG();
//    u8 busy_status_byte = flag_byte_array[0] & 0x01;
//
//    return busy_status_byte ;
//
//}



