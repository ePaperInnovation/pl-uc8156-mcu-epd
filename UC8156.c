/*
  Plastic Logic EPD project on MSP430

  Copyright (C) 2013 - 2017 Plastic Logic

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
/*
 * UC8156.c
 *
 * Author: andreas.meier
 */

#include <msp430.h>
#include <stdio.h>
#include <stdlib.h>
#include "UC8156.h"
#include "msp430/msp430-spi.h"
#include "msp430/msp430-gpio.h"
#include "types.h"
#include "utils.h"

//global variables
u8 UPDATE_COMMAND_WAVEFORMSOURCESELECT_PARAM = WAVEFORM_FROM_MTP;
extern u16 PIXEL_COUNT; //global variable
extern regSetting_t *REG_SETTINGS; //global variable
extern u8 NUMBER_OF_REGISTER_OVERWRITES; //global variable

// UC8156 hardware reset
void UC8156_hardware_reset()
{
	gpio_set_value_lo(PIN_RESET);
	mdelay(1);
	gpio_set_value_hi(PIN_RESET);
}

// waits for BUSY getting inactive = 1 (BUSY pin is low-active)
unsigned int UC8156_wait_for_BUSY_inactive_slave()
{
 	unsigned long counter=0;
  	while (gpio_get_value(PIN_BUSY_SLAVE)==0) // BUSY loop
  	{
  		mdelay(1);
 		counter++; // BUSY loop
 		if (counter>1000) abort_now("Busy-Loop Timeout", ABORT_UC8156_INIT);
  	}
  	return counter;
}

unsigned int UC8156_wait_for_BUSY_inactive()
{
	unsigned long counter=0;
 	while (gpio_get_value(PIN_BUSY)==0) // BUSY loop
 	{
 		mdelay(1);
		counter++; // BUSY loop
		if (counter>2000) abort_now("Busy-Loop Timeout", ABORT_UC8156_INIT);
 	}
 	return counter;
}

 // waits for BUSY getting inactive = 1 (BUSY pin is low-active)
 void UC8156_wait_for_BUSY_inactive_debug()
 {
 	printf("BUSY loop counter = %d\n", UC8156_wait_for_BUSY_inactive());
 }

// waits for Power_ON RDY
unsigned long UC8156_wait_for_PowerON_ready()
 {
 	unsigned long counter=0;
 	while (spi_read_command_1param(0x15)!=4 && counter<1000)
 	{
  		mdelay(1);
 		counter++; // BUSY loop
  	}
 	return(counter);
 }

// waits for Power_ON RDY with timeout
unsigned long UC8156_wait_for_PowerON_ready_timeout()
 {
 	unsigned long counter=0;
 	do
 	{
  		mdelay(1);
 		counter++; // BUSY loop
  	}
 	while (spi_read_command_1param(0x15)!=4 && counter < 1000);
 	if (counter >= 1000)
		abort_now("HV not enabled.\n", ABORT_UC8156_INIT);
 	return(counter);
 }

// waits for Power_ON RDY with timeout
unsigned long UC8156_wait_for_PowerON_ready_slave()
 {
 	unsigned long counter=0;
 	do
 	{
  		mdelay(1);
 		counter++; // BUSY loop
  	}
 	while (spi_read_command_1param_slave(0x15)!=4 && counter < 1000);
 	if (counter >= 1000)
		abort_now("HV not enabled.\n", ABORT_UC8156_INIT);
 	return(counter);
 }

// waits for Power_ON RDY -> print-out counter
void UC8156_wait_for_PowerON_ready_debug()
 {
	unsigned long counter = UC8156_wait_for_PowerON_ready();
 	//printf("PowerOn loop counter = %d\n", counter);
 }

// UC8156 change registers which need values different from power-up values
void UC8156_init_registers()
{
	int i;

	for (i=0; i<NUMBER_OF_REGISTER_OVERWRITES; i++)
	    spi_write_command((REG_SETTINGS+i)->addr, (REG_SETTINGS+i)->val, (REG_SETTINGS+i)->valCount);
}

void UC8156_print_registers()
{
	int i;

	for (i=0; i<NUMBER_OF_REGISTER_OVERWRITES; i++)
		print_spi_read_command((REG_SETTINGS+i)->addr, (REG_SETTINGS+i)->valCount);
}

// UC8156 change registers which need values different from power-up values
void UC8156_init_registers_slave()
{
	int i;

	for (i=0; i<NUMBER_OF_REGISTER_OVERWRITES; i++)
		spi_write_command_slave((REG_SETTINGS+i)->addr, (REG_SETTINGS+i)->val, (REG_SETTINGS+i)->valCount);
}

