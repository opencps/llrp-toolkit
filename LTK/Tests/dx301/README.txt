############################################################################
#   Copyright 2007,2010 Impinj, Inc.
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

dx301 tests are designed to test bad XML messages to ensure that 
XML parsers properly reject these messages. 

It can be automated as follows.

the dx301_<>.xml file can be converted to LLRP binary.  The 
binary should always contain the error packets with the following
format

...
TODO