//   Copyright (c) 2007 by the Board of Trustees of the University of Arkansas.
//
//   Licensed under the Apache License, Version 2.0 (the "License");
//   you may not use this file except in compliance with the License.
//   You may obtain a copy of the License at
//
//       http://www.apache.org/licenses/LICENSE-2.0
//
//   Unless required by applicable law or agreed to in writing, software
//   distributed under the License is distributed on an "AS IS" BASIS,
//   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied, 
//   including, without limitation, any warranties or conditions of TITLE, 
//   NON-INFRINGEMENT, MERCHANTABILITY, or FITNESS FOR A PARTICULAR PURPOSE.
//   See the License for the specific language governing permissions and
//   limitations under the License.

/*
 * Parameter.java
 *
 * Created on May 26, 2007, 12:39 PM
 */

package edu.uark.csce.llrp;
import java.io.*;
import java.util.HashMap;
import java.lang.reflect.*;

/**
 *
 * @author Joe Hoag
 */

// Builds a parameter name-to-number hash table
class paramnameHasher
{
  private HashMap<Integer, String> classHash = null;

  public paramnameHasher()
  {
    classHash = new HashMap<Integer, String>();

    // Add TV Parameters
    addBackwards("EPC96",13);
    addBackwards("ROSpecID",9);
    addBackwards("SpecIndex",14);
    addBackwards("InventoryParameterSpecID",10);
    addBackwards("AntennaID",1);
    addBackwards("PeakRSSI",6);
    addBackwards("ChannelIndex",7);
    addBackwards("FirstSeenTimestampUTC",2);
    addBackwards("FirstSeenTimestampUptime",3);
    addBackwards("LastSeenTimestampUTC",4);
    addBackwards("LastSeenTimestampUptime",5);
    addBackwards("TagSeenCount",8);
    addBackwards("ClientRequestOpSpecResult",15);
    addBackwards("AccessSpecID",16);
    addBackwards("OpSpecID",17);
    addBackwards("C1G2PC",12);
    addBackwards("C1G2CRC",11);
    addBackwards("C1G2SingulationDetails",18);
    
    // Add TLV parameters
    addBackwards("UTCTimestamp",128);
    addBackwards("Uptime",129);
    addBackwards("GeneralDeviceCapabilities",137);
    addBackwards("ReaderSensitivityTableEntry",139);
    addBackwards("PerAntennaAirProtocol",140);
    addBackwards("GPIOCapabilities",141);
    addBackwards("LLRPCapabilities",142);
    addBackwards("RegulatoryCapabilities",143);
    addBackwards("UHFBandCapabilities",144);
    addBackwards("TransmitPowerLevelTableEntry",145);
    addBackwards("FrequencyInformation",146);
    addBackwards("FrequencyHopTable",147);
    addBackwards("FixedFrequencyTable",148);
    addBackwards("PerAntennaReceiveSensitivityRange",149);
    addBackwards("ROSpec",177);
    addBackwards("ROBoundarySpec",178);
    addBackwards("ROSpecStartTrigger",179);
    addBackwards("PeriodicTriggerValue",180);
    addBackwards("GPITriggerValue",181);
    addBackwards("ROSpecStopTrigger",182);
    addBackwards("AISpec",183);
    addBackwards("AISpecStopTrigger",184);
    addBackwards("TagObservationTrigger",185);
    addBackwards("InventoryParameterSpec",186);
    addBackwards("RFSurveySpec",187);
    addBackwards("RFSurveySpecStopTrigger",188);
    addBackwards("AccessSpec",207);
    addBackwards("AccessSpecStopTrigger",208);
    addBackwards("AccessCommand",209);
    addBackwards("ClientRequestOpSpec",210);
    addBackwards("ClientRequestResponse",211);
    addBackwards("LLRPConfigurationStateValue",217);
    addBackwards("Identification",218);
    addBackwards("GPOWriteData",219);
    addBackwards("KeepaliveSpec",220);
    addBackwards("AntennaProperties",221);
    addBackwards("AntennaConfiguration",222);
    addBackwards("RFReceiver",223);
    addBackwards("RFTransmitter",224);
    addBackwards("GPIPortCurrentState",225);
    addBackwards("EventsAndReports",226);
    addBackwards("ROReportSpec",237);
    addBackwards("TagReportContentSelector",238);
    addBackwards("AccessReportSpec",239);
    addBackwards("TagReportData",240);
    addBackwards("EPCData",241);
    addBackwards("RFSurveyReportData",242);
    addBackwards("FrequencyRSSILevelEntry",243);
    addBackwards("ReadEventNotificationSpec",244);
    addBackwards("EventNotificationState",245);
    addBackwards("ReaderEventNotificationData",246);
    addBackwards("HoppingEvent",247);
    addBackwards("GPIEvent",248);
    addBackwards("ROSpecEvent",249);
    addBackwards("ReportBufferLevelWarningEvent",250);
    addBackwards("ReportBufferOverflowErrorEvent",251);
    addBackwards("ReaderExceptionEvent",252);
    addBackwards("RFSurveyEvent",253);
    addBackwards("AISpecEvent",254);
    addBackwards("AntennaEvent",255);
    addBackwards("ConnectionAttemptEvent",256);
    addBackwards("ConnectionCloseEvent",257);
    addBackwards("LLRPStatus",287);
    addBackwards("FieldError",288);
    addBackwards("ParameterError",289);
    addBackwards("Custom",1023);
    addBackwards("C1G2LLRPCapabilities",327);
    addBackwards("UHFC1G2RFModeTable",328);
    addBackwards("UHFC1G2RFModeTableEntry",329);
    addBackwards("C1G2InventoryCommand",330);
    addBackwards("C1G2Filter",331);
    addBackwards("C1G2TagInventoryMask",332);
    addBackwards("C1G2TagInventoryStateAwareFilterAction",333);
    addBackwards("C1G2TagInventoryStateUnawareFilterAction",334);
    addBackwards("C1G2RFControl",335);
    addBackwards("C1G2SingulationControl",336);
    addBackwards("C1G2TagInventoryStateAwareSingulationAction",337);
    addBackwards("C1G2TagSpec",338);
    addBackwards("C1G2TargetTag",339);
    addBackwards("C1G2Read",341);
    addBackwards("C1G2Write",342);
    addBackwards("C1G2Kill",343);
    addBackwards("C1G2Lock",344);
    addBackwards("C1G2LockPayload",345);
    addBackwards("C1G2BlockErase",346);
    addBackwards("C1G2BlockWrite",347);
    addBackwards("C1G2EPCMemorySelector",348);
    addBackwards("C1G2ReadOpSpecResult",349);
    addBackwards("C1G2WriteOpSpecResult",350);
    addBackwards("C1G2KillOpSpecResult",351);
    addBackwards("C1G2LockOpSpecResult",352);
    addBackwards("C1G2BlockEraseOpSpecResult",353);
    addBackwards("C1G2BlockWriteOpSpecResult",354);
  }

