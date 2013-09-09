/*-
 * Copyright (c) 2013 Javier M. Mellid
 * All rights reserved.
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

#include "pte.h"
#include "mtpr.h"
#include "rpb.h"
#include "kmutex.h"

#define KDELTA 0x80000000

struct mcb {
	struct rpb *rpb;
} mcb;

extern int getsecs();

void *memset(void *s, int c, unsigned long n);

void kmain_high() {

	/* executing in S0 from here ... */

	/* set new kernel stack in S0 (4 pages), using interrupt stack yet */
        asm("movl $0x8000A000, %sp");

	/* fill main control block */
	mcb.rpb = (struct rpb *)0x800f0000;

	/* show detected ram */
	kprintf("RAM %d bytes\n", mcb.rpb->pfncnt * 512);

	/* check minimum ram required */
	if (mcb.rpb->pfncnt * 512 < 8 * 1024 * 1024)
		panic("System needs 8MB or more to run.");

	/* it is ok showing virtual memory delayed message here */
	kprintf("VM enabled\n");

	/* init system control block */
	scb_init();

	kprintf("SCB initialized\n");

	/* start clock */
	clk_start();

	kprintf("clock started\n");

	/* start console */
	con_start();

	kprintf("console started\n");

	/* init console buffer */
	consbuffer_init();

	kprintf("console buffer initialized\n");

	/* init scheduler */
	sched_init();

	kprintf("scheduler initialized\n");

	/* set up proper environment to run process 0 */

	asm("  mtpr $0x80006000, 0  "); /* set up next KSP to leave interrupt stack */
        asm("  pushl $0             "); /* set up next PSL (modes, IPL 0, etc) */
	asm("  movl $ksp_on, -(%sp) ");
	asm("  rei                  ");
	asm("  ksp_on:              ");

	/* this path becomes process 0 */
	i_am_process_0();

	/* not reached */
	panic("process 0 returned");
}

void kmain() {

	/* physical memory size = 8 MB */
	unsigned int physmem_size = 0x800000;

	/* page table pointers */
	struct pte *pt_system;	/* System page table */
	struct pte *pt_p0;	/* P0 page table */
	struct pte *pt_p1;	/* P1 page table */

	/* real local vars */
	unsigned int num_frames = physmem_size / 512;
	unsigned int pt_size = num_frames * 4;
	unsigned int i;

	/* set page table locations */
	pt_system = (struct pte *)0x200000;
	pt_p0     = (struct pte *)0x210000;
	pt_p1     = (struct pte *)0x220000;

	/* relocated function pointers */
	void *(*memset_rel)(void *, int, unsigned long);
	void (*mtpr_rel)(unsigned int, int);

	memset_rel = memset - KDELTA;
	mtpr_rel = mtpr - KDELTA;

	/* fill with zeros */
	(*memset_rel)((void *)pt_system, 0, pt_size);
	(*memset_rel)((void *)pt_p0, 0, pt_size);
	(*memset_rel)((void *)pt_p1, 0, pt_size);

	/* fill page tables */
	for(i=0; i<num_frames; i++) {
		/* system table */
		pt_system[i].pg_pfn  = i;
		pt_system[i].pg_prot = 0xd;
		pt_system[i].pg_v    = 1;

		/* p0 table */
		pt_p0[i].pg_pfn  = i;
		pt_p0[i].pg_prot = 0xd;
		pt_p0[i].pg_v    = 1;

		/* p1 table */
		pt_p1[i].pg_pfn  = i;
		pt_p1[i].pg_prot = 0xd;
		pt_p1[i].pg_v    = 1;
	}

	/* fill S0 base/len registers */
	(*mtpr_rel)((unsigned int)pt_system, PR_SBR);
	(*mtpr_rel)(num_frames, PR_SLR);

	/* fill P0 and P1 base/len registers ... page tables are on virtual memory */
	(*mtpr_rel)((unsigned int)(((unsigned int)pt_p1)+KDELTA), PR_P1BR);
	(*mtpr_rel)((unsigned int)(((unsigned int)pt_p0)+KDELTA), PR_P0BR);
	(*mtpr_rel)(num_frames, PR_P1LR);
	(*mtpr_rel)(num_frames, PR_P0LR);

	/* enable virtual memory */
	(*mtpr_rel)(1, PR_MAPEN);

	void (*kmain_high_rel)(void);

	/* this code runs in P0 ... jumping to S0 ... */
	kmain_high_rel = kmain_high + KDELTA;
	(*kmain_high_rel)();
}
