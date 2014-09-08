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
#include "waveform.h"

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

 	spi_init(0,0); // initialize SPI interface towards the display
	gpio_init(); // initialize GPIO's used for display communication

	UC8156_reset(); // UC8156 hardware reset

	UC8156_wait_for_BUSY_inactive();

	//u8 revID = UC8156_read_RevID();

	spi_write_command_1param(0x01, 0x10);

	spi_write_command_2params(0x06, 0x67, 0x55);

	//spi_read_command_2params(0x02);
	//spi_write_command_2params(0x02, 0x50, 0x00);
	//spi_read_command_2params(0x02);

	UC8156_set_Vcom(4000);
	//spi_write_command_2params(0x1B, 0x4E, 0x01);
	spi_read_command_2params(0x1B);

	spi_write_command_2params(0x18, 0x80, 0x01); // Vcom/
	//spi_write_command_4params(0x18, 0x80, 0x01, 0x24, 0x07); // Vcom/

	UC8156_send_waveform(test_waveform);

	spi_write_command_1param(0x12, 0x33); //Bypass RAM data, all old data bypass to G0, all new data bypass to G3
	//UC8156_send_repeated_image_data(0x00);

	//spi_write_command_1param(0x50, 0x07);
	spi_write_command_1param(0x51, 0x88);
	spi_write_command_1param(0x52, 0x07);
	spi_write_command_1param(0x55, 0x00);

	spi_write_command_2params(0x02, 0x07, 0xFF);

	spi_write_command_3params(0x1F, 0x11, 0x21, 0x10);

	UC8156_power_on();

	UC8156_update_display();

	UC8156_power_off();

	//u8 revID = UC8156_read_RevID();

	// load waveform from SD-card and send the SSD1606
	//sdcard_load_waveform("Type25/display/WFR.wav", Waveform);
	//UC8156_send_waveform(test_waveform);

	// load Vcom from SD-card and send the SSD1606
	//sdcard_load_vcom("Type25/display/VCOM.txt", &Vcom_mv_value);
	//UC8156_set_Vcom(5000);

	//clear_display();

	// start slide-show in endless loop
	//while (1)
		//slideshow_run("Type25/img", show_image);

}

void clear_display()
{
	UC8156_send_repeated_image_data(0xff); // 0xff is white
	UC8156_update_display();
	UC8156_send_repeated_image_data(0xff); // 0xff is white
	UC8156_update_display();
}
