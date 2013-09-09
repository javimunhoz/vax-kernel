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

#include "kmutex.h"

/* testing non interlocked mutex */
void run_non_interlocked_mutex_test()
{
	struct kmutex mtx;

	MUTEX_INITIALIZE(&mtx);

	kprintf("Testing non interlocked mutex.\n");

	if(MUTEX_ACQUIRE(&mtx)) {
		kprintf("Ok mutex acquired!\n");
		if(!MUTEX_ACQUIRE(&mtx)) {
			MUTEX_RELEASE(&mtx);
			kprintf("Ok mutex released!\n");
			if(MUTEX_ACQUIRE(&mtx))
				kprintf("Ok mutex acquired!\n");
			else
				kprintf("Error acquiring mutex!\n");
			MUTEX_RELEASE(&mtx);
			kprintf("Ok mutex released!\n");
		} else {
			kprintf("Error releasing mutex!\n");
		}
	} else {
		kprintf("Error releasing mutex!\n");
	}
}

/* testing interlocked mutex */
void run_interlocked_mutex_test()
{
	struct kmutex mtx;

	MUTEX_INITIALIZE(&mtx);

	kprintf("Testing interlocked mutex.\n");

	if(MUTEX_ACQUIRE_INTERLOCKED(&mtx)) {
		kprintf("Ok mutex_i acquired!\n");
		if(!MUTEX_ACQUIRE_INTERLOCKED(&mtx)) {
			MUTEX_RELEASE_INTERLOCKED(&mtx);
			kprintf("Ok mutex_i released!\n");
			if(MUTEX_ACQUIRE_INTERLOCKED(&mtx))
				kprintf("Ok mutex_i acquired!\n");
			else
				kprintf("Error acquiring mutex_i!\n");
			MUTEX_RELEASE_INTERLOCKED(&mtx);
			kprintf("Ok mutex_i released!\n");
		} else {
			kprintf("Error releasing mutex_i!\n");
		}
	} else {
		kprintf("Error releasing mutex_i!\n");
	}
}

/* testing clk */
void run_clock_test()
{
	int count = 3;

	kprintf("Testing clk...\n");

	while(1) {
		if ((getsecs() % 2) == 0) {
			kprintf("clk!\n");
			if (--count == 0)
				break;
		}
	}
}

/* testing input console */
void run_console_input_test()
{
	kprintf("Type to test console (q to quit)...\n");

	while(1) {
		if (consbuffer_peek_char() != (unsigned char)-1) {
			unsigned char c;
			c = consbuffer_del_char();
			kprintf("%c", c);
			if (c == 'q') {
				kprintf("\n");
				break;
			}
		}
	}
}

void run_all_tests()
{
	/* test non interlocked mutex */
	run_non_interlocked_mutex_test();

	/* test interlocked mutex */
	run_interlocked_mutex_test();

	/* test clock */
	run_clock_test();

	/* test console input */
	run_console_input_test();
}