void UC8156_print_registers_slave()
{
	int i;

	for (i=0; i<NUMBER_OF_REGISTER_OVERWRITES; i++)
		print_spi_read_command_slave((REG_SETTINGS+i)->addr, (REG_SETTINGS+i)->valCount);
}

// waits for Power_OFF RDY
unsigned long UC8156_wait_for_PowerOFF_ready()
 {
 	unsigned long counter=0;

 	while ((spi_read_command_1param(0x15)&4) && counter<1000)
 	{
  		mdelay(1);
 		counter++;
  	}

// 	printf("Power-Off Counter=%d\n", counter);

 	return(counter);
 }

// waits for Power_OFF RDY
unsigned long UC8156_wait_for_PowerOFF_ready_slave()
 {
 	unsigned long counter=0;

 	while ((spi_read_command_1param_slave(0x15)&4) && counter<1000)
 	{
  		mdelay(1);
 		counter++;
  	}

// 	printf("Power-Off Counter=%d\n", counter);

 	return(counter);
 }


// UC8156 HV power-on (enable charge pumps, execute power-on sequence for outputs)
void UC8156_HVs_on()
{
	u8 reg_value = spi_read_command_1param(0x03); //read power control setting register
	reg_value |= 0x11; //switch on CLKEN+PWRON bits
	spi_write_command_1param (0x03, reg_value); //write power control setting register --> switch on CLKEN+PWRON bits

	UC8156_wait_for_PowerON_ready();
//	UC8156_wait_for_PowerON_ready_debug();
}

void UC8156_HVs_on_slave()
{
	u8 reg_value = spi_read_command_1param_slave(0x03); //read power control setting register
//	reg_value |= 0x01; //switch on CLKEN+PWRON bits
	reg_value |= 0x07; //switch on CLKEN+PWRON bits
	spi_write_command_1param_slave(0x03, reg_value); //write power control setting register --> switch on CLKEN+PWRON bits

	UC8156_wait_for_PowerON_ready_slave();
}

void UC8156_HVs_on_dual()
{
	u8 reg_value_master = spi_read_command_1param(0x03); //read power control setting register
	u8 reg_value_slave  = spi_read_command_1param_slave(0x03); //read power control setting register
	reg_value_master |= 0x01; //switch on CLKEN+PWRON bits

	reg_value_slave |= 0x07; //switch on CLKEN+PWRON bits
	spi_write_command_1param(0x03, reg_value_master); //write power control setting register --> switch on CLKEN+PWRON bits
	spi_write_command_1param_slave(0x03, reg_value_slave); //write power control setting register --> switch on CLKEN+PWRON bits

	UC8156_wait_for_PowerON_ready(); // BUSY loop -> wait for BUSY inactive

}

// UC8156 power-off sequence
void UC8156_HVs_off()
{
	u8 reg_value = spi_read_command_1param(0x03); //read power control setting register
	reg_value &= ~0x01; //switch off PWRON bit
	spi_write_command_1param (0x03, reg_value); //write power control setting register
	UC8156_wait_for_BUSY_inactive();
	reg_value &= ~0x10; //switch off CLKEN bit
	spi_write_command_1param (0x03, reg_value);
}

void UC8156_HVs_off_slave(int pin)
{
	u8 reg_value = spi_read_command_1param_slave(0x03); //read power control setting register
	reg_value &= ~0x01; //switch off PWRON bit
	spi_write_command_1param_slave(0x03, reg_value); //write power control setting register
	UC8156_wait_for_PowerOFF_ready_slave();
	UC8156_wait_for_BUSY_inactive_slave();
}

// UC8156 power-off sequence
void UC8156_HVs_off_dual()
{
	u8 reg_value_master = spi_read_command_1param(0x03); //read power control setting register
	u8 reg_value_slave  = spi_read_command_1param_slave(0x03); //read power control setting register
	reg_value_master &= ~0x01; // reset PWRON bit
	reg_value_slave &= ~0x01; //reset PWRON bit
	spi_write_command_1param(0x03, reg_value_master); //write power control setting register --> switch on PWRON bit
	spi_write_command_1param_slave(0x03, reg_value_slave); //write power control setting register --> switch on PWRON bit

	UC8156_wait_for_PowerOFF_ready();
	UC8156_wait_for_PowerOFF_ready_slave();
 	UC8156_wait_for_BUSY_inactive();
	UC8156_wait_for_BUSY_inactive_slave();
}

