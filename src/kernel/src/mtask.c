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

#include "sched.h"
#include "mtpr.h"

#define DUMMY_PROCESS_COUNT 5000

void dummy_process()
{
	int count = DUMMY_PROCESS_COUNT;

	kprintf("\nnew process will paint dots while contex " \
		"switches between painter process and console happen!\n");

	while(1) {
		count--;
		if (count == 0) {
			kprintf(".");
			count = DUMMY_PROCESS_COUNT;
		}
	}
}

void setup_processes()
{
	int i;

	/* init process list */
	for(i=0;i<MAX_PROCESSES;i++)
		process_list[i].id = -1;

	/* proof of concept with 2 processes */

	process_list[0].id = 0;
	process_list[0].status = PROC_READY;
	process_list[0].pcb.pcb_paddr = (long)((void *)&process_list[0].pcb - 0x80000000);
	process_list[0].pcb.p0br = (struct pte *)(0x210000 + 0x80000000);
	process_list[0].pcb.p0lr = 0x800000 / 512;
	process_list[0].pcb.p0lr |= 0x4000000; /* No pending AST */
	process_list[0].pcb.p1br = (struct pte *)(0x220000 + 0x80000000);
	process_list[0].pcb.p1lr = 0x800000 / 512;

	process_list[1].id = 1;
	process_list[1].status = PROC_READY;
	process_list[1].pcb.pcb_paddr = (long)((void *)&process_list[1].pcb - 0x80000000);
	process_list[1].pcb.p0br = (struct pte *)(0x210000 + 0x80000000);
	process_list[1].pcb.p0lr = 0x800000 / 512;
	process_list[1].pcb.p0lr |= 0x4000000; /* No pending AST */
	process_list[1].pcb.p1br = (struct pte *)(0x220000 + 0x80000000);
	process_list[1].pcb.p1lr = 0x800000 / 512;
	process_list[1].pcb.pc = (unsigned long)dummy_process + 2; /* 2 = function prologue offset */
	process_list[1].pcb.psl = 0x00000000;
	process_list[1].pcb.ksp = 0x80440000;
	process_list[1].pcb.esp = 0x80440000;
	process_list[1].pcb.ssp = 0x80440000;
	process_list[1].pcb.usp = 0x80440000;
	process_list[1].pcb.fp  = 0x80440000;

	/* set current process */
	current_process = 0;
	mtpr(process_list[current_process].pcb.pcb_paddr, PR_PCBB);
}

void run_multitask_on()
{
	setup_processes();
	sched_on = 1;
}

void run_multitask_off()
{
	sched_on = 0;
}
