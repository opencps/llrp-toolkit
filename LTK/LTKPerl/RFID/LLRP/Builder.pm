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
# RFID::LLRP::Builder.pm - LLRP binary message and parameter builder
# Generate Perl "POD" documentation using the pod generator of your choice
#
#############################################################################

=pod

=head1 RFID::LLRP::Builder

=head1 SYNOPSIS

This package consists of routines to encode LLRP-XML documents into LLRP Binary
Format, and to decode LLRP-XML documents from LLRP Binary format.

=head1 DESCRIPTION

LLRP messages and parameters are naturally modelled as XML.  This package
provides routines to compile LLRP-XML documents into LLRP Binary Format which
can then be transmitted to an LLRP Reader.

=head1 PRACTICAL EXAMPLE

	use RFID::LLRP::Builder qw(encode_message decode_message);
	use RFID::LLRP::Link qw(reader_connect reader_disconnect read_message);

	my $doc = <<'EOT';
	<?xml version="1.0" encoding="UTF-8"?>

	<GET_READER_CAPABILITIES MessageID="0">
		<RequestedData>0</RequestedData>
	</GET_READER_CAPABILITIES>

	EOT

	my $sock = reader_connect ('speedway-xx-yy-zz');
	$sock->send (encode_message ($doc));
	print (decode_message read_message ($sock))->toString (1);
	reader_disconnect ($sock);

=cut

=head1 DETAILS

=head1 API

=over 4

=cut

package RFID::LLRP::Builder;

use Sub::Exporter -setup => {
	exports => [
		qw(
		encode_message
		memoized_encode_message
		),
		decode_message => \&subclass
	]
};

sub subclass {
	my ($class, $name, $arg) = @_;
	
	return sub {
		$name->(@_, %$arg);
	}
}

#require Exporter;
#our @ISA	= qw(Exporter);
#our @EXPORT_OK	= qw(encode_message decode_message memoized_encode_message);

use strict;

use RFID::LLRP::Schema;
use XML::LibXML;

use Date::Parse;
use Date::Format;

use Data::Dumper;
use Data::HexDump;

use Clone qw(clone);

use threads;
use Thread::Semaphore;

use Memoize;
use bytes;
#memoize ('is_power_of_two');

# read schema
our ($llrp, $registry) = RFID::LLRP::Schema::read_schema ("./ReaderDef.xml");
my $vendor_lookup = $llrp->{Vendors};

# determine next xml element	
sub next_element {
	my $xml_node = (shift);
	if (!defined $xml_node) {
		return;
	}
	for ($xml_node = $xml_node->nextSibling; $xml_node; $xml_node = $xml_node->nextSibling) {
		last if $xml_node->nodeType == XML_ELEMENT_NODE;
	}
	return ($xml_node);
}

# determine first xml element
sub first_element {
	my $xml_node = (shift)->firstChild;
	return unless defined $xml_node;
	return ($xml_node->nodeType == XML_ELEMENT_NODE)
		? $xml_node
		: (next_element ($xml_node));
}

# network order pack/unpack for different bit lengths
my %pack_fmts = (
	'8_U'	=> "C",
	'8_S'	=> "c",
	'16_U'	=> "n",
	'16_S'	=> ['n','S', 's'],
	'32_U'	=> "N",
	'32_S'	=> ['N','L','l'],
	'64_U'	=> "NN",
	'96_U'	=> "CCCCCCCCCCCC",
);

use constant MSG_HEADER_FIELDS => 5;

sub is_numeric {
	use warnings FATAL => 'numeric';
	return defined eval { $_[0] == 0 };
}

sub trim($) {
	my $string = shift;
	
	$string =~ s/^\s+|\s+$//g ;
	return $string;
}

