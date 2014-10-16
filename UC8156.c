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

// UC8156 hardware reset
void UC8156_reset()
{
	gpio_set_value_lo(PIN_RESET);
	mdelay(5);
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
	spi_write_command_4params(0x0C, 0x00, 0xEF, 0x00, 0x9F); //panel resolution setting
	spi_write_command_2params(0x05, 0x01, 0x01); //VCOM and data interval setting
	spi_write_command_2params(0x06, 0x03, 0x11); //TCON timing setting
	spi_write_command_3params(0x1F, 0x11, 0x11, 0x10); //TCON timing setting
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
	spi_write_command_1param (0x03, 0xD0); //Power control setting --> switch on CLKEN+PWRON bits
	UC8156_wait_for_BUSY_inactive();
	spi_write_command_1param (0x03, 0xC0); //Power control setting --> switch on CLKEN+PWRON bits
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
	spi_write_command_and_bulk_data(0x1C, waveform, 120);
}

//send an image to UC8156 image data memory
void UC8156_send_image_data(u8 *image_data)
{
	spi_write_command_and_bulk_data(0x10, image_data, PIXEL_COUNT/4);
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
	spi_write_command_1param(0x14, mode);
	UC8156_wait_for_BUSY_inactive();
}
