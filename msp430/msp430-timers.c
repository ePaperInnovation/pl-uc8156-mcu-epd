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
 * msp430-timers.c -- MSP430 timer functions
 *
 * Authors: Nick Terry <nick.terry@plasticlogic.com>
 *
 */

#include <msp430.h>
#include "types.h"
#include "msp430-gpio.h"
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#define INIT_COUNT_L 0xC3
#define INIT_COUNT_H 0x50

//static int delay;

#define CPU_CYCLES_PER_USECOND (CPU_CLOCK_SPEED_IN_HZ/1000000L)
#define CPU_CYCLES_PER_MSECOND (CPU_CLOCK_SPEED_IN_HZ/1000L)


//#define MAX_TIMERS  10
//#define TIMER_RESOLUTION_MS    100
//
//#define SR_ALLOC() uint16_t __sr
//#define ENTER_CRITICAL() __sr = _get_interrupt_state(); __disable_interrupt()
//#define EXIT_CRITICAL() __set_interrupt_state(__sr)


//////////////////////////////Timer Capture Test/////
unsigned long millisecSinceBegin = 0;
bool startCounter = false;
unsigned long int counter = 0;
unsigned long int countMax;
bool countFinished = false;
//////////////////////////////Timer Capture Test/////


struct time
{
    unsigned int sec;
    unsigned int ms;
};


//
//
//struct timer
//{
//    uint16_t expiry;
//    uint16_t periodic;
//    void (*callback)(void *);
//    void *arg;
//};
//
//static struct timer _timer[MAX_TIMERS];
//static volatile uint16_t _timer_tick = 0;
//static volatile uint16_t _capture_tick = 0;
//static volatile uint16_t _capture_ta1ccr1 = 0;
//static volatile int _capture_flag = 0;
//
//
//int timer_init(void)
//{
//    /* Clear the timer structure */
//    memset(_timer, 0, sizeof(_timer));
//
//    /* Set timer to use SMCLK, clock divider 2, up-mode */
//    TA1CTL = TASSEL1 | ID0 | MC0;
//
//    /* TA1CCR0 set to the interval for the desires resolution based on 1MHz SMCLK */
//    TA1CCR0 = (((1000000 / 2) / 1000) * TIMER_RESOLUTION_MS) - 1;
//
//    /* Enable CCIE interupt */
//    TA1CCTL0 = CCIE;
//
//    /**
//     * Timer A1 capture/control block 1 set to the following configuration:
//     *  - capture mode
//     *  - syncronized mode
//     *  - capture on all edges
//     *  - interrupt enabled
//     */
//    TA1CCTL1 = CM_3 | CCIS_2 | SCS | CAP | CCIE;
//
//    return 0;
//}
//
//
//int timer_create(uint16_t timeout_ms, int periodic, void (*callback)(void *), void *arg)
//{
//    int handle = -1;
//    size_t i;
//
//    /* Find a free timer */
//    for (i = 0; i < MAX_TIMERS; i++) {
//        if (_timer[i].callback == NULL) {
//            break;
//        }
//    }
//
//    /* Make sure a valid timer is found */
//    if (i < MAX_TIMERS) {
//        SR_ALLOC();
//        ENTER_CRITICAL();
//
//        /* Set up the timer */
//        if (periodic != 0) {
//            _timer[i].periodic = (timeout_ms < 100) ? 1 : (timeout_ms / TIMER_RESOLUTION_MS);
//        } else {
//            _timer[i].periodic = 0;
//        }
//
//        _timer[i].callback = callback;
//        _timer[i].arg = arg;
//        _timer[i].expiry = _timer_tick + _timer[i].periodic;
//
//        EXIT_CRITICAL();
//        handle = i;
//    }
//
//    return handle;
//}
//
//int timer_delete(int handle)
//{
//    int status = -1;
//
//    if (handle < MAX_TIMERS) {
//        SR_ALLOC();
//        ENTER_CRITICAL();
//
//        /* Clear the callback to delete the timer */
//        _timer[handle].callback = NULL;
//
//        EXIT_CRITICAL();
//        status = 0;
//    }
//
//    return status;
//}
//
//int timer_capture(struct time *time)
//{
//    int err = -1;
//
//    if (time != NULL ) {
//        uint32_t ms;
//
//        /* Toggle the capture input select to trigger a capture event */
//        TA1CCTL1 ^= 0x1000;
//
//        /**
//         * Wait for the capture to complete
//         */
//        while (_capture_flag == 0);
//
//        /* Save the number of ms from the timer tick */
//        ms = (uint32_t) _capture_tick * 100;
//
//        /* Save captured timer value in ms */
//        ms += ((uint32_t) _capture_ta1ccr1 * 2) / 1000;
//
//        /* Save the number of milliseconds */
//        time->ms = ms % 1000;
//
//        /* Save number of seconds */
//        time->sec = ms / 1000;
//
//        /* Reset _capture_flag for next capture */
//        _capture_flag = 0;
//
//        err = 0;
//    }
//
//    return err;
//}

