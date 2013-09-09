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

#ifndef _ISO9660_H_
#define _ISO9660_H_

/* a volume descriptor block */
struct iso9660_voldesc {
	unsigned char	magic[8];	/* 0x01, "CD001", 0x01, 0x00 */
	unsigned char	systemid[32];	/* system identifier */
	unsigned char	volumeid[32];	/* volume identifier */
	unsigned char	unused[8];	/* character set in secondary desc */
	unsigned char	volsize[8];	/* volume size */
	unsigned char	charset[32];
	unsigned char	volsetsize[4];	/* volume set size = 1 */
	unsigned char	volseqnum[4];	/* volume sequence number = 1 */
	unsigned short  blocksize_lsb;	/* logical block size lsb */
	unsigned short  blocksize_msb;	/* logical block size msb */
	unsigned char	pathsize[8];	/* path table size */
	unsigned short  lpathloc_lsb;   /* Lpath (lsb) */
	unsigned short  lpathloc_msb;	/* Lpath (msb) */
	unsigned char	olpathloc[4];	/* optional Lpath */
	unsigned char	mpathloc[4];	/* Mpath */
	unsigned char	ompathloc[4];	/* optional Mpath */
	unsigned char	rootdir[34];	/* directory entry for root */
	unsigned char	volumeset[128];	/* volume set identifier */
	unsigned char	publisher[128];
	unsigned char	preparer[128];	/* data preparer identifier */
	unsigned char	application[128];	/* application identifier */
	unsigned char	notice[37];	/* copyright notice file */
	unsigned char	abstract[37];	/* abstract file */
	unsigned char	biblio[37];	/* bibliographic file */
	unsigned char	cdate[17];	/* creation date */
	unsigned char	mdate[17];	/* modification date */
	unsigned char	xdate[17];	/* expiration date */
	unsigned char	edate[17];	/* effective date */
	unsigned char	fsvers;		/* file system version = 1 */
};

/* a directory entry */
struct iso9660_direntry {
	unsigned char	len;
	unsigned char	xlen;
	unsigned int	dloc_lsb;
	unsigned int    dloc_msb;
	unsigned int	dlen_lsb;
	unsigned int    dlen_msb;
	unsigned char	date[7];
	unsigned char	flags;
	unsigned char	unitsize;
	unsigned char	gapsize;
	unsigned char	volseqnum[4];
	unsigned char	namelen;
	unsigned char	name[1];	/* chumminess */
} __attribute__ ((packed));

/* a path table entry */
struct iso9660_pathentry {
	unsigned char   namelen;
	unsigned char   xlen;
	unsigned char   dloc[4];
	unsigned char   parent[2];
	unsigned char   name[1];        /* chumminess */
};

#endif /* _ISO9660_H_ */
