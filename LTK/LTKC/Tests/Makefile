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

LIBDIR=../Library
DX101_REF_DIR=../../Tests/dx101/

INCL = -I$(LIBDIR)
#CFLAGS = -O -Wall
CFLAGS = -g -Wall $(INCL)

LTKC_LIB = $(LIBDIR)/ltkc.a

DX101_REF_IN  = $(DX101_REF_DIR)/dx101_a_in.bin
DX101_REF_OUT = $(DX101_REF_DIR)/dx101_a_out.txt
DX101_TMP_OUT = dx101_out.tmp

all: dx101 dx201

run101: dx101
	./dx101 $(DX101_REF_IN) > $(DX101_TMP_OUT)
	@if cmp $(DX101_TMP_OUT) $(DX101_REF_OUT); \
	 then echo PASSED; \
	 else echo FAILED; \
	fi

dx101 : dx101.o $(LTKC_LIB)
	$(CC) $(CFLAGS) -o dx101 dx101.o $(LTKC_LIB)

dx201 : dx201.o $(LTKC_LIB)
	$(CC) $(CFLAGS) -o dx201 dx201.o $(LTKC_LIB)

clean:
	rm -f *.o *.core core.[0-9]*
	rm -f *.tmp
	rm -f dx101 dx201
