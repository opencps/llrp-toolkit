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
 * File: C1G2TagSpec.java
 * auto-generated Fri Jun 08 08:46:17 CDT 2007
 *
 * Manually tweaked by Joe Hoag
 */
package edu.uark.csce.llrp;
import java.io.*;
import java.util.ArrayList;
public class C1G2TagSpec extends TagSpec
{
  // member variables
  private C1G2TargetTag _C1G2TargetTagParam1 = null;
  private C1G2TargetTag _C1G2TargetTagParam2 = null;

  public C1G2TagSpec()
  {
    _C1G2TargetTagParam1 = new C1G2TargetTag();
  }

  // accessors and mutators
  public void setTagPattern1(C1G2TargetTag p) {_C1G2TargetTagParam1 = p;}
  public C1G2TargetTag getTagPattern1() {return _C1G2TargetTagParam1;}
  public void setTagPattern2(C1G2TargetTag p) {_C1G2TargetTagParam2 = p;}
  public C1G2TargetTag getTagPattern2() {return _C1G2TargetTagParam2;}

  //Methods mandated by TagSpec class
  public int getParamType() {return 338;}
  public void serializeBody(DataOutputStream out) throws IOException
  {
    if(_C1G2TargetTagParam1!=null) _C1G2TargetTagParam1.serialize(out);
    if(_C1G2TargetTagParam2!=null) _C1G2TargetTagParam2.serialize(out);
  }

  public void deserializeBody(DataInputStream in, int bytes) throws IOException
  {
    int startAvail = in.available();
    _C1G2TargetTagParam1 = (C1G2TargetTag) Parameter.deserialize(in);
    if(startAvail - in.available() < bytes)
    {
      _C1G2TargetTagParam2 = (C1G2TargetTag) Parameter.deserialize(in);
    }
      
  }

  public void toXML(StringBuffer sb, String indent)
  {
    sb.append(indent+"<C1G2TagSpec>\n");
    if(_C1G2TargetTagParam1!=null) _C1G2TargetTagParam1.toXML(sb, indent+"  ");
    if(_C1G2TargetTagParam2!=null) _C1G2TargetTagParam2.toXML(sb, indent+"  ");
    sb.append(indent+"</C1G2TagSpec>\n");
  }



}
