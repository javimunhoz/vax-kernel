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
OBJ        = boot.o console.o assert.o

BUILD_TYPE ?= release

%.o: %.c
	$(CC) -c -o $@ $^ $(PRISTINE) $(DEBUG) -I$(HDIR)

rel: boot

dbg: debug

boot: $(OBJ)
	$(CC) -o str.o -c str.S -I$(HDIR)
	$(CC) -o func.o -c func.S -I$(HDIR)
	$(AS) -o start.o start.s
	$(LD) -o boot.out $^ start.o func.o str.o -T link.lds
ifeq ($(strip $(BUILD_TYPE)),release)
	$(STRIP) -x boot.out -N _etext -N _edata
endif
	$(SIZE) boot.out
	$(OBJCOPY) -O binary boot.out boot

debug: DEBUG += "-DDEBUG -g3"
debug: BUILD_TYPE = debug
debug:
	$(MAKE) DEBUG=$(DEBUG) BUILD_TYPE=$(BUILD_TYPE) rel

clean:
	rm -f boot boot.out *.o *~

.PHONY: clean
