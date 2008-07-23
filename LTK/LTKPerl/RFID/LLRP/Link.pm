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
# RFID::LLRP::Link.pm - LLRP binary message tcp communication link support
# Generate Perl "POD" documentation using the pod generator of your choice
#
#############################################################################

=pod

=head1 RFID::LLRP::Link

=head1 SYNOPSIS

This module provides routines to connect and gracefully disconnect an LLRP
session, as well as to support reading LLRP messages from the link.

=head1 DESCRIPTION

This package provides the necessary routines to establish and disestablish
an LLRP session. It provides a routine for reading complete LLRP messages
from the link.

This module is designed to do as little as possible... for example, it does not
derive from the socket class. Instead, it establishes the connection and
returns the socket for the caller to handle.

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

package RFID::LLRP::Link;

use Sub::Exporter -setup => {
	exports => [
		qw(
		reader_connect
		reader_accept
		reader_disconnect
		read_bytes
		read_message
		parse_envelope
		),
		transact => \&subclass,
		fasttran => \&subclass,
		monitor => \&subclass
	]
};

sub subclass {
	my ($class, $name, $arg) = @_;

	return sub {
		$name->(@_, %$arg);
	}
}


use Socket qw(:all);
use IO::Socket;
use IO::Select;
use Time::HiRes qw (time gettimeofday);
use fields;
use File::Spec;
use bytes;
use XML::LibXML;
use RFID::LLRP::Builder qw{encode_message decode_message memoized_encode_message};
use Data::HexDump;
use Data::Dumper;

# include TestRunner if it is there
BEGIN {
	my $runner = 'TestRunner';
	eval "use $runner";
	if ($@) {
		*windup = sub {};
		*windup_remove = sub {};
	}
}


use constant ENVELOPE_LEN => 10;

=item C<reader_connect ($name)>

This function will connect to the reader whose name is specified as $name.
C<$name> may be an IP address or a DNS name.

It handles reading the READER_EVENT_NOTIFICATION which indicates whether the
connection is now valid for LLRP traffic.  If the reader is busy the session is
closed and nothing is returned.

If the reader is not busy with another client, the socket is returned.

If called in list context, this routine will also return
C<$sock>, C<$doc>, C<$buf> in that order. C<$buf> is the undecoded LLRP binary
C<READER_EVENT_NOTIFICATION>, and C<$doc> is the XML DOM object of the same
message but decoded.

=cut

sub reader_connect {

	my ($addr, %params) = @_;
	defined $addr or die "Error: no IP address or reader name passed to reader_connect()\n";

	my $sock = new IO::Socket::INET (
			PeerAddr => $addr,
			PeerPort => 5084,
			Proto => 'tcp',
			Timeout=> 5000);
	$sock || die "Could not connect ($!)\n";

	setsockopt($sock, Socket::IPPROTO_TCP, Socket::TCP_NODELAY, 1);

	my $buf = read_message ($sock, 20);
	my $doc = decode_message ($buf);
	my $cleanup = !$params{NoCleanUp};

	my @nodes = $doc->findnodes ("//*[local-name()='ConnectionAttemptEvent']/*[local-name()='Status' and node()='Success']", $doc);

	if (@nodes == 1) {
		if($cleanup) {
			windup ($sock, \&reader_disconnect);
		}
	} else {
		reader_disconnect ($sock);
		$sock = undef;
	}

	eval { RunLog::StashDebug( "connectport" => $sock->sockport); };

	if (wantarray) {
		return ($sock, $doc, $buf);
	} else {
		return ($sock);
	}
}

=item C<reader_accept>

This function will accept a single LLRP connection.

It handles reading the READER_EVENT_NOTIFICATION which indicates whether the
connection is now valid for LLRP traffic.  If the reader is busy the session is
closed and nothing is returned.

If the reader is not busy with another client, the socket is returned.

If called in list context, this routine will also return
C<$sock>, C<$doc>, C<$buf> in that order. C<$buf> is the undecoded LLRP binary
C<READER_EVENT_NOTIFICATION>, and C<$doc> is the XML DOM object of the same
message but decoded.

