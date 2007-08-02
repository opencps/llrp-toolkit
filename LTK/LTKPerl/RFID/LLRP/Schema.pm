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
# RFID::LLRP::Schema.pm - LLRP schema interpreter
# Generate Perl "POD" documentation using the pod generator of your choice
# Example: pod2text ./Schema.pm
# 
# This file is only useful in concert with the llrp1.desc descriptor file
#
#############################################################################

=pod

=head1 RFID::LLRP::Schema

=head1 SYNOPSIS

Contains various facilities for dealing with LLRP schemas

=head1 DESCRIPTION

Produces LLRP schema objects. It is safe to create as many of these as you
need... only one object is created (and cached) per filename string.

=head1 PRACTICAL EXAMPLE

	use RFID::LLRP::Schema;
	my ($llrp, $lookup_msg, $lookup_param, $lookup_enum) =
		 read_schema ('./llrp.desc');

=cut

=head1 DETAILS

=head1 API

=over 4

=cut



package RFID::LLRP::Schema;

require Exporter;
@ISA		= qw(Exporter);
@EXPORT_OK	= qw(read_schema);

use Cwd;

use Text::ParseWords qw(quotewords);
use Date::Parse;
use File::Spec;

use XML::LibXSLT;
use XML::LibXML;

$schema_dir = $INC{"RFID/LLRP/Schema.pm"};
($schema_volume, $schema_dir) = File::Spec->splitpath ($schema_dir);

use Memoize;
memoize ('read_schema',
	NORMALIZER => sub { return getcwd() . ',' . $_[0] }
);

my %subparser_generator = (
	'parameter'		=> \&parameter_compiler_gen,
	'enumeration'		=> \&enumeration_compiler_gen,
	'message'		=> \&message_compiler_gen,
	'order'			=> \&order_compiler_gen,
	'vendor'		=> \&vendor_compiler_gen,
	'custom-message'	=> \&custom_message_compiler_gen,
	'custom-parameter'	=> \&custom_parameter_compiler_gen
);

my %llrp;
my @tokens;
my $compiler;
my %namespaces;

=item read_schema($path)

Returns a reference to the llrp binary format descriptor hash,
and references to message, parameter and enumeration lookup
tables.

The best way to understand these data structures is to print them using
Data::Dumper. Builder.pm just peers directly into these tables as a "layer" and
it seems hard, inefficient and more importantly, pointless (tight coupling with
a layered API or tight coupling with a data structure, any way you go you will
still have tight coupling) to obfuscate it with an API. If the layout doesn't
suit your needs, build additional indexing data structures.

C<read_schema> returns

=over 8

=item C<\%llrp>
a reference to the LLRP schema

=item C<\%lookup_msg>
a reference to a hash for looking up LLRP Message descriptors by name

=item C<\%lookup_param>
a reference to a hash for looking up LLRP Parameter descriptors by name

=item C<\%lookup_mid>
a reference to a hash for looking up LLRP Message descriptors by Type ID

=item C<\%lookup_pid>
a reference to a hash for looking up LLRP Parameter descriptors by Type ID

=back

Hint: the best way to understand the schema is to print it out with
C<Data::Dumper>.

=cut

use Data::Dumper;