# parses XML node text and formats as LLRP binary
sub gen_format_leaf {

	my $packed_int  = 0;
	my $packed_bits = 0;
	
	return sub {
		my ($desc, $xml_node) = @_;
		my $bits = $desc->{Bits};
		my $type = $desc->{Type};
		my $value;
		my $cells = 0;

		# handle byte vectors (strings) as a pass-through
		if (defined $desc->{Format}) {

			if ($desc->{Format} eq 'Utf8') {
				my $value = $xml_node->textContent;
				return $value, length ($value);
			}

			if ($type eq 'EPC96' || ($desc->{Format} eq 'Hex' && ($type =~ /Byte|Bit/) && $desc->{Array})) {
				my $value = pack ('H*', $xml_node->textContent);
				my $vector_len = $xml_node->getAttribute ('Count');
				if (!defined $vector_len) { $vector_len = (length ($value) * 8) / $bits; }
				return $value, ($vector_len + 0);
			}
		}

		# break xml text into array
		my @raw_values;
		if (defined $desc->{DefaultValue}) {
			@raw_values = ($desc->{DefaultValue});
		} else {
			@raw_values = split (/\s+/, trim ($xml_node->textContent));

			if (!$desc->{Array}) {
				if (@raw_values) {
					@raw_values = ($raw_values[0]);
				} else {
					die "Unexpected blank value for field " . $desc->{Name};
				}
			}
		}

		# lay values from this node
		my $byte_str = "";
		my %decode_xml = (
			'Hex'		=> sub { return hex (shift); },
			'Datetime'	=> sub { 
				my $dt = shift;
				if (is_numeric ($dt)) {
					return $dt + 0.0;
				}
				return str2time ($dt, 'utc') * 1000000.0 },
			'Boolean'	=> sub { 
				return ((shift) =~ /^\s*(true|yes|1)\s*$/i)
			},
			'Decimal' 	=> sub {
				if ($bits >= 64) {
					return shift() + 0.0;
				} else {
					return shift() + 0;
				}
			},
			'Enum'		=> sub { 
				if (is_numeric ($_[0])) {
					return ($_[0] + 0);
				}
				my $etype = $desc->{Enum};
				my $ns = $desc->{Namespace} || $llrp->{CoreNamespace};
				my $enum = $llrp->{Enumerations}->{$ns . '_' . $etype};
				defined $enum || die "Unknown enumeration type $etype";
				my $ident = shift;
				my $value = $enum->{Lookup}->{$ident};
				defined $value || die "Unknown enumeration value $ident ($etype)";
				return ($value);
			}
		);

		foreach my $raw_value (@raw_values) {

			# determine the value to place
			# (parse from XML or use DefaultValue)
			if (exists ($desc->{DefaultValue})) {
				$value = $raw_value;
			} elsif (exists $decode_xml{$desc->{Format}}) {
				$value = $decode_xml{$desc->{Format}}->($raw_value);
			} else {
				my $fmt = $desc->{Format};
				die "Value $raw_value of unimplemented format $fmt in descriptor file";
			}

			# accumulate
			if ($packed_bits || !($bits % 8)) {
				$packed_int  <<= $bits ;
				$packed_int  |=  $value;
				$packed_bits +=  $bits ;
			} else {
				$packed_int = $value;
				$packed_bits = $bits;
			}

			# place value if accumulated to 8-bit boundary
			my $pack_fmt = $pack_fmts{$packed_bits . '_' . ($desc->{Signed} ? 'S' : 'U')};
			if (defined $pack_fmt) {
				# note that there may be loss of precision
				# on 64-bit values.
				if ($packed_bits == 64) {
					my $upper_32 = int ($value / (2**32));
					my $lower_32 = int ($value % (2**32));
					$byte_str = pack ($pack_fmt, $upper_32, $lower_32);
				} else {
					$byte_str .= pack (
						(ref ($pack_fmt) ? $pack_fmt->[0] : $pack_fmt),
						$packed_int);
				}
				$packed_bits = $packed_int = 0;
			}
		}

		return $byte_str, scalar (@raw_values);
	}
}

sub gen_array_iter {
	my ($ary_ref, $ndx) = @_; 
	$ndx = 0 unless defined $ndx;

	return sub {
		return if ($ndx >= scalar (@$ary_ref));
		return $ary_ref->[$ndx++];
	}
}

sub gen_elem_iter {
	my $doc = shift;
	return undef unless defined $doc;
	my $xml_node = first_element ($doc);

	return sub {
		my $element = $xml_node;
		$xml_node = next_element ($xml_node);
		
		# break out the attributes
		my %attrs;
		if (defined ($xml_node)) {
			%attrs = map {$_->getName => $_->getValue}
				grep {ref ($_) ne 'XML::LibXML::Namespace'} $xml_node->attributes();
		}
		if (wantarray) {
			return $element;
		} else {
			return $element, \%attrs;
		}
	}
}
# match_xml()
#
# This pivotal routine performs the match logic for both encoding LLRP messages.
# It expects
# 	a) the current LLRP parameter matching rule descriptor
#	b) an element name
#	c) a namespace identifier string
#	
# It returns
# 	a) best-match parameter type descriptor
# 	b) exact-match flag
# 	c) the rule that matched if alternative match
#
# Note that for now, XML namespace prefixes are assumed to be identical
# to the Vendor name for each extension. This will probably change.

sub fq {
	my $group = shift;
	my $entry = shift;

	my @result = map {sprintf ("%s.%s.%s", $group, $_->{Namespace}, $_->{$entry})} @_;
	return wantarray ? @result : $result[0];
}

sub match_xml {

	my ($match_rule, $act_name) = @_;
	my $act_subdesc;
	my $exp_subdesc;
	my $allow_desc;

	# compute names
	my $exp_name		= fq (qw/P Name/, $match_rule);
	my $exp_type_name	= fq (qw/P Type/, $match_rule);

	# lookup predicted subdescriptor
	$exp_subdesc = $registry->{$exp_type_name};
	ref $exp_subdesc
		or die "Schema problem: couldn't locate predicted parameter type";

	# check for a direct match, and return if does match.
	if ($exp_name eq $act_name) {
		$exp_subdesc->{Concrete}
			|| die "Invalid XML: alternation type used as element";
		return ($exp_subdesc, 1, undef);
	}

	# return now if no alternatives
	my $alt = $match_rule->{Alternatives};
	defined $alt or return ($registry->{$act_name}, 0, undef);

	# lookup alternative
	my $alt_match_rule = $alt->{$act_name};
	if (!defined $alt_match_rule) {
		return ($registry->{$act_name}, 0, undef);
	}

	# lookup the subdescriptor
	$act_subdesc = $registry->{fq (qw/P Type/, $alt_match_rule)};
	return ($act_subdesc, 1, $alt_match_rule);
}