=cut

sub reader_accept {
	
	# create a listening socket
	my $listen_sock = IO::Socket::INET->new (
		Listen => 1,
		LocalAddr => '0.0.0.0',
		LocalPort => 5084,
		Reuse => 1
	);

	# accept the connection
	my $sock = $listen_sock->accept();
	$listen_sock->close();
	$listen_sock = undef;

	# read and decode the ConnectionAttemptEvent
	my $buf = read_message ($sock, 30);
	my $doc = decode_message $buf;

	# return socket (and optional stuff) if connection is established
	my @nodes = $doc->findnodes ('//*[local-name()="ConnectionAttemptEvent"]/*[local-name()="Status" and node()="Success"]');
	if (@nodes == 1) {
		windup ($sock, \&reader_disconnect);
		return ($sock);
		if (wantarray) {
			return ($sock, $doc, $buf);
		} else {
			return ($sock);
		}
	}
	
	# error
	$sock->close;
	return;
}


=item C<reader_disconnect ($sock)>

This function will gracefully close the provided socket.
Use this on the socket returned when calling C<reader_connect()>.

It is important that you disconnect from the reader, or else
subsequent connections to the reader may fail.

=cut

my $close_txt = <<'EOT';
<?xml version="1.0" encoding="UTF-8"?>
<CLOSE_CONNECTION MessageID="3233857728">
</CLOSE_CONNECTION>
EOT

my ($close_msg) = encode_message ($close_txt);

sub MIN { $_[0] < $_[1] ? $_[0] : $_[1]; }
sub MAX { $_[0] >= $_[1] ? $_[0] : $_[1]; }

sub reader_disconnect {

	my ($sock, $max_delay) = shift;
	my $rsp;
	windup_remove ($sock);
	$max_delay = 20 unless defined $max_delay;

	# send close document
	$sock->send ($close_msg);

	# measure time waiting for FIN
	my $left_bracket = time();

	# tell the reader we're done sending (send FIN)
	$sock->shutdown (SHUT_WR);
	my $rsp;
	my $remaining = $max_delay - (time() - $left_bracket);
	if ($remaining <= 0) {
		$sock->shutdown (SHUT_RDWR);
		goto CLEANUP;
	}
	eval {
		$rsp = decode_message (read_message ($sock, $remaining));
	};

	# we don't really care for the response, but it comes so
	# drain socket until no data or exception (waiting for reader's FIN)
	$remaining = $max_delay - (time() - $left_bracket);
	if ($remaining <= 0) {
		$sock->shutdown (SHUT_RDWR);
		goto CLEANUP;
	}
	while (1) {
		$remaining = $max_delay - (time() - $left_bracket);
		if ($remaining <= 0) {
			$sock->shutdown (SHUT_RDWR);
			goto CLEANUP;
		}
		my $dummy = eval { read_bytes ($sock, 1, $remaining); };
		last if $@;
	}

	# linger on close for whatever portion of the disconnect
	# timeout is left
CLEANUP: 
	$remaining = $max_delay - (time() - $left_bracket);
	my $linger_timeout = $remaining;
	$linger_timeout = 0 if ($linger_timeout < 0);
	my $linger = pack ('ii', 1, $linger_timeout);
	$sock->sockopt (SO_LINGER, $linger);

	# close with linger
	$sock->close();

	return $rsp;
}

=item C<parse_envelope ($buf)>

This utility function will permit decoding the envelope of a raw LLRP byte
string. It returns C<$ver, $msg_type, $msg_len> and C<$msg_id>, in that order.

=cut


sub parse_envelope {

	my $buf = shift;
	die "Submitted insufficient bytes to envelope parser" 
		unless length($buf) >= ENVELOPE_LEN;
	my ($prefix, $msg_len, $msg_id) = unpack ("nNN", $buf);
	my ($ver, $msg_type) = (($prefix >> 10) & 7, $prefix & ((2**10) - 1));

	return $ver, $msg_type, $msg_len, $msg_id;
}

=item C<read_bytes ($socket, $size, $timeout)>

