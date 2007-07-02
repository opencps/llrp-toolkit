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
 * File: TagReportData.java
 * auto-generated Wed Jun 06 13:35:06 CDT 2007
 *
 * Manually tweaked by Joe Hoag
 */
package edu.uark.csce.llrp;
import java.io.*;
import java.util.ArrayList;
public class TagReportData extends TLVParameter
{
  // member variables
  //private EPCData _EPCDataParam = null;
  private Parameter _EPCDataParam = null;
  private ROSpecID _ROSpecIDParam = null;
  private SpecIndex _SpecIndexParam = null;
  private InventoryParameterSpecID _InventoryParameterSpecIDParam = null;
  private AntennaID _AntennaIDParam = null;
  private PeakRSSI _PeakRSSIParam = null;
  private ChannelIndex _ChannelIndexParam = null;
  private FirstSeenTimestampUTC _FirstSeenTimestampUTCParam = null;
  private FirstSeenTimestampUptime _FirstSeenTimestampUptimeParam = null;
  private LastSeenTimestampUTC _LastSeenTimestampUTCParam = null;
  private LastSeenTimestampUptime _LastSeenTimestampUptimeParam = null;
  private TagSeenCount _TagSeenCountParam = null;
  private ArrayList<AirProtocolTagData> _AirProtocolTagDataParams = null;
  private AccessSpecID _AccessSpecIDParam = null;
  //private ArrayList<OpSpecResult> _OpSpecResultParams = null;
  private ArrayList<Parameter> _OpSpecResultParams = null;
  private ArrayList<Custom> _CustomParams = null;

  public TagReportData()
  {
    _EPCDataParam = new EPC96();
    _AirProtocolTagDataParams = new ArrayList<AirProtocolTagData>();
    _OpSpecResultParams = new ArrayList<Parameter>();
    _CustomParams = new ArrayList<Custom>();
  }

  // accessors and mutators
  public void setEPCDataParam(EPCData p) {_EPCDataParam = p;}
  public void setEPCDataParam(EPC96 p) {_EPCDataParam = p;}
  public Parameter getEPCDataParam() {return _EPCDataParam;}
  public void setROSpecIDParam(ROSpecID p) {_ROSpecIDParam = p;}
  public ROSpecID getROSpecIDParam() {return _ROSpecIDParam;}
  public void setSpecIndexParam(SpecIndex p) {_SpecIndexParam = p;}
  public SpecIndex getSpecIndexParam() {return _SpecIndexParam;}
  public void setInventoryParameterSpecIDParam(InventoryParameterSpecID p) {_InventoryParameterSpecIDParam = p;}
  public InventoryParameterSpecID getInventoryParameterSpecIDParam() {return _InventoryParameterSpecIDParam;}
  public void setAntennaIDParam(AntennaID p) {_AntennaIDParam = p;}
  public AntennaID getAntennaIDParam() {return _AntennaIDParam;}
  public void setPeakRSSIParam(PeakRSSI p) {_PeakRSSIParam = p;}
  public PeakRSSI getPeakRSSIParam() {return _PeakRSSIParam;}
  public void setChannelIndexParam(ChannelIndex p) {_ChannelIndexParam = p;}
  public ChannelIndex getChannelIndexParam() {return _ChannelIndexParam;}
  public void setFirstSeenTimestampUTCParam(FirstSeenTimestampUTC p) {_FirstSeenTimestampUTCParam = p;}
  public FirstSeenTimestampUTC getFirstSeenTimestampUTCParam() {return _FirstSeenTimestampUTCParam;}
  public void setFirstSeenTimestampUptimeParam(FirstSeenTimestampUptime p) {_FirstSeenTimestampUptimeParam = p;}
  public FirstSeenTimestampUptime getFirstSeenTimestampUptimeParam() {return _FirstSeenTimestampUptimeParam;}
  public void setLastSeenTimestampUTCParam(LastSeenTimestampUTC p) {_LastSeenTimestampUTCParam = p;}
  public LastSeenTimestampUTC getLastSeenTimestampUTCParam() {return _LastSeenTimestampUTCParam;}
  public void setLastSeenTimestampUptimeParam(LastSeenTimestampUptime p) {_LastSeenTimestampUptimeParam = p;}
  public LastSeenTimestampUptime getLastSeenTimestampUptimeParam() {return _LastSeenTimestampUptimeParam;}
  public void setTagSeenCountParam(TagSeenCount p) {_TagSeenCountParam = p;}
  public TagSeenCount getTagSeenCountParam() {return _TagSeenCountParam;}
  public int getNumAirProtocolTagDataParams() { return _AirProtocolTagDataParams.size();}
  public void addAirProtocolTagDataParam(AirProtocolTagData p) {_AirProtocolTagDataParams.add(p);}
  public AirProtocolTagData getAirProtocolTagDataParam(int idx) {return _AirProtocolTagDataParams.get(idx);}
  public void setAccessSpecIDParam(AccessSpecID p) {_AccessSpecIDParam = p;}
  public AccessSpecID getAccessSpecIDParam() {return _AccessSpecIDParam;}
  public int getNumOpSpecResultParams() { return _OpSpecResultParams.size();}
  public void addOpSpecResultParam(C1G2OpSpecResult p) {_OpSpecResultParams.add(p);}
  public void addOpSpecResultParam(ClientRequestOpSpecResult p) {_OpSpecResultParams.add(p);}
  public Parameter getOpSpecResultParam(int idx) {return _OpSpecResultParams.get(idx);}
  public int getNumCustomParams() { return _CustomParams.size();}
  public void addCustomParam(Custom p) {_CustomParams.add(p);}
  public Custom getCustomParam(int idx) {return _CustomParams.get(idx);}

