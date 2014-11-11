/*
 * main.c
 *
 *  Created on: 15.01.2014
 *      Author: andreas.meier
 */

//#define DEBUG_PRINTOUTS

#include <msp430.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "types.h"
#include "assert.h"
#include "FatFs/ff.h"
#include "msp430/msp430-spi.h"
#include "msp430/msp430-gpio.h"
#include "msp430/hal_pmm.h"
#include "Slideshow.h"
#include "UC8156.h"
#include "UC8156_MTP.h"
#include "read-sd.h"
#include "waveform.h"
#include "config.h"
#include "pattern.h"

void clear_display();

int _system_pre_init(void) {
 // stop WDT
 WDTCTL = WDTPW + WDTHOLD;
 // Perform C/C++ global data initialization
 return 1;
}

// setup clock to 20MHz
static void MSP430_init_clock(void)
{
	WDTCTL = WDTPW + WDTHOLD;				// Hold WDT
	SetVCore(PMMCOREV_3);					// Set VCore = 1.9V for 20MHz clock

	UCSCTL4 = SELM_4 | SELS_4 | SELA_1;		// MCLK = SMCLK = DCOCLKDIV = 20MHz /  ACLK = VLO = 10Khz
	UCSCTL3 |= SELREF_2;					// Set DCO FLL reference = REFO

	__bis_SR_register(SCG0);				// Disable the FLL control loop
	UCSCTL0 = 0x0000;						// Set lowest possible DCOx, MODx
	UCSCTL1 = DCORSEL_6;					// Select DCO range 30MHz operation
	UCSCTL2 = FLLD_1 + 609;					// Set DCO Multiplier for 20MHz
											// (N + 1) * FLLRef = Fdco
											// (609 + 1) * 32768 = 20MHz
											// Set FLL Div = fDCOCLK/2
	__bic_SR_register(SCG0);				// Enable the FLL control loop

	// Worst-case settling time for the DCO when the DCO range bits have been
	// changed is n x 32 x 32 x f_MCLK / f_FLL_reference. See UCS chapter in 5xx
	// UG for optimization.
	// 32 x 32 x 20 MHz / 32,768 Hz = 625000 = MCLK cycles for DCO to settle
	__delay_cycles(625000);

	// Loop until XT1,XT2 & DCO fault flag is cleared
	do {
		UCSCTL7 &= ~(XT2OFFG | XT1LFOFFG | XT1HFOFFG | DCOFFG);
												// Clear XT2,XT1,DCO fault flags
		SFRIFG1 &= ~OFIFG;                      // Clear fault flags
	} while (SFRIFG1 & OFIFG);
}