# formats all fields and parameters for the given LLRP message or parameter
sub format_node {

	my ($param_ary, $xml_node, %aux) = @_;
	my $bin;
	my $param_next	= gen_array_iter ($param_ary);
	my $node_name = $xml_node->localname;
	my $elem_next	= gen_elem_iter	 ($xml_node);

	# process all nodes
	my $rule = $param_next->();
	my %hattrs = map {$_->getName => $_->getValue}
		(grep {ref ($_) ne 'XML::LibXML::Namespace'} $xml_node->attributes());
	my $attrs = \%hattrs;

	($xml_node) = $elem_next->();
	my $xml_name;
	my $prefix;
	my $format_leaf = gen_format_leaf;

	# process leaves (they are all at the beginning)
	while ($rule && $rule->{Leaf} && ((defined $xml_node) || defined $rule->{DefaultValue})) {

		if (defined $xml_node) {

			$xml_name = $xml_node->localname;

			# allow inserting raw bytes into the packet
			if ($aux{Force} && $xml_name eq 'RawData') {
				my $value = pack ('H*', $xml_node->textContent);
				$bin .= $value;
				($xml_node, $attrs) = $elem_next->();
				next;
			}
		
			# demand matching XML node for all unfixed-value leaves
			if (   !$aux{Force}
			    && !defined ($rule->{DefaultValue})
			    && ($xml_name ne $rule->{Name})
			    && ($xml_name ne $rule->{Type})
			) {
				my $err_msg = "Field descriptor name " . $rule->{Name} .
					" does not match XML element name " . $xml_node->getName;
				die $err_msg;
			}

			# correct any "version independent" namespace URIs.
			my $ver_indep_ns = $xml_node->namespaceURI;
			if (defined $ver_indep_ns && exists $llrp->{Vendors}->{$ver_indep_ns}) {
				$xml_node->setNamespaceDeclURI (
					$xml_node->prefix,
					$llrp->{Prefix2NS}->{$xml_node->prefix}
				);
			}

		}

		# handle default value overrides
		if (   defined $rule->{DefaultValue}
		    && defined $attrs->{$rule->{Name}}) {
			my $tdesc = clone ($rule);
			$tdesc->{DefaultValue} = $attrs->{$tdesc->{Name}} + 0;
			$rule = $tdesc;
		}

		# callback if matching node
		if (defined ($xml_node) && exists $aux{EncodeCallback}) {
			while (my ($re, $cb) = each %{$aux{EncodeCallback}}) {
				if ($xml_node->localname =~ $re) {
					$cb->($xml_node);
				}
			}
		}

		my ($leaf_data, $cells) = $format_leaf->($rule, $xml_node);
		if ($rule->{Array} && $rule->{Counted}) {
			# fixup length
			substr ($bin, -2, 2, pack ("n", $cells));
		}
		$bin .= $leaf_data;
		($xml_node, $attrs) = $elem_next->() unless defined $rule->{DefaultValue};
		$rule = $param_next->();
	}

	# recursively format the parameters
	my $rep_count = 0;
	my %custom_seen;
	while (($aux{Force} || ref $rule) && (ref ($rule) || ref ($xml_node))) {

		# next descriptor if no xml node and optional
		if ((!ref $xml_node) && ($rule->{Optional} || $aux{Force})) {
			$rule = $param_next->();
			next;
		}

		# handle missing XML node
		if (!ref $xml_node && $rule && !$rule->{Optional}) {
			my $ns = $rule->{Namespace};
			my $name = $rule->{Name};
			die "Missing required parameter $ns:$name";
		}

		# correct a "version independent" namespace URI
		my $ver_indep_ns = $xml_node->namespaceURI;
		if (defined ($ver_indep_ns) && exists $llrp->{Vendors}->{$ver_indep_ns}) {
			$xml_node->setNamespaceDeclURI (
				$xml_node->prefix,
				$llrp->{Prefix2NS}->{$xml_node->prefix}
			);
		}

		# determine namespace and name
		my $fqn = join ('.', 'P',
			$xml_node->namespaceURI || $llrp->{CoreNamespace},
			$xml_node->localname) ;

		# match XML against the current rule
		my ($subdesc, $matched, $allow_rule) = match_xml ($rule, $fqn);

		# process
		if ($subdesc && ($matched || $aux{Force})) {
			$bin .= format_node ($subdesc->{Parameter}, $xml_node, %aux);

			# callback if matching node
			if (exists $aux{EncodeCallback}) {
				while (my ($re, $cb) = each %{$aux{EncodeCallback}}) {
					if ($xml_node->localname =~ $re) {
						$cb->($xml_node);
					}
				}
			}

			# consume XML node
			$rep_count++;
			$custom_seen{$fqn}++ if ($allow_rule && $allow_rule->{Extends});
			($xml_node, $attrs) = $elem_next->();

			# detect excess repetitions of parameter at extpt
			if (exists $custom_seen{$fqn} && $custom_seen{$fqn} > 1 && !$allow_rule->{Array} && !$aux{Force}) {
				die "Exceeded allowed repetitions at extension point";
			}

			# detect excess repetitions of parameter according to rule
			if ($rep_count > 1 && !$rule->{Array} && !$aux{Force}) {
				die "Exceeded allowed repetitions";
			}

			# next descriptor if it's not an array or there are no
			# more xml nodes to format at this level
			if (!$rule->{Array} || !defined ($xml_node)) {
				$rep_count = 0;
				%custom_seen = ();
				$rule = $param_next->();
			}
		} else {	# No match
			if ($rule->{Optional} || $rep_count) {
				$rep_count = 0;
				$rule = $param_next->();
				next;
			}
			my $type = $rule->{Type};
			my $name = $rule->{Name};
			die "Missing required parameter $name:$type (found $fqn instead)";
		}
	}

	# die if there are more XML nodes or parameter descriptors to process
	if ($xml_node && !$aux{Force})	{
		die "Invalid content at the end of infoset starting with: name=" . $xml_node->getName();
	}
	if (defined $rule && !$aux{Force})	{
		my $msg = "Invalid LLRP XML... missing " . $rule->{Name};
		die $msg;
	}

	# backpatch the length field based on TVEncoding bit with the correct
	# length or modified (possibly incorrect) length
	my $tvencoding = unpack ("C", $bin) & 128;
	if (!$tvencoding) {

		# default length is the length of the string just formatted
		my $len = length ($bin);

		# handle override, possibly relative to actual, of length
		if (defined $attrs->{Length}) {

			$len = $attrs->{Length};

			# handle relative change
			if ($len =~ /^[+-]/) {
				$len = length ($bin) + $len;
			}
		}
		substr ($bin, 2, 2, pack ("n", $len + 0));
	}

	return $bin;
}

