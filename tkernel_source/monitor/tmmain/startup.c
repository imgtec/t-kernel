/*
 *	original: monitor/hwdepend/tef_em1d/src/startup.c
 *
 *       system boot processing
 *
 *	monitor/tmmain/startup.c
 *	Copyright 2015, 2016 Du Huanpeng<u74147@gmail.com>
 */

/*
 *----------------------------------------------------------------------
 *    T-Kernel 2.0 Software Package
 *
 *    Copyright 2011 by Ken Sakamura.
 *    This software is distributed under the latest version of T-License 2.x.
 *----------------------------------------------------------------------
 *
 *    Released by T-Engine Forum(http://www.t-engine.org/) at 2011/05/17.
 *    Modified by TRON Forum(http://www.tron.org/) at 2015/06/01.
 *
 *----------------------------------------------------------------------
 */


#include "hwdepend.h"

/* No support for the progress report yet */
#if 0
#define	DispProgress(n)		/* nop */
#else
EXPORT void DispProgress( W n )
{
	char *progress[] = {
		"0",
		"1",
		"2",
		"3",
		"4",
		"5",
		"6. System reset",
		"7",
		"8. Initialize console serial port",
		"9",
		"A",
		"B",
		"C",
		"D. Initialize hardware (peripherals)",
		"E. LED powered up",
		"F. Call user reset init",
		"",
	};

	if(n<8) printk("%s\n", progress[n]);
	else printk("%s\n", progress[n]);

	return;
}
#endif

/*
 * debug port speed
 *       default setting is LO:38400bps, and HI:115200bps. But
 *       if you need a different set value, then define the following macros in {cpu}/{machine}/setup_xxx.h :
 *       LO_BAUD_RATE, and HI_BAUD_RATE.
 */
#ifndef	LO_BAUD_RATE
#  define LO_BAUD_RATE	 CONFIG_TM_BAUD_RATE
#endif
#ifndef	HI_BAUD_RATE
#  define HI_BAUD_RATE	CONFIG_TM_BAUD_RATE
#endif

/*
 * initial processing after reset
 */
EXPORT void procReset( void )
{
	const MEMSEG	*mp;
	W	i;
	W	speed;

#ifdef CONFIG_TM_EARLY_PRINTK
	speed = 115200;
	initSIO(getConPort(), speed);
	printk("early printk enabled\n");
#endif

	DispProgress(0x01);

        /* system basic set up */
	resetSystem(0);
	DispProgress(0x06);

        /* setting up the initial count for micro-wait */
	setupWaitUsec();
	DispProgress(0x07);

        /* initialize console serial port */
#if SW_BHI == 0
	speed = HI_BAUD_RATE; /* HI speed is fixed. */
#else
	speed = ( (DipSw & SW_BHI) != 0 )? HI_BAUD_RATE: LO_BAUD_RATE;
#endif
#ifndef CONFIG_TM_EARLY_PRINTK
	initSIO(getConPort(), speed);
#endif
	DispProgress(0x08);

        /* initialize hardware (peripherals) */
	initHardware();
	DispProgress(0x0d);

        /* memory clear is not done to save time when automatic reboot is under way. */
	if ( bootSelect() == BS_MONITOR ) {
		cpuLED(LED_MEMCLR);
#ifdef CONFIG_TM_CLEAR_MEMORY
                /* all memory clear (save the monitor area) */
		for ( i = 1; ; ++i ) {
			mp = MemArea(MSA_OS|MSA_ERAM, i);
			if ( mp == NULL ) break;

			memset((void*)mp->top, 0, mp->end - mp->top);
		}
#endif
	}
	cpuLED(LED_POWERUP);
	DispProgress(0x0e);

        /* initialize break processing */
	initBreak();

        /* Invoking user reset initialization routine */
	callUserResetInit();
	DispProgress(0x0f);

}
/* Test stuff for i.MX280 only */