This utility function will read C<$size> bytes from C<$socket> with
a C<$timeout> second timeout. Upon timeout it will die with
"Error: receive timed out\n"

If no C<$timeout> parameter is provided it will default to 5 seconds.

If you wish to block on read_bytes indefinitely, provide a negative
value for C<$timeout>.

The bytes read, if any, are returned as a Perl string.

If you do not know whether to immediately expect any response,
you should call this routine from within an C<eval {}> construct.

Programmers should eschew this function in favor of C<read_message()>.

=cut

sub read_bytes {

	my ($handle, $size, $timeout) = @_;
	$timeout = 5 unless defined $timeout;
	my $result = "";

	my $sel = IO::Select->new();
	$sel->add ($handle);

	my $fence = time() + $timeout;
	my $cur_time;

	while ((length ($result) < $size) &&
           (($timeout < 0) || (($cur_time = time()) < $fence))) {
	
		my @hlist;
		if ($timeout < 0) {
			@hlist = $sel->can_read ();
		} else {
			@hlist = $sel->can_read ($fence - $cur_time);
		}
		die "Error: receive timed out1\n" unless @hlist;
		my $buf = "";
		if (!defined ($hlist[0]->recv ($buf, $size - length ($result)))) {
			die "Failed to read any bytes ($!)\n";
		}

		if (length ($buf) == 0) {
			die "Error: EOF\n";
		}

		$result .= $buf;
	}

	if (length ($result) < $size) {
		die "Error: receive timed out2\n";
	}

	return ($result);
}

=item C<read_message ($socket, $timeout)>

This utility function will read a complete LLRP message
from the specified C<$socket> with a configurable seconds timeout.
Upon timeout it will die with "Error: receive timed out\n"

If no C<$timeout> parameters is provided it will default to 5 seconds.

If you wish to block on C<read_message()> indefinitely, provide a negative
value for C<$timeout>.

The LLRP binary-formatted message is returned as a Perl string.

=cut

sub read_message {

	my $sock = shift;
	my $timeout = shift;
	my %params = @_;
	$timeout = 5 unless defined $timeout;

	my $envelope;

	my $fence = time() + $timeout;
	my $cur_time;

	# read the header
	die "Error: timed out3\n"
        unless (($timeout < 0) || (($cur_time = time ()) < $fence));
	$envelope = read_bytes ($sock, ENVELOPE_LEN,
                            ($timeout < 0) ? $timeout : $fence - $cur_time);

	# parse the header (and validate the contents)
	my @envelope = parse_envelope ($envelope);
	my $msg_len = $envelope[2];

	# return the envelope if requested
	if (ref ($params{EnvelopeOut})) {
		${$params{EnvelopeOut}} = \@envelope;
	}

	# read the remainder of the packet
	my $payload = read_bytes ($sock, $msg_len - ENVELOPE_LEN, 3);
	
	return $envelope . $payload;
}

=item C<monitor ($sock, TimeFence =E<gt> ?, Timeout =E<gt> ?, ReturnUpon|ErrorUpon =E<gt> [...], Count =E<gt> {} )>

C<monitor()> expects the socket followed by a hash of named parameters. Any
parameter except for C<$sock> may be omitted. Omitting TimeFence and/or Timeout
indicates that the omitted timeout mechanisms should not be used. If none of
timeout, fence, ReturnUpon, and ErrorUpon is provided, then the only thing that
will stop C<monitor()> from running is an exception in the internal LLRP
routines.

This function reads LLRP messages
until one of the following events occurs:

=over 8

=item 1.
C<time()> surpasses C<TimeFence>

=item 2.
There has been no message received in C<Timeout> seconds

=item 3.
One of the XPath queries named in C<ReturnUpon> matches a received message

=item 4.
One of the XPath queries named in C<ErrorUpon> matches a received message
In this case, monitor will die with an error message.

=back

ReturnUpon and ErrorUpon Custom Handlers

You can provide a subroutine reference as a custom handler in place of
XPath strings. Returning 'true' triggers 'return' if it's a part of a
C<ReturnUpon> array. Returning 'true' triggers 'die' if the subroutine
reference is part of a ErrorUpon array.

