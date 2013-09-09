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

#include "console.h"
#include "mtpr.h"

/*
 * pr_putchar() using MTPR
 */
void pr_putchar(int c)
{
  int timeout = 1<<15; /* don't hang the machine! */

  while ((mfpr(PR_TXCS) & GC_RDY) == 0) /* Wait until xmit ready */
    if (--timeout < 0)
      break;

  mtpr(c, PR_TXDB); /* xmit character */
}

/*
 * pr_getchar() using MTPR
 */
unsigned int pr_getchar()
{
	unsigned int i;
	while ((mfpr(PR_RXCS) & GC_DON) == 0) /* receive chr */
		;
	i = mfpr(PR_RXDB) & 0x7f;
	if (i==13)
		i = 10;
	return i;
}

#define MAX_CONSBUFFER 80

struct {
	unsigned char data[MAX_CONSBUFFER];
	int head;
	int tail;
} consbuffer;

void consbuffer_init()
{
	consbuffer.head = 0;
	consbuffer.tail = 0;
}

int consbuffer_full_buffer()
{
	int head = consbuffer.head;
	int tail = consbuffer.tail;

	if ((head + 1) % MAX_CONSBUFFER == tail)
		return 1;
	else
		return 0;
}

void consbuffer_add_char(unsigned char c)
{
	int head = consbuffer.head;
	int tail = consbuffer.tail;

	if (consbuffer_full_buffer())
		return;

	consbuffer.data[head] = c;
	head = (head + 1) % MAX_CONSBUFFER;
	consbuffer.head = head;
}

unsigned char consbuffer_peek_char()
{
	int head = consbuffer.head;
	int tail = consbuffer.tail;

	/* char available? */
	if (tail == head)
		return (unsigned char)-1;

	return consbuffer.data[tail];
}

unsigned char consbuffer_del_char()
{
	int head = consbuffer.head;
	int tail = consbuffer.tail;
	int next;
	unsigned char c;

	c = consbuffer_peek_char();

	next = (tail+1) % MAX_CONSBUFFER;

	if (next <= head)
		consbuffer.tail = next;

	return c;
}
