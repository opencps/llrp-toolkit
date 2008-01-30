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
INCL = -I$(LIBDIR)

#CFLAGS = -O -Wall $(INCL)
AR          = ar
CC          = gcc
CFLAGS      = -g -Wall $(INCL)

LTKC_LIB = $(LIBDIR)/libltkc.a
LTKC_HDRS = \
	$(LIBDIR)/ltkc.h		\
	$(LIBDIR)/ltkc_base.h		\
	$(LIBDIR)/ltkc_connection.h	\
	$(LIBDIR)/ltkc_frame.h		\
	$(LIBDIR)/ltkc_platform.h	\
	$(LIBDIR)/ltkc_xmltext.h	\
	$(LIBDIR)/out_ltkc.h

all: dx101 dx201

everything:
	make all

dx101 : dx101.o $(LTKC_LIB)
	$(CC) $(CFLAGS) -o dx101 dx101.o $(LTKC_LIB)

dx101.o : dx101.c $(LTKC_HDRS)
	$(CC) -c $(CFLAGS) dx101.c -o dx101.o

dx201 : dx201.o $(LTKC_LIB)
	$(CC) $(CFLAGS) -o dx201 dx201.o $(LTKC_LIB)

dx201.o : dx201.c $(LTKC_HDRS)
	$(CC) -c $(CFLAGS) dx201.c -o dx201.o

clean:
	rm -f *.o *.core core.[0-9]*
	rm -f *.tmp
	rm -f dx101
	rm -f dx201