=item C<encode_message ($document, %options)>

This function will take an XML document and encode it as a LLRP Binary
formatted message.

C<$document> can be a string, a file name, or a XML::LibXML document.

To supply a string, call C<encode_message ($str)>.

To supply a file, call C<encode_message ($fname, File =E<gt> 1)>.

To supply an XML::LibXML document, call C<encode_message ($xmldoc, Tree =E<gt> 1)>.

This routine returns the LLRP Binary formatted message as a string.  In most
cases, this string will be written to a socket associated with a reader that
understands LLRP.

=cut

sub encode_message {
	my ($document, %options) = @_;
	my $parser = XML::LibXML->new();
	$parser->keep_blanks(0);
	my $tree;

	# parse the file
	if ($options{File}) {
		$tree = $parser->parse_file($document);
		delete $options{File};
	} elsif ($options{Tree}) {
		$tree = $document;
		delete $options{Tree};
	} elsif (ref($document) eq 'XML::LibXML::Document') {
		$tree = $document;
	} else {
		$tree = $parser->parse_string($document);
	}

	# get the tree root	
	my $root = $tree->getDocumentElement;
	if ((!defined $root->namespaceURI) || $root->namespaceURI eq 'Core') {
		$root->setNamespace ($llrp->{CoreNamespace}, '', 1);
		$root->setNamespaceDeclURI ('', $llrp->{CoreNamespace});
	}

	# fixup any version-generic namespaces
	my @namespaces = $root->getNamespaces;
	foreach my $attrib ($root->attributes) {
		next unless (ref ($attrib) eq 'XML::LibXML::Namespace');
		my $uri = $attrib->getData;
		my $prefix = $attrib->getLocalName;
		if (exists $llrp->{Vendors}->{$uri}) {
			$root->setNamespaceDeclURI(
				$prefix,
				$llrp->{Prefix2NS}->{$prefix}
			);
		}
	}

	# get the message descriptor
	my $msg_desc;
	my $msg_name = join ('.', 'M', $root->namespaceURI, $root->localname);
	$msg_desc = $registry->{$msg_name};
	defined $msg_desc || die "No format descriptor for $msg_name";

	# compile the XML file to binary format
	my $bin = format_node ($msg_desc->{Parameter}, $root, %options);
	
	# backpatch the message length
	my $len_override = $root->findvalue('@MessageLength');
	substr ($bin, 2, 4, pack ("N",
		length($len_override)?($len_override+0):length ($bin))
#		defined($len_override)?$len_override:length ($bin))
	);
	
	if (wantarray) {
		return ($bin, $tree);
	} else {
		return $bin;
	}
}

memoize ('encode_message', INSTALL => 'memoized_encode_message');

my @bit_tran = (
	undef, 
	[ 8, "C" ], [ 8, "C" ], [ 8, "C" ], [ 8, "C" ], [ 8, "C" ], [ 8, "C" ], [ 8, "C" ], [ 8, "C" ], 
	[16, "n" ], [16, "n" ], [16, "n" ], [16, "n" ], [16, "n" ], [16, "n" ], [16, "n" ], [16, "n" ], 
	[32, "N" ], [32, "N" ], [32, "N" ], [32, "N" ], [32, "N" ], [32, "N" ], [32, "N" ], [32, "N" ], 
	[32, "N" ], [32, "N" ], [32, "N" ], [32, "N" ], [32, "N" ], [32, "N" ], [32, "N" ], [32, "N" ], 
);