//__attribute__((interrupt(TIMER1_A0_VECTOR))) void timer1_isr(void)
//{
//    size_t i;
//
//    /* Clear the interrupt flag */
//    TA1CCTL0 &= ~CCIFG;
//
//    /* Increment the timer tick */
//    _timer_tick++;
//
//    for (i = 0; i < MAX_TIMERS; i++) {
//        /* If the timer is enabled and expired, invoke the callback */
//        if ((_timer[i].callback != NULL) && (_timer[i].expiry == _timer_tick)) {
//            _timer[i].callback(_timer[i].arg);
//
//            if (_timer[i].periodic > 0) {
//                /* Timer is periodic, calculate next expiration */
//                _timer[i].expiry += _timer[i].periodic;
//            } else {
//                /* If timer is not periodic, clear the callback to disable */
//                _timer[i].callback = NULL;
//            }
//        }
//    }
//}
//
//__attribute__((interrupt(TIMER1_A1_VECTOR))) void timer1_taiv_isr(void)
//{
//    /* Check for TACCR1 interrupt */
//    if (TA1IV & TA1IV_TACCR1) {
//        /* Save timer values */
//        _capture_tick = _timer_tick;
//        _capture_ta1ccr1 = TA1CCR1;
//
//        /* Set capture flag */
//        _capture_flag = 1;
//    }
//}




/* Functions for delay/sleep. Needs to be calibrated */
void udelay(u16 us)
{
	while (us--)
	{
		__delay_cycles(CPU_CYCLES_PER_USECOND);
	}
}

void mdelay(u16 ms)
{
    while (ms--)
    {
        __delay_cycles(CPU_CYCLES_PER_MSECOND);
    }
}

void msleep(u16 ms)
{
	mdelay(ms);
}


//void init_rtc()
//{
//	  // Setup RTC Timer
//	  RTCCTL01 |= RTCTEVIE;		// interrupt enable
//	  RTCCTL01 &= ~RTCMODE; 	// counter mode
//	  RTCCTL01 |= RTCSSEL_0;
//	  RTCCTL01 |= RTCTEV_1;
//
//	  // load the counter for a 10s timeout
//	  RTCNT1 = 0xFF - INIT_COUNT_H;
//	  RTCNT2 = 0xFF - INIT_COUNT_L;
//
//	  RTCCTL01 &= ~RTCHOLD; 	// start counter
//}
//
//#pragma vector=RTC_VECTOR
//__interrupt void RTC_ISR(void)
//{
//	unsigned int gie = __get_SR_register();
//
//	LPM3_EXIT;
//
//    __disable_interrupt();
//
//    switch(__even_in_range(RTCIV,16))
//    {
//		case 0: break;            		// No interrupts
//		case 2: break;            		// RTCRDYIFG
//		case 4: break;             		// RTCEVIFG => timer interrupt
//		case 6: break;                  // RTCAIFG
//		case 8: break;                  // RT0PSIFG
//		case 10: break;                 // RT1PSIFG
//		case 12: break;                 // Reserved
//		case 14: break;                 // Reserved
//		case 16: break;                 // Reserved
//		default: break;
//    }
//
//    __bis_SR_register(gie);
//}

//void timer_start(void)
//{
//	delay = 1;
//	TA0CTL = TASSEL_1 | ID_3 | MC_2 | TACLR | TAIE;         // ACK ( = 10kHz), contmode, clear interrupt enable
//	TA0R = 0xFFFF - 0x927C;									// Configure the timer
//}
//
//void timer_stop(void)
//{
//	TA0CTL = MC_0;
//}
//
//#pragma vector = TIMER0_A1_VECTOR
//__interrupt void TIMER0_ISR(void)
//{
//	unsigned int gie = __get_SR_register();
//
//	__disable_interrupt();
//
//	switch(__even_in_range(TA0IV,16))
//	{
//	case 0x0E:
//		delay--;
//		if(delay < 0)
//		{
//			timer_stop();
//			timer_start();
//		}
//
//		// reload the timer for 1 minute
//		TA0R = 0xFFFF - 0x927C;
//
//		break;
//	default:break;
//
//	}
//
//    __bis_SR_register(gie);
//}


///////////////////////////////////////////////////////////////Timer Test/////////////////////////////

unsigned long millis(void){

    return  millisecSinceBegin/5;
}

void timerbStart(unsigned int countVal){

    countMax = countVal;


    startCounter = true;
}

unsigned long int timerbRead(void){


    return counter;
}


void timerbWait(void){


    while(!countFinished);


    countFinished = false;
}

bool timerbFinished(void){


    if(countFinished){
        countFinished = false;
        return true;
    }else{
        return false;
    }
}

unsigned long int timerbStop(void)
{

  _disable_interrupts();
  TB0CTL = MC_0;
  unsigned long int time_end_ms = millis();
  if(millis() > 0)
  {
      printf("time capture = %lu ms\n", time_end_ms);
  }
  else
  {
      printf("please check timerbInit()");
  }
  return time_end_ms;

}

unsigned long int timerbInit(void){

    /*
    Number of counts  = 0x07CE + 0x0001 = 1998 + 1 = 1999
    TBSSEL__SMCLK -> Select SMCLK as timer source b
    ID__2         -> Select prescaler for fclk/4
    TBCLR         -> Clear clock and reset settings
    T = 1ms
     */
    TB0CCR0 = 0x07CF;
    TB0CTL = TBSSEL__SMCLK + ID__2 + TBIE + TBCLR;


    counter = 0;


    TB0CTL |= MC__UP;
    _enable_interrupts();
    unsigned long int time_start_ms = millis();
    return time_start_ms;

}

#pragma vector = TIMER0_B1_VECTOR
__interrupt void timerB (void){


    TB0CTL &= ~TBIFG;
    TB0CCTL1 &= ~CCIFG;


    if(millisecSinceBegin == 0xFFFFFFFF){
        millisecSinceBegin = 0;
    }else{
        millisecSinceBegin++;
    }

    if(startCounter == true){

        counter++;

        if(counter == countMax){

            countFinished = true;

            counter = 0;

            startCounter = false;
        }
    }

}





