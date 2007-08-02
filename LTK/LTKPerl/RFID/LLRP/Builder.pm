#############################################################################
#                                                                           #
#  Copyright 2007 Impinj, Inc.                                              #
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

require Exporter;
our @ISA	= qw(Exporter);
our @EXPORT_OK	= qw(encode_message decode_message memoized_encode_message);

use RFID::LLRP::Schema;
use XML::LibXML;

use Date::Parse;
use Date::Format;

use Data::Dumper;
use Data::HexDump;

use Clone qw(clone);

use Memoize;
memoize ('is_power_of_two');

$| = 1;

# read schema
our ($llrp, $msg, $lookup_param, $lookup_mid, $lookup_pid) =
	RFID::LLRP::Schema::read_schema ("./llrpdef.xml");

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
	8	=> "C",
	16	=> "n",
	32	=> "N",
	64	=> "NN",
	96	=> "CCCCCCCCCCCC"
);

use constant MSG_HEADER_LEN => 5;

sub is_numeric {
	use warnings FATAL => 'numeric';
	return defined eval { $_[0] == 0 };
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
			if ($desc->{Format} eq 'Hex' && ($type =~ /Byte|Bit/) && $desc->{Array}) {
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
			@raw_values = split (/\s+/, $xml_node->textContent);
			if (scalar (@raw_values) && !$desc->{Array}) {
				@raw_values = ($raw_values[0]);
			}
		}

		# lay values from this node
		my $byte_str = "";
		my %decode_xml = (
			'Hex'		=> sub { return hex (shift); },
			'Datetime'	=> sub { return str2time ((shift), 'utc') * 1000000.0 },
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
				my $enum = $llrp->{Enumerations}->{$etype};
				defined $enum || die "Unknown enumeration type $etype";
				my $ident = shift;
				my $value = $enum->{Lookup}->{$ident};
				defined $value || die "Unknown enumeration value $etype:$ident";
				return ($value);
			}
		);

		foreach $raw_value (@raw_values) {

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
			my $pack_fmt = $pack_fmts{$packed_bits};
			if (defined $pack_fmt) {
				# note that there may be loss of precision
				# on 64-bit values.
				if ($packed_bits == 64) {
					my $upper_32 = int ($value / (2**32));
					my $lower_32 = int ($value % (2**32));
					$byte_str = pack ($pack_fmt, $upper_32, $lower_32);
				} else {
					$byte_str .= pack ($pack_fmt, $packed_int);
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

# formats all fields and parameters for the given LLRP message or parameter
sub format_node {

	my ($param_ary, $xml_node, %aux) = @_;
	my $bin;
	my $param_next	= gen_array_iter ($param_ary);
	my $node_name = $xml_node->getName;
	my $elem_next	= gen_elem_iter	 ($xml_node);

	# process all nodes
	my $desc = $param_next->();
	%hattrs = map {$_->getName => $_->getValue}
		(grep {ref ($_) ne 'XML::LibXML::Namespace'} $xml_node->attributes());
	my $attrs = \%hattrs;

	($xml_node) = $elem_next->();
	my $xml_name;
	my $format_leaf = gen_format_leaf;

	# process leaves (they are all at the beginning)
	while ($desc && $desc->{Leaf} && ((defined $xml_node) || defined $desc->{DefaultValue})) {

		if (defined $xml_node) {
			$xml_name = $xml_node->getName;

#turn this on to trace walk through fields
#print "xml: $xml_name  schema: ", $desc->{Name}, "::", $desc->{Type}, "\n";

			# allow inserting raw bytes into the packet
			if ($aux{Force} && $xml_name eq 'RawData') {
				my $value = pack ('H*', $xml_node->textContent);
				$bin .= $value;
				($xml_node, $attrs) = $elem_next->();
				next;
			}
		
			# demand matching XML node for all unfixed-value leaves
			if (   !$aux{Force}
			    && !defined ($desc->{DefaultValue})
			    && ($xml_name ne $desc->{Name})
			    && ($xml_name ne $desc->{Type})
			) {
				my $err_msg = "Field descriptor name " . $desc->{Name} .
					" does not match XML element name " . $xml_node->getName;
				die $err_msg;
			}

		}

		# handle default value overrides
		if (   defined $desc->{DefaultValue}
		    && defined $attrs->{$desc->{Name}}) {
			my $tdesc = clone ($desc);
			$tdesc->{DefaultValue} = $attrs->{$tdesc->{Name}} + 0;
			$desc = $tdesc;
		}

		my ($leaf_data, $cells) = $format_leaf->($desc, $xml_node);
		if ($desc->{Array} && $desc->{Counted}) {
			# fixup length
			substr ($bin, -2, 2, pack ("n", $cells));
		}
		$bin .= $leaf_data;
		($xml_node, $attrs) = $elem_next->() unless defined $desc->{DefaultValue};
		$desc = $param_next->();
	}

	# recursively format the parameters
	my $rep_count = 0;
	while (($aux{Force} || defined $desc)
		&& ((defined ($desc) && exists $desc->{Optional}) || defined $xml_node)) {

		# next descriptor if no xml node and optional
		if ((!defined $xml_node) && ($desc->{Optional} || $aux{Force})) {
			$desc = $param_next->();
			next;
		}

		# reference the nested parameter descriptor
		$xml_name = $xml_node->getName;
		
		# determine match between schema and xml
		my $matched = 0;
		my $subdesc = $lookup_param->{$desc->{Type}};
		if ($xml_name eq $desc->{Name}) {
			$matched = 1;
		} elsif (!$subdesc->{Concrete}) {
			# nb: relies on Name == Type for all alternation points
			
			if (exists $subdesc->{Choices}->{$xml_name}) {
				$subdesc = $subdesc->{Choices}->{$xml_name};
				$matched = 1;
			}
		}

		# process
		if ($subdesc && ($matched || $aux{Force})) { # Match

			$bin .= format_node ($subdesc->{Parameter}, $xml_node, %aux);

			# consume XML node
			$rep_count++;
			($xml_node, $attrs) = $elem_next->();

			# next descriptor if it's not an array or there are no
			# more xml nodes to format at this level
			if (!$desc->{Array} || !defined ($xml_node)) {
				$rep_count = 0;
				$desc = $param_next->();
			}
		} else {	# No match
			if ($desc->{Optional} || $rep_count) {
				$rep_count = 0;
				$desc = $param_next->();
				next;
			}
			my $type = $desc->{Type};
			my $name = $desc->{Name};
			die "Missing required parameter $name:$type (found $xml_name instead)";
		}
	}

	# die if there are more XML nodes or parameter descriptors to process
	if ($xml_node && !$aux{Force})	{
		die "Unformatted XML nodes in the tail"
	}
	if (defined $desc && !$aux{Force})	{
		my $msg = "Invalid LLRP XML... missing " . $desc->{Name};
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

=item C<C<encode_message ($document, %options)>>

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
	my $tree;
	my %aux;
	if (exists $options{Force}) {
		%aux = (Force => $options{Force});
	}

	# parse the file
	if ($options{File}) {
		$tree = $parser->parse_file($document);
	} elsif ($options{Tree}) {
		$tree = $document;
	} else {
		$tree = $parser->parse_string($document);
	}

	# get the tree root	
	my $root = $tree->getDocumentElement;

	# get the message descriptor
	my $msg_name = $root->getName;
	my $msg_desc = $msg->{$msg_name};
	defined $msg_desc || die "No format descriptor for $msg_name";

	# compile the XML file to binary format
	my $bin = format_node ($msg_desc->{Parameter}, $root, %aux);
	
	# backpatch the message length
	substr ($bin, 2, 4, pack ("N", length ($bin)));
	
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
		return join (' ', (map (sprintf ("%d", $_), @$values)));
	} elsif ($format eq 'utf8') {
		return pack ("C*", @$values);
	} elsif ($format eq 'boolean') {
		return join (' ', (map (($_ ? 'true' : 'false'), @{$values})));
	} elsif ($format eq 'enum') {
		my $lkp = $llrp->{Enumerations}->{$desc->{Enum}}->{Definition};
		return join (' ', (map ((defined ($lkp->[$_]) ? $lkp->[$_] : $_), @$values)));
	} elsif ($format eq 'datetime') {
		my $usecs = (($values->[0] * 1.0) * (2.0**32) + ($values->[1] * 1.0));
		my $dtime = $usecs / 1000000.0;
		my $decimal = sprintf ("%020.0f", $usecs);
		return time2str ("%Y:%m:%dT%T", $dtime, 'utc') .
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
				push @values, unpack ($pack_fmts{$bits} . $remaining, substr ($buf, $ndx));
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

	my ($buf) = @_;


	my $next_desc = gen_array_iter ($llrp->{Messages}->[0]->{Parameter});
	my ($desc, %msg_hdr);
	my $n_fields = MSG_HEADER_LEN;
	my $unpack_field = gen_unpack_fields ($buf);
	while (($desc = $next_desc->()) && $n_fields) {
		my $values;
		($values, $ndx) = $unpack_field->($desc, 1);
		$msg_hdr{$desc->{Name}} = $values->[0];	
		$n_fields--;
	}

	return \%msg_hdr, $ndx;
}

sub parse_param_head {

	my ($buf) = @_;
	my %param_hdr;
	my $first = ord ($buf);
	my ($ndx, $header_fields);
	if ($first & 0x80) {
		$param_hdr{TVEncoding} = 1;
		$param_hdr{Type} = $first & 0x7F;
		$ndx = 1;
		$header_fields = 2;
	} else {
		$param_hdr{TVEncoding} = 0;
		($param_hdr{Type}, $param_hdr{Length}) = unpack ("nn", $buf);
		$param_hdr{Type} &= 0x3ff;
		$ndx = 4;
		$header_fields = 4;
	}

	return \%param_hdr, $ndx, $header_fields;
}

sub parse_param_head_old {

	my ($buf) = @_;
	my %param_hdr;

	my ($tpl_tvplist, $tpl_tlvplist);
	$tpl_tvplist	= $lookup_param->{'AccessSpecID'}->{Parameter};
	$tpl_tlvplist	= $lookup_param->{'Uptime'}->{Parameter};
	my $unpack_field = gen_unpack_fields ($buf);

	my $desc = $tpl_tvplist->[0];
	my $tvencode = $param_hdr{$desc->{Name}} = $unpack_field->($desc, 1);
	my ($n_fields, $plist) = $tvencode ? (3, $tpl_tlvplist): (1, $tpl_tvplist);
	my $header_fields = $n_fields + 1;
	my $next_desc = gen_array_iter ($plist, 1);
	while (($desc = $next_desc->()) && $n_fields) {
		my $values;
		($values, $ndx) = $unpack_field->($desc, 1);
		$param_hdr{$desc->{Name}} = $values->[0];	
		$n_fields--;
	}

	return \%param_hdr, $ndx, $header_fields;
}

sub match_parameter {

	# !! should compare ID's instead of names for efficiency
	# !! requires caching the ID somewhere besides the field
	# !! list
	
	# match expected descriptor against actual
	# $expected and $actual must be parameter descriptors
	# (they must NOT be field descriptors)
	
	my ($expected, $actual) = @_;

	if ($expected->{Concrete}) {
		if ($actual->{TypeID} == $expected->{TypeID}) {return $actual;}
	} else {
		my ($desc) = $expected->{Choices}->{$actual->{Name}};
		if (ref $desc) {return $desc}
	}

	return;

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

sub decode_body {

	my ($tree, $root, $next_desc, $buf, %aux) = @_;
	my $desc;
	my ($unpack_field) = gen_unpack_fields ($buf);
	my $cells = 1;
	my $ndx = 0;
	my $values;
	my $matches;
	my $remainder = length ($buf);

	# note: 'bytesToEnd' algorithm only works if there are no params

	# decode the fields
	$desc = $next_desc->();

	while (ref $desc && $desc->{Leaf}) {

		# compute 'cells' if bytesToEnd
		if (ref $desc && $desc->{Array} && !$desc->{Counted}) {
			$cells = $remainder;
		}

		# unpack
		eval {
			my $old_ndx = $ndx;
			($values, $ndx) = $unpack_field->($desc, $cells);

			# adjust remainder for bytesToEnd contingency
			$remainder -= ($ndx - $old_ndx);
		};
		if ($@) {
			if ($aux{Force}) { last }
			else { die $@ }
		}

		# format the field data as XML
		if (!$desc->{BinaryOnly}) {
			my $node = $tree->createElement ($desc->{Name});
			my $text = format_text ($desc, $values);
			my $text_node = XML::LibXML::Text->new ($text);
			if (($cells % 8) && $desc->{Format} eq 'Hex' && $desc->{Bits} == 1) {
				$node->setAttribute ('Count', $cells);
			}
			$node->addChild ($text_node);
			$root->addChild ($node);
		}

		# set #cells for the _next_ field
		if ($desc->{BinaryOnly} && $desc->{Name} eq 'Length') {
			$cells = $values->[0];
		} else {
			$cells = 1;
		}
		
		$desc = $next_desc->();
	}

	# decode the parameters
	my $old_ndx = $ndx + 1;
	my ($param_hdr, $ofs, $header_fields);
	while ($ndx < length ($buf) && (ref ($desc) || $aux{Force})) {

		# parse the next parameter header if the index has advanced
		if ($old_ndx != $ndx) {
			($param_hdr, $ofs, $header_fields) = parse_param_head (substr ($buf, $ndx));
			$old_ndx = $ndx;
		}

		# next descriptor if doesn't match and optional
		my $name = $desc->{Name};
		my $type = $desc->{Type};

		my $expected = $lookup_param->{$type};

		die "Unknown expected parameter $name:$type"
			unless (ref ($expected) || $aux{Force});
		
		my $actual = $lookup_pid->{$param_hdr->{Type}};
		my $id = $param_hdr->{Type};
		die "Unknown actual parameter with ID $id" unless ref $actual;
		my ($child_desc) = match_parameter ($expected, $actual);
		my $matched = (ref $child_desc) ? 1 : 0;
		if ($matched || $aux{Force}) { # match, or forcing...

			# handle force an expected not defined
			if (!defined $expected) {
				$expected = $actual;
			}

			# consume the parameter header, read the next one
			$ndx += $ofs; # matched, so move past the actual parameter header

			# create the node
			my $node = $tree->createElement (
				(exists ($expected->{Concrete}) && !$expected->{Concrete})
					? $actual->{Name}
					: (defined ($name) ? $name : $actual->{Name}));
			$root->addChild ($node);
			my $next_param = gen_array_iter ($actual->{Parameter}, $header_fields);
			my $sublen = defined ($param_hdr->{Length})
				? $param_hdr->{Length}
				: $actual->{FixedLength};
			$ndx += decode_body ($tree, $node, $next_param,
				substr ($buf, min ($ndx, length ($buf)), max ($sublen - $ofs, 0)),
				%aux);
			$matches++ if ($matched);

			# match against the expected descriptor again if 'array'
			next if ($desc->{Array});

			# next descriptor if current descriptor got used up
			if ($matched) {
				$desc = $next_desc->();
				$matches = 0;
			}
		} else { # no match

			# raise an exception if expected is required but
			# missing
			if (!$matches && !$desc->{Optional}) {
				die "Missing non-optional parameter $name:$type";
			}

			# (now match current actual against next expected... )
			($desc, $matches) = ($next_desc->(), 0);
		}

	}

	return $ndx;
}

=item C<decode_message ($str)>

This function accepts an LLRP Binary formatted message decodes it, and returns
an analogous LLRP/XML formatted message.

Once in XML format, the LLRP message can be subjected to further analysis or
modification using the full power of XML::LibXML and XPath, validated against
the LLRP W3C XML Schema (LLRP.xsd), or serialized to a text file for version
control or archival purposes.

=cut

sub decode_message {

	my ($buf, %aux) = @_;

	# create the XML document
	my $tree = XML::LibXML::Document->new();

	# lookup the message
	my ($msg_hdr, $ndx);
	($msg_hdr, $ndx) = parse_msg_head ($buf);
	my $msg_type = $msg_hdr->{Type};
	die "No MID lookup table" unless ref $lookup_mid;
	my $msg_desc = $lookup_mid->{$msg_type};
	ref $msg_desc or die "Unknown message ID $msg_type";

	# add the message header
	my $root = $tree->createElement ($msg_desc->{Name});
	$tree->setDocumentElement ($root);

	# decode the body of the message
	decode_body ($tree, $root, gen_array_iter ($msg_desc->{Parameter}, MSG_HEADER_LEN),
		substr ($buf, $ndx, $msg_hdr->{MessageLength} - $ndx), %aux);

	# set the version and message ID attributes
	$root->setAttribute ('Version'	,	$msg_hdr->{'Version'}	);
	$root->setAttribute ('MessageID',	$msg_hdr->{'MessageID'}	);

	return ($tree);
}

1;

=back

=head1 AUTHOR

John R. Hogerhuis

Chris Delaney

=head1 BUGS

None

=head1 SEE ALSO

EPCGlobal LLRP Specification

=head1 COPYRIGHT
                                                                           
Copyright 2007 Impinj, Inc.

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