sub format_text {
	my ($desc, $values) = @_;
	my ($format) = lc($desc->{Format});
	my $bits = $desc->{Bits};
	if ($bits == 96) {$bits = 8};

	if ($format eq 'hex') {
		my $fmt;
		if ($bits == 1) {
			my $packed = pack ('B*', join ('', @$values));
			return uc(unpack ('H' . length ($packed) * 2, $packed));
		
		} elsif ($bits >=8 && is_power_of_two ($bits)) {
			$fmt = "%0" . ($bits / 4) . "X";
		} else {
			die "Unimplemented formatter for $bits bitlength hex";
		}
		my $sep = ($bits == 8) ? '' : ' ';
		return uc(join ($sep, (map (sprintf ($fmt, $_), @$values))));
	} elsif ($format eq 'decimal') {
		if ($bits != 64) {
			return join (' ', (map (sprintf (($desc->{Signed} ? "%d" : "%u"), $_), @$values)));
		} else {
			return sprintf ("%.0f", $values->[0] * (2.0**32) + $values->[1]);
		}
	} elsif ($format eq 'utf8') {
		return pack ("C*", @$values);
	} elsif ($format eq 'boolean') {
		return join (' ', (map (($_ ? 'true' : 'false'), @{$values})));
	} elsif ($format eq 'enum') {
		my $ns = $desc->{Namespace} || $llrp->{CoreNamespace};
		my $lkp = $llrp->{Enumerations}->{$ns . '_' . $desc->{Enum}}->{Definition};
		return join (' ', (map ((defined ($lkp->[$_]) ? $lkp->[$_] : $_), @$values)));
	} elsif ($format eq 'datetime') {
		my $usecs = (($values->[0] * 1.0) * (2.0**32) + ($values->[1] * 1.0));
		my $dtime = $usecs / 1000000.0;
		my $decimal = sprintf ("%020.0f", $usecs);
		return time2str ("%Y-%m-%dT%T", $dtime, 'utc') .
			'.' . substr ($decimal, -6, 6);
	} else {
		die "Unknown format descriptor $format";
	}
	
}

sub is_power_of_two {
	return !($_[0] & $_[0] - 1);
}

sub gen_unpack_fields {

	my ($buf) = @_;
	my $ndx = 0;
	my $remainder = '';

	return sub {

		my ($desc, $cells) = @_;
		my $remaining = $cells;
		my $bits = $desc->{Bits};
		my @values;

		while ($remaining) {

			if (!length ($remainder) && $bits >= 8 && ($bits == 96 || is_power_of_two ($bits))) {
				my $fmt = $pack_fmts{$bits . '_' . ($desc->{Signed} ? 'S' : 'U')};
				if (ref $fmt) {
					my @native =	unpack	($fmt->[0] . $remaining, substr ($buf, $ndx));
					my $packed =	pack	($fmt->[1] . '*', @native);
					push @values,	unpack	($fmt->[2] . $remaining, $packed);
				} else {
					push @values, unpack ($fmt . $remaining, substr ($buf, $ndx));
				}
				$ndx += ($bits / 8) * $cells;
				$remaining = 0;
			} else {

				# unpack 8 more bits if don't have enough for 1 cell
				while (length ($remainder) < $bits) {

					# ensure that we have not run past the end of the available data
					$ndx < length ($buf)
						or die "Packed bitfield was truncated; " .
							"ndx: $ndx >= length (buf): " . length ($buf);
					$remainder .= unpack ("B8", substr ($buf, $ndx));
					$ndx++;
				}

				my ($pad_to, $fmt) = @{$bit_tran[$bits]};
				$remainder = '0' x ($pad_to - $bits) . $remainder;
				push @values, unpack ($fmt, pack ("B$pad_to", substr ($remainder, 0, $pad_to, '')));

				$remaining--;
			}
		}

		return (\@values, $ndx);
	};
}

sub parse_msg_head {

	my $ndx;

	my ($header, @raw) = unpack ('nNNNC', $_[0]);

	my %msg_hdr;
	@msg_hdr{qw/Reserved Version Type/} = 
		(($header >> 13), (($header >> 10) & 7), ($header & 1023));
	@msg_hdr{qw/MessageLength MessageID/} = splice (@raw, 0, 2);
	if ($msg_hdr{Type} == 1023) {
		@msg_hdr{qw/VendorIdentifier MessageSubtype/} = @raw;
	}

	return (\%msg_hdr, 10);
}