u8 UC8156_read_RevID()
{
	return spi_read_command_1param(0x00);
}

u8 UC8156_read_RevID_slave()
{
	return spi_read_command_1param_slave(0x00);
}

// send Vcom value (in mV) to UC8156
void UC8156_set_Vcom(int Vcom_mv_value)
{
	u16 Vcom_register_value = (float)Vcom_mv_value/(float)30.0;
	spi_write_command_2params(0x1B, (u8)Vcom_register_value, (u8)((Vcom_register_value>>8)&0x03));
}

// send Vcom value (in mV) to UC8156
void UC8156_set_Vcom_slave(int Vcom_mv_value)
{
	u16 Vcom_register_value = (float)Vcom_mv_value/(float)30.0;
	spi_write_command_2params_slave(0x1B, (u8)Vcom_register_value, (u8)((Vcom_register_value>>8)&0x03));
}


// send waveform to UC8156
void UC8156_send_waveform(u8 *waveform)
{
	spi_write_command_and_bulk_data(0x1C, waveform, WAVEFORM_LENGTH);
}

// send waveform to UC8156
void UC8156_send_waveform_slave(u8 *waveform)
{
	spi_write_command_and_bulk_data_slave(0x1C, waveform, WAVEFORM_LENGTH);
}

void UC8156_print_waveform()
{
	u8 *buffer;
	buffer = (u8*) malloc(WAVEFORM_LENGTH * sizeof(u8));
	spi_read_command_and_bulk_data(0x1C, buffer, WAVEFORM_LENGTH);
	int i;
	for (i=0;i<WAVEFORM_LENGTH;i++)
		printf("0x%02x ", *(buffer+i));
	printf("\n");
}

void UC8156_print_waveform_slave()
{
	u8 *buffer;
	buffer = (u8*) malloc(WAVEFORM_LENGTH * sizeof(u8));
	spi_read_command_and_bulk_data_slave(0x1C, buffer, WAVEFORM_LENGTH);
	int i;
	for (i=0;i<WAVEFORM_LENGTH;i++)
		printf("0x%02x ", *(buffer+i));
	printf("\n");
}

//send an image to UC8156 image data memory
void UC8156_send_image_data(u8 *image_data)
{
	spi_write_command_and_bulk_data(0x10, image_data, PIXEL_COUNT/4);
}


void UC8156_send_image_data_inv(u8 *image_data)
{
    spi_write_command_and_bulk_data_inv(0x10, image_data, PIXEL_COUNT/4);
}


void UC8156_send_image_data_GL0(u8 *image_data)
{
    spi_write_command_and_bulk_data_GL0(0x10, image_data, PIXEL_COUNT/4);
}


void UC8156_send_image_data_GL4(u8 *image_data)
{
    spi_write_command_and_bulk_data_GL4(0x10, image_data, PIXEL_COUNT/4);
}


void UC8156_send_image_data_GL15(u8 *image_data)
{
    spi_write_command_and_bulk_data_GL15(0x10, image_data, PIXEL_COUNT/4);
}




void UC8156_send_image_data_slave(u8 *image_data)
{
	spi_write_command_and_bulk_data_slave(0x10, image_data, PIXEL_COUNT/4);
}

