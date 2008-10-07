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
		 read_schema ('./ReaderDef.xml');

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
use Clone qw(clone);

use XML::LibXSLT;
use XML::LibXML;

# use Data::Dumper;

$schema_dir = $INC{"RFID/LLRP/Schema.pm"};
($schema_volume, $schema_dir) = File::Spec->splitpath ($schema_dir);

use Memoize;
memoize ('read_schema',
	NORMALIZER => sub { return getcwd() . ',' . $_[0] }
);

my %subparser_generator = (
	'parameter'		=> \&parameter_compiler_gen,
	'enumeration'		=> \&enumeration_compiler_gen,
	'custom-enumeration'	=> \&custom_enum_compiler_gen,
	'message'		=> \&message_compiler_gen,
	'order'			=> \&order_compiler_gen,
	'vendor'		=> \&vendor_compiler,
	'custom-message'	=> \&custom_message_compiler_gen,
	'custom-parameter'	=> \&custom_parameter_compiler_gen,
	'custom-union'		=> \&custom_parameter_compiler_gen,
	'core-namespace'	=> \&core_namespace_compiler,
	'namespace'		=> \&ext_namespace_compiler
);

my %llrp;
my @tokens;
my $compiler;
my %registry;

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

=item C<\%registry>
A reference for looking up (mangled) LLRP types, including extensions

=back

=cut


sub fqp_type {
	my @result = map {sprintf ("P.%s.%s", $_->{Namespace}, $_->{Type})} @_;
	return wantarray ? @result : $result[0];
}

sub fq_type {
	my @result = map {sprintf ("%s.%s.%s", $_->{Group}, $_->{Namespace}, $_->{Name})} @_;
	return wantarray ? @result : $result[0];
}

sub fqp_name {
	my @result = map {sprintf ("P.%s.%s", $_->{Namespace}, $_->{Name})} @_;
	return wantarray ? @result : $result[0];
}

sub flatten {
	# this routine creates a lookup hash for alternation match rule
	# includes in hash any extensions permitted at the alternation point

	my $head = shift;
	return unless defined $head; # handle end-of-list

	# lookup the type descriptor
	my $type_name = fqp_type ($head);

	my $param_desc = $registry{$type_name};

	if (!defined $param_desc) {
		die "Found an undef parameter type descriptor for $type_name";

	} elsif ($head->{Extends}) {

		my $allow_rules = $registry{$head->{Extends}}->{Extensions};

		my %allowed;
		foreach $allow_rule (@{$allow_rules}) {
			$allowed{fqp_name ($allow_rule)} = $allow_rule;
		}

		# clone the current parameter descriptor to allow Custom blobs
		# using clone to avoid circular reference
		my $temp_head;
		$allowed{fqp_type ($head)} = $temp_head = clone $head;
		delete $temp_head->{Extends};

		return %allowed, flatten (@_);

	} elsif ($param_desc->{Concrete}) {
		return $type_name, $head, flatten (@_)

	} else { # it's a nested alternation point... squash it!
		return flatten (@{$param_desc->{Parameter}}), flatten (@_);
	}
}