/* ICOLL */
#define HW_ICOLL_VECTOR   0x80000000
#define HW_ICOLL_LEVELACK 0x80000010
#define HW_ICOLL_CTRL     0x80000020
#define HW_ICOLL_VBASE    0x80000040
#define HW_ICOLL_STAT     0x80000070
#define HW_ICOLL_RAW0     0x800000A0
#define HW_ICOLL_RAW1     0x800000B0
#define HW_ICOLL_RAW2     0x800000C0
#define HW_ICOLL_RAW3     0x800000D0





struct HW_TIMROT_T {
	unsigned long HW_TIMROT_ROTCTRL[4];
	unsigned long HW_TIMROT_TIMCTRL[4];
	unsigned long HW_TIMROT_RUNNING_COUNT[4];
	unsigned long HW_TIMROT_FIXED_COUNT[4];
	unsigned long HW_TIMROT_MATCH_COUNT[4];
};

#define IRQ         (1<<15)
#define IRQ_EN      (1<<14)
#define MATCH_MODE  (1<<11)
#define POLARITY    (1<<8)
#define UPDATA      (1<<7)
#define RELOAD      (1<<6)
#define PRESCALE(n) ((n)<<4)
  #define IDV_BY_8     (0x3)
#define SELECT(n)   ((n)<<0)
  #define TICK_ALWAYS  (0XF)



volatile struct HW_TIMROT_T *hw_timer_rotary[3];

void irq(void)
{
}

void timer_irq(unsigned r0, unsigned r1, unsigned r2, unsigned r3)
{
#define BIT(n) (1<<n)
#define BITTST(val, n) ((val) & BIT(n))
	volatile unsigned long val;
	int i;

	static unsigned long icoll_vector[128];

	for(i=0; i<128; i++) {
		icoll_vector[i] = 4U * i;
	}


	out_w(HW_ICOLL_VECTOR, icoll_vector);








	hw_timer_rotary[0] = (void *)(0x80068000); /* have ROTCTRL */
	hw_timer_rotary[1] = (void *)(0x80068050);
	hw_timer_rotary[2] = (void *)(0x80068080);
	hw_timer_rotary[3] = (void *)(0x800680C0);


	val = hw_timer_rotary[0]->HW_TIMROT_ROTCTRL[0];
	if(BITTST(val,25)) printk("Have timer 0\n");
	if(BITTST(val,26)) printk("Have timer 1\n");
	if(BITTST(val,27)) printk("Have timer 2\n");
	if(BITTST(val,28)) printk("Have timer 3\n");

#define IRQ         (1<<15)
#define IRQ_EN      (1<<14)
#define MATCH_MODE  (1<<11)
#define POLARITY    (1<<8)
#define UPDATA      (1<<7)
#define RELOAD      (1<<6)
#define PRESCALE(n) ((n)<<4)
  #define DIV_BY_8     (0x3)
#define SELECT(n)   ((n)<<0)
  #define TICK_ALWAYS  (0XF)


#define SET 1
#define CLR 2
#define TOG 3

	hw_timer_rotary[1]->HW_TIMROT_FIXED_COUNT[0] = 0x00080000;
	val = IRQ_EN | UPDATA | PRESCALE(TICK_ALWAYS) | SELECT(0xB);
  	hw_timer_rotary[1]->HW_TIMROT_TIMCTRL[0] = val;
	hw_timer_rotary[1]->HW_TIMROT_TIMCTRL[SET] = RELOAD;
	

	do {

		val = (unsigned long)&(hw_timer_rotary[1]->HW_TIMROT_RUNNING_COUNT[0]);
		printk("Reading register [%016x]:\n", val);
		printk("Reading register [% 16x]:\n", val);
		printk("Reading register [%-16x]:\n", val);
		printk("Reading register [%116x]:\n", val);

		val = (hw_timer_rotary[1]->HW_TIMROT_RUNNING_COUNT[0]);

		printk("%08X\n", val);

		if(val<0x10000) break;

		val = 0x400000;
		while(val--);
	} while(1);

	printk("Goodbye TIMER!\n");
		
	
}

