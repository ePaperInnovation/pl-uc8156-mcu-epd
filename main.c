/*
 * main.c
 *
 *  Created on: 15.01.2014
 *      Author: andreas.meier
 */


#include <msp430.h>
#include <stdint.h>
#include <stdio.h>
#include "types.h"
#include "assert.h"
#include "FatFs/ff.h"
#include "msp430/msp430-spi.h"
#include "msp430/msp430-gpio.h"
#include "Slideshow.h"
#include "UC8156.h"
#include "read-sd.h"

void clear_display();


int _system_pre_init(void) {
 // stop WDT
 WDTCTL = WDTPW + WDTHOLD;
 // Perform C/C++ global data initialization
 return 1;
}

int main(void)
{
	u8 Waveform[90];
	int Vcom_mv_value;

 	WDTCTL = WDTPW+WDTHOLD;// Stop watchdog timer

 	//sdcard_init(); // initialize SD-card using FatFS

 	spi_init(0,8); // initialize SPI interface towards the display
	gpio_init(); // initialize GPIO's used for display communication

	//UC8156_reset(); // UC8156 hardware reset

	//UC8156_power_on();

	//u8 revID = UC8156_read_RevID();

	// load waveform from SD-card and send the SSD1606
	//sdcard_load_waveform("Type25/display/WFR.wav", Waveform);
	//solomon_send_waveform(Waveform);

	// load Vcom from SD-card and send the SSD1606
	//sdcard_load_vcom("Type25/display/VCOM.txt", &Vcom_mv_value);
	UC8156_set_Vcom(5000);

	//clear_display();

	// start slide-show in endless loop
	//while (1)
		//slideshow_run("Type25/img", show_image);

}

void clear_display()
{
	solomon_send_repeated_image_data(0xff); // 0xff is white
	solomon_update_display();
	solomon_send_repeated_image_data(0xff); // 0xff is white
	solomon_update_display();
}