sub parse_param_head {

	my ($buf) = @_;
	my %param_hdr;
	my $first = ord ($buf);
	my ($ndx, $header_fields);
	if (length ($buf) < 1) {
		die "Insufficient bytes in subparam to determine encoding type";
	}
	if ($first & 0x80) {
		$param_hdr{TVEncoding} = 1;
		$param_hdr{Type} = $first & 0x7F;
		$ndx = 1;
		$header_fields = 2;
	} else {
		if (length ($buf) < 4) {
			die "Insufficient bytes in subparam to determine type and length";
		}
		$param_hdr{TVEncoding} = 0;
		($param_hdr{Type}, $param_hdr{Length}) = unpack ("nn", $buf);
		$param_hdr{Type} &= 0x3ff;
		$ndx = 4;
		$header_fields = 4;

		# peek ahead to the VendorIdentifier and ParameterSubtype
		if ($param_hdr{Type} == 1023) {
			length ($buf) >= 12 
				|| die "short packet... missing PEN and/or Subtype";
			@param_hdr{qw/VendorIdentifier ParameterSubtype/} =
				unpack ('NN', substr ($buf, 4, 8));
		}
	}

	return \%param_hdr, $ndx, $header_fields;
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

sub match_binary {

	my ($match_rule, $act_type_name) = @_;
	my $act_subdesc;
	my $exp_subdesc;
	my $allow_desc;

	# compute names
	my $exp_type_name = fq (qw/P Type/, $match_rule);

	# lookup predicted subdescriptor
	$exp_subdesc = $registry->{$exp_type_name};
	ref $exp_subdesc
		or die "Schema problem: couldn't locate predicted parameter type";

	# construct "expected" type name in ID form
	if (exists $exp_subdesc->{TypeID}) {
		$exp_type_name = $exp_subdesc->{Concrete} ? fq ('P', 'TypeID', $exp_subdesc) : '';
	} else {
		$exp_type_name = join ('.', 
			'P',
			$llrp->{Vendors}->{$exp_subdesc->{VendorName}},
			$exp_subdesc->{ParameterSubtype}
		);
	}

	# check for a direct match, and return if does match.
	if ($exp_type_name eq $act_type_name) {
		$exp_subdesc->{Concrete}
			|| die "Invalid XML: alternation type used as element";
		return ($exp_subdesc, 1, undef);
	}

	# return now if no alternatives
	my $alt = $match_rule->{Alternatives};
	defined $alt or return ($registry->{$act_type_name}, 0, undef);

	# lookup alternative
	my $alt_match_rule = $alt->{$act_type_name};
	if (!defined $alt_match_rule) {
		return ($registry->{$act_type_name}, 0, undef);
	}

	# lookup the subdescriptor
	$act_subdesc = $registry->{fq (qw/P Type/, $alt_match_rule)};
	defined $act_subdesc || die "Schema issue: unregistered type in match rule.";
	return ($act_subdesc, 1, $alt_match_rule);
}

sub decode_body {

	my ($tree, $root, $parent_ns, $parent_prefix, $next_rule, $buf, %aux) = @_;
	my $rule;
	my ($unpack_field) = gen_unpack_fields ($buf);
	my $cells = 1;
	my $ndx = 0;
	my $values;
	my $matches;
	my $remainder = length ($buf);

	my $hash_gp_key	= $aux{HashGPKey};
	my $hash_gp 	= $aux{HashGP};
	my $hash_parent	= $aux{HashParent};

	# note: 'bytesToEnd' algorithm only works if there are no params

	# decode the fields
	$rule = $next_rule->();

	while (ref $rule && $rule->{Leaf}) {

		# compute 'cells' if bytesToEnd
		if (ref $rule && $rule->{Array} && !$rule->{Counted}) {
			$cells = $remainder;
		}

		# unpack
		eval {
			my $old_ndx = $ndx;
			($values, $ndx) = $unpack_field->($rule, $cells);

			# adjust remainder for bytesToEnd contingency
			$remainder -= ($ndx - $old_ndx);
		};
		if ($@) {
			if ($aux{Force}) { last }
			else { die $@ }
		}

		# format the field data as XML
		if (!$rule->{BinaryOnly}) {

			my $node;
			if ($aux{QualifyCore} || $parent_ns ne $llrp->{CoreNamespace}) {
				$node = $tree->createElement ($parent_prefix . ':' . $rule->{Name});
			} else {
				$node = $tree->createElement ($rule->{Name});
			}
			my $text = format_text ($rule, $values);

			# store data in perl data structure
			if (ref $hash_parent) {
				my $leaf;

				if ($rule->{Array}) {
					if ($rule->{Format} eq 'Utf8') {
						$leaf = pack ('C*', @{$values});
					} else {
						my @da = @{$values};
						$leaf = \@da;
					}

				} else {

					if ($rule->{Type} eq 'EPC96') {
						$leaf = pack ('C*', @{$values});
					} elsif ($rule->{Bits} == 64) {
						$leaf = sprintf ("%.0f", $values->[0] * (2.0**32) + $values->[1]);
					} else {
						$leaf = $values->[0];
					}
				}
				if ((ref $hash_gp) && $aux{Hoistable}) {
					$hash_gp->{$hash_gp_key} = $leaf;
				} else {
					$hash_parent->{$rule->{Name}} = $leaf; 
				}
			}

			my $text_node = XML::LibXML::Text->new ($text);
			if (($cells % 8) && $rule->{Format} eq 'Hex' && $rule->{Bits} == 1) {
				$node->setAttribute ('Count', $cells);
			}
			$node->addChild ($text_node);
			$root->addChild ($node);
		}

		# set #cells for the _next_ field
		if ($rule->{BinaryOnly} && $rule->{Name} eq 'Length') {
			$cells = $values->[0];
		} else {
			$cells = 1;
		}
		
		$rule = $next_rule->();
	}

	# decode the parameters
	my $old_ndx = $ndx + 1;
	my %custom_seen;
	my ($param_hdr, $ofs, $header_fields, $fqt);
	my ($best_match, $matched, $rule_used);
	while ($ndx < length ($buf) && (ref ($rule) || $aux{Force})) {

		# parse the next parameter header if the index has advanced
		if ($old_ndx != $ndx) {
			$fqt = undef;

			($param_hdr, $ofs, $header_fields) = parse_param_head (substr ($buf, $ndx));

			# calculate fully qualified type name; prefer structured form
			if ($param_hdr->{Type} == 1023) {
				my ($vendor_id, $type) = (@{$param_hdr}{
					'VendorIdentifier', 'ParameterSubtype'});
				if (ref ($registry->{"P.$vendor_id.$type"})) {
					
					# ensure header fields are skipped
					$fqt = "P.$vendor_id.$type";
					$ofs += 8; 
					$header_fields += 2;
				}
			} 
			$fqt = 'P.' . $llrp->{CoreNamespace} . '.' .
				$param_hdr->{Type} unless defined $fqt;

			$old_ndx = $ndx;

		}

		# match
		($best_match, $matched, $rule_used) = match_binary ($rule, $fqt);

		if (($matched || $aux{Force}) && ref $best_match) { # match, or forcing...

			# consume the parameter header
			$ndx += $ofs; # matched, so move past the actual parameter header

			my $ns = $best_match->{Namespace};
			my $prefix = ($ns eq $llrp->{CoreNamespace}) ? 'llrp':
				$llrp->{NS2Prefix}->{$ns};

			# add the extension namespace
			if ($prefix) {
				$tree->getDocumentElement->setNamespace ($ns, $prefix || '', 0);
			}

			# create, add the node
			my $lname = ref ($rule_used) ?  $rule_used->{Name} : $rule->{Name};
			my $node = $tree->createElement ($lname);
			$root->addChild ($node);
			if (($ns ne $llrp->{CoreNamespace}) || $aux{QualifyCore}) {
				$node->setNamespace ($ns, $prefix || '', 1);
			}

			my $elem_name = $node->localname;

			my $next_param = gen_array_iter (
				$best_match->{Parameter}, $header_fields);

			my $sublen = defined ($param_hdr->{Length})
				? $param_hdr->{Length}
				: $best_match->{FixedLength};

			# insert node into hash
			if (ref $hash_parent && !(($rule_used || $rule)->{Array})) {

				$hash_parent->{$elem_name} = $aux{HashParent} = {};
				$aux{Hoistable} = 1 if ($best_match->{Hoistable});
				$aux{HashGP} = $hash_parent;
				$aux{HashGPKey} = $elem_name;

			} elsif (ref $hash_parent) { # handle array of subnodes

				my ($ary, $subhash);
				$hash_parent->{$elem_name} = $ary = [] unless ref ($ary = $hash_parent->{$elem_name});
				$aux{HashParent} = $subhash = {};
				push @{$ary}, $subhash;
				$aux{Hoistable} = 0;
				$aux{HashGP} = undef;
			}

			# recursively decode nested portion
			$ndx += decode_body ($tree, $node, $ns, $prefix, $next_param,
				substr ($buf, min ($ndx, length ($buf)), max ($sublen - $ofs, 0)),
				%aux);

			# restore to current level of hash
			if (ref $hash_parent) {
				delete $aux{Hoistable};
				$aux{HashParent} = $hash_parent;
				$aux{HashGP} = $hash_gp;
				$aux{HashGPKey} = $hash_gp_key;
			}

			# increment repetition counters
			$matches++ if ($matched);
			$custom_seen{$fqt}++ if ($matched && ref $rule_used && $rule->{Extends});

			# enforce global caps on params at ep's
			if (	ref ($rule_used)
				&& !$rule_used->{Array}
				&& !$aux{Force}
				&& exists $custom_seen{$fqt}
				&& $custom_seen{$fqt} > 1) {
				die "Exceeded cap on $fqt occurance at extension point";
			}

			# match against the expected descriptor again if 'array'
			next if ($rule->{Array});

			# next expected match rule if current match rule got used up
			if ($matched && !($rule->{Array})) {
				$rule = $next_rule->();
				$matches = 0;
				%custom_seen = ();
			}
		} else { # no match

			if (!ref ($best_match)) {

				die "Unknown parameter $fqt" unless $aux{Force};

				# put hexdump for the unrecognized parameter
				my $parser = XML::LibXML->new();
				my $fragtxt = 
					"<UnknownParameter " .
					"type_id='" .  $param_hdr->{Type} . "'";
				if ($param_hdr->{Type} == 1023) { $fragtxt .= ' ' .
					"vendor='" . $param_hdr->{VendorIdentifier} . "' " .
					"subtype='" . $param_hdr->{ParameterSubtype} . "'";
				}
				$fragtxt .= ">" .
					join (' ', (map { sprintf ("%02X", $_) } unpack ('C*', $buf))) .
					"</UnknownParameter>";
				my $node = $parser->parse_balanced_chunk ($fragtxt);
				$root->addChild ($node);

				$ndx += $ofs;
				next;
	
			}

			# raise an exception if expected is required but
			# missing
			if (!$matches && !$rule->{Optional} && !$aux{Force}) {
				die "Missing non-optional parameter " . fq (qw/P Name/, $rule);
			}

			# (now match current actual against next expected... )
			($rule, $matches) = ($next_rule->(), 0);
			%custom_seen = ();
		}
	}

	if ($ndx < length ($buf)) {
		if (ref $best_match) {
			die $best_match->{Namespace}. '.' . $best_match->{Name} . " never matched a rule";
		} else {
			die "Some of the binary message never matched a rule";
		}
	}

	return $ndx;
}

sub raw_hex_bytes {

	my $buf = shift;	
	my $level = shift or 0;

	my @buf = unpack ('C*', $buf);
	my $doctxt = ' ' x $level;
	while (@buf) {
		for (my $i = 0; @buf && $i < 16; $i++) {
			
			# space line, newlines at end
			$doctxt .= sprintf ("%02X%s", shift (@buf),
				(($i == 15 || !(@buf)) ? "\n" : ' '));

			# indent
			$doctxt .= ' ' x $level if $i == 15 && @buf;
		}
	}

	return $doctxt;

}

=item C<decode_message ($str)>

This function accepts an LLRP Binary formatted message decodes it, and returns
an analogous LLRP/XML formatted message.

Once in XML format, the LLRP message can be subjected to further analysis or
modification using the full power of XML::LibXML and XPath, validated against
the LLRP W3C XML Schema (LLRP.xsd), or serialized to a text file for version
control or archival purposes.

C<QualifyCore>

This routine accepts the C<QualifyCore> parameter. Its use and purpose is described
in the POD for C<RFID::LLRP::Link> C<transact>.

=cut

sub decode_message {

	my ($buf, %aux) = @_;

	if (!exists $aux{QualifyCore}) {
		$aux{QualifyCore} = 1;
	}

	# create the XML document
	my $tree = XML::LibXML::Document->new();

	# lookup the message
	my ($msg_hdr, $ndx);
	($msg_hdr, $ndx) = parse_msg_head ($buf);

	# fail if length is shorter than minimal LLRP message header
	$msg_hdr->{MessageLength} >= 6
		|| die "Message Length (" .
			$msg_hdr->{MessageLength} .
			") is shorter than the minimal LLRP message header";

	my $ns = $llrp->{CoreNamespace};
	my $msg_type = $msg_hdr->{Type};

	# lookup best matching message type in registry
	my $msg_desc;
	my $msg_name;
	my $hidden_fields = 0;
	my $fqt;
	$msg_desc = $registry->{"M.$ns.$msg_type"};
	if ($msg_type != 1023) {
		ref ($msg_desc) or $aux{Force} or die "Unknown message ID $msg_type";
	} else {{

		my ($ns_id, $msg_subtype) = @{$msg_hdr}{
			'VendorIdentifier',
			'MessageSubtype'
		};
		last unless (exists $vendor_lookup->{$ns_id});
		my $ns_temp = $vendor_lookup->{$ns_id};
		$fqt = "M.$ns_id.$msg_subtype";

		my $msg_desc_temp = $registry->{$fqt};
		if (ref $msg_desc_temp) {
			$msg_desc	= $msg_desc_temp;
			$ns		= $msg_desc_temp->{Namespace};

			# treat the vendor/subtype as header bytes: skip them
			$ndx += 5;
			$hidden_fields = 2;

		}
	}}

	if (!ref ($msg_desc)) {

		my $parser = XML::LibXML->new();
		$tree = $parser->parse_string (
			"<UnknownMessage type_id='" .
			$msg_hdr->{Type} . "'>\n" .
			raw_hex_bytes ($buf, 2) .
			"</UnknownMessage>\n"
		);
		return ($tree);
	}

	$msg_name = $msg_desc->{Name};

	# lookup the prefix
	my $prefix = ($ns eq $llrp->{CoreNamespace}) ? 'llrp' : $llrp->{NS2Prefix}->{$ns};

	# add the message header
	# create the node
	my $fqn = "$prefix:$msg_name";
	my $elem;
	if ($aux{QualifyCore} || ($ns ne $llrp->{CoreNamespace})) {
		$elem = $tree->createElementNS ($ns, $fqn);
	} else {
		$elem = $tree->createElementNS ('', $msg_name);
	}
	$tree->setDocumentElement ($elem);
	my $root = $tree->getDocumentElement;

	my $hash_parent = $aux{HashParent};
	if (ref $hash_parent) {
		$hash_parent->{$msg_name} = $aux{HashParent} = {};
	}

	# decode the body of the message
	decode_body ($tree, $root, $ns, $prefix, gen_array_iter ($msg_desc->{Parameter}, MSG_HEADER_FIELDS + $hidden_fields),
		substr ($buf, $ndx, $msg_hdr->{MessageLength} - $ndx), %aux);

	# set the version and message ID attributes
	$root->setAttribute ('Version'	,	$msg_hdr->{'Version'}	);
	$root->setAttribute ('MessageID',	$msg_hdr->{'MessageID'}	);

	# workaround for prefix / xpath issue
	my $parser = XML::LibXML->new();
	$tree = $parser->parse_string($tree->toString(1));

	return ($tree);
}



1;

=pod

=back

=head1 AUTHOR

John R. Hogerhuis

Chris Delaney

=head1 BUGS

None

=head1 SEE ALSO

EPCGlobal LLRP Specification

=head1 COPYRIGHT
Copyright 2008, 2008 Impinj, Inc.

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