  // Need this to be able to cut-n-paste from the LLRP spec to
  // this file.
  private void addBackwards(String s, int i) {classHash.put(i,s);}

  public String getClassName(int i) {return classHash.get(i);}
}

public abstract class Parameter {
    
    public static paramnameHasher paramHash = new paramnameHasher();


    //
    // Parameter enumeration
    //
  
    // TV-parameters
    public static final int PT_EPC96=13;
    public static final int PT_ROSpecID=9;
    public static final int PT_SpecIndex=14;
    public static final int PT_InventoryParameterSpecID=10;
    public static final int PT_AntennaID=1;
    public static final int PT_PeakRSSI=6;
    public static final int PT_ChannelIndex=7;
    public static final int PT_FirstSeenTimestampUTC=2;
    public static final int PT_FirstSeenTimestampUptime=3;
    public static final int PT_LastSeenTimestampUTC=4;
    public static final int PT_LastSeenTimestampUptime=5;
    public static final int PT_TagSeenCount=8;
    public static final int PT_ClientRequestOpSpecResult=15;
    public static final int PT_AccessSpecID=16;
    public static final int PT_OpSpecID=17;
    public static final int PT_C1G2PC=12;
    public static final int PT_C1G2CRC=11;
    public static final int PT_C1G2SingulationDetails=18;

