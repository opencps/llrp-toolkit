#############################################################################
#                                                                           #
#  Copyright 2007, 2008 Impinj, Inc.                                        #
#                                                                           #
#  Licensed under the Apache License, Version 2.0 (the "License");          #
#  you may not use this file except in compliance with the License.         #
#  You may obtain a copy of the License at                                  #
#                                                                           #
#      http://www.apache.org/licenses/LICENSE-2.0                           #
#                                                                           #
#  Unless required by applicable law or agreed to in writing, software      #
#  distributed under the License is distributed on an "AS IS" BASIS,        #
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. #
#  See the License for the specific language governing permissions and      #
#  limitations under the License.                                           #
#                                                                           #
#                                                                           #
#############################################################################
# $$
#
# RFID::LLRP::Helper.pm - Helper routines to make life with LLRP easier
# Generate Perl "POD" documentation using the pod generator of your choice
#
#############################################################################

=pod

=head1 RFID::LLRP::Helper

=head1 SYNOPSIS

LLRP helpers

=head1 DESCRIPTION

Certain operations are done quite often in LLRP. For instance, it is often
necessary to delete all ROSpecs and AccessSpecs which may be outstanding from a
previous session. This module has helper routines to make that simple.

In general, if not 'link' related, you can factor commonly useful LLRP stuff
into this module. At least, until this module needs refactoring.

=head1 DETAILS

=head1 API

=over 4

=cut


package RFID::LLRP::Helper;

require Exporter;
our @ISA	= qw(Exporter);
our @EXPORT_OK	= qw(
	decode_gpi_state
	delete_rospecs
	delete_access_specs
	are_identical
	compare_subtrees
	factory_default
	cleanup_all
	cleanup_and_die
	max min uniq
	get_values
	get_msg_name
);

use Carp;
use RFID::LLRP::Builder qw(encode_message decode_message);
use RFID::LLRP::Link qw(reader_connect reader_disconnect read_message transact);

=item C<decode_gpi_state ($node)>

This function decodes the GPI state froma GET_READER_CONFIG_RESPONSE document
and returns the GPI values as an array (gpi1, gpi2, gpi3, gpi4)
of zero and one values or, if called in a scalar context, the numerical
equivalent (gpi1 = lsb, gpi4=msb)

=cut

sub decode_gpi_state {
	my $root = shift;
        foreach my $g ($root->findnodes('//*[local-name() = "GPIPortCurrentState"]')) {
                $gpi[$g->findvalue('./*[local-name() = "GPIPortNum"]')-1] 
			= $g->findvalue('./*[local-name() = "State"]') eq 'High' ? 1 : 0;
        }
	return wantarray? @gpi : 
		unpack("C",pack("b8",join('',@gpi). "0"x8));

}



=item C<delete_rospecs ($sock, %params)>

This function deletes all ROSpecs from the reader, or if C<spec_id>s are
provided at the end of the argument list, it deletes the ones you ask it to.

=cut


sub delete_rospecs {
	my ($sock, %params) = @_;

	my @spec_ids;
	if (exists $params{SpecIDs}) {
		@spec_ids = @{$params{SpecIDs}};
	} else {
		push @spec_ids, 0;
	}

	foreach (@spec_ids) {
		my $mid = 'MessageID="345"';
		my $spec = "<ROSpecID>$_</ROSpecID>";
		transact ($sock, qq{
			<DELETE_ROSPEC $mid>$spec</DELETE_ROSPEC>
		}, Timeout => $params{Timeout}, 
		Trace => $params{Trace});
	}
}

=item C<decode_access_specs ($sock, @spec_ids)>

This function deletes all AccessSpecs from the reader, or if C<spec_id>s are
provided at the end of the argument list, it deletes the ones you ask it to.

=cut

sub delete_access_specs {

	my ($sock, %params) = @_;

	my @spec_ids;
	if (exists $params{SpecIDs}) {
		@spec_ids = @{$params{SpecIDs}};
	} else {
		push @spec_ids, 0;
	}

	foreach (@spec_ids) {
		my $mid = 'MessageID="346"';
		my $spec = "<AccessSpecID>$_</AccessSpecID>";
		transact ($sock, qq{
			<DELETE_ACCESSSPEC $mid>$spec</DELETE_ACCESSSPEC>
		}, Trace => $params{Trace});
	}
}

