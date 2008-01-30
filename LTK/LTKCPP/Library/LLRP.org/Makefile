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

#CPPFLAGS       = -O -Wall -I..
AR              = ar
CXX             = g++
CPPFLAGS        = -g -Wall -I..

LLRPORGDEF          = ../../../Definitions/LLRP.org/LLRPOrgExampleDef.xml

CODE_GEN_H_XSLT     = ../ltkcpp_gen_h.xslt
CODE_GEN_CPP_XSLT   = ../ltkcpp_gen_cpp.xslt

LTKCPP_HDRS = \
	../ltkcpp.h		\
	../ltkcpp_base.h	\
	../ltkcpp_connection.h	\
	../ltkcpp_frame.h	\
	../ltkcpp_platform.h	\
	../ltkcpp_xmltext.h	\
	../ltkcpp_platform.h

LLRPORG_LTKCPP_LIB      = libltkcppllrporg.a
LLRPORG_LTKCPP_OBJS = \
	llrporg_ltkcpp_genout.o


all:    $(LLRPORG_LTKCPP_LIB)

$(LLRPORG_LTKCPP_LIB) : $(LLRPORG_LTKCPP_OBJS)
	$(AR) crv $(LLRPORG_LTKCPP_LIB) $(LLRPORG_LTKCPP_OBJS)

$(LLRPORG_LTKCPP_OBJS) : $(LTKCPP_HDRS)

llrporg_ltkcpp_genout.o : out_llrporg_ltkcpp.inc
llrporg_ltkcpp_genout.o : out_llrporg_ltkcpp.h
llrporg_ltkcpp_genout.o : llrporg_ltkcpp_genout.cpp
	$(CXX) -c $(CPPFLAGS) -Wno-unused llrporg_ltkcpp_genout.cpp \
		-o llrporg_ltkcpp_genout.o

out_llrporg_ltkcpp.h: $(CODE_GEN_H_XSLT) $(LLRPORGDEF)
	xsltproc --stringparam RegistryName LLRPOrg \
		-o out_llrporg_ltkcpp.h \
		$(CODE_GEN_H_XSLT) \
		$(LLRPORGDEF)

out_llrporg_ltkcpp.inc: $(CODE_GEN_CPP_XSLT) $(LLRPORGDEF)
	xsltproc --stringparam RegistryName LLRPOrg \
		-o out_llrporg_ltkcpp.inc \
		$(CODE_GEN_CPP_XSLT) \
		$(LLRPORGDEF)

clean:
	rm -f *.o *.core core.[0-9]*
	rm -f out_*.inc out_*.h
	rm -f *.a
