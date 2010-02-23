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

LIBDIR=../Library
XML2DIR=../../opensource/include/libxml2
INCL = -I$(LIBDIR) -I$(XML2DIR)

#CPPFLAGS = -O -Wall $(INCL)
AR              = ar
CXX             = g++
CPPFLAGS        = -g -Wall $(INCL)

LTKCPP_LIB = $(LIBDIR)/libltkcpp.a
XML2_LIB = ../../opensource/lib/libxml2.a
LTKCPP_HDRS = \
	$(LIBDIR)/ltkcpp.h		\
	$(LIBDIR)/ltkcpp_base.h		\
	$(LIBDIR)/ltkcpp_connection.h	\
	$(LIBDIR)/ltkcpp_frame.h	\
	$(LIBDIR)/ltkcpp_platform.h	\
	$(LIBDIR)/ltkcpp_xmltext.h	\
	$(LIBDIR)/out_ltkcpp.h

all: xml2llrp llrp2xml dx201

everything:
	make all

xml2llrp : xml2llrp.o $(LTKCPP_LIB) $(XML2_LIB)
	$(CXX) $(CPPFLAGS) -o xml2llrp xml2llrp.o $(LTKCPP_LIB) $(XML2_LIB) 

xml2llrp.o : xml2llrp.cpp $(LTKCPP_HDRS)
	$(CXX) -c $(CPPFLAGS) xml2llrp.cpp -o xml2llrp.o

llrp2xml : llrp2xml.o $(LTKCPP_LIB)
	$(CXX) $(CPPFLAGS) -o llrp2xml llrp2xml.o $(LTKCPP_LIB)

llrp2xml.o : llrp2xml.cpp $(LTKCPP_HDRS)
	$(CXX) -c $(CPPFLAGS) llrp2xml.cpp -o llrp2xml.o

dx201 : dx201.o $(LTKCPP_LIB)
	$(CXX) $(CPPFLAGS) -o dx201 dx201.o $(LTKCPP_LIB)

dx201.o : dx201.cpp $(LTKCPP_HDRS)
	$(CXX) -c $(CPPFLAGS) dx201.cpp -o dx201.o

clean:
	rm -f *.o *.core core.[0-9]*
	rm -f *.tmp
	rm -f xml2llrp
	rm -f llrp2xml
	rm -f dx201
