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
 * File: FrequencyRSSILevelEntry.java
 * auto-generated Wed Jun 06 13:09:31 CDT 2007
 *
 * Manually tweaked by Joe Hoag.
 */
package edu.uark.csce.llrp;
import java.io.*;
import java.util.ArrayList;
public class FrequencyRSSILevelEntry extends TLVParameter
{
  // member variables
  private int _Frequency = 0;
  private int _Bandwidth = 0;
  private byte _AvgRSSI = 0;
  private byte _PeakRSSI = 0;
  private Timestamp _TimestampParam = null;

  public FrequencyRSSILevelEntry()
  {
    //_TimestampParam = new Timestamp();
    _TimestampParam = new UTCTimestamp(); // needed non-abstract ctor
  }

  // accessors and mutators
  public void setFrequency(int b) { _Frequency = b;}
  public void setFrequency(String s) { _Frequency = Integer.parseInt(s);}
  public int getFrequency() {return _Frequency;}
  public void setBandwidth(int b) { _Bandwidth = b;}
  public void setBandwidth(String s) { _Bandwidth = Integer.parseInt(s);}
  public int getBandwidth() {return _Bandwidth;}
  public void setAvgRSSI(byte b) { _AvgRSSI = b;}
  public void setAvgRSSI(String s) { _AvgRSSI = Byte.parseByte(s);}
  public byte getAvgRSSI() {return _AvgRSSI;}
  public void setPeakRSSI(byte b) { _PeakRSSI = b;}
  public void setPeakRSSI(String s) { _PeakRSSI = Byte.parseByte(s);}
  public byte getPeakRSSI() {return _PeakRSSI;}
  public void setTimestampParam(Timestamp p) {_TimestampParam = p;}
  public Timestamp getTimestampParam() {return _TimestampParam;}

  //Methods mandated by TLVParameter class
  public int getParamType() {return 243;}
  public void serializeBody(DataOutputStream out) throws IOException
  {
    out.writeInt(_Frequency);
    out.writeInt(_Bandwidth);
    out.writeByte(_AvgRSSI);
    out.writeByte(_PeakRSSI);
    if(_TimestampParam!=null) _TimestampParam.serialize(out);
  }

  public void deserializeBody(DataInputStream in, int bytes) throws IOException
  {
    _Frequency = in.readInt();
    _Bandwidth = in.readInt();
    _AvgRSSI = in.readByte();
    _PeakRSSI = in.readByte();
    _TimestampParam = (Timestamp) Parameter.deserialize(in);
  }


  public void toXML(StringBuffer sb, String indent)
  {
    sb.append(indent+"<FrequencyRSSILevelEntry>\n");
    sb.append(indent+"  <Frequency value=\""+_Frequency+"\"/>\n");
    sb.append(indent+"  <Bandwidth value=\""+_Bandwidth+"\"/>\n");
    sb.append(indent+"  <AvgRSSI value=\""+_AvgRSSI+"\"/>\n");
    sb.append(indent+"  <PeakRSSI value=\""+_PeakRSSI+"\"/>\n");
    if(_TimestampParam!=null) _TimestampParam.toXML(sb, indent+"  ");
    sb.append(indent+"</FrequencyRSSILevelEntry>\n");
  }


}
