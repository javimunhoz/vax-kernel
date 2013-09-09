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

_start:
	.globl _start

.org    0x00                    # uVAX booted from TK50 starts here

	brb     from_0x00       # continue behind dispatch-block

.org    0x02                    # information used by uVAX-ROM

	.byte   0xcf            # offset in words to identification area
	.byte   1               # this byte must be 1
	.word   0               # logical block number (word swapped)
	.word   0               # of the secondary image

from_0x00:
	brw	start

/*
 * Parameter block for uVAX boot.
 */

	VOLINFO	= 0		# 1=single-sided  81=double-sided volumes
	SISIZE	= 16		# size in blocks of secondary image
	SILOAD	= 0		# load offset (usually 0) from the default
	SIOFF	= 0x200		# byte offset into secondary image

.org    0x19e

	.byte   0x18            # must be 0x18
	.byte   0x00            # must be 0x00 (MBZ)
	.byte   0x00            # any value
	.byte   0xFF - (0x18 + 0x00 + 0x00)
		/* 4th byte holds 1s' complement of sum of previous 3 bytes */

	.byte   0x00            # must be 0x00 (MBZ)
	.byte   VOLINFO
	.byte   0x00            # any value
	.byte   0x00            # any value
	.long   SISIZE          # size in blocks of secondary image
	.long   SILOAD          # load offset (usually 0)
	.long   SIOFF           # byte offset into secondary image
	.long   (SISIZE + SILOAD + SIOFF)       # sum of previous 3

	.align	2

.org	0x200

start:
	movl    $_start, %sp			# move stack to a better
	pushr   $0x1fff				# save all regs, used later

	moval	_start, %r1			# get actual base-address of code
	movl	$_start, %r2			# get relocated base-address of code
	subl3	%r2, $_edata, %r0		# get size of text+data

	movc3	%r0, (%r1), (%r2)		# copy code to new location

	movpsl  -(%sp)
	movl    $relocated, -(%sp)		# return-address on top of stack
	rei					# can be replaced with new address

relocated:

	# now relocation is done !!!
	movl    %sp, bootregs
	calls   $0, bmain			# call bmain
	halt					# not reached!
