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
 * File: C1G2TagInventoryMask.java
 * auto-generated Thu Jun 07 18:41:37 CDT 2007
 *
 * Manually tweaked by Joe Hoag
 */
package edu.uark.csce.llrp;
import java.io.*;
import java.util.ArrayList;
public class C1G2TagInventoryMask extends TLVParameter
{
  // member variables
  private int _MB = 0;
  private short _Pointer = 0;
  private byte[] _Mask = null;

  public C1G2TagInventoryMask()
  {
  }

  // accessors and mutators
  public void setMB(int i) {_MB = i;}
  public void setMB(String s) {_MB = Integer.parseInt(s);}
  public int getMB() {return _MB;}
  public void setPointer(short b) { _Pointer = b;}
  public void setPointer(String s) { _Pointer = Short.parseShort(s);}
  public short getPointer() {return _Pointer;}
  public short getMaskBitCount() 
  {
    return (_Mask == null) ? ((short)0) : ((short)(_Mask.length * 8));
  }
  public void setMask(byte[] b) {_Mask = b;}
  public byte[] getMask() {return _Mask;}


  //Methods mandated by TLVParameter class
  public int getParamType() {return 332;}
  public void serializeBody(DataOutputStream out) throws IOException
  {
    byte x = 0;
    x = (byte) (x | ((_MB & 0x03) << 6));
    out.writeByte(x);
    out.writeShort(getMaskBitCount());
    if(_Mask != null) out.write(_Mask);
  }

  public void deserializeBody(DataInputStream in, int bytes) throws IOException
  {
    byte x = in.readByte();
    _MB = ( (x >> 6) & 0x03);
    _Pointer = in.readShort();
    short bitcount = in.readShort();
    if(bitcount != 0)
    {
	_Mask = new byte[bitcount >> 3];
	in.readFully(_Mask);
    }
  }


  public void toXML(StringBuffer sb, String indent)
  {
    sb.append(indent+"<C1G2TagInventoryMask>\n");
    sb.append(indent+"  <MB value=\""+_MB+"\"/>\n");
    sb.append(indent+"  <Pointer value=\""+_Pointer+"\"/>\n");
    String s = new String(_Mask);
    s = s.replaceAll("\"","&quot;");
    sb.append(indent+"  <Mask value=\""+s+"\"/>\n");
    sb.append(indent+"</C1G2TagInventoryMask>\n");
  }



}
