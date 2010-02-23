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

Last updated: 21 Feb 2010


These files are used for unit testing. They are generally used
as inputs to the dx101 self tests of the various implementations.

The files with names ending in .bin are binary files containing
a sequence of LLRP frames as they would be transfered over-the-wire.
The files with names ending in .xml are XML representations. The base
name of the file is the same between the .bin and its .xml counterpart.

A bit about each...

dx101_a.bin, dx101_a.xml
    Appears to mostly be a capture of the examples running, and
    maybe some in-house unit tests for LLRP. It is all standard
    LLRP and there are no extensions. It is a bit of a mystery
    exactly where this came from. These were amont the original
    test files (but named dx101_a_in.bin and dx101_a_out.txt).
    Hence we keep them around with their new name.

dx101_c.bin, dx101_c.xml
    LLRP RO_ACCESS_REPORTS of varying shapes and sizes.
    It was generated in ../Scripts/.

dx101_d.bin, dx101_d.xml
    LLRP ADD_ACCESSSPEC and GET_ACCESSSPECS_RESPONSE of varying
    sizes and shapes.
    It was generated in ../Scripts/.

dx101_e.bin, dx101_e.xml
    LLRP messages with custom parameters at all allowable
    extension points.
    It was generated in ../Scripts/.

dx101_f.bin, dx101_f.xml
   This is a copy of dx101_a except that the XML file
   has been modified to include various XML parsing conditions
   like carriage returns, spaces, tabs, floating point formats,
   hex case etc.

  