sub read_schema {

	my $fname = shift;

	# create the (llrp1.desc format) descriptor file in memory
	my $parser = XML::LibXML->new();
	my $xslt = XML::LibXSLT->new();
	my $schema_path = File::Spec->catpath ($schema_volume, $schema_dir, $fname);
	my $xslt_path = File::Spec->catpath ($schema_volume, $schema_dir, './llrpdef2llrp1.xslt');
	my $source = $parser->parse_file ($schema_path);
	$parser->process_xincludes ($source);
	my $style_doc = $parser->parse_file ($xslt_path);
	my $stylesheet = $xslt->parse_stylesheet ($style_doc);
	my $results = $stylesheet->transform ($source);
	my $strfile = $stylesheet->output_string ($results);

	# process the descriptor file
	foreach $line (split (/^/, $strfile)) {

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

	# build the name registry
	my %category = (
		Messages		=> { Abbrev => 'M', Fields => ['Namespace', 'TypeID'		] },
		CustomMessages		=> { Abbrev => 'M', Fields => ['VendorName', 'MessageSubtype'	] },
		Parameters		=> { Abbrev => 'P', Fields => ['Namespace', 'TypeID'		] },
		CustomParameters	=> { Abbrev => 'P', Fields => ['VendorName', 'ParameterSubtype'	] }
	);
	while (($type, $conv) = each (%category)) {
		foreach $desc (@{$llrp{$type}}) {
			my ($abbr, $ns, $name) = ($conv->{Abbrev}, @{$desc}{'Namespace', 'Name'});
			my $sym_key = "$abbr.$ns.$name";
			$registry{$sym_key} = $desc;
			next unless $desc->{Concrete} || $abbr eq 'M';
			my ($ns_field, $id_field) = @{$conv->{Fields}};
			if ($ns_field eq 'VendorName') {
				$registry{join ('.', $abbr,
					$llrp{Vendors}->{$desc->{VendorName}},
					$desc->{$id_field})
				} = $desc;
			} else {
				$registry{join ('.', $abbr, @{$desc}{$ns_field, $id_field})} = $desc;
			}
		}
	}

	# build a core parameter name enumeration
	my $param_names = {};
	foreach $desc (@{$llrp{Parameters}}) {
		next unless defined $desc->{TypeID};
		my ($type_id, $name) = ($desc->{TypeID}, $desc->{Name});
		$param_names->{Definition}->[$type_id] = $name;
		$param_names->{Lookup}->{$name} = $type_id;
	}
	$llrp{Enumerations}->{$llrp{CoreNamespace}. '_' . '_ParamNames'} = $param_names;

	# annotate all extension and alternation points with alternatives
	while (($type, $conv) = each (%category)) {
		foreach $desc (@{$llrp{$type}}) {
			foreach $rule (@{$desc->{Parameter}}) {

				next if $rule->{Leaf};

				# given the match rule, lookup the type definition
				my $whole_name = fqp_type ($rule);
				my $context = $registry{$whole_name};

				ref $context or
					die "Unable to locate $whole_name type defn";

				next unless !$context->{Concrete}
					or $whole_name eq
						'P.' .
						$llrp{CoreNamespace} .
						'.Custom';

				my %choices = flatten ($rule);

				# add in all ID forms to alternatives
				my %additions;
				while (my ($key, $value) = each (%choices)) {
					my $sp = $registry{$key};
					my $idkey = join ('.', 'P',
						((exists $sp->{Extension} && $sp->{Extension})
						? ($llrp{Vendors}->{$sp->{VendorName}}, $sp->{ParameterSubtype})
						: @{$sp}{qw/Namespace TypeID/}));
					$additions{$idkey} = $value;
				}
				%choices = (%choices, %additions);
				$rule->{Alternatives} = \%choices;
			}
		}
	}

	# mark all parameters and messages whose last field is hoistable
	foreach my $param (
		@{$llrp{Parameters}},
		@{$llrp{Messages}},
		@{$llrp{CustomMessages}},
		@{$llrp{CustomParameters}}
	) {
		$param->{Hoistable} = 0;

		if (($param->{FieldCount} == 1 && $param->{ParamCount} == 0)
		    || ($param->{FieldCount} == 0 && $param->{ParamCount} == 1)) {
			$param->{Hoistable} = 1;
		}
	}

	# calculate lengths of TV parameters
	PARAMETER: foreach $param (@{$llrp{Parameters}}) {
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

	return (\%llrp, \%registry);
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
			$dname =~ s/ns/Namespace/i;
			if ($dname eq 'Namespace') {
				$dvalue = $llrp{Prefix2NS}->{$dvalue};
			}
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

	my ($body_ref, $subparams, $desc_name, $class) = splice (@_, 0, 4);
	my ($prefix, $ns);

	# delete the compiler if this is the end of the block
	if ($class eq 'end') {

		$body_ref->{ParamCount} =  $body_ref->{ParamCount} || 0;
		$body_ref->{FieldCount} =  $body_ref->{FieldCount} || 0;
	
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
				$value = ucfirst lc $value;
			} else {
				$name = ucfirst lc $name;
			}
			push @nvpairs, ($name, $value);
		}

		# translate the type scribble to one or more binary descriptors
		insert_type ($type, $name, $subparams, @nvpairs);

		$body_ref->{FieldCount} += 1;
		$body_ref->{ParamCount} = 0;

	# parse a non-leaf node
	} elsif ($class eq 'param' || $class eq 'allow') {

		my ($cardinality, $name, @named) = @_;

		# extract the optional namespace qualifier if present
		if (defined ($named[0]) && !($named[0] =~ /=/)) {
			$ns = $llrp{Prefix2NS}->{shift (@named)};
		}

		# process named param def extensions
		my %nvpairs;
		foreach $pair (@named) {
			my ($key, $value) = split (/=/, $pair);
			next unless defined $value;
			$nvpairs{ucfirst lc $key} = $value;
			
		}

		my ($min_occurs, $max_occurs) = ($cardinality =~ /(\d+)-?(\d+|n|)/);
		if (!defined $min_occurs) {$min_occurs = 1}
		if (!defined $max_occurs) {$max_occurs = 1}

		my $dest = $class eq 'param' ? $subparams : $body_ref->{Extensions};

		push @$dest, {
			Type		=> $name,
			Name		=> $nvpairs{'Name'} || $name,
			Optional	=> (($min_occurs + 0) ? 0 : 1),
			Array		=> (($max_occurs eq 'n') ? 1 : 0),
			Leaf		=> 0,
			Namespace	=> $ns || $llrp{CoreNamespace}
		};

		$body_ref->{ParamCount}++;
		
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
		$body_ref->{Reference} = shift; 

	} elsif ($class eq 'extension-point') {

		# push a custom blob descriptor
		#   - This is what will actually match each binary custom parameter.
		#   - When serializing XML, we need to match this, or something in
		#     the Extensions list on the parent.
		push @$subparams, {
			Type => 'Custom',
			Name => 'Custom',
			Namespace => $llrp{CoreNamespace},
			Extends => fq_type ($body_ref),
			Optional => 1,
			Array => 1,
			Leaf => 0
		};
		$body_ref->{Extensions} =[];
		$body_ref->{ParamCount}++;

	# error for unknown classes
	} else { die "Unknown class $class in body of parameter $desc_name\n"; }

}

