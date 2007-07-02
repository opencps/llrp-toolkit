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
 * File: ROSpec.java
 * auto-generated Wed Jun 06 12:58:22 CDT 2007
 *
 * Manually tweaked by Joe Hoag
 */
package edu.uark.csce.llrp;
import java.io.*;
import java.util.ArrayList;
public class ROSpec extends TLVParameter
{
  // member variables
  private int _ROSpecID = 0;
  private byte _Priority = 0;
  private byte _CurrentState = 0;
  private ROBoundarySpec _ROBoundarySpecParam = null;
  private ArrayList<Parameter> _SpecParams = null;
  private ROReportSpec _ROReportSpecParam = null;

  public ROSpec()
  {
    _ROBoundarySpecParam = new ROBoundarySpec();
    _SpecParams = new ArrayList<Parameter>();
  }

  // accessors and mutators
  public void setROSpecID(int b) { _ROSpecID = b;}
  public void setROSpecID(String s) { _ROSpecID = Integer.parseInt(s);}
  public int getROSpecID() {return _ROSpecID;}
  public void setPriority(byte b) { _Priority = b;}
  public void setPriority(String s) { _Priority = Byte.parseByte(s);}
  public byte getPriority() {return _Priority;}
  public void setCurrentState(byte b) { _CurrentState = b;}
  public void setCurrentState(String s) { _CurrentState = Byte.parseByte(s);}
  public byte getCurrentState() {return _CurrentState;}
  public void setROBoundarySpecParam(ROBoundarySpec p) {_ROBoundarySpecParam = p;}
  public ROBoundarySpec getROBoundarySpecParam() {return _ROBoundarySpecParam;}
  public int getNumSpecParams() { return _SpecParams.size();}
  public void addSpecParam(AISpec p) {_SpecParams.add(p);}
  public void addSpecParam(RFSurveySpec p) {_SpecParams.add(p);}
  public void addSpecParam(Custom p) {_SpecParams.add(p);}
  public Parameter getSpecParam(int idx) {return _SpecParams.get(idx);}
  public void setROReportSpecParam(ROReportSpec p) {_ROReportSpecParam = p;}
  public ROReportSpec getROReportSpecParam() {return _ROReportSpecParam;}

  //Methods mandated by TLVParameter class
  public int getParamType() {return 177;}
  public void serializeBody(DataOutputStream out) throws IOException
  {
    out.writeInt(_ROSpecID);
    out.writeByte(_Priority);
    out.writeByte(_CurrentState);
    if(_ROBoundarySpecParam!=null) _ROBoundarySpecParam.serialize(out);
    {
      int i;
      for(i=0; i<_SpecParams.size(); i++)
        _SpecParams.get(i).serialize(out);
    }
    if(_ROReportSpecParam!=null) _ROReportSpecParam.serialize(out);
  }

  public void deserializeBody(DataInputStream in, int bytes) throws IOException
  {
    int startAvail = in.available();
    _ROSpecID = in.readInt();
    _Priority = in.readByte();
    _CurrentState = in.readByte();
    while( (startAvail - in.available()) < bytes)
    {
      Parameter p = Parameter.deserialize(in);
      if(ROBoundarySpec.class.isInstance(p))
        setROBoundarySpecParam((ROBoundarySpec)p);
      else if(AISpec.class.isInstance(p))
        addSpecParam((AISpec)p);
      else if(RFSurveySpec.class.isInstance(p))
        addSpecParam((RFSurveySpec)p);
      else if(Custom.class.isInstance(p))
        addSpecParam((Custom)p);
      else if(ROReportSpec.class.isInstance(p))
        setROReportSpecParam((ROReportSpec)p);
      else throw new IOException("ROSpec.deserializeBody(): Unhandled parameter class "+p.getClass().getName());
    }
  }


  public void toXML(StringBuffer sb, String indent)
  {
    sb.append(indent+"<ROSpec>\n");
    sb.append(indent+"  <ROSpecID value=\""+_ROSpecID+"\"/>\n");
    sb.append(indent+"  <Priority value=\""+_Priority+"\"/>\n");
    sb.append(indent+"  <CurrentState value=\""+_CurrentState+"\"/>\n");
    if(_ROBoundarySpecParam!=null) _ROBoundarySpecParam.toXML(sb, indent+"  ");
    {
      int i;
      for(i=0;i<_SpecParams.size();i++)
        _SpecParams.get(i).toXML(sb, indent+"  ");
    }
    if(_ROReportSpecParam!=null) _ROReportSpecParam.toXML(sb, indent+"  ");
    sb.append(indent+"</ROSpec>\n");
  }


}
