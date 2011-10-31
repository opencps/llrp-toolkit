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

#CPPFLAGS = -O -Wall
AR              = ar
CXX             = g++
CPPFLAGS        = -g -Wall -I../../opensource/include/libxml2

LLRPDEF         = ../../Definitions/Core/llrp-1x0-def.xml

CODE_GEN_H_XSLT = ltkcpp_gen_h.xslt
CODE_GEN_CPP_XSLT = ltkcpp_gen_cpp.xslt

LTKCPP_HDRS = \
        version.inc		\
	ltkcpp.h		\
	ltkcpp_base.h		\
	ltkcpp_connection.h	\
	ltkcpp_frame.h		\
	ltkcpp_platform.h	\
	ltkcpp_xmltext.h	\
	out_ltkcpp.h

LTKCPP_LIB = libltkcpp.a
LTKCPP_OBJS = \
	ltkcpp_array.o		\
	ltkcpp_connection.o	\
	ltkcpp_element.o	\
	ltkcpp_encdec.o	\
	ltkcpp_error.o		\
	ltkcpp_framedecode.o	\
	ltkcpp_frameencode.o	\
	ltkcpp_frameextract.o	\
	ltkcpp_hdrfd.o		\
	ltkcpp_xmltextencode.o	\
	ltkcpp_xmltextdecode.o	\
	ltkcpp_typeregistry.o	\
	ltkcpp_genout.o


all:    $(LTKCPP_LIB)
	cd LLRP.org; make all

everything:
	make all

$(LTKCPP_LIB) : $(LTKCPP_OBJS)
	$(AR) crv $(LTKCPP_LIB) $(LTKCPP_OBJS)

$(LTKCPP_OBJS) :  $(LTKCPP_HDRS)

ltkcpp_array.o         : ltkcpp_array.cpp
	$(CXX) -c $(CPPFLAGS) ltkcpp_array.cpp \
		-o ltkcpp_array.o

ltkcpp_connection.o    : ltkcpp_connection.cpp
	$(CXX) -c $(CPPFLAGS) ltkcpp_connection.cpp \
		-o ltkcpp_connection.o

ltkcpp_element.o       : ltkcpp_element.cpp
	$(CXX) -c $(CPPFLAGS) ltkcpp_element.cpp \
		-o ltkcpp_element.o

ltkcpp_encdec.o        : ltkcpp_encdec.cpp
	$(CXX) -c $(CPPFLAGS) ltkcpp_encdec.cpp \
		-o ltkcpp_encdec.o

ltkcpp_error.o         : ltkcpp_error.cpp
	$(CXX) -c $(CPPFLAGS) ltkcpp_error.cpp \
		-o ltkcpp_error.o

ltkcpp_framedecode.o   : ltkcpp_framedecode.cpp
	$(CXX) -c $(CPPFLAGS) ltkcpp_framedecode.cpp \
		-o ltkcpp_framedecode.o

ltkcpp_frameencode.o   : ltkcpp_frameencode.cpp
	$(CXX) -c $(CPPFLAGS) ltkcpp_frameencode.cpp \
		-o ltkcpp_frameencode.o

ltkcpp_frameextract.o  : ltkcpp_frameextract.cpp
	$(CXX) -c $(CPPFLAGS) ltkcpp_frameextract.cpp \
		-o ltkcpp_frameextract.o

ltkcpp_hdrfd.o         : ltkcpp_hdrfd.cpp
	$(CXX) -c $(CPPFLAGS) ltkcpp_hdrfd.cpp \
		-o ltkcpp_hdrfd.o

ltkcpp_xmltextencode.o : ltkcpp_xmltextencode.cpp
	$(CXX) -c $(CPPFLAGS) ltkcpp_xmltextencode.cpp \
		-o ltkcpp_xmltextencode.o

ltkcpp_typeregistry.o  : ltkcpp_typeregistry.cpp
	$(CXX) -c $(CPPFLAGS) ltkcpp_typeregistry.cpp \
		-o ltkcpp_typeregistry.o

ltkcpp_genout.o        : out_ltkcpp.inc out_ltkcpp.h
ltkcpp_genout.o        : ltkcpp_genout.cpp
	$(CXX) -c $(CPPFLAGS) -Wno-unused ltkcpp_genout.cpp \
		-o ltkcpp_genout.o

out_ltkcpp.h:  ltkcpp_gen_h.xslt     $(LLRPDEF)
	xsltproc --stringparam RegistryName Core \
		-o out_ltkcpp.h   $(CODE_GEN_H_XSLT)   $(LLRPDEF)

out_ltkcpp.inc:  ltkcpp_gen_cpp.xslt $(LLRPDEF)
	xsltproc --stringparam RegistryName Core \
		-o out_ltkcpp.inc $(CODE_GEN_CPP_XSLT) $(LLRPDEF)

clean:
	rm -f *.o *.core core.[0-9]*
	rm -f out_*.inc out_*.h
	rm -f *.a
	cd LLRP.org; make clean

# Handy target to generate code when building with Visual Studio
outs: out_ltkcpp.h out_ltkcpp.inc