sub parameter_compiler_gen {

	my ($class, $id, $desc_name);
	if ($_[1] ne 'union') {
		($head, $class, $id, $desc_name) = @_;
	} else {
		($head, $class, $desc_name) = @_;
	}

	# construct the parameter definition
	my $body_ref = {
		Name		=> $desc_name,
		Concrete	=> ($class ne 'union') + 0,
		Parameter	=> [],
		Group		=> 'P',
		TypeID		=> $id,
		Namespace	=> $llrp{CoreNamespace}
	};
	push @{$llrp{Parameters}}, $body_ref;

	# construct a "concrete" set of default parameters (the common
	# parameter header)
	if ($body_ref->{Concrete}) {
		if ($class eq 'tv') {
			$body_ref->{Parameter} = [ {
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
			$body_ref->{Parameter} = [ {
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
		$body_ref->{Parameter} = [];
	}

	my $subparams = $body_ref->{Parameter};

	# return a parser which can process the body of a parameter descriptor
	return sub { compile_params ($body_ref, $subparams, $desc_name, @_); }

}

sub compile_common_message_header {

	my ($body_ref, $id) = @_;

	# construct the header field descriptors
	$body_ref->{Parameter} = [ {
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
	my $body_ref = {
		Name		=> $desc_name,
		ToReader	=> (($direction eq 'cmd') ? 1 : 0),
		Parameter	=> [],
		TypeID		=> $id,
		Namespace	=> $llrp{CoreNamespace},
		Group		=> 'M'
	};
	push @{$llrp{Messages}}, $body_ref;

	# construct the header field descriptors
	compile_common_message_header ($body_ref, $id);

	my $subparams = $body_ref->{Parameter};

	# return a parser which can process the body of a message descriptor
	return sub { compile_params ($body_ref, $subparams, $desc_name, @_); }
}

sub enumeration_compiler_gen {

	my ($enum_class, $name) = @_;

	# create the enumeration table
	my $enum_ref = {};
	my $ns = $llrp{CoreNamespace};
	$llrp{Enumerations}->{$ns . '_' . $name} = $enum_ref;
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

sub custom_enum_compiler_gen {

	my ($enum_class, $prefix, $name) = @_;
	my $ns = $llrp{Prefix2NS}->{$prefix};


	# create the enumeration table
	my $enum_ref = {};
	$llrp{Enumerations}->{$ns . '_' . $name} = $enum_ref;
	$enum_ref->{Definition} = [];
	$enum_ref->{Name} = $name;
	$enum_ref->{Namespace} = $ns;

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

sub vendor_compiler {
	my ($head, $name, $pen) = @_;
	$llrp{Vendors}->{$name}	= $pen;
	$llrp{Vendors}->{$pen}	= $name;
	return sub { $compiler = undef }
}

sub core_namespace_compiler {
	my ($class, $uri) = @_;
	$llrp{CoreNamespace} = $uri;

	return sub { $compiler = undef }
}

sub ext_namespace_compiler {
	my ($class, $prefix, $uri) = @_;

	$llrp{NS2Prefix}->{$uri} = $prefix;
	$llrp{Prefix2NS}->{$prefix} = $uri;
	return sub { $compiler = undef }
}

sub custom_message_compiler_gen {
	my ($head, $vendor_name, $subtype, $prefix, $name) = @_;
	my $ns = $llrp{Prefix2NS}->{$prefix};

	# construct the parameter definition
	my $body_ref = {
		Name			=> $name,
		Parameter		=> [],
		MessageSubtype		=> $subtype + 0,
		VendorName		=> $vendor_name,
		Namespace		=> $ns,
		Group			=> 'M'
	};
	compile_common_message_header ($body_ref, 1023);
	push @{$llrp{CustomMessages}}, $body_ref;
	push @{$body_ref->{Parameter}}, (
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
			DefaultValue	=> $subtype + 0,
			Bits		=> 8,
			Leaf		=> 1,
			Type		=> 'UnsignedInteger',
			Format		=> 'Decimal',
			BinaryOnly	=> 1,
			Optional	=> 0
		}
	);

	# return a compiler for the remaining fields and params
	my $subparams = $body_ref->{Parameter};

	# return a parser which can process the body of a message descriptor
	return sub { compile_params ($body_ref, $subparams, $desc_name, @_); }

}

sub custom_parameter_compiler_gen {

	my ($vendor_name, $subtype, $prefix, $desc_name, $ns);
	if ($_[0] eq 'custom-union') {
		($head, $prefix, $desc_name) = @_;
		$subtype = 0;
		$vendor_name = undef;
	} else {
		($head, $vendor_name, $subtype, $prefix, $desc_name) = @_;
	}
	$ns = $llrp{Prefix2NS}->{$prefix};

	# construct the custom parameter definition
	my $body_ref = {
		Name			=> $desc_name,
		Concrete		=> ($head ne 'custom-union') + 0,
		Parameter		=> [],
		ParameterSubtype	=> $subtype + 0,
		VendorName		=> $vendor_name,
		Group			=> 'P',
		Namespace		=> $ns,
		Extension		=> 1
	};
	push @{$llrp{CustomParameters}}, $body_ref;

	# construct a "concrete" set of default parameters (the common
	# parameter header)
	if ($body_ref->{Concrete}) {
		$body_ref->{Parameter} = [ {
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
				DefaultValue	=> $subtype + 0,
				Bits		=> 32,
				Leaf		=> 1,
				Type		=> 'UnsignedInteger',
				Format		=> 'Decimal',
				BinaryOnly	=> 1,
				Optional	=> 0
			}
		];
	} else {
		$body_ref->{Parameter} = [];
	}

	my $subparams = $body_ref->{Parameter};

	# return a parser which can process the body of a parameter descriptor
	return sub { compile_params ($body_ref, $subparams, $desc_name, @_); }

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



#gen_llrp_binary_schema;

#format_xsd;

=back

=head1 AUTHOR

John R. Hogerhuis
Chris Delaney

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

