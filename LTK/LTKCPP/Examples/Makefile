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

#CPPFLAGS = -O -Wall $(INCL)
AR              = ar
CXX             = g++
CPPFLAGS        = -g -Wall $(INCL)

LTKCPP_LIB = $(LIBDIR)/libltkcpp.a
LTKCPP_HDRS = \
	$(LIBDIR)/ltkcpp.h		\
	$(LIBDIR)/ltkcpp_base.h		\
	$(LIBDIR)/ltkcpp_connection.h	\
	$(LIBDIR)/ltkcpp_frame.h	\
	$(LIBDIR)/ltkcpp_platform.h	\
	$(LIBDIR)/ltkcpp_xmltext.h	\
	$(LIBDIR)/out_ltkcpp.h

all: example1

everything:
	make all

example1 : example1.o $(LTKCPP_LIB)
	$(CXX) $(CPPFLAGS) -o example1 example1.o $(LTKCPP_LIB)

example1.o : example1.cpp $(LTKCPP_HDRS)
	$(CXX) -c $(CPPFLAGS) example1.cpp -o example1.o

clean:
	rm -f *.o *.core core.[0-9]*
	rm -f example1
