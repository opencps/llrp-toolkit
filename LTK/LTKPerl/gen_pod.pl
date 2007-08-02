#!/usr/bin/env perl

use Pod::Html;

my @llrp_src_files = qw{ Builder Schema Link Helper };

foreach $fname (@llrp_src_files) {

	pod2html (
		"--htmlroot=./documentation",
		"--infile=./RFID/LLRP/$fname.pm",
		"--title=LLRP-XML/Perl $fname",
		"--outfile=./documentation/$fname.html"
	);
}

