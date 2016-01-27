/*
 * main.c
 *
 *  Created on: 15.01.2014
 *      Author: andreas.meier
 */

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
#include "image.h"

#define DEBUG_PRINTOUTS 0

int DO_MTP_PROGRAMMING = 0;

void clear_display();
int _system_pre_init(void);
static void MSP430_init_clock(void);
extern u8 UPDATE_COMMAND_WAVEFORMSOURCESELECT_PARAM;

int main(void)
{
	sdcard_init(); // initialize SD-card using FatFS

//	read_config_file("display-config.txt");

 	MSP430_init_clock();

	spi_init(0,16); // initialize SPI interface towards the display

	gpio_init(); // initialize GPIO's used for display communication
	gpio_set_value_hi(PIN_3V3_ENABLE);
	mdelay(5);
	gpio_set_value_hi(PIN_RESET);

	UC8156_wait_for_BUSY_inactive(); // wait for power-up completed

	mdelay(5);
	UC8156_hardware_reset(); // UC8156 hardware reset
	UC8156_wait_for_BUSY_inactive(); // wait for RESET completed

	UC8156_init_registers();

	u8 status_reg = 	spi_read_command_1param(0x15);
	if (status_reg!=0) //check Status Register
	{
		fprintf(stderr, "Status Register not clear (%x).\n", status_reg);
		exit(EXIT_FAILURE);
	}
	#if DEBUG_PRINTOUTS
	fprintf(stderr, "Status Register = %x\n", status_reg);
	#endif
	static u8 revID;
	revID = UC8156_read_RevID();
	if (revID!=0x56)
	{
		fprintf(stderr, "RevID 0x56 not read correctly (%x).\n", revID);
		exit(EXIT_FAILURE);
	}
	#if DEBUG_PRINTOUTS
	fprintf(stderr, "RevID = %x\n", revID);
	#endif

	int vcom_mv_value;
//MTP program - should be used by Plastic Logic only
	if (DO_MTP_PROGRAMMING)
	{
		write_single_waveform_table_to_MTP("Type31/waveforms/S031_V7D035_2xGL_V1.23.0.uc8156_lut");
		//write_single_waveform_table_to_MTP("waveforms/waveform.bin");

		UC8156_hardware_reset(); // UC8156 hardware reset
		UC8156_wait_for_BUSY_inactive(); // wait for RESET completed
		UC8156_init_registers();

		if (sdcard_load_vcom(&vcom_mv_value)==0)
			write_Vcom_to_MTP(vcom_mv_value);

		UC8156_hardware_reset(); // UC8156 hardware reset
		UC8156_wait_for_BUSY_inactive(); // wait for RESET completed
		UC8156_init_registers();
	}

	//set Vcom from SD card data -> if "/700xxx/display/vcom.txt" exist
	if (sdcard_load_vcom(&vcom_mv_value)==0)
		UC8156_set_Vcom(vcom_mv_value);

#if DEBUG_PRINTOUTS
	//read-out VCOM setting
	u8 return_value = spi_read_command_1param(0x1b);
	fprintf(stderr, "Vcom read = 0x%x\n", return_value);
#endif

	//write waveform from SD card data to LUT -> if "/700xxx/display/waveform.bin" exist
	u8 *waveform_p;
	u8 waveform_from_file[WAVEFORM_LENGTH];
	if (sdcard_load_waveform(waveform_from_file, WAVEFORM_LENGTH)==0)
	{
		waveform_p = waveform_from_file;
		UC8156_send_waveform(waveform_p);
		UPDATE_COMMAND_WAVEFORMSOURCESELECT_PARAM =  WAVEFORM_FROM_LUT;
	}

//	spi_write_command_2params(0x04, 0x01, 0x24);

	clear_display();

//	playground();

	while(1)
		slideshow_run(FULL_UPDATE, 2000);
//		slideshow_run(PARTIAL_UPDATE, 2000);
}

void clear_display()
{
	u8 reg0fh_value = spi_read_command_1param(0x0f);

	spi_write_command_1param(0x0f, reg0fh_value|0x10); //
	UC8156_send_repeated_image_data(0xff); // 0xff is white
	spi_write_command_1param(0x0f, reg0fh_value&(~0x10)); //
	UC8156_send_repeated_image_data(0xff); // 0xff is white

	UC8156_HVs_on();
	UC8156_update_display(FULL_UPDATE);
	UC8156_update_display(FULL_UPDATE);
	//UC8156_update_display(INIT_UPDATE);
	UC8156_HVs_off();
}

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

void playground()
{
	//	 UC8156_show_image((u8 *)visa_180x100, FULL_UPDATE);
	//	 exit(EXIT_SUCCESS);

		//print_characters_312x74();

		//spi_write_command_1param(0x07, 0x0a); //Temperature Auto Retrieval enabled from internal sensor -> works only with waveform from MTP

	//	show_image_from_SDcard("TestPic.pgm", FULL_UPDATE);
	//	mdelay(1000);
	//	show_image_from_SDcard("TestPic.pgm", FULL_UPDATE);
}
