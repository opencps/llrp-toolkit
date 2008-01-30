############################################################################
#   Copyright 2007,2008 Impinj, Inc.
#
#   Licensed under the Apache License, Version 2.0 (the "License");
#   you may not use this file except in compliance with the License.
#   You may obtain a copy of the License at
#
#       http://www.apache.org/licenses/LICENSE-2.0
#
#   Unless required by applicable law or agreed to in writing, software
#   distributed under the License is distributed on an "AS IS" BASIS,
#   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#   See the License for the specific language governing permissions and
#   limitations under the License.
#
############################################################################

#CFLAGS     = -O -Wall -I..
AR          = ar
CC          = gcc
CFLAGS      = -g -Wall -I..

LLRPORGDEF      = ../../../Definitions/LLRP.org/LLRPOrgExampleDef.xml

CODE_GEN_H_XSLT = ../ltkc_gen_h.xslt
CODE_GEN_C_XSLT = ../ltkc_gen_c.xslt

LTKC_HDRS = \
	../ltkc.h		\
	../ltkc_base.h		\
	../ltkc_connection.h	\
	../ltkc_frame.h		\
	../ltkc_platform.h	\
	../out_ltkc.h

LLRPORG_LTKC_LIB = libltkcllrporg.a
LLRPORG_LTKC_OBJS = \
	llrporg_ltkc_genout.o


all:    $(LLRPORG_LTKC_LIB)

$(LLRPORG_LTKC_LIB) : $(LLRPORG_LTKC_OBJS)
	$(AR) crv $(LLRPORG_LTKC_LIB) $(LLRPORG_LTKC_OBJS)

$(LLRPORG_LTKC_OBJS) : $(LTKC_HDRS)

llrporg_ltkc_genout.o : out_llrporg_ltkc.inc
llrporg_ltkc_genout.o : out_llrporg_ltkc.h
llrporg_ltkc_genout.o : llrporg_ltkc_genout.c
	$(CC) -c $(CFLAGS) -Wno-unused llrporg_ltkc_genout.c \
		-o llrporg_ltkc_genout.o

out_llrporg_ltkc.h: $(CODE_GEN_H_XSLT) $(LLRPORGDEF)
	xsltproc --stringparam RegistryName LLRPOrg \
		-o out_llrporg_ltkc.h \
		$(CODE_GEN_H_XSLT) \
		$(LLRPORGDEF)

out_llrporg_ltkc.inc: $(CODE_GEN_C_XSLT) $(LLRPORGDEF)
	xsltproc --stringparam RegistryName LLRPOrg \
		-o out_llrporg_ltkc.inc \
		$(CODE_GEN_C_XSLT) \
		$(LLRPORGDEF)

clean:
	rm -f *.o *.core core.[0-9]*
	rm -f out_*.inc out_*.h
	rm -f *.a
