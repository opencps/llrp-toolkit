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

my $binFile = "tmp_dx101_c.bin";
my $xmlFile = "tmp_dx101_c.xml";

my @combos = (
    {
        epcSize     => 0,
        reportSize  => 1,
        reportContent =>
        {
            Caption                         => 'Just EPC',
            EnableROSpecID                  => 'FALSE',
            EnableSpecIndex                 => 'FALSE',
            EnableInventoryParameterSpecID  => 'FALSE',
            EnableAntennaID                 => 'FALSE',
            EnablePeakRSSI                  => 'FALSE',
            EnableChannelIndex              => 'FALSE',
            EnableFirstSeenTimestampUTC     => 'FALSE',
            EnableFirstSeenTimestampUptime  => 'FALSE',
            EnableLastSeenTimestampUTC      => 'FALSE',
            EnableLastSeenTimestampUptime   => 'FALSE',
            EnableTagSeenCount              => 'FALSE',
            EnablePC                        => 'FALSE',
            EnableCRC                       => 'FALSE',
            EnableAccessSpecID              => 'FALSE',
            AccessCommandOpSpecResult       => 0,
        },
    },
    {
        epcSize     => 16,
        reportSize  => 1,
        reportContent =>
        {
            Caption                         => 'Just EPC',
            EnableROSpecID                  => 'FALSE',
            EnableSpecIndex                 => 'FALSE',
            EnableInventoryParameterSpecID  => 'FALSE',
            EnableAntennaID                 => 'FALSE',
            EnablePeakRSSI                  => 'FALSE',
            EnableChannelIndex              => 'FALSE',
            EnableFirstSeenTimestampUTC     => 'FALSE',
            EnableFirstSeenTimestampUptime  => 'FALSE',
            EnableLastSeenTimestampUTC      => 'FALSE',
            EnableLastSeenTimestampUptime   => 'FALSE',
            EnableTagSeenCount              => 'FALSE',
            EnablePC                        => 'FALSE',
            EnableCRC                       => 'FALSE',
            EnableAccessSpecID              => 'FALSE',
            AccessCommandOpSpecResult       => 0,
        },
    },
    {
        epcSize     => 64,
        reportSize  => 1,
        reportContent =>
        {
            Caption                         => 'Just EPC',
            EnableROSpecID                  => 'FALSE',
            EnableSpecIndex                 => 'FALSE',
            EnableInventoryParameterSpecID  => 'FALSE',
            EnableAntennaID                 => 'FALSE',
            EnablePeakRSSI                  => 'FALSE',
            EnableChannelIndex              => 'FALSE',
            EnableFirstSeenTimestampUTC     => 'FALSE',
            EnableFirstSeenTimestampUptime  => 'FALSE',
            EnableLastSeenTimestampUTC      => 'FALSE',
            EnableLastSeenTimestampUptime   => 'FALSE',
            EnableTagSeenCount              => 'FALSE',
            EnablePC                        => 'FALSE',
            EnableCRC                       => 'FALSE',
            EnableAccessSpecID              => 'FALSE',
            AccessCommandOpSpecResult       => 0,
        },
    },
    {
        epcSize     => 96,
        reportSize  => 1,
        reportContent =>
        {
            Caption                         => 'Just EPC',
            EnableROSpecID                  => 'FALSE',
            EnableSpecIndex                 => 'FALSE',
            EnableInventoryParameterSpecID  => 'FALSE',
            EnableAntennaID                 => 'FALSE',
            EnablePeakRSSI                  => 'FALSE',
            EnableChannelIndex              => 'FALSE',
            EnableFirstSeenTimestampUTC     => 'FALSE',
            EnableFirstSeenTimestampUptime  => 'FALSE',
            EnableLastSeenTimestampUTC      => 'FALSE',
            EnableLastSeenTimestampUptime   => 'FALSE',
            EnableTagSeenCount              => 'FALSE',
            EnablePC                        => 'FALSE',
            EnableCRC                       => 'FALSE',
            EnableAccessSpecID              => 'FALSE',
            AccessCommandOpSpecResult       => 0,
        },
    },
    {
        epcSize     => 96,
        reportSize  => 100,
        reportContent =>
        {
            Caption                         => 'Just EPC',
            EnableROSpecID                  => 'FALSE',
            EnableSpecIndex                 => 'FALSE',
            EnableInventoryParameterSpecID  => 'FALSE',
            EnableAntennaID                 => 'FALSE',
            EnablePeakRSSI                  => 'FALSE',
            EnableChannelIndex              => 'FALSE',
            EnableFirstSeenTimestampUTC     => 'FALSE',
            EnableFirstSeenTimestampUptime  => 'FALSE',
            EnableLastSeenTimestampUTC      => 'FALSE',
            EnableLastSeenTimestampUptime   => 'FALSE',
            EnableTagSeenCount              => 'FALSE',
            EnablePC                        => 'FALSE',
            EnableCRC                       => 'FALSE',
            EnableAccessSpecID              => 'FALSE',
            AccessCommandOpSpecResult       => 0,
        },
    },
    {
        epcSize     => 96,
        reportSize  => 10000,
        reportContent =>
        {
            Caption                         => 'Just EPC',
            EnableROSpecID                  => 'FALSE',
            EnableSpecIndex                 => 'FALSE',
            EnableInventoryParameterSpecID  => 'FALSE',
            EnableAntennaID                 => 'FALSE',
            EnablePeakRSSI                  => 'FALSE',
            EnableChannelIndex              => 'FALSE',
            EnableFirstSeenTimestampUTC     => 'FALSE',
            EnableFirstSeenTimestampUptime  => 'FALSE',
            EnableLastSeenTimestampUTC      => 'FALSE',
            EnableLastSeenTimestampUptime   => 'FALSE',
            EnableTagSeenCount              => 'FALSE',
            EnablePC                        => 'FALSE',
            EnableCRC                       => 'FALSE',
            EnableAccessSpecID              => 'FALSE',
            AccessCommandOpSpecResult       => 0,
        },
    },
    {
        epcSize     => 96,
        reportSize  => 10000,
        reportContent =>
        {
            Caption                         => 'Everything',
            EnableROSpecID                  => 'TRUE',
            EnableSpecIndex                 => 'TRUE',
            EnableInventoryParameterSpecID  => 'TRUE',
            EnableAntennaID                 => 'TRUE',
            EnablePeakRSSI                  => 'TRUE',
            EnableChannelIndex              => 'TRUE',
            EnableFirstSeenTimestampUTC     => 'TRUE',
            EnableFirstSeenTimestampUptime  => 'TRUE',
            EnableLastSeenTimestampUTC      => 'TRUE',
            EnableLastSeenTimestampUptime   => 'TRUE',
            EnableTagSeenCount              => 'TRUE',
            EnablePC                        => 'TRUE',
            EnableCRC                       => 'TRUE',
            EnableAccessSpecID              => 'TRUE',
            AccessCommandOpSpecResult       => 8,
        },
    },
    {
        epcSize     => 240,
        reportSize  => 1,
        reportContent =>
        {
            Caption                         => 'Just EPC',
            EnableROSpecID                  => 'FALSE',
            EnableSpecIndex                 => 'FALSE',
            EnableInventoryParameterSpecID  => 'FALSE',
            EnableAntennaID                 => 'FALSE',
            EnablePeakRSSI                  => 'FALSE',
            EnableChannelIndex              => 'FALSE',
            EnableFirstSeenTimestampUTC     => 'FALSE',
            EnableFirstSeenTimestampUptime  => 'FALSE',
            EnableLastSeenTimestampUTC      => 'FALSE',
            EnableLastSeenTimestampUptime   => 'FALSE',
            EnableTagSeenCount              => 'FALSE',
            EnablePC                        => 'FALSE',
            EnableCRC                       => 'FALSE',
            EnableAccessSpecID              => 'FALSE',
            AccessCommandOpSpecResult       => 0,
        },
    },
    {
        epcSize     => 512,
        reportSize  => 100,
        reportContent =>
        {
            Caption                         => 'Just EPC',
            EnableROSpecID                  => 'FALSE',
            EnableSpecIndex                 => 'FALSE',
            EnableInventoryParameterSpecID  => 'FALSE',
            EnableAntennaID                 => 'FALSE',
            EnablePeakRSSI                  => 'FALSE',
            EnableChannelIndex              => 'FALSE',
            EnableFirstSeenTimestampUTC     => 'FALSE',
            EnableFirstSeenTimestampUptime  => 'FALSE',
            EnableLastSeenTimestampUTC      => 'FALSE',
            EnableLastSeenTimestampUptime   => 'FALSE',
            EnableTagSeenCount              => 'FALSE',
            EnablePC                        => 'FALSE',
            EnableCRC                       => 'FALSE',
            EnableAccessSpecID              => 'FALSE',
            AccessCommandOpSpecResult       => 0,
        },
    },
    {
        epcSize     => 240,
        reportSize  => 10000,
        reportContent =>
        {
            Caption                         => 'Everything',
            EnableROSpecID                  => 'TRUE',
            EnableSpecIndex                 => 'TRUE',
            EnableInventoryParameterSpecID  => 'TRUE',
            EnableAntennaID                 => 'TRUE',
            EnablePeakRSSI                  => 'TRUE',
            EnableChannelIndex              => 'TRUE',
            EnableFirstSeenTimestampUTC     => 'TRUE',
            EnableFirstSeenTimestampUptime  => 'TRUE',
            EnableLastSeenTimestampUTC      => 'TRUE',
            EnableLastSeenTimestampUptime   => 'TRUE',
            EnableTagSeenCount              => 'TRUE',
            EnablePC                        => 'TRUE',
            EnableCRC                       => 'TRUE',
            EnableAccessSpecID              => 'TRUE',
            AccessCommandOpSpecResult       => 4,
        },
    },
    {
        epcSize     => 512,
        reportSize  => 2500,
        reportContent =>
        {
            Caption                         => 'Everything',
            EnableROSpecID                  => 'TRUE',
            EnableSpecIndex                 => 'TRUE',
            EnableInventoryParameterSpecID  => 'TRUE',
            EnableAntennaID                 => 'TRUE',
            EnablePeakRSSI                  => 'TRUE',
            EnableChannelIndex              => 'TRUE',
            EnableFirstSeenTimestampUTC     => 'TRUE',
            EnableFirstSeenTimestampUptime  => 'TRUE',
            EnableLastSeenTimestampUTC      => 'TRUE',
            EnableLastSeenTimestampUptime   => 'TRUE',
            EnableTagSeenCount              => 'TRUE',
            EnablePC                        => 'TRUE',
            EnableCRC                       => 'TRUE',
            EnableAccessSpecID              => 'TRUE',
            AccessCommandOpSpecResult       => 16,
        },
    },
);



