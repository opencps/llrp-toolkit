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
MSVSDIR=../VS_LTKCPP
OPENSOURCEDIR    = ../../opensource/lib

LTKCPP_HDRS = \
	$(LIBDIR)/ltkcpp.h		\
	$(LIBDIR)/ltkcpp_base.h		\
	$(LIBDIR)/ltkcpp_connection.h	\
	$(LIBDIR)/ltkcpp_frame.h	\
	$(LIBDIR)/ltkcpp_platform.h	\
	$(LIBDIR)/ltkcpp_xmltext.h	\
	$(LIBDIR)/out_ltkcpp.h

LTKCPP_LIB_A        = $(LIBDIR)/libltkcpp.a
LIBXML2_LIB_A      = $(OPENSOURCEDIR)/libxml2.a
LTKCPP_MSVS_DIR     = $(MSVSDIR)/libltkcpp
LTKCPP_MSVS_R       = $(LTKCPP_MSVS_DIR)/Release/libltkcpp_r.lib
LTKCPP_LIBS         = $(LTKCPP_LIB_A) $(LTKCPP_MSVS_R)
LIBXML2_LIBS        = $(LIBXML2_LIB_A)
LTKCPP_FILES        = $(LTKCPP_HDRS) $(LTKCPP_LIBS) $(LIBXML2_LIBS)


EXAMPLE_FILES       = ../Examples/example1.cpp ../Examples/Example-Makefile

all: std

std: $(LTKCPP_FILES) $(EXAMPLE_FILES)
	-rm -rf std
	-rm -rf ltkcpp_dox.log
	mkdir std
	chmod 777 std
	mkdir std/libltkcpp
	chmod 777 std/libltkcpp
	cp $(LTKCPP_FILES) std/libltkcpp/.
	chmod 444 std/libltkcpp/*
	mkdir std/libltkcpp/example
	cp ../Examples/example1.cpp std/libltkcpp/example/example1.cpp
	cp ../Examples/Example-Makefile std/libltkcpp/example/Makefile
	chmod 644 std/libltkcpp/example/*
	chmod 755 std/libltkcpp/example
	mkdir std/Documentation
	chmod 777 std/Documentation
	(cat ltkcpp.dox ; echo OUTPUT_DIRECTORY="std/Documentation" ) | doxygen - 
	chmod 755 std/Documentation
	chmod 755 std/libltkcpp
	chmod 755 std
	cd std; tar czf libltkcpp_std.tgz libltkcpp Documentation
	cd std; zip -rq libltkcpp_std     libltkcpp Documentation

clean:
	rm -rf *.tgz *.zip
	rm -rf std
