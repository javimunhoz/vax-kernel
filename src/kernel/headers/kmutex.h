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

#ifndef _KMUTEX_H_
#define _KMUTEX_H_

#include "mtpr.h"

struct kmutex {
	unsigned int mtx_id;
	unsigned int mtx_ipl;
	unsigned int mtx_lock;
};

static inline void
MUTEX_INITIALIZE(struct kmutex *mtx)
{
	mtx->mtx_id   = 0;
	mtx->mtx_lock = 0;
	/* save current IPL */
	mtx->mtx_ipl  = mfpr(PR_IPL);
}

static inline unsigned int
MUTEX_ACQUIRE(struct kmutex *mtx)
{
	unsigned int rv;

	/* raise IPL */
	mtpr(0x1f, PR_IPL);

	/* non-atomic test and test :( */
	__asm __volatile (
		"clrl %1;"
		"incl %1;"
		"bbcs $0,%0,1f;"
		"decl %1;"
		"1:"
	    : "=m"(mtx->mtx_lock), "=r"(rv));

	/* restore IPL */
	mtpr(mtx->mtx_ipl, PR_IPL);

	return rv;
}

static inline void
MUTEX_RELEASE(struct kmutex *mtx)
{
	__asm __volatile (
		"bbsc $0,%0,1f;"
		"1:"
	    : "=m" (mtx->mtx_lock));
}

static inline unsigned int
MUTEX_ACQUIRE_INTERLOCKED(struct kmutex *mtx)
{
	unsigned int rv;

	/* non-atomic test and test :( */
	__asm __volatile (
		"clrl %1;"
		"bbssi $0,%0,1f;"
		"incl %1;"
		"1:"
	    : "=m"(mtx->mtx_lock), "=r"(rv));

	return rv;
}

static inline void
MUTEX_RELEASE_INTERLOCKED(struct kmutex *mtx)
{
	__asm __volatile (
		"bbcci $0,%0,1f;"
		"1:"
	    : "=m" (mtx->mtx_lock));
}

#endif /* _KMUTEX_H_ */
