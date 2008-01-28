#!/usr/bin/env perl
# vim60:fdm=marker:

#############################################################################
#                                                                           #
#  Copyright 2008 Impinj, Inc.                                              #
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
# This is a simple example of using of the Perl-LLRP document-oriented API
#
# Generate Perl "POD" documentation using the pod generator of your choice
#
#############################################################################

use lib "..";

use Data::Dumper;
use Date::Parse;
use Data::HexDump;

use Time::HiRes qw(time);

use XML::LibXML;

use RFID::LLRP::Builder qw(encode_message decode_message);
use RFID::LLRP::Link qw(
	reader_connect
	reader_disconnect
	),
        transact => {QualifyCore => 0},
	monitor => {QualifyCore => 0};

use RFID::LLRP::Helper qw(delete_rospecs are_identical delete_access_specs
              factory_default cleanup_all compare_subtrees);

use diagnostics;

use Text::Template qw(fill_in_file fill_in_string);

while (@ARGV and $ARGV[0] =~ /^-/) {
    $_ = shift;

    last if /^--$/;
    if (/^-n=(.*)/) { $reader_name = $1 }
}

%DELIM = ('DELIMITERS' => ['[[_', '_]]']);
sub expand {
    my $str = shift;
    my $result = Text::Template::fill_in_string ($str, HASH => {@_}, %DELIM);
}
sub fexpand {
    my $fname = shift;
    my $result = Text::Template::fill_in_file ($fname, HASH => {@_}, %DELIM);
    $result =~ s/\n\s+\n/\n/sg;
    return $result;
}
sub get_msg_name {
    return $_[0]->getDocumentElement->nodeName;
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

sub uniq {
	my %seen;
	foreach (@_) {
		$seen{$_}++;
	}
	return keys %seen;
}


sub test_usecase_survey_tags {

	my $trace = 0;
	my ($sock) = reader_connect ($reader_name);
	$sock || die "failed to connect";
	cleanup_all ($sock);
	my $trid ="<ROSpecID>1</ROSpecID>";
	my $cap;

	# Configure the reader #{{{
	$cap = transact ($sock, q{
	<SET_READER_CONFIG MessageID="0">
	  <ResetToFactoryDefault>true</ResetToFactoryDefault>
	  <ReaderEventNotificationSpec>
	    <EventNotificationState>
	      <EventType>Upon_Hopping_To_Next_Channel</EventType>
	      <NotificationState>true</NotificationState>
	    </EventNotificationState>
	    <EventNotificationState>
	      <EventType>GPI_Event</EventType>
	      <NotificationState>false</NotificationState>
	    </EventNotificationState>
	    <EventNotificationState>
	      <EventType>ROSpec_Event</EventType>
	      <NotificationState>true</NotificationState>
	    </EventNotificationState>
	    <EventNotificationState>
	      <EventType>Report_Buffer_Fill_Warning</EventType>
	      <NotificationState>false</NotificationState>
	    </EventNotificationState>
	    <EventNotificationState>
	      <EventType>Reader_Exception_Event</EventType>
	      <NotificationState>true</NotificationState>
	    </EventNotificationState>
	    <EventNotificationState>
	      <EventType>AISpec_Event</EventType>
	      <NotificationState>false</NotificationState>
	    </EventNotificationState>
	    <EventNotificationState>
	      <EventType>AISpec_Event_With_Details</EventType>
	      <NotificationState>false</NotificationState>
	    </EventNotificationState>
	    <EventNotificationState>
	      <EventType>Antenna_Event</EventType>
	      <NotificationState>false</NotificationState>
	    </EventNotificationState>
	  </ReaderEventNotificationSpec>
	  <AntennaConfiguration>
	    <AntennaID>0</AntennaID>
	    <C1G2InventoryCommand>
	      <TagInventoryStateAware>false</TagInventoryStateAware>
	      <C1G2RFControl>
		<ModeIndex>0</ModeIndex>
		<Tari>0</Tari>
	      </C1G2RFControl>
	      <C1G2SingulationControl>
		<Session>1</Session>
		<TagPopulation>100</TagPopulation>
		<TagTransitTime>3000</TagTransitTime>
	      </C1G2SingulationControl>
	    </C1G2InventoryCommand>
	  </AntennaConfiguration>
	</SET_READER_CONFIG>

	}, Trace => $trace); #}}}

	#  add ROSpec to perform an inventory until 2 attempts to see all tags {{{
	$cap = transact ($sock, expand (q{
		<!--
		[[_

		if (!@antennas) {
		  @antennas = (0);
		}  else {
		  @antennas = map { $_ + 1 } @antennas;
		}
		  
		_]]
		-->
		<ADD_ROSPEC MessageID="0">
		  <ROSpec>
		    <ROSpecID>[[_ $rid _]]</ROSpecID>
		    <Priority>0</Priority>
		    <CurrentState>Disabled</CurrentState>
		    <ROBoundarySpec>
		      <ROSpecStartTrigger>
			<ROSpecStartTriggerType>Null</ROSpecStartTriggerType>
		      </ROSpecStartTrigger>
		      <ROSpecStopTrigger>
			<ROSpecStopTriggerType>Null</ROSpecStopTriggerType>
			<DurationTriggerValue>0</DurationTriggerValue>
		      </ROSpecStopTrigger>
		    </ROBoundarySpec>
		    <AISpec>
		      <AntennaIDs>[[_ join (' ', @antennas) _]]</AntennaIDs>
		      <AISpecStopTrigger>
			<AISpecStopTriggerType>Tag_Observation</AISpecStopTriggerType>
			<DurationTrigger>0</DurationTrigger>
			<TagObservationTrigger>
			  <TriggerType>N_Attempts_To_See_All_Tags_In_FOV_Or_Timeout</TriggerType>
			  <NumberOfTags>0</NumberOfTags>
			  <NumberOfAttempts>3</NumberOfAttempts>
			  <T>0</T>
			  <Timeout>10000</Timeout>
			</TagObservationTrigger>
		      </AISpecStopTrigger>
		      <InventoryParameterSpec>
			<InventoryParameterSpecID>1</InventoryParameterSpecID>
			<ProtocolID>EPCGlobalClass1Gen2</ProtocolID>
		      </InventoryParameterSpec>
		    </AISpec>
		    <ROReportSpec>
		      <ROReportTrigger>Upon_N_Tags_Or_End_Of_AISpec</ROReportTrigger>
		      <N>0</N>
		      <TagReportContentSelector>
			<EnableROSpecID>false</EnableROSpecID>
			<EnableSpecIndex>false</EnableSpecIndex>
			<EnableInventoryParameterSpecID>false</EnableInventoryParameterSpecID>
			<EnableAntennaID>true</EnableAntennaID>
			<EnableChannelIndex>false</EnableChannelIndex>
			<EnablePeakRSSI>true</EnablePeakRSSI>
			<EnableFirstSeenTimestamp>true</EnableFirstSeenTimestamp>
			<EnableLastSeenTimestamp>true</EnableLastSeenTimestamp>
			<EnableTagSeenCount>true</EnableTagSeenCount>
			<EnableAccessSpecID>false</EnableAccessSpecID>
			<C1G2EPCMemorySelector>
			  <EnableCRC>1</EnableCRC>
			  <EnablePCBits>1</EnablePCBits>
			</C1G2EPCMemorySelector>
		      </TagReportContentSelector>
		    </ROReportSpec>
		  </ROSpec>
		</ADD_ROSPEC>
		}, 'rid' => 1),
		Trace => $trace
	);#}}}

	# start and enable the ROSpec
	foreach $op ('ENABLE', 'START') {
		transact ($sock, "<${op}_ROSPEC MessageID=\"0\">$trid</${op}_ROSPEC>",
			Trace => $trace
		);
	}

	# monitor for final report
	my (@ntf) = monitor ($sock, Timeout => 10, Trace => $trace,
		ReturnUpon => [ '/RO_ACCESS_REPORT' ]
	);

	# extract EPCS, uniq, show EPCs and count.
	my @epcs = uniq get_values ($ntf[$#ntf], '//EPC');

	# print the unique tag count
	print "Observed ", scalar (@epcs), " total unique tags\n\n";

	# print out all tags
        print "Unique tags:\n";
        foreach $epc (@epcs) {
                my @groups;
                push @groups, substr $epc, 0, 4, '' while length $epc;
                my $fmtd_epc = join ('-', @groups);
                print $fmtd_epc, "\n";
        }

	reader_disconnect ($sock);
}

test_usecase_survey_tags;
