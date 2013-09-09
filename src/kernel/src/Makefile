#
# Copyright (c) 2013 Javier M. Mellid
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND CONTRIBUTORS
# ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
# TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
# PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
# BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
#

CC         = vax--netbsdelf-gcc
AS         = vax--netbsdelf-as
LD         = vax--netbsdelf-ld
SIZE       = vax--netbsdelf-size
OBJCOPY    = vax--netbsdelf-objcopy
STRIP      = vax--netbsdelf-strip

PRISTINE   = -nostdlib -nostartfiles -nodefaultlibs

HDIR       = ../headers
OBJ        = kernel.o console.o assert.o scb.o shell.o sched.o mtask.o tests.o heap.o

BUILD_TYPE ?= release

%.o: %.c
	$(CC) -c -o $@ $^ $(PRISTINE) $(DEBUG) -I$(HDIR) -Xassembler -W

rel: kernel

dbg: debug

kernel: $(OBJ)
	$(CC) -o str.o -c str.S -I$(HDIR)
	$(AS) -o reloc.o reloc.s
	$(LD) -o kernel.out reloc.o str.o $^ -T link.lds
ifeq ($(strip $(BUILD_TYPE)),release)
	$(STRIP) -x kernel.out -N _relocated
endif
	$(SIZE) kernel.out
	$(OBJCOPY) -O binary kernel.out kernel

debug: DEBUG += "-DDEBUG -g3"
debug: BUILD_TYPE = debug
debug:
	$(MAKE) DEBUG=$(DEBUG) BUILD_TYPE=$(BUILD_TYPE) rel

clean:
	rm -f kernel kernel.out *.o *~

.PHONY: clean
