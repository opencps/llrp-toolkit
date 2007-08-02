############################################################################
#   Copyright 2007 Impinj, Inc.
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

#CFLAGS = -O -Wall
CFLAGS = -g -Wall

LLRPDEF = ../../Definitions/Core/llrp-1x0-def.xml

LTKC_LIB = ltkc.a

LTKC_HDRS = \
	ltkc.h			\
	ltkc_base.h		\
	ltkc_connection.h	\
	ltkc_frame.h		\
	ltkc_prxml.h		\
	ltkc_platform.h

LTKC_OBJS = \
	ltkc_array.o		\
	ltkc_connection.o	\
	ltkc_element.o		\
	ltkc_encdec.o		\
	ltkc_error.o		\
	ltkc_framedecode.o	\
	ltkc_frameencode.o	\
	ltkc_frameextract.o	\
	ltkc_hdrfd.o		\
	ltkc_prxmlencode.o	\
	ltkc_typeregistry.o	\
	ltkc_genout.o


all:    $(LTKC_LIB)

$(LTKC_LIB) : $(LTKC_OBJS)
	$(AR) crv $(LTKC_LIB) $(LTKC_OBJS)

$(LTKC_OBJS) :  $(LTKC_HDRS) out_ltkc_h.inc

ltkc_genout.o : ltkc_genout.c out_ltkc_c.inc out_ltkc_h.inc
	$(CC) -c $(CFLAGS) -Wno-unused ltkc_genout.c

out_ltkc_h.inc: ltkc_gen_h.xslt $(LLRPDEF)
	xsltproc -o out_ltkc_h.inc ltkc_gen_h.xslt $(LLRPDEF)

out_ltkc_c.inc: ltkc_gen_c.xslt $(LLRPDEF)
	xsltproc -o out_ltkc_c.inc ltkc_gen_c.xslt $(LLRPDEF)

clean:
	rm -f *.o *.core core.[0-9]*
	rm -f out_ltkc_c.inc out_ltkc_h.inc
	rm -f $(LTKC_LIB)