=item C<are_identical (@docs)>

This function attempts to normalize and compare a list of LLRP-XML DOM trees.

It returns false on the first match failure, or true if all trees appear
identical.

=cut

# compare xml documents (returns false if any missing)
sub are_identical {

	my $last = undef;
	foreach $doc (@_) {

		$_ = $doc->toString(1);
		s/\s+/ /sg;		# collapse whitespace
		s/ *> *< */></sg;	# tighten up bracketing
		s/^\s+//s;		# remove leading spaces
		s/\s+$//s;		# removed trailing spaces
		s/>\s+/>/s;		# remove whitespace around enums
		s/\s+</</s;		# remove whitespace around enums

		# tighten empty element close
		s/<(\w+)(\s+.*|)>\s*<\/\1>/<$1$2\/>/g;

#		print "\n$_\n";

		if (defined $last && $last ne $_) {
			return 0;
		}
		$last = $_;
	}

	return 1;
}

=item C<compare_subtrees (@docs)>

This function attempts to normalize and compare subtrees within a list of
LLRP-XML DOM trees, based upon an XPath.

It returns false on the first match failure, or true if all trees appear
identical.

=cut


# compare subtrees of XML documents for identity
sub compare_subtrees {
	my ($xpath, @docs) = @_;
	my $last = undef;
	foreach $doc (@docs) {
		my ($node) = $doc->findnodes ('//*[local-name() = "' . $xpath . '"]');
		if (defined $last) {
			return 0 unless are_identical ($last, $node);
		}
		$last = $node;
	}

	return 1;
}


=item C<factory_default ($sock)>

This function returns the reader to "factory default state," as defined
by LLRP using a C<SET_READER_CONFIG> message.

It returns false on the first match failure, or true if all trees appear
identical.

Additional parameters beyond $sock are passed through to the transact routine.

=cut

sub factory_default {

	my $sock = shift;

	my $mid = 'MessageID="0"';

	transact ($sock,
		qq{	
		<SET_READER_CONFIG $mid>
			<ResetToFactoryDefault>true</ResetToFactoryDefault>
		</SET_READER_CONFIG>
		}, @_
	);
}

=item C<cleanup_all ($sock)>

This routine restores the reader to factory default state, deletes
any C<ROSpec>s and/or C<AccessSpec>s.

=cut

sub cleanup_all {
	foreach (qw{factory_default delete_rospecs delete_access_specs}) {
		$_->(@_);
	}
}

=item C<cleanup_and_die ($sock, $msg, %params)>

This routine restores the reader to factory default state, deletes
any C<ROSpec>s and/or C<AccessSpec>s, disconnects and dies with $msg.

=cut

sub cleanup_and_die {
	my ($sock, $msg, %params) = @_;
	foreach (qw{factory_default delete_rospecs delete_access_specs}) {
		$_->($sock, Trace => $params{Trace});
	}
	reader_disconnect ($sock);
	croak $msg;
}

sub max {
	scalar (@_) || die "No list provided";
	my $best = shift;
	foreach (@_) {
		$best = $_ unless $best >= $_;
	}
	return $best;
}

sub min {
	scalar (@_) || die "No list provided";
	my $best = shift;
	foreach (@_) {
		$best = $_ unless $best <= $_;
	}
	return $best;
}

sub get_values {
	my ($doc, $xpath) = @_;
	$xpath .= '/node()';
	return map { $_->getData } ($doc->findnodes ($xpath));
}

sub get_msg_name {
    return $_[0]->getDocumentElement->localname;
}

sub uniq {
	my %seen;
	foreach (@_) {
		$seen{$_}++;
	}
	return keys %seen;
}



1;

=back

=head1 AUTHOR

John R. Hogerhuis
Kunal Singh
Joel Peshkin

=head1 BUGS

None

=head1 SEE ALSO

EPCGlobal LLRP Specification

=head1 COPYRIGHT

Copyright 2007, 2008 Impinj, Inc.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

=cut