void send_drift_test_image()
{
	  u8 image_data[PIXEL_COUNT/4];
	  int i;

	  for(i=0;i<PIXEL_COUNT/4/3;i++)
		  image_data[i]=0x00;
	  for(i=PIXEL_COUNT/4/3;i<PIXEL_COUNT/4;i++)
		  image_data[i]=0xff;

	  UC8156_send_image_data(image_data);
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

void drift_test()
{
	UC8156_HVs_on();
/*
	UC8156_send_repeated_image_data(0xff); // 0xff is white
	UC8156_update_display(FULL);

	UC8156_send_waveform(waveform_long_null);
	UC8156_send_repeated_image_data(0xff); // 0xff is white
	UC8156_update_display(FULL);

	UC8156_send_waveform(waveform_default);
	UC8156_send_repeated_image_data(0x00); // 0xff is white
	UC8156_update_display(FULL);

	UC8156_send_waveform(waveform_long_null);
	UC8156_send_repeated_image_data(0x00); // 0xff is white
	UC8156_update_display(FULL);
*/
	//spi_write_command_1param(0x0f, 0x06); //
	send_drift_test_image();
	UC8156_update_display(FULL_UPDATE);

	UC8156_send_waveform(waveform_long_null);
	UC8156_update_display(FULL_UPDATE);

	UC8156_HVs_off();

}

int main(void)
{
	u8 return_value;
	static u8 revID;

 	MSP430_init_clock();

	sdcard_init(); // initialize SD-card using FatFS
	spi_init(0,16); // initialize SPI interface towards the display

	gpio_init(); // initialize GPIO's used for display communication
	gpio_set_value_hi(PIN_3V3_ENABLE); //enable 3.3V on interface board (also controller VDD)
	gpio_set_value_hi(PIN_RESET);
	UC8156_wait_for_BUSY_inactive(); // wait for power-up completed
 	//while (gpio_get_value(PIN_BUSY)!=0); // BUSY loop
	//UC8156_wait_for_BUSY_inactive(); // wait for power-up completed

	UC8156_reset(); // UC8156 hardware reset
	UC8156_wait_for_BUSY_inactive(); // wait for RESET completed

	UC8156_init_registers();

	return_value = 	spi_read_command_1param(0x15);
	if (return_value!=0) //check Status Register
	{
		fprintf(stderr, "Status Register not clear (%x).\n", return_value);
		return 1;
	}
	#ifdef DEBUG_PRINTOUTS
	fprintf(stderr, "Status Register = %x\n", return_value);
	#endif
	revID = UC8156_read_RevID();
	if (revID!=0x56)
	{
		fprintf(stderr, "RevID 0x56 not read correctly (%x).\n", revID);
		return 1;
	}
	#ifdef DEBUG_PRINTOUTS
	fprintf(stderr, "RevID = %x\n", revID);
	#endif

	//measure_Vcom();

//#define TEMP_SENSOR_DEBUG
#ifdef TEMP_SENSOR_DEBUG //debug temperature sensor
	return_value = 	spi_read_command_1param(0x07); //read temperature value register
	fprintf(stderr, "R07h = %xh\n", return_value);
	return_value = 	spi_read_command_1param(0x08); //read temperature value register
	fprintf(stderr, "temperature (R08h) = %dd\n", return_value);

	spi_write_command_1param(0x07, 0x09); //trigger internal temperature sensor read
	mdelay(170);
 	while (spi_read_command_1param(0x15)!=0); // check status register bit for temperature sensor
	return_value = 	spi_read_command_1param(0x07); //read temperature value register
	fprintf(stderr, "R07h = %xh\n", return_value);

	return_value = 	spi_read_command_1param(0x08);
	fprintf(stderr, "temperature (R08h) = %dd\n", return_value);
#endif

	UC8156_set_Vcom(2500);

	u8 waveform[WAVEFORM_LENGTH];
	sdcard_load_waveform("waveform.bin", waveform);

/*
	//one_Byte_MTP_program(0x060, 0x00);
	write_waveform_to_MTP(waveform);
	//one_Byte_MTP_program(0x4B0, 0x7f);

    spi_write_command_1param(0x07, 0x00);
    spi_write_command_1param(0x08, 0x00);

	int i=0,j=0;
	fprintf(stderr, "\n");
	for(i=0;i<30;i++)
	{
		fprintf(stderr, "%2x: ", i*4+j);
		while(j<4)
		{
			return_value = read_MTP_address(i*4+j);
			fprintf(stderr, "%2x ", return_value);
			j++;
		}
		fprintf(stderr, "\n");
		j=0;
	}

	show_image("240x160/13_240~1.PGM", 0x03);

	u8 buffer[120];
	spi_read_command_and_bulk_data(0x1c, buffer, 120);
*/
//	UC8156_send_waveform(waveform);
	UC8156_send_waveform(waveform_new);

	//show_image("240x160/13_240~1.PGM", 0x03);
/*
	while(1)
	{
		alt_source();
		inv_alt_source();
		inv_alt_source();
		alt_source();
	}
*/
/*
while(1)
{
	alt_source();
	inv_alt_source();
	inv_alt_source();
	alt_source();
}
*/
	clear_display(); // initialize display with 2 white updates

	while (1)
		slideshow_run(PATH, FULL_UPDATE, 1000);

	//diagonale();

	//walking_source_line();
	//alt_source_4er();
	checkerboard_debug(0x33, 0, SOURCE_LINES/4);

	checkerboard();
	white_update();
	checkerboard();
	alt_source();

	show_image("240x160/08_240~1.PGM", FULL_UPDATE);

	diagonale();

	while(1)
	{
		alt_source();
		inv_alt_source();
		white_update();
		alt_source();
		white_update();
		inv_alt_source();
		checkerboard();
		inv_checkerboard();
		white_update();
		checkerboard();
		white_update();
		inv_checkerboard();
	}


	//drift_test();

	//UC8156_send_waveform(waveform_test);
	//show_image("1st4pxl.PGM");
	//show_image("4GL.PGM", FULL_UPDATE);
	//mdelay(3000);
	//show_image("TestPicA.PGM", FULL_UPDATE);
/*
	show_image("240x160/13_240~1.PGM", FULL_UPDATE);
	u8 *value;
	value = spi_read_command_4params(0x1c);
	fprintf(stderr, "value = %x\n", *value++);
	fprintf(stderr, "value = %x\n", *value++);
	fprintf(stderr, "value = %x\n", *value++);
	fprintf(stderr, "value = %x\n", *value++);
	value = spi_read_command_2params(0x1b);
	fprintf(stderr, "value = %x\n", *value++);
	fprintf(stderr, "value = %x\n", *value++);

	show_image("240x160/13_240~1.PGM", 3);
	value = spi_read_command_4params(0x1c);
	fprintf(stderr, "value = %x\n", *value++);
	fprintf(stderr, "value = %x\n", *value++);
	fprintf(stderr, "value = %x\n", *value++);
	fprintf(stderr, "value = %x\n", *value++);
	value = spi_read_command_2params(0x1b);
	fprintf(stderr, "value = %x\n", *value++);
	fprintf(stderr, "value = %x\n", *value++);
*/
	while (1)
		slideshow_run(PATH, FULL_UPDATE, 1000);
		//slideshow_run(PATH, show_image);
		//slideshow_run("240x80", show_image);

//	clear_display(); // initialize display with 2 white updates

	/*	GPIO output verification
 	spi_write_command_1param(0x09, 0xf0); //configure GPIO's for output
	spi_write_command_1param(0x09, 0xf1); //configure GPIO's for output
	spi_write_command_1param(0x09, 0xf2); //configure GPIO's for output
	spi_write_command_1param(0x09, 0xf4); //configure GPIO's for output
	spi_write_command_1param(0x09, 0xf8); //configure GPIO's for output
*/

/*	return_value = 	spi_read_command_1param(0x15);
	fprintf(stderr, "return_value = %x\n", return_value);
	UC8156_HVs_on();
	return_value = 	spi_read_command_1param(0x15);
	fprintf(stderr, "return_value = %x\n", return_value);
*/

	//one_Byte_MTP_program();
	//new_MTP_program();

}

void clear_display()
{
	UC8156_HVs_on();

/*old
	UC8156_send_repeated_image_data(0xff); // 0xff is white
	UC8156_update_display(FULL_UPDATE);
	UC8156_send_repeated_image_data(0xff); // 0xff is white
	UC8156_update_display(FULL_UPDATE);
*/
//new
	u8 reg0fh_backup = spi_read_command_1param(0x0f);

	spi_write_command_1param(0x0f, 0x10); //
	UC8156_send_repeated_image_data(0xff); // 0xff is white
	spi_write_command_1param(0x0f, 0x00); //
	UC8156_send_repeated_image_data(0xff); // 0xff is white
	UC8156_update_display(FULL_UPDATE);
	UC8156_update_display(FULL_UPDATE);

	UC8156_HVs_off();

	spi_write_command_1param(0x0f, reg0fh_backup); //
}

