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
# This is a simple example of using of the Perl-LLRP Builder.pm
#
# Generate Perl "POD" documentation using the pod generator of your choice
#
#############################################################################

use lib "..";


use XML::LibXML;

use RFID::LLRP::Builder qw(encode_message decode_message);

use RFID::LLRP::Helper qw(are_identical);

use diagnostics;

# LLRP ADD_ROSPEC in binary form
$bmsg = "\x{04}\x{14}\x{00}\x{00}\x{00}\x{61}\x{00}\x{00}" .
	"\x{00}\x{00}\x{00}\x{b1}\x{00}\x{57}\x{00}\x{00}" .
	"\x{00}\x{01}\x{00}\x{00}\x{00}\x{b2}\x{00}\x{12}" .
	"\x{00}\x{b3}\x{00}\x{05}\x{00}\x{00}\x{b6}\x{00}" .
	"\x{09}\x{00}\x{00}\x{00}\x{00}\x{00}\x{00}\x{b7}" .
	"\x{00}\x{2e}\x{00}\x{04}\x{00}\x{01}\x{00}\x{02}" .
	"\x{00}\x{03}\x{00}\x{04}\x{00}\x{b8}\x{00}\x{19}" .
	"\x{03}\x{00}\x{00}\x{75}\x{30}\x{00}\x{b9}\x{00}" .
	"\x{10}\x{01}\x{00}\x{00}\x{64}\x{00}\x{00}\x{27}" .
	"\x{10}\x{00}\x{00}\x{75}\x{30}\x{00}\x{ba}\x{00}" .
	"\x{07}\x{00}\x{01}\x{01}\x{00}\x{ed}\x{00}\x{0d}" .
	"\x{01}\x{01}\x{f4}\x{00}\x{ee}\x{00}\x{06}\x{17}" .
	"\x{c0}";


sub test_encode_decode {

	my $msg_xml = q{
<ADD_ROSPEC Version="1" MessageID="0">
  <ROSpec>
    <ROSpecID>1</ROSpecID>
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
      <AntennaIDs>1 2 3 4</AntennaIDs>
      <AISpecStopTrigger>
	<AISpecStopTriggerType>Tag_Observation</AISpecStopTriggerType>
	<DurationTrigger>30000</DurationTrigger>
	<TagObservationTrigger>
	  <TriggerType>Upon_Seeing_No_More_New_Tags_For_Tms_Or_Timeout</TriggerType>
	  <NumberOfTags>100</NumberOfTags>
	  <NumberOfAttempts>0</NumberOfAttempts>
	  <T>10000</T>
	  <Timeout>30000</Timeout>
	</TagObservationTrigger>
      </AISpecStopTrigger>
      <InventoryParameterSpec>
	<InventoryParameterSpecID>1</InventoryParameterSpecID>
	<ProtocolID>EPCGlobalClass1Gen2</ProtocolID>
      </InventoryParameterSpec>
    </AISpec>
    <ROReportSpec>
      <ROReportTrigger>Upon_N_Tags_Or_End_Of_AISpec</ROReportTrigger>
      <N>500</N>
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
	<EnableAccessSpecID>true</EnableAccessSpecID>
      </TagReportContentSelector>
    </ROReportSpec>
  </ROSpec>
</ADD_ROSPEC>
};

	my $bexmsg = encode_message $msg_xml;

	# check the binary message against the canned one
	if ($bmsg = $bexmsg) {
		print "The canned message and the encoded message are identical\n"
	} else {
		die "Error: the canned binary message and the encode message do not match"
	}

	# decode the canned message
	my $decoded = decode_message ($bmsg);

	print "Message encoded:\n$msg_xml\n\n";
	print "Decoded canned message:\n" . $decoded->toString(1) . "\n\n";

}

test_encode_decode;
