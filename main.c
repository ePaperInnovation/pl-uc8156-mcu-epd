/*
 * main.c
 *
 *  Created on: 15.01.2014
 *      Author: andreas.meier
 */

#include <stdlib.h>
#include <msp430.h>

#include "msp430/msp430-spi.h"
#include "msp430/msp430-gpio.h"
#include "msp430/hal_pmm.h"

#include "types.h"
#include "flows.h"
#include "config_display_type.h"
#include "read-sd.h"

#define DEBUG_PRINTOUTS 0

int _system_pre_init();
static void MSP430_clock_init();
void MSP430_HBZ9_3V3_enable();
void MSP430_HBZ9_3V3_disable();
void MSP430_spi_init();
void MSP430_gpio_init();

int main(void)
{
	sdcard_init(); // initialize SD-card using FatFS

	MSP430_clock_init(); // set MSP430 clock to 20MHz

	MSP430_spi_init(0,16); // initialize SPI interface towards the display

	MSP430_gpio_init(); // initialize GPIO's used for display communication

	MSP430_HBZ9_3V3_enable(); // switch on 3.3V power supply for the UC8156 on the HB_Z9 board

	eval_kit_flow();

//	set_display_type(S021_T1_1); //enum DISPLAY_TYPE {S014_T1_1, S031_T1_1, S011_T1_1}; --> see config_display_type.c/h
	set_display_type(S011_T1_1); //enum DISPLAY_TYPE {S014_T1_1, S031_T1_1, S011_T1_1}; --> see config_display_type.c/h
//	set_display_type(S031_T1_1); //enum DISPLAY_TYPE {S014_T1_1, S031_T1_1, S011_T1_1}; --> see config_display_type.c/h
//	set_display_type(S014_T1_1); //enum DISPLAY_TYPE {S014_T1_1, S031_T1_1, S011_T1_1}; --> see config_display_type.c/h
	debug_flow();

//	basic_flow();
}

int _system_pre_init(void) {
 // stop WDT
 WDTCTL = WDTPW + WDTHOLD;
 // Perform C/C++ global data initialization
 return 1;
}

// setup MSP430 clock to 20MHz
static void MSP430_clock_init(void)
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

void MSP430_spi_init() // initialize SPI interface towards the display
{
	spi_init(0,16);
}

void MSP430_gpio_init() // initialize GPIO's used for display communication
{
	gpio_init();
}

// enable the 3.3V on the HB_Z9 -> switch enable pin on the 5V->3.3V voltage regulator sitting on the HB_Z9 board
void MSP430_HBZ9_3V3_enable()
{
	gpio_set_value_hi(PIN_3V3_ENABLE);
	mdelay(5);
	gpio_set_value_hi(PIN_RESET);
}

// disable the 3.3V on the HB_Z9
void MSP430_HBZ9_3V3_disable()
{
	gpio_set_value_lo(PIN_3V3_ENABLE);
	mdelay(5);
}