//send an image to UC8156 image data memory
void UC8156_send_image_data_area(u8 *image_data, int col_start, int col_size, int row_start, int row_size)
{
	spi_write_command_4params(0x0d, row_start*2, row_start*2+row_size*2-1, col_start/2, col_start/2+col_size/2-1);
	spi_write_command_2params(0x0e, row_start*2, col_start/2);

	spi_write_command_and_bulk_data(0x10, image_data, col_size*row_size/4);
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

//send an repeated byte to the image buffer --> used to create a solid image like all white
void UC8156_send_repeated_image_data_slave(u8 image_data)
{
	spi_write_command_byte_repeat_slave(0x10, image_data, PIXEL_COUNT/4);
}

//update display using full update mode and wait for BUSY-pin low
void UC8156_update_display_full()
{
	spi_write_command_1param(0x14, 0x01);
	UC8156_wait_for_BUSY_inactive();
}

//update display and wait for BUSY-pin low
void UC8156_update_display(u8 update_mode, u8 waveform_mode)
{
	spi_write_command_1param(0x40, spi_read_command_1param(0x40) | waveform_mode);
	spi_write_command_1param(0x14, UPDATE_COMMAND_WAVEFORMSOURCESELECT_PARAM | update_mode | 1 );
	//spi_write_command_1param(0x14, UPDATE_COMMAND_WAVEFORMSOURCESELECT_PARAM | update_mode | 1 | 0x20); // test area update mode
	UC8156_wait_for_BUSY_inactive();
	//UC8156_wait_for_BUSY_inactive_debug();
}

void UC8156_update_display_all_set(u8 update_mode, u8 waveform_mode, u8 transparency_key_value, u8 transparency_display_enable, u8 display_mode_select)
{
    spi_write_command_1param(0x40, spi_read_command_1param(0x40) | waveform_mode);

    spi_write_command_1param(0x14, UPDATE_COMMAND_WAVEFORMSOURCESELECT_PARAM | update_mode | 1 | transparency_key_value | transparency_display_enable | display_mode_select); // 0x20 for area update
    UC8156_wait_for_BUSY_inactive();

}


//update display and wait for BUSY-pin low
void UC8156_update_display_slave_only(u8 update_mode, u8 waveform_mode)
{
	spi_write_command_1param(0x40, spi_read_command_1param(0x40) | waveform_mode);
	spi_write_command_1param_slave(0x14, UPDATE_COMMAND_WAVEFORMSOURCESELECT_PARAM | update_mode | 1 );

	UC8156_wait_for_BUSY_inactive();

}

//update display and wait for BUSY-pin low
void UC8156_update_display_dual(u8 update_mode, u8 waveform_mode)
{
	spi_write_command_1param(0x40, spi_read_command_1param(0x40) | waveform_mode);
	spi_write_command_1param(0x14, UPDATE_COMMAND_WAVEFORMSOURCESELECT_PARAM | update_mode | 1 );
	spi_write_command_1param_slave(0x14, UPDATE_COMMAND_WAVEFORMSOURCESELECT_PARAM | update_mode | 1 );

	UC8156_wait_for_BUSY_inactive();
	UC8156_wait_for_BUSY_inactive_slave();
}

void UC8156_update_display_with_power_on_off(u8 update_mode, u8 waveform_mode)
{
	  UC8156_HVs_on();
	  UC8156_update_display(update_mode, waveform_mode);
      UC8156_HVs_off();
}


void UC8156_update_display_with_power_on_off_all_set(u8 update_mode, u8 waveform_mode, u8 transparency_key_value, u8 transparency_display_enable, u8 display_mode_select)
{
      UC8156_HVs_on();
      UC8156_update_display_all_set(update_mode, waveform_mode, transparency_key_value, transparency_display_enable, display_mode_select);
      UC8156_HVs_off();
}



void UC8156_update_display_with_power_on_off_dual(u8 update_mode, u8 waveform_mode)
{
	  UC8156_HVs_on_dual();
	  UC8156_update_display_dual(update_mode, waveform_mode);
      UC8156_HVs_off_dual();
}

void UC8156_show_image(u8 *image_data, u8 update_mode, u8 waveform_mode)
{
	  UC8156_send_image_data(image_data);

	  UC8156_update_display_with_power_on_off(update_mode, waveform_mode);
}


void UC8156_show_image_inv(u8 *image_data, u8 update_mode, u8 waveform_mode)
{
      UC8156_send_image_data_inv(image_data);

      UC8156_update_display_with_power_on_off(update_mode, waveform_mode);
}



void UC8156_show_image_all_set(u8 *image_data, u8 update_mode, u8 waveform_mode, u8 transparency_key_value, u8 transparency_display_enable, u8 display_mode_select, bool inv_enable)
{
    if(inv_enable)
    {
        UC8156_send_image_data_inv(image_data);
    }
    else if (!inv_enable)
    {
        UC8156_send_image_data(image_data);
    }
      UC8156_update_display_with_power_on_off_all_set(update_mode,  waveform_mode, transparency_key_value, transparency_display_enable,  display_mode_select);
}




void UC8156_show_image_GL(u8 *image_data, u8 update_mode, u8 waveform_mode, int GL_name)
{
        if (GL_name == 0)
        {
            UC8156_send_image_data_GL0(image_data);
        }
        else if (GL_name == 4)
        {
            UC8156_send_image_data_GL4(image_data);
        }
        else if (GL_name == 15)
        {
            UC8156_send_image_data_GL15(image_data);
        }


      UC8156_update_display_with_power_on_off(update_mode, waveform_mode);
}

void UC8156_show_image_dual(u8 *image_data, u8 update_mode, u8 waveform_mode)
{
	//UC8156_send_image_data(image_data);
	// No need to send data, because data is already sent to the controllers by the read_sd-function
	UC8156_update_display_with_power_on_off_dual(update_mode, waveform_mode);
}

void UC8156_show_image_area(u8 *image_data, int col_start, int col_size, int row_start, int row_size, u8 update_mode, u8 waveform_mode)
{
	  UC8156_send_image_data_area(image_data, col_start, col_size, row_start, row_size);

	  UC8156_HVs_on();
	  UC8156_update_display(update_mode, waveform_mode);
      UC8156_HVs_off();
}

//measure Vcom with continuous readout
void UC8156_measure_Vcom_curve()
{
	u8 return_value;

	spi_write_command_4params(0x18, 0x40, 0x01,0x24, 0x05); //TPCOM=Hi-Z before update and during null-frame drive

	UC8156_HVs_on();
	return_value = 	spi_read_command_1param(0x15);
	printf("R15h after HV_on = %x\n", return_value);
	//spi_write_command_1param(0x19, 0x29); //trigger Vcom sensing with waiting time 5sec
	spi_write_command_1param(0x19, 0x81); //trigger Vcom sensing with waiting time 15sec
	//spi_write_command_1param(0x19, 0xfd); //trigger Vcom sensing with max waiting time ~30sec

	int i;
	u8 value[1600][2];
	for (i=0;i<1600;i++)
	{
		value[i][0]=0;
		value[i][1]=0;
	}

	//UC8156_wait_for_BUSY_inactive(); // wait for power-up completed
	unsigned long counter=0;
	const int DIVIDER=25;
	do
	{
		mdelay(1);
		if (counter % DIVIDER == 0)
			spi_read_command_2params1(0x1a, &value[counter/DIVIDER][0]);
		counter++;
	} while (gpio_get_value(PIN_BUSY)==0); // BUSY loop

	UC8156_HVs_off();

	for (i=0;i<(int)(counter/DIVIDER)+1;i++)
	{
		//printf("%f sec = %f V\n", i*0.25, value[i][0] * 0.03);
//		if (value[i][0]>0)
//			printf("%f sec = %f V\n", i*DIVIDER/1000.0, value[i][0] * 0.03);
	}
}

void print_temperature_sensor_value()
{
	u8 return_value = spi_read_command_1param(0x08);
	printf("Temperatur sensor value read from Reg[08h] = 0x%x\n", return_value);
}

u8 print_current_VCOM()
{
	u8 return_value = spi_read_command_1param(0x1b);
	return return_value;
//	printf("Vcom read from Reg[1Bh] = 0x%x = %.3fV\n", return_value, return_value*0.03);
}

void UC8156_check_status_register(u8 expected_value)
{
	char error_message[30];

	u8 status_reg_value = spi_read_command_1param(0x15);

	#if DEBUG_PRINTOUTS
	printf("Status Register = %x\n", status_reg_value);
	#endif

	if (status_reg_value != expected_value) //check Status Register
	{
		sprintf(error_message, "Status Register not %x but %x.\n", expected_value, status_reg_value);
		abort_now(error_message, ABORT_UC8156_INIT);
	}
}

void UC8156_check_RevID()
{
	char error_message[30];
	u8 revID = UC8156_read_RevID();

	printf("RevID = %x\n", revID);

	if (revID != 0x56)
	{
		sprintf(error_message, "RevID 0x56 not read correctly (%x).\n", revID);
		abort_now(error_message, ABORT_UC8156_INIT);
	}
}

void UC8156_check_RevID_slave()
{

	char error_message[30];
	u8 revID = UC8156_read_RevID_slave();

	printf("Slave: RevID = %x\n", revID);

	if (revID != 0x56)
	{
		sprintf(error_message, "RevID 0x56 not read correctly (%x).\n", revID);
		abort_now(error_message, ABORT_UC8156_INIT);
	}
}

//measure Vcom and give back only last readout (final value)
float UC8156_measure_VCOM()
{
	u8 values[2];
	int sign=1;
	spi_write_command_1param(0x19,0x14 | 0x01);
	UC8156_wait_for_BUSY_inactive();
	spi_read_command_2params1(0x1a,values);
	spi_write_command_1param(0x19,0x14);
	if (values[1]==2)
		sign=-1;
	return sign*values[0]*30/1000;
}

void print_measured_VCOM()
{
//	printf("Vcom measured = %.3fV\n", UC8156_measure_VCOM());
}


void drive_voltage_setting(u8 vg_lv, u8 vs_lv)
{
    spi_write_command_2params(0x02, vg_lv, vs_lv);
}


void tcom_timing_setting(u8 gap, u8 s2g)
{
    spi_write_command_2params(0x06, gap, s2g);
}





