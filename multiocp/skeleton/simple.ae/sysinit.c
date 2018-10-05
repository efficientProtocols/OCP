/*
 * $Id: sysinit.c,v 1.1.1.1 1998/01/30 01:13:13 mohsen Exp $
 *
 */
/* ---- Include Files ---------------------------------------- */

#include "target.h"
#include <stdio.h>
#include <dos.h>
#include "amd186es.h"
#include "ae_init.h"
#include "ae_timer.h"
// #include "env.h"
#include "ae_pio.h"
// #include "tmr.h"
// #include "dbg.h"

// extern BOOL dbgInit(ComPort Port);

// Timer ISR prototypes
static void interrupt far t2_isr (void);
static void interrupt far t0_isr (void);

#define HW_TICK			55

unsigned long MSTICK;

/*------------------------------------------------------------
 * void InitSystem(void)
 *
 *	Initialize A-Engine and start Milisecond timer
 *
 * Inputs:
 *
 * Outputs:
 *
 * Returns:
 *
 */
void InitSystem(void)
{
 	unsigned int ta,tb,tm;

	MSTICK = 0;

	// A-Engine initialization */
	AeInit(AE_PROFILE);       /* A-Engine initialization */

	// Turn off LED
	AeLed(FALSE);

	// Start timer 2 prescale at 1ms
	// 20 MHz, 2 ns per timer clk
	ta=(unsigned int)5000;	// pre-scale for timer0 2x5000 = 1 ms
	tm = 0xc001;	//	start clk countdown, continuous, int disabled 	
	AeTim2Init(tm,ta,t2_isr);	 

	// Start timer 0 pre scaled by timer 2 at 1ms
	ta=(unsigned int)HW_TICK;	// 
	tb=(unsigned int)HW_TICK;	//	
	tm = 0xe00b;	//	start - timer2 prescale countdown, int. enabled 	
	AeTim0Init(tm,ta,tb,t0_isr);	 
}

/*
//		Function:  t0_isr
//
//		Timer0 Interrupt handler. 
*/

void	interrupt far t0_isr (void)
{

	MSTICK += HW_TICK;

	/* Issue EOI for the interrupt */
	// output to EOI register Interupt type (8) timer 0
	outport(0xff22,0x0008);
}

/*
//		Function:  t2_isr
//
//		Timer2 Interrupt handler. 
*/
// NOT USED
void	interrupt far t2_isr (void)
{
	/* Issue EOI for the interrupt */
	// output to EOI register Interupt type (13h) timer 2
	outport(0xff22,0x0008);
}