  //Methods mandated by TLVParameter class
  public int getParamType() {return 240;}
  public void serializeBody(DataOutputStream out) throws IOException
  {
    if(_EPCDataParam!=null) _EPCDataParam.serialize(out);
    if(_ROSpecIDParam!=null) _ROSpecIDParam.serialize(out);
    if(_SpecIndexParam!=null) _SpecIndexParam.serialize(out);
    if(_InventoryParameterSpecIDParam!=null) _InventoryParameterSpecIDParam.serialize(out);
    if(_AntennaIDParam!=null) _AntennaIDParam.serialize(out);
    if(_PeakRSSIParam!=null) _PeakRSSIParam.serialize(out);
    if(_ChannelIndexParam!=null) _ChannelIndexParam.serialize(out);
    if(_FirstSeenTimestampUTCParam!=null) _FirstSeenTimestampUTCParam.serialize(out);
    if(_FirstSeenTimestampUptimeParam!=null) _FirstSeenTimestampUptimeParam.serialize(out);
    if(_LastSeenTimestampUTCParam!=null) _LastSeenTimestampUTCParam.serialize(out);
    if(_LastSeenTimestampUptimeParam!=null) _LastSeenTimestampUptimeParam.serialize(out);
    if(_TagSeenCountParam!=null) _TagSeenCountParam.serialize(out);
    {
      int i;
      for(i=0; i<_AirProtocolTagDataParams.size(); i++)
        _AirProtocolTagDataParams.get(i).serialize(out);
    }
    if(_AccessSpecIDParam!=null) _AccessSpecIDParam.serialize(out);
    {
      int i;
      for(i=0; i<_OpSpecResultParams.size(); i++)
        _OpSpecResultParams.get(i).serialize(out);
    }
    {
      int i;
      for(i=0; i<_CustomParams.size(); i++)
        _CustomParams.get(i).serialize(out);
    }
  }

