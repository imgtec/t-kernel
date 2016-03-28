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

/*
 *	monhdr.S
 *
 *       Monitor handler (after ARMv6)
 *
 *	hwdepend/arm/cpu/monhdr.S
 *	monitor entry, for arm926ej-s
 */

#define	_in_asm_source_

#include <machine.h>
#include <sys/sysinfo.h>
#include <asm/cpudep.h>

/*
 * Monitor entry (registered as default handler)
 *		+---------------+
 *       sp  -> |R3             | <- only in the case of interrupt
 *		+---------------+
 *		|R12=ip		|
 *              |R14=lr         | <- return address from exception / interupt
 *		|SPSR		|
 *		+---------------+
 *      registers upon handler invocation
 *       r3 = varies according machine type and situation (only in the case of interrupt)
 *       ip = vector table address
 *       lr = indeterminate
 */
	.text
	.balign	4
	.globl	Csym(_defaultHdr)
	.type	Csym(_defaultHdr), %function
Csym(_defaultHdr):

	.global	__defaultHdr	
__defaultHdr:
@ assume called from SVC exception, we have a mini stack
@ hmmm... big stack
@ save sp, lr in 'system' stack
	push	{lr}

	ldr	lr, =__stack_system
	ldr	lr, [lr]
	str	sp, [lr, #-4]
	sub	sp, lr, #4

	bl	entMonitor

@ sp still on the top of stack?
	ldr	lr, =__stack_system
	ldr	lr, [lr]
	ldr	sp, [lr, #-4]

	pop	{lr}
	bx	lr

/* ------------------------------------------------------------------------ */

/*
 * calling an external program
 * W callExtProg( FP entry )
 */
	.text
	.balign	4
	.globl	Csym(callExtProg)
	.type	Csym(callExtProg), %function
Csym(callExtProg):
	stmfd	sp!, {r4-r10, fp, lr}	// save registers
	blx	r0			// call entry(void)
	ldmfd	sp!, {r4-r10, fp, pc}	// restore registers