Count Hash

Pass an anonymous hash with key 'Count' to count up occurances of a set of
XPath queries. The keys of the hash are XPath queries. The associated values
are references to scalars. Monitor will initialize the scalar variables to
zero.

Counts are always performed before the ReturnUpon and ErrorUpon handlers.

QualifyCore => 0 or 1

See the description of QualifyCore given for C<transact()>.

MaxQueue => 0 or more

Depending on the Timeout and TimeFence limits, and the ReturnUpon and
ErrorUpon rules you set, C<monitor()> could block for a long time.

If provided and greater than zero, the MaxQueue parameter is used to
determine the maximum number of recent messages up to and including
the timeout or rule match that will be retained (and returned).

0 or not present means "keep all messages"
1 or more means "keep at most MaxQueue count of messages."

Monitor returns:

=over 8

=item The timeout string (Timeout [inactivity] or TimeUp [hit fence])
or the decoded message that matched if called in scalar context

=item The timeout string or decoded message, followed by all messages which
occured from the time the monitor operation started up until the match or
timeout occured. If there was a match, the final message is the message that
matched. You can tell the difference between a timeout and a match since the
first parameter will be a reference or not, depending (use the C<ref> call).

=back

It is anticipated that C<monitor()> will be called multiple times with the same
C<$time_fence>. This permits running, for example, 20 seconds of inventory. Eventually you
will call C<monitor()> and it will return with 'TimeUp' indicating that the operation
has completed.

The C<$timeout> parameter is provided mainly for cases where you expect a specific
response to a command in a fairly short period of time. If it doesn't come, ('Timeout') will
be returned instead.

=cut