  public void deserializeBody(DataInputStream in, int bytes) throws IOException
  {
    int startAvail = in.available();
    while( (startAvail - in.available()) < bytes)
    {
      Parameter p = Parameter.deserialize(in);
      if(EPCData.class.isInstance(p))
        setEPCDataParam((EPCData)p);
      else if(EPC96.class.isInstance(p))
        setEPCDataParam((EPC96)p);
      else if(ROSpecID.class.isInstance(p))
        setROSpecIDParam((ROSpecID)p);
      else if(SpecIndex.class.isInstance(p))
        setSpecIndexParam((SpecIndex)p);
      else if(InventoryParameterSpecID.class.isInstance(p))
        setInventoryParameterSpecIDParam((InventoryParameterSpecID)p);
      else if(AntennaID.class.isInstance(p))
        setAntennaIDParam((AntennaID)p);
      else if(PeakRSSI.class.isInstance(p))
        setPeakRSSIParam((PeakRSSI)p);
      else if(ChannelIndex.class.isInstance(p))
        setChannelIndexParam((ChannelIndex)p);
      else if(FirstSeenTimestampUTC.class.isInstance(p))
        setFirstSeenTimestampUTCParam((FirstSeenTimestampUTC)p);
      else if(FirstSeenTimestampUptime.class.isInstance(p))
        setFirstSeenTimestampUptimeParam((FirstSeenTimestampUptime)p);
      else if(LastSeenTimestampUTC.class.isInstance(p))
        setLastSeenTimestampUTCParam((LastSeenTimestampUTC)p);
      else if(LastSeenTimestampUptime.class.isInstance(p))
        setLastSeenTimestampUptimeParam((LastSeenTimestampUptime)p);
      else if(TagSeenCount.class.isInstance(p))
        setTagSeenCountParam((TagSeenCount)p);
      else if(AirProtocolTagData.class.isInstance(p))
        addAirProtocolTagDataParam((AirProtocolTagData)p);
      else if(AccessSpecID.class.isInstance(p))
        setAccessSpecIDParam((AccessSpecID)p);
      else if(C1G2OpSpecResult.class.isInstance(p))
        addOpSpecResultParam((C1G2OpSpecResult)p);
      else if(ClientRequestOpSpecResult.class.isInstance(p))
        addOpSpecResultParam((ClientRequestOpSpecResult)p);
      else if(Custom.class.isInstance(p))
        addCustomParam((Custom)p);
      else throw new IOException("TagReportData.deserializeBody(): Unhandled parameter class "+p.getClass().getName());
    }
  }

  public void toXML(StringBuffer sb, String indent)
  {
    sb.append(indent+"<TagReportData>\n");
    if(_EPCDataParam!=null) _EPCDataParam.toXML(sb, indent+"  ");
    if(_ROSpecIDParam!=null) _ROSpecIDParam.toXML(sb, indent+"  ");
    if(_SpecIndexParam!=null) _SpecIndexParam.toXML(sb, indent+"  ");
    if(_InventoryParameterSpecIDParam!=null) _InventoryParameterSpecIDParam.toXML(sb, indent+"  ");
    if(_AntennaIDParam!=null) _AntennaIDParam.toXML(sb, indent+"  ");
    if(_PeakRSSIParam!=null) _PeakRSSIParam.toXML(sb, indent+"  ");
    if(_ChannelIndexParam!=null) _ChannelIndexParam.toXML(sb, indent+"  ");
    if(_FirstSeenTimestampUTCParam!=null) _FirstSeenTimestampUTCParam.toXML(sb, indent+"  ");
    if(_FirstSeenTimestampUptimeParam!=null) _FirstSeenTimestampUptimeParam.toXML(sb, indent+"  ");
    if(_LastSeenTimestampUTCParam!=null) _LastSeenTimestampUTCParam.toXML(sb, indent+"  ");
    if(_LastSeenTimestampUptimeParam!=null) _LastSeenTimestampUptimeParam.toXML(sb, indent+"  ");
    if(_TagSeenCountParam!=null) _TagSeenCountParam.toXML(sb, indent+"  ");
    {
      int i;
      for(i=0;i<_AirProtocolTagDataParams.size();i++)
        _AirProtocolTagDataParams.get(i).toXML(sb, indent+"  ");
    }
    if(_AccessSpecIDParam!=null) _AccessSpecIDParam.toXML(sb, indent+"  ");
    {
      int i;
      for(i=0;i<_OpSpecResultParams.size();i++)
        _OpSpecResultParams.get(i).toXML(sb, indent+"  ");
    }
    {
      int i;
      for(i=0;i<_CustomParams.size();i++)
        _CustomParams.get(i).toXML(sb, indent+"  ");
    }
    sb.append(indent+"</TagReportData>\n");
  }


}
