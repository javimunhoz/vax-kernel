/*-
 * Copyright (c) 2008 The NetBSD Foundation, Inc.
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

#include "asm.h"

ENTRY(unit_init, R6|R7|R8|R9|R10|R11)
	mfpr    $17,%r7         # Wanted bu KDB
	movl    4(%ap),%r0      # init routine address
	movl    8(%ap),%r9      # RPB in %r9
	movl    12(%ap),%r1     # VMB argument list
	callg   (%r1),(%r0)
	ret

/*
 * romread_uvax (int lbn, int size, void *buf, struct rpb *rpb)
 */
ENTRY(romread_uvax, 0xFFE)
	movl    16(%ap),%r11    # restore boot-contents of %r11 (rpb)
	movl    52(%r11), %r7   # load iovec/bqo into %r7
	addl3   (%r7), %r7, %r6 # load qio into %r6
	pushl   %r11            # base of rpb
	pushl   $0              # virtual-flag
	pushl   $33             # read-logical-block
	pushl   4(%ap)          # lbn to start reading
	pushl   8(%ap)          # number of bytes to read
	pushl   12(%ap)         # buffer-address
	calls   $6, (%r6)       # call the qio-routine
	ret                     # %r0 holds the result

/*
 * romwrite_uvax (int lbn, int size, void *buf, struct rpb *rpb)
 */
ENTRY(romwrite_uvax, 0xFFE)
	movl    16(%ap), %r11   # restore boot-contents of %r11 (rpb)
	movl    52(%r11), %r7   # load iovec/bqo into %r7
	addl3   (%r7), %r7, %r6 # load qio into %r6
	pushl   %r11            # base of rpb
	pushl   $0              # virtual-flag
	pushl   $32             # write-logical-block
	pushl   4(%ap)          # lbn to start reading
	pushl   8(%ap)          # number of bytes to read
	pushl   12(%ap)         # buffer-address
	calls   $6, (%r6)       # call the qio-routine
	ret                     # %r0 holds the result
