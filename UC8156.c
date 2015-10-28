/*
 * solomon.c
 *
 * Author: andreas.meier
 */

#include <msp430.h>
#include <stdio.h>

#include "UC8156.h"
#include "msp430/msp430-spi.h"
#include "msp430/msp430-gpio.h"
#include "config.h"
#include "types.h"

//global variables
u8 x_start, y_start;

// UC8156 hardware reset
void UC8156_hardware_reset()
{
	gpio_set_value_lo(PIN_RESET);
	mdelay(1);
	gpio_set_value_hi(PIN_RESET);
}

// waits for BUSY getting inactive = 1 (BUSY pin is low-active)
void UC8156_wait_for_BUSY_inactive()
{
 	while (gpio_get_value(PIN_BUSY)==0); // BUSY loop
}

// waits for BUSY getting inactive = 1 (BUSY pin is low-active)
unsigned long UC8156_wait_for_BUSY_inactive_debug()
{
	unsigned long counter=0;
 	while (gpio_get_value(PIN_BUSY)==0) // BUSY loop
		counter++; // BUSY loop
	return counter;
}

// UC8156 change registers which need values different from power-up values
void UC8156_init_registers()
{
	int i;

	for (i=0; i<sizeof(reg_settings)/sizeof(regSetting_t); i++) {
		spi_write_command(reg_settings[i].addr, reg_settings[i].val, reg_settings[i].valCount);
	}
}

// UC8156 HV power-on (enable charge pumps, execute power-on sequence for outputs)
void UC8156_HVs_on()
{
	u8 reg_value = spi_read_command_1param (0x03); //read power control setting register
	reg_value |= 0x11; //switch on CLKEN+PWRON bits
	spi_write_command_1param (0x03, reg_value); //write power control setting register --> switch on CLKEN+PWRON bits
	UC8156_wait_for_BUSY_inactive();
 	while (spi_read_command_1param(0x15)!=4); // BUSY loop
}

// UC8156 power-off sequence
void UC8156_HVs_off()
{
	u8 reg_value = spi_read_command_1param (0x03); //read power control setting register
	reg_value &= ~0x01; //switch off PWRON bit
	spi_write_command_1param (0x03, reg_value); //write power control setting register
	UC8156_wait_for_BUSY_inactive();
	reg_value &= ~0x10; //switch off CLKEN bit
	spi_write_command_1param (0x03, reg_value);
}

u8 UC8156_read_RevID()
{
	return spi_read_command_1param(0x00);
}

// send Vcom value (in mV) to UC8156
void UC8156_set_Vcom(int Vcom_mv_value)
{
	u16 Vcom_register_value = (float)Vcom_mv_value/(float)30.0;
	spi_write_command_2params(0x1B, (u8)Vcom_register_value, (u8)((Vcom_register_value>>8)&0x03));
}

// send waveform to UC8156
void UC8156_send_waveform(u8 *waveform)
{
	spi_write_command_and_bulk_data(0x1C, waveform, WAVEFORM_LENGTH);
}

//send an image to UC8156 image data memory
void UC8156_send_image_data(u8 *image_data)
{
	spi_write_command_and_bulk_data(0x10, image_data, PIXEL_COUNT/4);
}

//send any data to UC8156 image data memory --> e.g. used for MTP programming
void UC8156_send_data_to_image_RAM_for_MTP_program(u8 *waveform_data, size_t size)
{
	spi_write_command_and_bulk_data(0x10, waveform_data, size);
}

//send an repeated byte to the image buffer --> used to create a solid image like all white
void UC8156_send_repeated_image_data(u8 image_data)
{
	spi_write_command_byte_repeat(0x10, image_data, PIXEL_COUNT/4);
}

//update display using full update mode and wait for BUSY-pin low
void UC8156_update_display_full()
{
	spi_write_command_1param(0x14, 0x01);
	UC8156_wait_for_BUSY_inactive();
}

//update display and wait for BUSY-pin low
void UC8156_update_display(u8 mode)
{
	spi_write_command_1param(0x14, UPDATE_WAVEFORMSOURCESELECT | mode | 1);
	UC8156_wait_for_BUSY_inactive();
}

void measure_Vcom()
{
	u8 return_value;

	spi_write_command_4params(0x18, 0x40, 0x01,0x24, 0x05); //TPCOM=Hi-Z before update and during null-frame drive

	UC8156_HVs_on();
	return_value = 	spi_read_command_1param(0x15);
	fprintf(stderr, "R15h after HV_on = %x\n", return_value);
	//spi_write_command_1param(0x19, 0x29); //trigger Vcom sensing with waiting time 5sec
	spi_write_command_1param(0x19, 0x81); //trigger Vcom sensing with waiting time 5sec


	int i;
	u8 value[60][2];
	for (i=0;i<60;i++)
	{
		spi_read_command_2params1(0x1a, &value[i][0]);
		mdelay(250);
	}

	UC8156_wait_for_BUSY_inactive(); // wait for power-up completed
	UC8156_HVs_off();

	for (i=0;i<60;i++)
	{
		fprintf(stderr, "%f sec = %f V\n", i*0.25, value[i][0] * 0.03);
	}
}
