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
 * File: AccessCommand.java
 * auto-generated Wed Jun 06 13:05:33 CDT 2007
 *
 * Manually tweaked by Joe Hoag.
 */
package edu.uark.csce.llrp;
import java.io.*;
import java.util.ArrayList;
public class AccessCommand extends TLVParameter
{
  // member variables
  private TagSpec _TagSpecParam = null;
  private ArrayList<OpSpec> _OpSpecParams = null;
  private ArrayList<Custom> _CustomParams = null;

  public AccessCommand()
  {
    //_TagSpecParam = new TagSpec();
    _TagSpecParam = new C1G2TagSpec(); // Needs to be non-abstract ctor
    _OpSpecParams = new ArrayList<OpSpec>();
    _CustomParams = new ArrayList<Custom>();
  }

  // accessors and mutators
  public void setTagSpecParam(TagSpec p) {_TagSpecParam = p;}
  public TagSpec getTagSpecParam() {return _TagSpecParam;}
  public int getNumOpSpecParams() { return _OpSpecParams.size();}
  public void addOpSpecParam(OpSpec p) {_OpSpecParams.add(p);}
  public OpSpec getOpSpecParam(int idx) {return _OpSpecParams.get(idx);}
  public int getNumCustomParams() { return _CustomParams.size();}
  public void addCustomParam(Custom p) {_CustomParams.add(p);}
  public Custom getCustomParam(int idx) {return _CustomParams.get(idx);}

  //Methods mandated by TLVParameter class
  public int getParamType() {return 209;}
  public void serializeBody(DataOutputStream out) throws IOException
  {
    if(_TagSpecParam!=null) _TagSpecParam.serialize(out);
    {
      int i;
      for(i=0; i<_OpSpecParams.size(); i++)
        _OpSpecParams.get(i).serialize(out);
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
      if(TagSpec.class.isInstance(p))
        setTagSpecParam((TagSpec)p);
      else if(OpSpec.class.isInstance(p))
        addOpSpecParam((OpSpec)p);
      else if(Custom.class.isInstance(p))
        addCustomParam((Custom)p);
      else throw new IOException("AccessCommand.deserializeBody(): Unhandled parameter class "+p.getClass().getName());
    }
  }

  public void toXML(StringBuffer sb, String indent)
  {
    sb.append(indent+"<AccessCommand>\n");
    if(_TagSpecParam!=null) _TagSpecParam.toXML(sb, indent+"  ");
    {
      int i;
      for(i=0;i<_OpSpecParams.size();i++)
        _OpSpecParams.get(i).toXML(sb, indent+"  ");
    }
    {
      int i;
      for(i=0;i<_CustomParams.size();i++)
        _CustomParams.get(i).toXML(sb, indent+"  ");
    }
    sb.append(indent+"</AccessCommand>\n");
  }


}