sub monitor {
	my ($sock, %options) = @_;
	my $time_fence;
	my $trace;
	my $timeout;
	my $dumphex;
	my $counters;
	my $return_upon;
	my $error_upon;
	my $perl_queue;
	my $max_queue;
	my $remaining = 0;
	my $qualify_core = 1;
	my @ntf;

	my %param_tbl = (
		'TimeFence'	, \$time_fence	,
		'Timeout'	, \$timeout	,
		'Trace'		, \$trace	,
		'DumpHex'	, \$dumphex	,
		'ReturnUpon'	, \$return_upon	,
		'ErrorUpon'	, \$error_upon	,
		'PerlQueue'	, \$perl_queue	,
		'QualifyCore'	, \$qualify_core,
		'Count'		, \$counters,
		'MaxQueue'	, \$max_queue
	);

	while (my ($key, $value) = each (%param_tbl)) {
		if (defined $options{$key}) {
			$$value = $options{$key} || 0;
			delete $options{$key};
		}
	}

	# initialize counters to zero
	if (ref $counters) {
		while ((undef, $ctrp) = each %{$counters}) {
			$$ctrp = 0;
		}
	}

	MAIN_LOOP: while (1) {

		# handle overall timeout on the run, calculate the read timeout
		$cur_time = time();
		my $tout = $timeout;
		if ($time_fence) {
			if ($cur_time >= $time_fence) {
				unshift @ntf, 'TimeUp',;
				last MAIN_LOOP;
			}
			$remaining = $time_fence - $cur_time;
			$tout = $remaining if (defined ($tout) && $tout > $remaining);
		}

		# read the message, handle timeout exceptions
		eval {
			my $packet = read_message ($sock, $tout ? $tout : $remaining); 
			if ($dumphex) {
				print HexDump $packet, "\n";
			}
			my @decode_opt;
			my $tree;
			if (ref ($perl_queue)) {
				@decode_opt = ('HashParent' => $tree = {});
			}
			my $doc = decode_message ($packet, %options, @decode_opt, QualifyCore => $qualify_core);
			push  @ntf, $doc;
			push @{$perl_queue}, $tree unless !ref ($perl_queue);

			if ($trace) {
				print STDERR $doc->toString(1), "\n";
			}
		};
		if ($@) {
			if ($@ =~ /timed out/) {
				unshift @ntf, ((($tout && ($tout < $remaining)) || !$time_fence) ? 'Timeout' : 'TimeUp');
				last MAIN_LOOP;
			} else {
				die $@;
			}
		}

		# count occurances of XPath matches
		if (ref $counters) {
			my ($xpath, $ctrp);
			while (($xpath, $ctrp) = each %{$counters}) {
				my ($match) = $ntf[$#ntf]->findnodes ($xpath);
				if (defined $match) {($$ctrp)++}
			}
		}

		# match ReturnUpon list
		if (ref $return_upon) {
			foreach $query (@{$return_upon}) {
				my $last_ntf = $ntf[$#ntf];
				if (ref $query eq 'CODE') {
					last MAIN_LOOP if ($query->($last_ntf));
				} else {
					my ($match) = $ntf[$#ntf]->findnodes ($query);
					last MAIN_LOOP if (defined $match);
				}
			}
		}

		# match ErrorUpon list
		if (ref $error_upon) {
			foreach $query (@{$error_upon}) {
				my $last_ntf = $ntf[$#ntf];
				if (ref $query eq 'CODE') {
					$query->($last_ntf) and
						die 'ErrorUpon triggered by sub';
				} else {
					my ($match) = $ntf[$#ntf]->findnodes ($query);
					defined $match and 
						die 'ErrorUpon due to match of ' . $query;
				}
			}
		}

		# enforce queue size limit
		shift @ntf while ($max_queue && @ntf > $max_queue);
	}

	# return results depending on wantarray
	if (!wantarray) {
		if (ref ($ntf[0])) {

			# return the last message received
			return pop (@ntf);
		} else {

			# return timeout/timeup string
			return shift (@ntf);
		}
	}

	return @ntf;
}

=item C<transact ($socket, $doc, %encode_params)>

This high-level routine will perform a complete LLRP transaction:
it transmits the provided document, first transforming it from string or
file to an XML DOM tree, if necessary.

The allowed C<%encode_params> include:

=over 8

=item 1.
C<Timeout =E<gt>> maximum time to wait

=item 2.
C<Trace =E<gt> C<1>> (print LLRP-XML decoded messages as received) or
C<undef> (no trace)

=item 3.
C<BadNewsOK =E<gt> C<1>> (don't raise exception on bad status) or
C<undef> (raise exceptions on bad status)

=item 4.
C<Force =E<gt> C<1>> (Best effort to encode a message with errors) or C<undef> (raise exceptions)

=item 5.
C<Tree =E<gt> C<1>> (DOM object is provided) or C<undef> (string or file is provided)

=item 6.
C<File =E<gt> C<1>> (String represents a file path) or C<undef> (XML document provided in string form)

=item 7.
C<QualifyCore =E<gt> C<0 or 1>>

C<QualifyCore> allows you to control whether XML returned to you
puts LLRP parameters and fields into the LTK-XML namespace.

This is important for XPath 1.0 queries, which is the only kind
XML::LibXML supports. Elements that are in "no-namespace" do not
need to be qualified with a prefix. This tends to make XPath
query strings much easier to read and type.

0 => Core parameters and fields are to be placed in "no-namespace",

1 => Core parameters and fields are to be placed in LLRP core namespace

Note that since we use Sub::Exporter, you can specialize the C<monitor()>
and C<transact()> routines at import time, and never have to pass
QualifyCore again.

=back

In "list context" transact returns the (C<$req>, C<$rsp>, C<@ntf>).

=over 8

=item 1.
C<$req> an XML DOM tree representing the original request

=item 2.
C<$rsp> an XML DOM tree representing the reader response

=item 3.
C<@ntfs> a list of XML DOM tree objects including messages strictly
between $req and $rsp

=back 

=cut

#$$ this should be computed from the Schema.
@mids_with_status = (
	4, 11, 12, 13, 30, 31, 32, 33, 34, 35, 36, 50, 51, 52, 53, 54, 100, 1023
);

%lookup_mids_with_status = map {$_ => 1} @mids_with_status;


sub transact {

	# handle parameters
        my ($sock, $doc, %encode_params) = @_;
	my @ntfs;
	my $timeout = 3;
	my $trace = 0;
	my $bad_news_ok = 0;
	my $first_time = 1;
	my $perl_queue = undef;
	my $nfy_queue = \@ntfs;
	my $dump_string = 0;
	my $memo = 0;
	my $force_audit_yes = undef;
	my $qualify_core = 1;

	my %param_tbl = (
		'Timeout'	, \$timeout	,
		'Trace'		, \$trace	,
		'BadNewsOK'	, \$bad_news_ok	,
		'DumpString'	, \$dump_string	,
		'Queue'		, \$nfy_queue	,
		'PerlQueue'	, \$perl_queue	,
		'MemoEncode'	, \$memo,
		'ForceAuditYes' , \$force_audit_yes,
		'QualifyCore'	, \$qualify_core	
	);

	while (my ($key, $value) = each (%param_tbl)) {
		if (defined $encode_params{$key}) {
			$$value = $encode_params{$key};
			delete $encode_params{$key};
		}
	}

	# print the document in string form if requested
	print "$doc\n" if $dump_string;

	# format the request
        my ($msg, $req_doc);
	if ($memo) {
		($msg, $req_doc) = memoized_encode_message ($doc, %encode_params);
	} else {
		($msg, $req_doc) = encode_message ($doc, %encode_params);
	}

	# ban GET_REPORT() from transact()
	my $rmid = unpack ("n", $msg) & 0x3FF;
	if ($rmid == 60) {
		die "transact() does not support GET_REPORT. Use encode_message()/send()/monitor()";
	}

	if ($trace && $first_time) {
		my ($tdoc) = decode_message ($msg, %encode_params);
		print STDERR $tdoc->toString(1), "\n";
		$first_time = 0;
	}
        $sock->send ($msg);

	my $rsp;
	
	MESSAGE: while (1) {

		my $envelope;
		my $rsp_bin = read_message ($sock, $timeout, 'EnvelopeOut' => \$envelope);

		my $tree = {};
		my @decode_opt = ();
		if (ref ($perl_queue)) {
			@decode_opt = ('HashParent' => $tree);
		}
		$rsp = decode_message ($rsp_bin, @decode_opt, QualifyCore => $qualify_core);
		push (@$perl_queue, $tree) unless !ref ($perl_queue);
		if ($trace) {
			print STDERR $rsp->toString(1), "\n";
		}

		# always raise exception on ReaderExceptionEvent.
		my ($ver, $msg_type, $msg_len, $msg_id) = @$envelope;
		if ($msg_type == 63) {
			my @node = $rsp->findnodes (
				'/*[local-name()="READER_EVENT_NOTIFICATION"]/' .
				'*[local-name()="ReaderEventNotificationData"]/*[local-name()="ReaderExceptionEvent"]');
			if (@node) {
				print STDERR "ReaderExceptionEvent: ", $node[0]->toString(1);
				die "FAILED: ReaderExceptionEvent, " . $node[0]>findvalue ('Message');
			}
		}

		# can avoid this code if this is not one of ours
		if (!$lookup_mids_with_status{$msg_type}) {
			push @{$nfy_queue}, $rsp;
			next MESSAGE;
		}

		# raise exception if any error codes
		if (!$bad_news_ok) {
			
			my $dump_err = 0;

			my @status_tnodes = $rsp->findnodes ('//*[local-name()="StatusCode"]/node()');

			if (!@status_tnodes) {
				push @{$nfy_queue}, $rsp;
				next MESSAGE;
			}

			if (!@status_tnodes) {
				$dump_err = 1;
			}
			
			foreach $status_tnode (@status_tnodes) {
				if (! ($status_tnode->nodeValue  =~ /M_Success/)) {
					$dump_err = 1;
					last;
				}
			}

			if ($dump_err) {

				print STDERR "\nProblem Response:\n", $rsp->toString(1), "\n";
				die "Encountered missing or unexpected status";
			}
		}

		last;
	}

	if (wantarray) {
		return ($req_doc, $rsp, @{$nfy_queue});
	} else {
		return $rsp;
	}
}

=item C<fasttran ($socket, $msg, %encode_params)>

This special-purpose routine will perform a complete LLRP transaction: it
transmits the provided binary message and waits for a status response. It is a
lightweight alternative to transact for cases when you can't afford decoding
the XML to a DOM tree during the transaction.

The allowed C<%encode_params> include:

=over 8

=item 1.
C<Timeout =E<gt>> maximum time to wait

=item 2.
C<Trace =E<gt> C<1>> (print LLRP-XML decoded messages as received) or
C<undef> (no trace)

=item 3.
C<BadNewsOK =E<gt> C<1>> (don't raise exception on bad status) or
C<undef> (raise exceptions on bad status)

=back

In "list context" transact returns the (C<$status_code>, C<@ntf>).

=over 8

=item 1.
C<$status_code> is an integer representing the transactions status.
0 is 'Success.'

=item 2.
C<@ntfs> a list of all binary LLRP messages received from the reader
including the message causing fasttran to return control to the
caller.

=back 

=cut

@skip_nfys = (
	247, 248, 249, 250, 251
);

sub fasttran {

	# handle parameters
        my ($sock, $msg, %params) = @_;
	my $timeout = 3;
	my @ntfs;
	my $nfy_queue = \@ntfs;

	# ban GET_REPORT()
	my $rmid = unpack ("n", $msg) & 0x3FF;
	if ($rmid == 60) {
		die "fasttran() does not support GET_REPORT. Use encode_message()/send()/monitor()";
	}

	if ($params{Trace}) {
		print STDERR decode_message ($msg)->toString(1);
	}

	$timeout = defined $params{Timeout}  ? $params{Timeout} : $timeout;
	$sock->send ($msg);

	MESSAGE: while (1) {

		# read the next message
		my $envelope;
		my $rsp = read_message ($sock, $timeout, 'EnvelopeOut' => \$envelope);
		if ($params{Trace}) {
			print STDERR decode_message ($rsp)->toString(1);
		}

		push @{$nfy_queue}, $rsp;	

		# get message header fields
		my ($ver, $msg_type, $msg_len, $msg_id) = @$envelope;

		# ignore message if no status code
		next unless $msg_type == 63 || $lookup_mids_with_status{$msg_type};

		# grab the status code
		my $error_code = 0;
		if ($msg_type == 63) {

			# --- optimized search for ReaderExceptionEvent
				
			# skip the message header, reader event nfy header, Timestamp
			my $remainder = substr ($rsp, 10 + 4 + 12);

			SUBPARAM: while (length ($remainder)) {
				
				# unpack tlv header
				my ($hdr, $plen) = unpack ("nn", substr ($remainder, 0, 4));
				my $ptype = $hdr & 0x3FF;

				# not an error if ptype > 252 or < 247
				next MESSAGE if ($ptype > 252 || $ptype < 247);
				
				# move to next parameter if prior sibling
				if ($ptype != 252) {
					$remainder = substr ($remainder, $plen);
					next SUBPARAM;
				}
				
				# otherwise throw an exception
				if(!$params{BadNewsOK}) {								
					die "Encountered missing or unexpected status:\n" .
						decode_message ($rsp)->toString(1);
				}
				# the status error code is 16 bits, shifting the parameter type id 16 bits to distinguish the two
				$error_code = $ptype << 16;
				last SUBPARAM;
			}

			next MESSAGE if ($error_code == 0);

		} else {

			# assumes LLRPStatus is first parameter
			($error_code) = unpack ('n', substr ($rsp, 14, 2));
		}

		# handle the error as requested
		if ($error_code && !$params{BadNewsOK}) {
			die "Encountered missing or unexpected status:\n" .
				decode_message ($rsp)->toString(1);
		}

		# exit the loop	
		if (wantarray) {
			return ($error_code, @{$nfy_queue});
		} else {
			return $error_code;
		}
	}

	die "should never get here... programming error";
}


=back

=head1 AUTHOR

John R. Hogerhuis

Kunal Singh

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

1;