sub read_schema {

	my $fname = shift;

	# create the descriptor file in memory
	my $parser = XML::LibXML->new();
	my $xslt = XML::LibXSLT->new();
	my $schema_path = File::Spec->catpath ($schema_volume, $schema_dir, $fname);
	my $xslt_path = File::Spec->catpath ($schema_volume, $schema_dir, './llrpdef2llrp1.xslt');
	my $source = $parser->parse_file ($schema_path);
	my $style_doc = $parser->parse_file ($xslt_path);
	my $stylesheet = $xslt->parse_stylesheet ($style_doc);
	my $results = $stylesheet->transform ($source);
	my $strfile = $stylesheet->output_string ($results);	

	# process the descriptor file
	foreach $line (split(/^/, $strfile)) {
		# split into tokens, filtering out whitespace, blanklines,
		# comments, and notes
		$line =~ s/^\s+//;
		$line =~ s/\s+$//;
		chomp ($line);
		if ($line =~ /^#/ || $line =~ /^\s*$/) {
			next;
		}
		@tokens = &quotewords ('\s+', 0, $line);
		splice (@tokens, $#tokens, 1) if ($tokens[$#tokens] =~ /^\*/);
		# process
		my $head = lc shift @tokens;
		if (exists $subparser_generator{$head}) {
			$compiler = $subparser_generator{$head}->($head, @tokens);
		} else {
			$compiler->($head, @tokens);
		}
	}
	close (INFILE);

	# build a parameter name enumeration
	my $param_names = {};
	foreach $desc (@{$llrp{Parameters}}) {
		my ($type_id, $name) = ($desc->{TypeID}, $desc->{Name});
		$param_names->{Definition}->[$type_id] = $name;
		$param_names->{Lookup}->{$name} = $type_id;
	}
	$llrp{Enumerations}->{'_ParamNames'} = $param_names;
	
	# build lookup table for messages
	my %lookup_msg;
	my %lookup_mid;
	foreach $desc (@{$llrp{Messages}}) {
		$lookup_msg{$desc->{Name}} = $desc;
		$lookup_mid{$desc->{TypeID}} = $desc;
	}

	# build lookup table for parameters
	my %lookup_param;
	my %lookup_pid; 
	foreach $desc (@{$llrp{Parameters}}) {
		$lookup_param{$desc->{Name}} = $desc;
		if ($desc->{Concrete}) {
			$lookup_pid{$desc->{TypeID}} = $desc;
		}
	}

	# build mini-lookup tables for each abstract parameter type
	foreach $abstract (@{$llrp{Parameters}}) {
		next if ($abstract->{Concrete});
		my $flatten;
		$flatten = sub {

			my $head = shift;
			return unless defined $head;
			my $param_desc = $lookup_param{$head->{Name}};
			if (!defined $param_desc) {
				return;
			} elsif ($param_desc->{Concrete}) {
				return $param_desc->{Name}, $param_desc, $flatten->(@_)
			} else {
				return $flatten->(@{$param_desc->{Parameter}}, @_);
			}
		};

		my %choice_hash = $flatten->(@{$abstract->{Parameter}});
		$abstract->{Choices} = \%choice_hash;
	}

	# calculate lengths of TV parameters
PARAMETER:	
	foreach $param (@{$llrp{Parameters}}) {
		$param->{Concrete} || next PARAMETER;
		my $bits = 0;
		foreach $desc (@{$param->{Parameter}}) {

			# only compute length for TV parameters
			if ($desc->{Name} eq 'TVEncoding' && 
				!$desc->{DefaultValue}) {
				next PARAMETER;
			}

			# accumulate # bits
			$bits += $desc->{Bits};
		}
		$param->{FixedLength} = ($bits + 7) / 8;
	}

	return (\%llrp, \%lookup_msg, \%lookup_param, \%lookup_mid, \%lookup_pid);
}

# explodes the abbreviated type representation into nv pairs
# may result in multiple records since vectors map to a 
# 16-bit length followed by the value.
sub insert_type {


	my ($type_name, $name, $subparams, %nvpairs) = @_;
	my %sign_name = ('u' => "Unsigned", 's' => "Signed");
	my %bit_name = (1 => 'Bit', 8 => 'Byte', 16 => 'Short', 64 => 'Long', 96 => 'EPC');
	my $orig_name = $type_name;
	if ($type_name =~ /(utf|[us])(\d+)(v|)\s*$/) {
		my ($type, $bits, $vector) = ($1, $2, $3);
		my $length_name;
		if ($type eq 'utf') {
			$type = 'u';
			if (!defined $nvpairs{Format}) {
				$nvpairs{Format} = 'Utf8';
			}
		}

		# build the concrete type name
		if ($bits == 1) {
			$type_name = $bit_name{$bits};
		} elsif ($bits == 8) {
			if (defined $nvpairs{Format} && (lc($nvpairs{Format}) =~ /hex|utf8/)) {
				$type_name = $bit_name{$bits};
			} else {
				$type_name = $sign_name{$type} . 'Integer';
			}
		} elsif ($bits == 96) {
			$type_name = 'EPC96';
		} else {
			$type_name = $sign_name{$type} .
				(defined $bit_name{$bits} ? $bit_name{$bits} : "")
				. 'Integer';
		}
		my $is_array = ($vector eq 'v') ? 1 : 0;

		# handle special case of uncounted vector
		my $is_counted = $is_array && !($orig_name =~ /data_/);
		
		# push a 16-bit length field if this is a counted vector
		if ($is_counted) {
			push @{$subparams}, {
				Name => 'Length',
				Type => 'UnsignedShortInteger',
				Format => 'Decimal',
				Bits => 16,
				Leaf => 1,
				BinaryOnly => 1,
				DefaultValue => 0
			};
		}

		# create the base record based upon exploded fields
		push @{$subparams}, {
			Name		=> $name,
			Type		=> $type_name,
			Optional	=> 0,
			Bits		=> $bits,
			Leaf		=> 1,
			Array		=> $is_array,
			Counted		=> $is_counted,
			Signed		=> (($type eq 's') ? 1 : 0)

		};

		# process nv-pairs
		my ($dname, $dvalue);
		my $param = $subparams->[-1];
		while (($dname, $dvalue) = each %nvpairs) {
			$param->{$dname} = $dvalue;
		}

		# set default Format if omitted
		if (!defined $param->{Format}) {
			if (defined $param->{Enum}) {
				$param->{Format} = 'Enum';
			} else {
				if ($bits == 1) {
					$param->{Format} = 'Boolean';
				} else {
					$param->{Format} = 'Decimal';
				}
			}
		}

	} else {
		die "BUG: Unexpected call to insert_type\n";
	}
}

# compiles a parameter descriptor list
sub compile_params {

	my ($subparams, $desc_name, $class) = splice (@_, 0, 3);

	# delete the compiler if this is the end of the block
	if ($class eq 'end') {
	
		$compiler = undef;

	# parse a leaf node
	} elsif ($class eq 'field') {

		my ($type, $name, @named) = @_;

		# split name-value pairs at end of line
		my @nvpairs;
		foreach $detail (@named) {
			my ($name, $value) = split (/=/, $detail);
			next unless defined $value;
			
			if ($name eq 'fmt') {
				$name =~ s/fmt/Format/;
			} else {
				$name = ucfirst lc $name;
			}
			push @nvpairs, ($name, $value);
		}

		# translate the type scribble to one or more binary descriptors
		insert_type ($type, $name, $subparams, @nvpairs);

	# parse a non-leaf node
	} elsif ($class eq 'param') {

		my ($cardinality, $name, @named) = @_;

		# process named param def extensions
		my %nvpairs;
		foreach $pair (@named) {
			my ($key, $value) = split (/=/, $pair);
			next unless defined $value;
			$nvpairs{ucfirst lc $key} = $value;
			
		}

		$cardinality = lc $cardinality;
		my ($min_occurs, $max_occurs) = ($cardinality =~ /(\d+)-?(\d+|n|)/);
		if (!defined $min_occurs) {$min_occurs = 1}
		if (!defined $max_occurs) {$max_occurs = 1}

		push @$subparams, {
			Type		=> $name,
			Name		=> $nvpairs{'Name'} || $name,
			Optional	=> (($min_occurs + 0) ? 0 : 1),
			Array		=> (($max_occurs eq 'n') ? 1 : 0),
			Leaf		=> 0
		};
		
	} elsif ($class eq 'reserved') {
		my $bits = shift;
		
		push @$subparams, {
			Type		=> 'UnsignedInteger',
			Name		=> 'Reserved',
			DefaultValue	=> 0,
			Bits		=> $bits,
			Leaf		=> 1,
			BinaryOnly	=> 1,
			Optional	=> 0
		};
		
	# incorporate references
	} elsif ($class eq 'reference') {
		$message_ref->{Reference} = shift; 

	# error for unknown classes
	} else { die "Unknown class $class in body of parameter $desc_name\n"; }

}

sub parameter_compiler_gen {

	my ($class, $id, $desc_name);
	($head, $class, $id, $desc_name) = @_;

	# construct the parameter definition
	my $param_ref = {
		Name		=> $desc_name,
		Concrete	=> ($class ne 'union') + 0,
		Parameter	=> [],
		TypeID		=> $id
	};
	push @{$llrp{Parameters}}, $param_ref;

	# add to the list of namespaces
	$namespaces{LLRP}->{$desc_name} = 1;

	# construct a "concrete" set of default parameters (the common
	# parameter header)
	if ($param_ref->{Concrete}) {
		if ($class eq 'tv') {
			$param_ref->{Parameter} = [ {
				Name		=> 'TVEncoding',
				DefaultValue	=> 1,
				Bits		=> 1,
				Leaf		=> 1,
				Type		=> 'UnsignedInteger',
				Format		=> 'Boolean',
				BinaryOnly	=> 1,
				Optional	=> 0
			}, {
				Name		=> 'Type',
				DefaultValue	=> $id + 0,
				Bits		=> 7,
				Leaf		=> 1,
				Type		=> 'UnsignedInteger',
				Format		=> 'Decimal',
				BinaryOnly	=> 1,
				Optional	=> 0
			}
			];
		} else {
			$param_ref->{Parameter} = [ {
				Name		=> 'TVEncoding',
				DefaultValue	=> 0,
				Bits		=> 1,
				Leaf		=> 1,
				Type		=> 'UnsignedInteger',
				Format		=> 'Boolean',
				BinaryOnly	=> 1,
				Optional	=> 0
			}, {
				Name		=> 'Reserved',
				DefaultValue	=> 0,
				Bits		=> 5,
				Leaf		=> 1,
				Type		=> 'UnsignedInteger',
				Format		=> 'Decimal',
				BinaryOnly	=> 1,
				Optional	=> 0
			}, {
				Name		=> 'Type',
				DefaultValue	=> $id + 0,
				Bits		=> 10,
				Leaf		=> 1,
				Type		=> 'UnsignedInteger',
				Format		=> 'Decimal',
				BinaryOnly	=> 1,
				Optional	=> 0
			}, {
				Name		=> 'Length',
				DefaultValue	=> 4,
				Bits		=> 16,
				Leaf		=> 1,
				Type		=> 'UnsignedInteger',
				Format		=> 'Decimal',
				BinaryOnly	=> 1,
				Optional	=> 0
			}
			];
		}
	} else {
		$param_ref->{Parameter} = [];
	}

	my $subparams = $param_ref->{Parameter};

	# return a parser which can process the body of a parameter descriptor
	return sub { compile_params ($subparams, $desc_name, @_); }

}

sub compile_common_message_header {

	my ($message_ref, $id) = @_;

	# construct the header field descriptors
	$message_ref->{Parameter} = [ {
		Name		=> 'Reserved',
		DefaultValue	=> 0,
		Bits		=> 3,
		Leaf		=> 1,
		Type		=> 'UnsignedInteger',
		Format		=> 'Decimal',
		BinaryOnly	=> 1,
		Optional	=> 0
	}, {
		Name		=> 'Version',
		DefaultValue	=> 1,
		Bits		=> 3,
		Leaf		=> 1,
		Type		=> 'UnsignedInteger',
		Format		=> 'Decimal',
		BinaryOnly	=> 1,
		Optional	=> 0
	}, {
		Name		=> 'Type',
		DefaultValue	=> $id + 0,
		Bits		=> 10,
		Leaf		=> 1,
		Type		=> 'UnsignedInteger',
		Format		=> 'Decimal',
		BinaryOnly	=> 1,
		Optional	=> 0
	}, {
		Name		=> 'MessageLength',
		DefaultValue	=> 10,
		Bits		=> 32,
		Leaf		=> 1,
		Type		=> 'UnsignedInteger',
		Format		=> 'Decimal',
		BinaryOnly	=> 1,
		Optional	=> 0
	}, {
		Name		=> 'MessageID',
		DefaultValue	=> 0xacc01ade,
		Bits		=> 32,
		Leaf		=> 1,
		Type		=> 'UnsignedInteger',
		Format		=> 'Decimal',
		BinaryOnly	=> 1,
		Optional	=> 0
	}
	];

}

sub message_compiler_gen {

	my ($msg_class, $direction, $id, $desc_name);
	($head, $direction, $id, $desc_name) = @_;

	# construct the parameter definition
	my $message_ref = {
		Name		=> $desc_name,
		ToReader	=> (($direction eq 'cmd') ? 1 : 0),
		Parameter	=> [],
		TypeID		=> $id
	};
	push @{$llrp{Messages}}, $message_ref;
	$namespaces{LLRP}->{$desc_name} = 1;

	# construct the header field descriptors
	compile_common_message_header ($message_ref, $id);

	my $subparams = $message_ref->{Parameter};

	# return a parser which can process the body of a message descriptor
	return sub { compile_params ($subparams, $desc_name, @_); }
}

sub enumeration_compiler_gen {

	my ($enum_class, $id, $name) = @_;

	# create the enumeration table
	my $enum_ref = {};
	$llrp{Enumerations}->{$name} = $enum_ref;
	$enum_ref->{Definition} = [];

	# return a subroutine to compile the enumeration
	return sub {
	
		if ($_[0] eq 'reference') {
			$enum_ref->{Reference} = $_[1];
			return;
		} elsif ($_[0] eq 'end') {
			$compiler = undef;
			return;
		}

		# fail if unknown type
		if ($_[0] ne 'enum') { die "Unknown enum syntax $_[0]" }
		
		# handle enumeration name-value pair
		my ($head, $ndx, $identifier) = @_;
		$enum_ref->{Definition}->[$ndx] = $identifier;
		$enum_ref->{Lookup}->{$identifier} = $ndx;
	}
}

sub vendor_compiler_gen {
	my ($head, $pen, $name) = @_;
	$llrp{Vendors}->{$name}	= $pen;
	$llrp{Vendors}->{$pen}	= $name;
	return sub { $compiler = undef }
}

sub custom_message_compiler_gen {
	my ($head, $vendor_name, $subtype, $name) = @_;

	# construct the parameter definition
	my $message_ref = {
		Name			=> $desc_name,
		Parameter		=> [],
		MessageSubtype		=> $subtype,
		VendorName		=> $vendor_name
	};
	compile_common_message_header ($message_ref, 1023);
	$namespaces{$vendor_name}->{$desc_name} = 1;
	push @{$llrp{CustomMessages}}, $message_ref;
	push @{$message_ref->{Parameter}}, (
		{
			Name		=> 'VendorIdentifier',
			DefaultValue	=> $llrp{Vendors}->{$vendor_name},
			Bits		=> 32,
			Leaf		=> 1,
			Type		=> 'UnsignedInteger',
			Format		=> 'Decimal',
			BinaryOnly	=> 1,
			Optional	=> 0
		}, {
			Name		=> 'MessageSubtype',
			DefaultValue	=> $subtype,
			Bits		=> 8,
			Leaf		=> 1,
			Type		=> 'UnsignedInteger',
			Format		=> 'Decimal',
			BinaryOnly	=> 1,
			Optional	=> 0
		}
	);

	# return a compiler for the remaining fields and params
	my $subparams = $message_ref->{Parameter};

	# return a parser which can process the body of a message descriptor
	return sub { compile_params ($subparams, $desc_name, @_); }

}

sub custom_parameter_compiler_gen {

	my ($vendor_name, $subtype, $desc_name);
	($head, $vendor_name, $subtype, $desc_name) = @_;

	# construct the custom parameter definition
	my $param_ref = {
		Name			=> $desc_name,
		Concrete		=> 1,
		Parameter		=> [],
		TypeID			=> $id,
		ParameterSubtype	=> $subtype,
		VendorName		=> $vendor_name
	};
	push @{$llrp{CustomParameters}}, $param_ref;
	$namespaces{$vendor_name}->{$desc_name} = 1;

	# construct a "concrete" set of default parameters (the common
	# parameter header)
	$param_ref->{Parameter} = [ {
			Name		=> 'TVEncoding',
			DefaultValue	=> 0,
			Bits		=> 1,
			Leaf		=> 1,
			Type		=> 'UnsignedInteger',
			Format		=> 'Boolean',
			BinaryOnly	=> 1,
			Optional	=> 0
		}, {
			Name		=> 'Reserved',
			DefaultValue	=> 0,
			Bits		=> 5,
			Leaf		=> 1,
			Type		=> 'UnsignedInteger',
			Format		=> 'Decimal',
			BinaryOnly	=> 1,
			Optional	=> 0
		}, {
			Name		=> 'Type',
			DefaultValue	=> 1023,
			Bits		=> 10,
			Leaf		=> 1,
			Type		=> 'UnsignedInteger',
			Format		=> 'Decimal',
			BinaryOnly	=> 1,
			Optional	=> 0
		}, {
			Name		=> 'Length',
			DefaultValue	=> 4,
			Bits		=> 16,
			Leaf		=> 1,
			Type		=> 'UnsignedInteger',
			Format		=> 'Decimal',
			BinaryOnly	=> 1,
			Optional	=> 0
		}, {
			Name		=> 'VendorIdentifier',
			DefaultValue	=> $llrp{Vendors}->{$vendor_name},
			Bits		=> 32,
			Leaf		=> 1,
			Type		=> 'UnsignedInteger',
			Format		=> 'Decimal',
			BinaryOnly	=> 1,
			Optional	=> 0
		}, {
			Name		=> 'ParameterSubtype',
			DefaultValue	=> $subtype,
			Bits		=> 32,
			Leaf		=> 1,
			Type		=> 'UnsignedInteger',
			Format		=> 'Decimal',
			BinaryOnly	=> 1,
			Optional	=> 0
		}
	];

	my $subparams = $param_ref->{Parameter};

	# return a parser which can process the body of a parameter descriptor
	return sub { compile_params ($subparams, $desc_name, @_); }

}

sub order_compiler_gen {

	# for now, just discard the order information
	# otherwise, before 'return' is where the order handling would go

	return sub {
		# only action is to uninstall the compiler since
		# orders have no body to compile
		$compiler = undef;
	};
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

