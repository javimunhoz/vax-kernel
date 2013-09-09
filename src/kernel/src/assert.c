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

#include "stdarg.h"

void int2str(int val, char *str_val)
{
	int m, q, v, l;
	v = val;
	l = -1;
	while (v)
	{
		v = v / 10;
		l++;
	}
	v = val;
	while (1)
	{
		m = v % 10;
		q = v / 10;
		if ((q == 0) && (m == 0))
			break;
		v = q;
		str_val[l] = '0' + m;
		l--;
	}
}

void kprintf(char *msg, ...);

void _kprintf(char *msg, __gnuc_va_list ap)
{
	char *aux = msg;
	int i;

	while(*aux != 0) {
		if (*aux == '%')
		{
			switch(*(++aux))
			{
				case 'c':
					aux++;
					unsigned int c = va_arg(ap, unsigned int);
					static unsigned char str_c[2];
					str_c[0] = (unsigned char)c;
					str_c[1] = '\0';
					kprintf(str_c);
					break;
				case 's':
					aux++;
					char *str = va_arg(ap, char *);
					kprintf(str);
					break;
				case 'd':
					aux++;
					int val = va_arg(ap, int);
					static char str_val[20];
					for (i=0; i<20; i++)
						str_val[i] = '\0';
					int2str(val, str_val);
					kprintf(str_val);
					break;
				default:
					/* do nothing */
					break;

			}
		}

		if (*aux == '\n') {
			pr_putchar('\n');
			pr_putchar('\r');
			aux++;
		}

		if (*aux != 0) {
			pr_putchar(*aux);
			aux++;
		}
	}
}

void kprintf(char *msg, ...)
{
	__gnuc_va_list ap;

	va_start(ap, msg);

	_kprintf(msg, ap);

	va_end(ap);
}

void panic(char *fmt, ...)
{
	__gnuc_va_list ap;

	kprintf("panic: ");

	va_start(ap, fmt);

	_kprintf(fmt, ap);

	va_end(ap);

	kprintf("\nhalting now ...\n");

	/* halt now! */
	asm("halt");
}

/*
 * t = assertion type
 * f = file
 * l = line failing
 * e = error
 */
void kernel_assert(const char *t, const char *f, int l, const char *e)
{
	panic("kernel %sassertion \"%s\" failed: file \"%s\", line %d", t, e, f, l);
}
