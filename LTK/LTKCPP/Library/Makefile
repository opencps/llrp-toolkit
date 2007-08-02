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

#CPPFLAGS = -O -Wall
CPPFLAGS = -g -Wall

LLRPDEF = ../../Definitions/Core/llrp-1x0-def.xml

LTKCPP_LIB = ltkcpp.a

LTKCPP_HDRS = \
	ltkcpp.h		\
	ltkcpp_base.h		\
	ltkcpp_connection.h	\
	ltkcpp_frame.h		\
	ltkcpp_prxml.h		\
	ltkcpp_platform.h

LTKCPP_OBJS = \
	ltkcpp_array.o		\
	ltkcpp_connection.o	\
	ltkcpp_element.o	\
	ltkcpp_encdec.o		\
	ltkcpp_error.o		\
	ltkcpp_framedecode.o	\
	ltkcpp_frameencode.o	\
	ltkcpp_frameextract.o	\
	ltkcpp_hdrfd.o		\
	ltkcpp_prxmlencode.o	\
        ltkcpp_typeregistry.o	\
	ltkcpp_genout.o


all:    $(LTKCPP_LIB)

$(LTKCPP_LIB) : $(LTKCPP_OBJS)
	$(AR) crv $(LTKCPP_LIB) $(LTKCPP_OBJS)

$(LTKCPP_OBJS) :  $(LTKCPP_HDRS) out_ltkcpp_h.inc

ltkcpp_genout.o : ltkcpp_genout.cpp out_ltkcpp_cpp.inc out_ltkcpp_h.inc
	g++ -c $(CPPFLAGS) -Wno-unused ltkcpp_genout.cpp

out_ltkcpp_h.inc:  ltkcpp_gen_h.xslt     $(LLRPDEF)
	xsltproc -o out_ltkcpp_h.inc   ltkcpp_gen_h.xslt   $(LLRPDEF)

out_ltkcpp_cpp.inc:  ltkcpp_gen_cpp.xslt $(LLRPDEF)
	xsltproc -o out_ltkcpp_cpp.inc ltkcpp_gen_cpp.xslt $(LLRPDEF)

# Handy target to generate code when building with Visual Studio
outs: out_ltkcpp_h.inc out_ltkcpp_cpp.inc

clean:
	rm -f *.o *.core core.[0-9]*
	rm -f out_ltkcpp_cpp.inc out_ltkcpp_h.inc
	rm -f $(LTKCPP_LIB)
