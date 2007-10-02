#!/usr/bin/env perl
# vim60:fdm=marker:

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
# This is a test script to generate test packets for the dx101 test cases
#
# Generate Perl "POD" documentation using the pod generator of your choice
#
#############################################################################

use lib "../../LTKPerl";
use strict;
use XML::LibXML;
use Time::Piece;
use RFID::LLRP::Builder qw(encode_message decode_message);
use POSIX qw(ceil floor);
use RFID::LLRP::Helper qw(are_identical);
use diagnostics;

my $binFile = "dx101_c.bin";
my $xmlFile = "dx101_c.xml";

my @epcSizes = (#1,
	        #8,
		#9,
		#64,
		96,
		#512
		);
my @reportSizes = (#1,
		   #100,
		   10000);
my @reportContents = (	
#	{EnableROSpecID=> 'FALSE', EnableSpecIndex=>'FALSE', EnableInventoryParameterSpecID=>'FALSE', EnableAntennaID=>'FALSE', EnableFirstSeenTimestampUTC=>'FALSE', EnableLastSeenTimestampUTC=>'FALSE', EnableTagSeenCount=>'FALSE', EnablePC=>'FALSE', EnableCRC=>'FALSE'},
#	{EnableROSpecID=> 'TRUE', EnableSpecIndex=>'FALSE', EnableInventoryParameterSpecID=>'FALSE', EnableAntennaID=>'FALSE', EnableFirstSeenTimestampUTC=>'FALSE', EnableLastSeenTimestampUTC=>'FALSE', EnableTagSeenCount=>'FALSE', EnablePC=>'FALSE', EnableCRC=>'FALSE'},
#	{EnableROSpecID=> 'TRUE', EnableSpecIndex=>'TRUE', EnableInventoryParameterSpecID=>'FALSE', EnableAntennaID=>'FALSE', EnableFirstSeenTimestampUTC=>'FALSE', EnableLastSeenTimestampUTC=>'FALSE', EnableTagSeenCount=>'FALSE', EnablePC=>'FALSE', EnableCRC=>'FALSE'},
#	{EnableROSpecID=> 'TRUE', EnableSpecIndex=>'TRUE', EnableInventoryParameterSpecID=>'TRUE', EnableAntennaID=>'FALSE', EnableFirstSeenTimestampUTC=>'FALSE', EnableLastSeenTimestampUTC=>'FALSE', EnableTagSeenCount=>'FALSE', EnablePC=>'FALSE', EnableCRC=>'FALSE'},
#	{EnableROSpecID=> 'TRUE', EnableSpecIndex=>'TRUE', EnableInventoryParameterSpecID=>'TRUE', EnableAntennaID=>'TRUE', EnableFirstSeenTimestampUTC=>'FALSE', EnableLastSeenTimestampUTC=>'FALSE', EnableTagSeenCount=>'FALSE', EnablePC=>'FALSE', EnableCRC=>'FALSE'},
#	{EnableROSpecID=> 'TRUE', EnableSpecIndex=>'TRUE', EnableInventoryParameterSpecID=>'TRUE', EnableAntennaID=>'TRUE', EnableFirstSeenTimestampUTC=>'TRUE', EnableLastSeenTimestampUTC=>'FALSE', EnableTagSeenCount=>'FALSE', EnablePC=>'FALSE', EnableCRC=>'FALSE'},
#	{EnableROSpecID=> 'TRUE', EnableSpecIndex=>'TRUE', EnableInventoryParameterSpecID=>'TRUE', EnableAntennaID=>'TRUE', EnableFirstSeenTimestampUTC=>'TRUE', EnableLastSeenTimestampUTC=>'TRUE', EnableTagSeenCount=>'FALSE', EnablePC=>'FALSE', EnableCRC=>'FALSE'},
#	{EnableROSpecID=> 'TRUE', EnableSpecIndex=>'TRUE', EnableInventoryParameterSpecID=>'TRUE', EnableAntennaID=>'TRUE', EnableFirstSeenTimestampUTC=>'TRUE', EnableLastSeenTimestampUTC=>'TRUE', EnableTagSeenCount=>'TRUE', EnablePC=>'FALSE', EnableCRC=>'FALSE'},
#	{EnableROSpecID=> 'TRUE', EnableSpecIndex=>'TRUE', EnableInventoryParameterSpecID=>'TRUE', EnableAntennaID=>'TRUE', EnableFirstSeenTimestampUTC=>'TRUE', EnableLastSeenTimestampUTC=>'TRUE', EnableTagSeenCount=>'TRUE', EnablePC=>'TRUE', EnableCRC=>'FALSE'},
	{EnableROSpecID=> 'TRUE', EnableSpecIndex=>'TRUE', EnableInventoryParameterSpecID=>'TRUE', EnableAntennaID=>'TRUE', EnableFirstSeenTimestampUTC=>'TRUE', EnableLastSeenTimestampUTC=>'TRUE', EnableTagSeenCount=>'TRUE', EnablePC=>'TRUE', EnableCRC=>'TRUE'},
);


open(XML, ">$xmlFile") or die("Could not open file $xmlFile for test generation output");
open (BIN, ">$binFile") or die("Could not open file $binFile for test generation output");


