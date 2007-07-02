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
 * File: EPCData.java
 * auto-generated Wed Jun 06 13:48:20 CDT 2007
 *
 * Manually tweaked by Joe Hoag
 */
package edu.uark.csce.llrp;
import java.io.*;
import java.util.ArrayList;
public class EPCData extends TLVParameter
{
  // member variables
  private short _EPCLengthBits = 0;
  private byte[] data = null; 

  public EPCData()
  {
  }

  // accessors and mutators
  public short getEPCLengthBits() {return (short) (data.length * 8);}
  public void setData(byte[] b) {data = b;}
  public void setData(String s) {data = s.getBytes();}
  public byte[] getData() {return data;}

  //Methods mandated by TLVParameter class
  public int getParamType() {return 241;}
  public void serializeBody(DataOutputStream out) throws IOException
  {
    out.writeShort(getEPCLengthBits());
    out.write(data);
  }

  public void deserializeBody(DataInputStream in, int bytes) throws IOException
  {
    short bitLength = in.readShort();
    data = new byte[bytes-2];
    in.readFully(data);
  }


  public void toXML(StringBuffer sb, String indent)
  {
    sb.append(indent+"<EPCData>\n");
    String s = new String(data);
    s = s.replaceAll("\"","&quot;");
    sb.append(indent+"  <data value=\"" + s + "\"/>\n");
    sb.append(indent+"</EPCData>\n");
  }


}
