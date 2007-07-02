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
 * File: CustomMessage.java
 * auto-generated Thu Jun 07 12:30:14 CDT 2007
 *
 * Manually tweaked by Joe Hoag.
 */
package edu.uark.csce.llrp;
import java.io.*;
import java.util.ArrayList;
public class CustomMessage extends Message
{
  // member variables
  private byte _Subtype = 0;
  //private ArrayList<Byte> PayloadElements = null;
  private byte[] _Payload = null;

  public CustomMessage()
  {
  }

  // accessors and mutators
  public void setSubtype(byte b) { _Subtype = b;}
  public void setSubtype(String s) { _Subtype = Byte.parseByte(s);}
  public byte getSubtype() {return _Subtype;}
  public void setPayload(byte[] b) {_Payload = b;}
  public void setPayload(String s) {setPayload(s.getBytes());}
  public byte[] getPayload() {return _Payload;}

  //Methods mandated by Message class
  public int getMessageType() {return 1023;}
  public void serializeBody(DataOutputStream out) throws IOException
  {
    out.writeByte(_Subtype);
    byte[] payload = getPayload();
    if(payload != null) out.write(payload);
  }

  public void deserializeBody(DataInputStream in) throws IOException
  {
    _Subtype = in.readByte();
    int bytesLeft = in.available();
    if(bytesLeft > 0)
    {
      byte[] payload = new byte[bytesLeft];
      in.readFully(payload);
      setPayload(payload);
    }
  }


  public void toXML(StringBuffer sb, String indent)
  {
    sb.append(indent+"<Message type=\"CustomMessage\">\n");
    sb.append(indent+"  <Subtype value=\""+_Subtype+"\"/>\n");
    byte[] payload = getPayload();
    if(payload != null)
    {
      String s = new String(payload);
      s = s.replaceAll("\"","&quot;");
      sb.append(indent+"  <Payload=\"" + s + "\"/>\n");
    }
    sb.append(indent+"</Message>\n");
  }


}