    // TLV-parameters
    public static final int PT_UTCTimestamp=128;
    public static final int PT_Uptime=129;
    public static final int PT_GeneralDeviceCapabilities=137;
    public static final int PT_ReaderSensitivityTableEntry=139;
    public static final int PT_PerAntennaAirProtocol=140;
    public static final int PT_GPIOCapabilities=141;
    public static final int PT_LLRPCapabilities=142;
    public static final int PT_RegulatoryCapabilities=143;
    public static final int PT_UHFBandCapabilities=144;
    public static final int PT_TransmitPowerLevelTableEntry=145;
    public static final int PT_FrequencyInformation=146;
    public static final int PT_FrequencyHopTable=147;
    public static final int PT_FixedFrequencyTable=148;
    public static final int PT_PerAntennaReceiveSensitivityRange=149;
    public static final int PT_ROSpec=177;
    public static final int PT_ROBoundarySpec=178;
    public static final int PT_ROSpecStartTrigger=179;
    public static final int PT_PeriodicTriggerValue=180;
    public static final int PT_GPITriggerValue=181;
    public static final int PT_ROSpecStopTrigger=182;
    public static final int PT_AISpec=183;
    public static final int PT_AISpecStopTrigger=184;
    public static final int PT_TagObservationTrigger=185;
    public static final int PT_InventoryParameterSpec=186;
    public static final int PT_RFSurveySpec=187;
    public static final int PT_RFSurveySpecStopTrigger=188;
    public static final int PT_AccessSpec=207;
    public static final int PT_AccessSpecStopTrigger=208;
    public static final int PT_AccessCommand=209;
    public static final int PT_ClientRequestOpSpec=210;
    public static final int PT_ClientRequestResponse=211;
    public static final int PT_LLRPConfigurationStateValue=217;
    public static final int PT_Identification=218;
    public static final int PT_GPOWriteData=219;
    public static final int PT_KeepaliveSpec=220;
    public static final int PT_AntennaProperties=221;
    public static final int PT_AntennaConfiguration=222;
    public static final int PT_RFReceiver=223;
    public static final int PT_RFTransmitter=224;
    public static final int PT_GPIPortCurrentState=225;
    public static final int PT_EventsAndReports=226;
    public static final int PT_ROReportSpec=237;
    public static final int PT_TagReportContentSelector=238;
    public static final int PT_AccessReportSpec=239;
    public static final int PT_TagReportData=240;
    public static final int PT_EPCData=241;
    public static final int PT_RFSurveyReportData=242;
    public static final int PT_FrequencyRSSILevelEntry=243;
    public static final int PT_ReadEventNotificationSpec=244;
    public static final int PT_EventNotificationState=245;
    public static final int PT_ReaderEventNotificationData=246;
    public static final int PT_HoppingEvent=247;
    public static final int PT_GPIEvent=248;
    public static final int PT_ROSpecEvent=249;
    public static final int PT_ReportBufferLevelWarningEvent=250;
    public static final int PT_ReportBufferOverflowErrorEvent=251;
    public static final int PT_ReaderExceptionEvent=252;
    public static final int PT_RFSurveyEvent=253;
    public static final int PT_AISpecEvent=254;
    public static final int PT_AntennaEvent=255;
    public static final int PT_ConnectionAttemptEvent=256;
    public static final int PT_ConnectionCloseEvent=257;
    public static final int PT_LLRPStatus=287;
    public static final int PT_FieldError=288;
    public static final int PT_ParameterError=289;
    public static final int PT_Custom=1023;
    public static final int PT_C1G2LLRPCapabilities=327;
    public static final int PT_UHFC1G2RFModeTable=328;
    public static final int PT_UHFC1G2RFModeTableEntry=329;
    public static final int PT_C1G2InventoryCommand=330;
    public static final int PT_C1G2Filter=331;
    public static final int PT_C1G2TagInventoryMask=332;
    public static final int PT_C1G2TagInventoryStateAwareFilterAction=333;
    public static final int PT_C1G2TagInventoryStateUnawareFilterAction=334;
    public static final int PT_C1G2RFControl=335;
    public static final int PT_C1G2SingulationControl=336;
    public static final int PT_C1G2TagInventoryStateAwareSingulationAction=337;
    public static final int PT_C1G2TagSpec=338;
    public static final int PT_C1G2TargetTag=339;
    public static final int PT_C1G2Read=341;
    public static final int PT_C1G2Write=342;
    public static final int PT_C1G2Kill=343;
    public static final int PT_C1G2Lock=344;
    public static final int PT_C1G2LockPayload=345;
    public static final int PT_C1G2BlockErase=346;
    public static final int PT_C1G2BlockWrite=347;
    public static final int PT_C1G2EPCMemorySelector=348;
    public static final int PT_C1G2ReadOpSpecResult=349;
    public static final int PT_C1G2WriteOpSpecResult=350;
    public static final int PT_C1G2KillOpSpecResult=351;
    public static final int PT_C1G2LockOpSpecResult=352;
    public static final int PT_C1G2BlockEraseOpSpecResult=353;
    public static final int PT_C1G2BlockWriteOpSpecResult=354;

    /** Creates a new instance of Parameter */
    public Parameter() {
    }
    
    public abstract int getParamType();
    public abstract void serializeBody(DataOutputStream out) throws IOException;
    public abstract void deserializeBody(DataInputStream in, int bytes) throws IOException;
    public abstract void toXML(StringBuffer sb, String indent);
    
    public abstract void serialize(DataOutputStream out) throws IOException;
    
    public static Parameter deserialize(DataInputStream in) throws IOException
    {
	int b1 = in.readUnsignedByte();
	int length = 0;
	int ptype = 0;
	Parameter rval = null;
	if((b1 & 0x80) != 0)
	{
	  // this is a TV parameter!
	  // just need to decode the type and read the body
	  ptype = b1 & 0x7F;
	}
	else
	{
	  // this is a TLV parameter
	  // Need to decode additional bytes for type and length
	  int b2 = in.readUnsignedByte();
	  ptype = (b1 << 8) | b2;
	  length = in.readShort() - 4;
	}

	String cname = paramHash.getClassName(ptype);
	if(cname == null)
	{
	  throw new IOException("Parameter.deserialize(): Unknown param type " + ptype);
	}

	// Properly prefix the name
	cname = "edu.uark.csce.llrp." + cname;

	try
	{
	  Class c = Class.forName(cname);
	  rval = (Parameter) c.newInstance();
	}
	catch(Exception e)
	{
	  throw new IOException("Parameter.deserialize(): Problem instantiating: " + e);
	}
	rval.deserializeBody(in, length);
	return rval;
    }

    public void show()
    {
	StringBuffer sb = new StringBuffer();
	toXML(sb,"");
        System.out.println(sb.toString());
    }
    
}