my $testXMLHdr = <<OUTXMLHDR;
<ps:packetSequence  xmlns="http://www.llrp.org/ltk/schema/core/encoding/xml/0.9"
  		    xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
		    xmlns:ps="http://www.llrp.org/ltk/schema/testing/encoding/xml/0.5"
		     xsi:schemaLocation="http://www.llrp.org/ltk/schema/core/encoding/xml/0.9 http://www.llrp.org/ltk/schema/core/encoding/xml/0.9/LLRP.xsd
					 http://www.llrp.org/ltk/schema/testing/encoding/xml/0.5 ../../Tests/Definitions/llrpSequence.xsd">
OUTXMLHDR

print XML $testXMLHdr; 

my $messageID = 0;
foreach my $epcSize (@epcSizes) {
	print "\nEPC $epcSize";
	foreach my $reportSize (@reportSizes) {
		print "\nReportSize $reportSize\n";
	       foreach my $reportContent (@reportContents) {
		       print ".";

		       my $testXML = "<RO_ACCESS_REPORT Version=\"1\" MessageID=\"$messageID\">";

		       $messageID++;
		       my $tagCnt = 0;
		       while ($tagCnt++ < $reportSize) {

				# build an EPC 
			        my $rand_hex = join "", map { unpack "H2", chr(rand(256)) } 1..ceil($epcSize/8);
				
			        $testXML .= "  <TagReportData>\n";
			        if ($epcSize == 96) {
			            $testXML .= "    <EPC_96>\n";
				    $testXML .= "      <EPC>$rand_hex</EPC>\n";
			            $testXML .= "    </EPC_96>\n";
			        } else {
				    $testXML .= "    <EPCData>\n";
				    $testXML .= "      <EPC Count=\"$epcSize\">$rand_hex</EPC>\n";
				    $testXML .= "    </EPCData>\n";
			        }

				if($$reportContent{"EnableROSpecID"} eq 'TRUE') {
				    my $id = int(rand(4294967295)) + 1; #zero is illegal
				    $testXML .="    <ROSpecID>\n";
				    $testXML .="      <ROSpecID>$id</ROSpecID>\n";
				    $testXML .="    </ROSpecID>\n";
				}
				if($$reportContent{"EnableSpecIndex"} eq 'TRUE') {
				    my $id = int(rand(65536));
				    $testXML .="    <SpecIndex>\n";
				    $testXML .="      <SpecIndex>$id</SpecIndex>\n";
				    $testXML .="    </SpecIndex>\n";
				}
				if($$reportContent{"EnableInventoryParameterSpecID"} eq 'TRUE') {
				    my $id = int(rand(65536));
				    $testXML .="    <InventoryParameterSpecID>\n";
				    $testXML .="      <InventoryParameterSpecID>$id</InventoryParameterSpecID>\n";
				    $testXML .="    </InventoryParameterSpecID>\n"			
		     	        }
				if($$reportContent{"EnableAntennaID"} eq 'TRUE') {
				    my $id = int(rand(65536));
				    $testXML .="    <AntennaID>\n";
				    $testXML .="      <AntennaID>$id</AntennaID>\n";
				    $testXML .="    </AntennaID>\n";
			        }	
				if($$reportContent{"EnableFirstSeenTimestampUTC"} eq 'TRUE') {
				    my $dd = localtime;
				    my $time = $dd->datetime;
				    $testXML .="    <FirstSeenTimestampUTC>\n";
				    $testXML .="      <Microseconds>$time</Microseconds>\n";
				    $testXML .="    </FirstSeenTimestampUTC>\n"
			        }
				if($$reportContent{"EnableLastSeenTimestampUTC"} eq 'TRUE') {
				    my $dd = localtime;
				    my $time = $dd->datetime;
				    $testXML .="    <LastSeenTimestampUTC>\n";
				    $testXML .="      <Microseconds>$time</Microseconds>\n";
				    $testXML .="    </LastSeenTimestampUTC>\n"
			        }
				if($$reportContent{"EnableTagSeenCount"} eq 'TRUE') {
				    my $id = int(rand(65536));
				    $testXML .="    <TagSeenCount>\n";
				    $testXML .="      <TagCount>$id</TagCount>\n";
				    $testXML .="    </TagSeenCount>\n";
			        }				
				if($$reportContent{"EnablePC"} eq 'TRUE') {
				    my $id = int(rand(65536));
				    $testXML .="    <C1G2_PC>\n";
				    $testXML .="      <PC_Bits>$id</PC_Bits>\n";
				    $testXML .="    </C1G2_PC>\n";
				}
				if($$reportContent{"EnableCRC"} eq 'TRUE') {
				    my $id = int(rand(65536));
				    $testXML .="    <C1G2_CRC>\n";
				    $testXML .="      <CRC>$id</CRC>\n";
				    $testXML .="    </C1G2_CRC>\n";			
			    	}
			       
			       $testXML .= "  </TagReportData>\n";
		       } 

		       $testXML .= "</RO_ACCESS_REPORT>\n";

		       my $bin = encode_message($testXML);
		       my $msgSize = length($bin);
		       print "Encoded Message is $msgSize bytes\n"; 
#		       if($msgSize == 44) {print $testXML; }
		       print XML $testXML;
		       print BIN $bin;
	       }
       }
}       

print XML "</ps:packetSequence>\n";
# my $decoded = decode_message ($bmsg);
close(BIN);
close(XML);