open(XML, ">$xmlFile") or die("Could not open file $xmlFile for test generation output");
open (BIN, ">$binFile") or die("Could not open file $binFile for test generation output");


my $testXMLHdr = <<OUTXMLHDR;
<ps:packetSequence
  xmlns='http://www.llrp.org/ltk/schema/core/encoding/xml/1.0'
  xmlns:xsi='http://www.w3.org/2001/XMLSchema-instance'
  xmlns:ps='http://www.llrp.org/ltk/schema/testing/encoding/xml/0.6'
  xsi:schemaLocation='http://www.llrp.org/ltk/schema/core/encoding/xml/1.0
                      http://www.llrp.org/ltk/schema/core/encoding/xml/1.0/llrp.xsd'>

OUTXMLHDR

print XML $testXMLHdr;

my $messageID = 10;
foreach my $combo (@combos) {
    my $epcSize = $$combo{"epcSize"};
    my $reportSize = $$combo{"reportSize"};
    my $reportContent = $$combo{"reportContent"};
    my $caption = $$reportContent{"Caption"};

    print "EPC:$epcSize  ReportSize:$reportSize  Report:$caption\n";

    my $testXML = "<RO_ACCESS_REPORT Version=\"1\" MessageID=\"$messageID\">\n";

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
        if($$reportContent{"EnablePeakRSSI"} eq 'TRUE') {
            my $rssi = int(rand(256)) - 128;
            $testXML .="    <PeakRSSI>\n";
            $testXML .="      <PeakRSSI>$rssi</PeakRSSI>\n";
            $testXML .="    </PeakRSSI>\n";
        }
        if($$reportContent{"EnableChannelIndex"} eq 'TRUE') {
            my $id = int(rand(65536));
            $testXML .="    <ChannelIndex>\n";
            $testXML .="      <ChannelIndex>$id</ChannelIndex>\n";
            $testXML .="    </ChannelIndex>\n";
        }
        if($$reportContent{"EnableFirstSeenTimestampUTC"} eq 'TRUE') {
            my $dd = localtime;
            my $time = $dd->datetime;
            $testXML .="    <FirstSeenTimestampUTC>\n";
            $testXML .="      <Microseconds>$time</Microseconds>\n";
            $testXML .="    </FirstSeenTimestampUTC>\n"
        }
        if($$reportContent{"EnableFirstSeenTimestampUptime"} eq 'TRUE') {
            my $time = "";
            for my $i (0 .. (int(rand(17)) + 1)) { $time .= int(rand(10)); }
            $testXML .="    <FirstSeenTimestampUptime>\n";
            $testXML .="      <Microseconds>$time</Microseconds>\n";
            $testXML .="    </FirstSeenTimestampUptime>\n";
        }
        if($$reportContent{"EnableLastSeenTimestampUTC"} eq 'TRUE') {
            my $dd = localtime;
            my $time = $dd->datetime;
            $testXML .="    <LastSeenTimestampUTC>\n";
            $testXML .="      <Microseconds>$time</Microseconds>\n";
            $testXML .="    </LastSeenTimestampUTC>\n"
        }
        if($$reportContent{"EnableLastSeenTimestampUptime"} eq 'TRUE') {
            my $time = "";
            for my $i (0 .. (int(rand(17)) + 1)) { $time .= int(rand(10)); }
            $testXML .="    <LastSeenTimestampUptime>\n";
            $testXML .="      <Microseconds>$time</Microseconds>\n";
            $testXML .="    </LastSeenTimestampUptime>\n";
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
        if($$reportContent{"EnableAccessSpecID"} eq 'TRUE') {
            my $id = int(rand(4294967295)) + 1; #zero is illegal
            $testXML .="    <AccessSpecID>\n";
            $testXML .="      <AccessSpecID>$id</AccessSpecID>\n";
            $testXML .="    </AccessSpecID>\n";
        }
        if($$reportContent{"AccessCommandOpSpecResult"} > 0) {
            my $ops = int(rand($$reportContent{"AccessCommandOpSpecResult"})) + 1;
            for my $op (0 .. $ops) {
                my $oper = int(rand(6));
                if (0 == $oper) {
                    my @results = ( 'Success', 'Nonspecific_Tag_Error',
                                    'No_Response_From_Tag', 'Nonspecific_Reader_Error' );
                    my $result = $results[int(rand(scalar @results))];
                    my $id = int(rand(65536));
                    $testXML .="    <C1G2ReadOpSpecResult>\n";
                    $testXML .="      <Result>$result</Result>\n";
                    $testXML .="      <OpSpecID>$id</OpSpecID>\n";
                    $testXML .="      <ReadData>";
                    if ('Success' eq $result) {
                        for my $word (0 .. int(rand(32)) + 1) {
                            $testXML .= sprintf("%04X ", int(rand(65535)));
                        }
                    }
                    $testXML .="</ReadData>\n";
                    $testXML .="    </C1G2ReadOpSpecResult>\n";
                }
                elsif (1 == $oper) {
                    my @results = ( 'Success', 'Tag_Memory_Overrun_Error',
                                    'Tag_Memory_Locked_Error', 'Insufficient_Power',
                                    'Nonspecific_Tag_Error', 'No_Response_From_Tag',
                                    'Nonspecific_Reader_Error' );
                    my $result = $results[int(rand(scalar @results))];
                    my $id = int(rand(65536));
                    my $words = int(rand(65535));
                    $testXML .="    <C1G2WriteOpSpecResult>\n";
                    $testXML .="      <Result>$result</Result>\n";
                    $testXML .="      <OpSpecID>$id</OpSpecID>\n";
                    $testXML .="      <NumWordsWritten>$words</NumWordsWritten>\n";
                    $testXML .="    </C1G2WriteOpSpecResult>\n";
                }
                elsif (2 == $oper) {
                    my @results = ( 'Success', 'Zero_Kill_Password_Error',
                                    'Insufficient_Power', 'Nonspecific_Tag_Error',
                                    'No_Response_From_Tag', 'Nonspecific_Reader_Error' );
                    my $result = $results[int(rand(scalar @results))];
                    my $id = int(rand(65536));
                    $testXML .="    <C1G2KillOpSpecResult>\n";
                    $testXML .="      <Result>$result</Result>\n";
                    $testXML .="      <OpSpecID>$id</OpSpecID>\n";
                    $testXML .="    </C1G2KillOpSpecResult>\n";
                }
                elsif (3 == $oper) {
                    my @results = ( 'Success', 'Insufficient_Power',
                                    'Nonspecific_Tag_Error', 'No_Response_From_Tag',
                                    'Nonspecific_Reader_Error' );
                    my $result = $results[int(rand(scalar @results))];
                    my $id = int(rand(65536));
                    $testXML .="    <C1G2LockOpSpecResult>\n";
                    $testXML .="      <Result>$result</Result>\n";
                    $testXML .="      <OpSpecID>$id</OpSpecID>\n";
                    $testXML .="    </C1G2LockOpSpecResult>\n";
                }
                elsif (4 == $oper) {
                    my @results = ( 'Success', 'Tag_Memory_Overrun_Error',
                                    'Tag_Memory_Locked_Error', 'Insufficient_Power',
                                    'Nonspecific_Tag_Error', 'No_Response_From_Tag',
                                    'Nonspecific_Reader_Error' );
                    my $result = $results[int(rand(scalar @results))];
                    my $id = int(rand(65536));
                    $testXML .="    <C1G2BlockEraseOpSpecResult>\n";
                    $testXML .="      <Result>$result</Result>\n";
                    $testXML .="      <OpSpecID>$id</OpSpecID>\n";
                    $testXML .="    </C1G2BlockEraseOpSpecResult>\n";
                }
                elsif (5 == $oper) {
                    my @results = ( 'Success', 'Tag_Memory_Overrun_Error',
                                    'Tag_Memory_Locked_Error', 'Insufficient_Power',
                                    'Nonspecific_Tag_Error', 'No_Response_From_Tag',
                                    'Nonspecific_Reader_Error' );
                    my $result = $results[int(rand(scalar @results))];
                    my $id = int(rand(65536));
                    my $words = int(rand(65535));
                    $testXML .="    <C1G2BlockWriteOpSpecResult>\n";
                    $testXML .="      <Result>$result</Result>\n";
                    $testXML .="      <OpSpecID>$id</OpSpecID>\n";
                    $testXML .="      <NumWordsWritten>$words</NumWordsWritten>\n";
                    $testXML .="    </C1G2BlockWriteOpSpecResult>\n";
                }
            }
        }
        $testXML .= "  </TagReportData>\n";
    }

    $testXML .= "</RO_ACCESS_REPORT>\n";

    my $bin = encode_message($testXML);
    my $msgSize = length($bin);
    print "Encoded Message is $msgSize bytes\n";
    print XML $testXML;
    print BIN $bin;
}

print XML "\n</ps:packetSequence>\n";

close(BIN);
close(XML);

