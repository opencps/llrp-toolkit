use File::Spec;

# provide the name of the file to convert as parameter
# should work on cs, csproj, xslt files.
# may not catch some stuff in .cs

my $old_namespace = 'Org.LLRP.LTK.LLRPV1';
my $new_namespace = 'Com.Example.LLRPV1';

foreach my $infile (@ARGV) {

	my ($vol, $dir, $fname) = File::Spec->splitpath ($infile);
	(uc($fname) =~ /\.CS|\.XSLT|\.CSPROJ$/) || die "This utility only handles csproj, xslt and cs files.";
	my $outfile = File::Spec->catpath ($vol, $dir, '_' . $fname);

	open INFILE, "<$infile";
	open OUTFILE, ">$outfile";
	while (<INFILE>) {
		if ($_ =~ /^(\s*namespace\s+)($old_namespace)(.*)$/) {
			print "CHANGE>",$_;
			print OUTFILE "$1$new_namespace$3\n";
			print "old namespace: $old_namespace\n";
		} elsif ($_ =~ /^(\s*using\s+)($old_namespace)(.*)$/) {
			print "CHANGE>",$_;
			print OUTFILE "$1$new_namespace$3\n";
		} elsif ($_ =~ /^(.*<RootNamespace>)$old_namespace(<\/RootNamespace>)/) {
			print OUTFILE "$1$new_namespace$2\n";
		} else {
			print OUTFILE $_;
		}
	}
	close OUTFILE;
	close INFILE;

	system "copy $outfile $infile";
	system "del $outfile";


}
