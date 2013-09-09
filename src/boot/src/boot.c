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

#include "rpb.h"
#include "data.h"
#include "asm.h"
#include "iso9660.h"
#include "assert.h"

unsigned *bootregs;
struct rpb *rpb = (struct rpb *)0xf0000;
struct bqo *bqo = (struct bqo *)0xf1000;

struct udadevice {
	unsigned short udaip;
	unsigned short udasa;
};
volatile struct udadevice *csr;

void reinit_vmb_boot_device();
void *alloc(unsigned long int size);
char *load_kernel();
void bcopy(const void *, void *, unsigned long size);

int bmain() {
	void (*kaddress)();

	/* relocate rpb/bqo (which are used by ROM-routines) */

	bcopy ((void*)bootregs[11], rpb, sizeof(struct rpb));
	bcopy ((void*)rpb->iovec, bqo, rpb->iovecsz);

	rpb->rpb_base = rpb;
	rpb->iovec = (int)bqo;

	reinit_vmb_boot_device();

	kprintf("booting ...\n");

	kprintf("loading kernel ...\n");
	kaddress = (void (*))load_kernel();

	kprintf("starting kernel ...\n");
	kaddress();

	kprintf("fatal error! halting ...\n");
	asm("halt");
}

void reinit_vmb_boot_device() {

	int initfn;

	if (bqo->unit_init) {

		initfn = rpb->iovec + bqo->unit_init;
		if (rpb->devtyp == BDEV_UDA || rpb->devtyp == BDEV_TK) {
			/*
			 * This reset do not seem to be done in the
			 * ROM routines, so we have to do it manually.
			 */
			csr = (struct udadevice *)rpb->csrphy;
			csr->udaip = 0;
			/*
			 * MP_STEP1 = 0x0800, bits in UDA status register
			 * during initialisation. step 1 has started.
			 */
			while ((csr->udasa & 0x0800) == 0)
					;
		}

		/*
		 * AP (R12) have a pointer to the VMB argument list,
		 * wanted by bqo->unit_init.
		 */
		unit_init(initfn, rpb, bootregs[12]);

	}
}

extern char _edata[];
static char *top = _edata;

void *alloc(unsigned long int size) {
	void *ut = top;
	top += size;
	return ut;
}

#define SECTOR_SZ_ISO9660 2048
#define SECTOR_SZ_VAX     512

#define S_ISO2VAX(X) \
	(X*SECTOR_SZ_ISO9660/SECTOR_SZ_VAX)

char *load_kernel() {

	int dummy = 0;

	struct iso9660_voldesc *pvd;
	struct iso9660_pathentry *pte;
	struct iso9660_direntry *pde;

	char *pvd_buff;
	char *pte_buff;
	char *pde_buff;

	char *kbuff;
	unsigned int kbuff_len;

	pvd_buff = alloc(SECTOR_SZ_ISO9660);

	/* load primary volume descriptor */
	romread_uvax(S_ISO2VAX(16), SECTOR_SZ_ISO9660, pvd_buff, rpb);

	pvd = (struct iso9660_voldesc *)pvd_buff;

	/* check pvd signature 0x01, 'CD001', 0x01, 0x00*/
	BDASSERT( pvd->magic[0] == 0x01 && \
		  pvd->magic[1] == 0x43 && \
		  pvd->magic[2] == 0x44 && \
		  pvd->magic[3] == 0x30 && \
		  pvd->magic[4] == 0x30 && \
		  pvd->magic[5] == 0x31 && \
		  pvd->magic[6] == 0x01 && \
		  pvd->magic[7] == 0x00 );

	/* logical block size == SECTOR_SZ_ISO9660 */
	BDASSERT( pvd->blocksize_lsb == SECTOR_SZ_ISO9660 );

	pte_buff = alloc(SECTOR_SZ_ISO9660);

	/* read L path table (lsb) pvd->lpathloc_lsb */
	romread_uvax(S_ISO2VAX(pvd->lpathloc_lsb), SECTOR_SZ_ISO9660, pte_buff, rpb);

	pte = (struct iso9660_pathentry *)pte_buff;

	BDASSERT( (pte->namelen == 0x01) && (*(unsigned short *)pte->parent == 0x01) );

	pde_buff = alloc(SECTOR_SZ_ISO9660);

	/* read logical block number of the first logical block
	   allocated to the directory */
	romread_uvax(S_ISO2VAX(*(unsigned int *)pte->dloc), SECTOR_SZ_ISO9660, pde_buff, rpb);

	pde = (struct iso9660_direntry *)pde_buff;

	while (1)
	{
		if ((pde->namelen == 9) && (strncmp(pde->name, "KERNEL", 6) == 0))
			break;
		pde = (struct iso9660_direntry *)((unsigned int)pde + (unsigned int)pde->len);
	}

	kbuff_len = ((pde->dlen_lsb/SECTOR_SZ_ISO9660)+1)*SECTOR_SZ_ISO9660;

	/* XXX: allocating twice to move heap's top indirectly, make room to avoid
	 * kernel corruption when running long relocation */
	kbuff = alloc(kbuff_len);
	kbuff = alloc(kbuff_len);

	/* read kernel from disk */
	romread_uvax(S_ISO2VAX(pde->dloc_lsb), kbuff_len, kbuff, rpb);

	return kbuff;
}
