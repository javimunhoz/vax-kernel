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

#ifndef _VAX_CONSOLE_H_
#define _VAX_CONSOLE_H_

/* PR_TXCS */
#define	GC_RDY	0x80	/* Console ready to xmit chr */
#define	GC_TIE	0x40	/* xmit interrupt enable */
#define	GC_LT	0x80000	/* VAX8600: Enable logical terminal */
#define	GC_WRT	0x8000	/* VAX8600: Allow mtpr's to console */

/* PR_RXCS */
#define	GC_DON	0x80	/* character received */
#define	GC_RIE	0x40	/* recv interrupt enable */

/* PR_RXDB */
#define	GC_ERR	0x8000	/* received character error */
#define	GC_CON	0xf00	/* mfpr($PR_RXDB)&GC_CON==0 then console chr */

/* PR_TXDB */
#define	GC_CONS	0xf00	/* Console software !8600 */
#define	GC_BTFL	0x2	/* boot machine */
#define	GC_CWFL	0x3	/* clear warm start flag */
#define	GC_CCFL	0x4	/* clear cold start flag */

/* Interrupt vectors used */
#define	SCB_G0R	0xf8
#define	SCB_G0T	0xfc
#define	SCB_G1R	0xc8
#define	SCB_G1T	0xcc
#define	SCB_G2R	0xd0
#define	SCB_G2T	0xd4
#define	SCB_G3R	0xd8
#define	SCB_G3T	0xdc

#endif /* _VAX_CONSOLE_H_ */
