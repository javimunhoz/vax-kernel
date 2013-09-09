/*-
 * Copyright (c) 2008 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * Copyright (c) 2013 Javier M. Mellid
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "mtpr.h"
#include "console.h"
#include "sched.h"
#include "scb.h"

volatile int tickcnt = 100;

int getsecs(void) {
	return tickcnt/100;
}

struct ivec_dsp **scb;
struct ivec_dsp *scb_vec;

extern struct ivec_dsp idsptch;

#define VAX_PGSHIFT  9
#define VAX_NBPG     (1 << VAX_PGSHIFT)
#define VAX_PGOFSET  (VAX_NBPG - 1)

void scb_stray(void *arg);
void switchto(void);

/*
 * Init the SCB and set up a handler for all vectors in the lower space,
 * to detect unwanted interrupts.
 */
void scb_init(void) {

	unsigned i;

	/*
	 * Allocate space. We need one page for the SCB, and 128*20 == 2.5k
	 * for the vectors. The SCB must be on a page boundary.
	 */

	i = (unsigned)alloc(VAX_NBPG + 128*sizeof(scb_vec[0])) + VAX_PGOFSET;
	i &= ~VAX_PGOFSET;

	mtpr(i, PR_SCBB);
	scb = (void *)i;
	scb_vec = (struct ivec_dsp *)(i + VAX_NBPG);

	for (i = 0; i < 128; i++) {
		scb[i] = &scb_vec[i];
		scb[i] = (void *)((unsigned)scb[i] | SCB_ISTACK);      /* Only interrupt stack */
		scb_vec[i] = idsptch;
		scb_vec[i].hoppaddr = scb_stray;
		scb_vec[i].pushlarg = (void *) (i * 4);
		scb_vec[i].ev = 0;
	}

	/* soft interrupt under IPL 3 used to handle context switches. we need to
	   go in/out with maximum control so we handle this interrupt aside */
	scb[0x8c/4] = (void *)((unsigned long)switchto);
	scb[0x8c/4] = (void *)((unsigned)scb[0x8c/4] | SCB_ISTACK);      /* Only interrupt stack */
}

void timer_handler(void *arg) {

	tickcnt++;
	mtpr(0xc1, PR_ICCS);

	/* start multitask? */
	if ((sched_on == 1) && (sched_running == 0)) {
		sched_running = 1;
	}

	/* stop multitask? */
	if (current_process == 0) {
		if ((sched_on == 0) && (sched_running == 1)) {
			sched_running = 0;
		}
	}

	/* schedule? */
	if (sched_running) {
		switch_context(); /* schedule and pre-dispatch in clock IPL */
		mtpr(3, PR_SIRR); /* request soft int in IPL 3 to real hardware switch */
	}
}

void clk_start(void) {
	scb_vec[0xc0/4].hoppaddr = timer_handler;
	mtpr(0x800000d1, PR_ICCS);      /* Start clock and enable interrupt */
}

void console_handler(void *arg) {

	unsigned char c;

	c = (unsigned char)pr_getchar();

	if (!consbuffer_full_buffer()) {
		consbuffer_add_char(c);
	}
}

void con_start(void) {

	scb_vec[0xf8/4].hoppaddr = console_handler;
	mtpr(GC_RIE, PR_RXCS);
}

#ifdef __ELF__
#define IDSPTCH "idsptch"
#define EIDSPTCH "eidsptch"
#define CMN_IDSPTCH "cmn_idsptch"
#else
#define IDSPTCH "_idsptch"
#define EIDSPTCH "_eidsptch"
#define CMN_IDSPTCH "_cmn_idsptch"
#endif

asm("	.text         ");
asm("	.align  2     ");
asm("	.globl  " IDSPTCH ", " EIDSPTCH " ");
asm(" " IDSPTCH ": ");
asm("	pushr   $0x3f ");
asm("	.word   0x9f16 ");
asm("	.long   " CMN_IDSPTCH " ");
asm("	.long   0 ");
asm("	.long   0 ");
asm("	.long   0 ");
asm(" " EIDSPTCH ": ");
asm(" " CMN_IDSPTCH ": ");
asm("	movl    (%sp)+,%r0 ");
asm("	pushl   4(%r0) ");
asm("	calls   $1,*(%r0) ");
asm("	popr    $0x3f ");
asm("	rei ");

/*
 * Stray interrupt handler.
 * This function must _not_ save any registers (in the reg save mask).
 */
void scb_stray(void *arg) {

	static int vector, ipl;

	ipl = mfpr(PR_IPL);
	vector = (int) arg;
	/* printf("stray interrupt: vector 0x%x, ipl %d\n", vector, ipl); */
	kprintf("stray interrupt: vector %d, ipl %d\n", vector, ipl);
}
