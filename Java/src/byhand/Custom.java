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
 * File: Custom.java
 * auto-generated Wed Jun 06 14:05:48 CDT 2007
 *
 * Manually tweaked by Joe Hoag
 */
package edu.uark.csce.llrp;
import java.io.*;
import java.util.ArrayList;
public class Custom extends TLVParameter
{
  // member variables
  private int _VendorID = 0;
  private int _Subtype = 0;
  private byte[] _VendorData = null;

  public Custom()
  {
  }

  // accessors and mutators
  public void setVendorID(int b) { _VendorID = b;}
  public void setVendorID(String s) { _VendorID = Integer.parseInt(s);}
  public int getVendorID() {return _VendorID;}
  public void setSubtype(int b) { _Subtype = b;}
  public void setSubtype(String s) { _Subtype = Integer.parseInt(s);}
  public int getSubtype() {return _Subtype;}
  public void setVendorData(byte[] b) { _VendorData = b;} 
  public void setVendorData(String s) { setVendorData(s.getBytes());}
  public byte[] getVendorData() {return _VendorData;}

  //Methods mandated by TLVParameter class
  public int getParamType() {return 1023;}
  public void serializeBody(DataOutputStream out) throws IOException
  {
    out.writeInt(_VendorID);
    out.writeInt(_Subtype);
    byte[] vendorBytes = getVendorData();
    if(vendorBytes!=null)out.write(vendorBytes);
  }

  public void deserializeBody(DataInputStream in, int bytes) throws IOException
  {
    _VendorID = in.readInt();
    _Subtype = in.readInt();
    byte[] _VendorBytes = new byte[bytes - 8];
    in.readFully(_VendorBytes);
    setVendorData(_VendorBytes);
  }


  public void toXML(StringBuffer sb, String indent)
  {
    sb.append(indent+"<Custom>\n");
    sb.append(indent+"  <VendorID value=\""+_VendorID+"\"/>\n");
    sb.append(indent+"  <Subtype value=\""+_Subtype+"\"/>\n");
    byte[] vendorBytes = getVendorData();
    if(vendorBytes != null)
    {
      String s = new String(vendorBytes);
      s = s.replaceAll("\"","&quot;");
      sb.append(indent+"  <VendorData value=\""+s+"\"/>\n");
    }
    sb.append(indent+"</Custom>\n");
  }


}
