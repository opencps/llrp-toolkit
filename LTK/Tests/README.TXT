

OVERVIEW OF THE LLRP Tool Kit "Golden" Test files
Last updated 2 Aug 2007




INTRODUCTION
============

This LTK/Tests/ directory contains files that facilitate
common tests across LTK implementations.

The most recent versions of and news about the
LLRP Tool Kit is available on SourceForge.net

    http://sourceforge.net/projects/llrp-toolkit/




LIST OF FILES
=============
dx101/
    Directory of files used for the common dx101 test.
    Dx101 reads a file containing LLRP binary frames. For each frame:
        - Decode it into an object tree
        - Encode it into XML text
        - Encode it into an LLRP frame
        - Compare the original input frame to the one produced by the encoder
        - Destruct the object tree.
    It is used as a regression to make sure things work consistently
    and to verify there are no memory leaks.

dx101/dx101_a_in.bin
    The common input to the dx101 tests

dx101/dx101_a_out.txt
    The reference output from a dx101 test run.
    A dx101 test run's output should match this file.
    On Linux systems, cmp(1) is usually good enough.
    On Windows, the difference in line ending prevents
    a direct byte-by-byte comparisson. We need to change
    the test of have a tool for the comparisson.

dx101/dx101_b.bin -- a second test case aimed specifically at
    EPC size in LLRP RO_ACCESS_REPORT MESSAGES

dx101/dx101_b.xml -- the corresponding reference XML output
	for dx101_b.bin

dx101/dx101_c.bin -- a second test case aimed specifically at
    large RO_ACCESS_REPORTS

dx101/dx101_c.xml -- the corresponding reference XML output
	for dx101_c.bin

README.TXT
    This file that you are reading.

