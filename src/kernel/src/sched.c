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

#include "pcb.h"

#include "sched.h"

unsigned long next_pcbb;

asm("  .text               ");
asm("  .align 2            ");
asm("  .globl switchto     ");
asm("  switchto:           ");
asm("  svpctx              ");
asm("  movl next_pcbb,%r0  ");
asm("  mtpr %r0,$16        "); /* set PA of new pcb */
asm("  ldpctx              ");
asm("  rei                 ");

void dispatcher(int new_proc)
{
	kprintf("|");

	next_pcbb = (unsigned long)process_list[new_proc].pcb.pcb_paddr;

	current_process = new_proc;
}

int scheduler()
{
	int next_process;

	/* just toggle procs :) */
	if(current_process)
		next_process = 0;
	else
		next_process = 1;

	return next_process;
}

void switch_context()
{
	int new_proc = scheduler();
	dispatcher(new_proc);
}

void sched_init()
{
	sched_on = 0;
	sched_running = 0;
}
