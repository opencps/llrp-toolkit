#!/usr/bin/env perl

use Pod::Html;

my @llrp_src_files = qw{ Builder Schema Link Helper };

foreach $fname (@llrp_src_files) {

	pod2html (
		"--htmlroot=./Documentation",
		"--infile=./RFID/LLRP/$fname.pm",
		"--title=LTK-Perl $fname",
		"--outfile=./Documentation/$fname.html"
	);
}

