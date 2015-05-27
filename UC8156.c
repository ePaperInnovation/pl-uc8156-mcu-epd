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
#ifdef WORKAROUND_FOR_SOO=1_BUG
	spi_write_command_1param(0x01, 0x10); //GVS=1 (p-mos, selected gate to VGL and non-selected gate to VGH) + SOO=1
#else
	spi_write_command_1param(0x01, 0x12); //GVS=1 (p-mos, selected gate to VGL and non-selected gate to VGH) + SOO=1
#endif
	spi_write_command_2params(0x02, 0x25, 0xFF); // set Vgate to +17V/-25V
	spi_write_command_2params(0x06, 0x67, 0x55); // set timing to LAT=105, S2G+G2S=5
	spi_write_command_2params(0x18, 0x40, 0x02); //BPCOM=GND, TPCOM=Hi-Z after update, gate_out=VGH after update

	// image RAM configuration
	spi_write_command_1param(0x0f, 0x02); //DEM=010 --> Y-decrement
	spi_write_command_4params(0x0c, 0x00, SOURCE_LINES, GATE_LINES_MAX-GATE_LINES, GATE_LINES_MAX-1); // Panel resolution setting --> SOURCE_E needs to be SOURCELINE instead of SOURCELINE-1 for 180x100, don't know why
	spi_write_command_4params(0x0d, 0x00, SOURCE_LINES-1, GATE_LINES_MAX-GATE_LINES, GATE_LINES_MAX-1); // RAM window setup
	spi_write_command_2params(0x0e, 0x00, GATE_LINES_MAX-1); //start Y from 159d/9fh, related to R0fh/DEM setting

	//spi_write_command_4params(0x18, 0x40, 0x01, 0x24, 0x0a); //BPCOM=GND, TPCOM=Hi-Z after update, gate_out=VGH after update

#ifdef WORKAROUND_FOR_VCOM2VBORDER_BUG //work-around for TPCOM_to_VBorder short on first 1.38inch displays
	spi_write_command_4params(0x18, 0x00, 0x00, 0x24, 0x07); //BPCOM=GND, TPCOM=GND after update, gate_out=GND after update
	spi_write_command_1param(0x1d, 0x04); //set Vborder to Vcom
	spi_write_command_3params(0x1f, 0x00, 0x00, 0x00); //set all NF's and waiting times to 0
#endif
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
