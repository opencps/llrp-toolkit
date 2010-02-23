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

#CFLAGS = -O -Wall $(INCL)
AR          = ar
CC          = gcc
CFLAGS      = -g -Wall $(INCL)

LTKC_LIB = $(LIBDIR)/libltkc.a
XML2_LIB = ../../opensource/lib/libxml2.a
LTKC_HDRS = \
	$(LIBDIR)/ltkc.h		\
	$(LIBDIR)/ltkc_base.h		\
	$(LIBDIR)/ltkc_connection.h	\
	$(LIBDIR)/ltkc_frame.h		\
	$(LIBDIR)/ltkc_platform.h	\
	$(LIBDIR)/ltkc_xmltext.h	\
	$(LIBDIR)/out_ltkc.h

all: xml2llrp llrp2xml dx201

everything:
	make all

llrp2xml.o : llrp2xml.c $(LTKC_HDRS)
	$(CC) -c $(CFLAGS) llrp2xml.c -o llrp2xml.o

llrp2xml : llrp2xml.o $(LTKC_LIB)
	$(CC) $(CFLAGS) -o llrp2xml llrp2xml.o $(LTKC_LIB) $(XML2_LIB)

xml2llrp.o : xml2llrp.c $(LTKC_HDRS)
	$(CC) -c $(CFLAGS) xml2llrp.c -o xml2llrp.o

xml2llrp : xml2llrp.o $(LTKC_LIB) $(XML2_LIB)
	$(CC) $(CFLAGS) -o xml2llrp xml2llrp.o $(LTKC_LIB) $(XML2_LIB)

dx201 : dx201.o $(LTKC_LIB)
	$(CC) $(CFLAGS) -o dx201 dx201.o $(LTKC_LIB)

dx201.o : dx201.c $(LTKC_HDRS)
	$(CC) -c $(CFLAGS) dx201.c -o dx201.o

clean:
	rm -f *.o *.core core.[0-9]*
	rm -f *.tmp
	rm -f xml2llrp
	rm -f llrp2xml
	rm -f dx201
