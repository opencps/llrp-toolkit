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
 * File: ReaderEventNotificationData.java
 * auto-generated Wed Jun 06 13:09:31 CDT 2007
 *
 * Manually tweaked by Joe Hoag
 */
package edu.uark.csce.llrp;
import java.io.*;
import java.util.ArrayList;
public class ReaderEventNotificationData extends TLVParameter
{
  // member variables
  private Timestamp _TimestampParam = null;
  private HoppingEvent _HoppingEventParam = null;
  private GPIEvent _GPIEventParam = null;
  private ROSpecEvent _ROSpecEventParam = null;
  private ReportBufferLevelWarningEvent _ReportBufferLevelWarningEventParam = null;
  private ReportBufferOverflowErrorEvent _ReportBufferOverflowErrorEventParam = null;
  private ReaderExceptionEvent _ReaderExceptionEventParam = null;
  private RFSurveyEvent _RFSurveyEventParam = null;
  private AISpecEvent _AISpecEventParam = null;
  private AntennaEvent _AntennaEventParam = null;
  private ConnectionAttemptEvent _ConnectionAttemptEventParam = null;
  private ConnectionCloseEvent _ConnectionCloseEventParam = null;
  private ArrayList<Custom> _CustomParams = null;

  public ReaderEventNotificationData()
  {
    //_TimestampParam = new Timestamp();
    _TimestampParam = new UTCTimestamp(); // Needed non-abstract ctor
    _CustomParams = new ArrayList<Custom>();
  }

  // accessors and mutators
  public void setTimestampParam(Timestamp p) {_TimestampParam = p;}
  public Timestamp getTimestampParam() {return _TimestampParam;}
  public void setHoppingEventParam(HoppingEvent p) {_HoppingEventParam = p;}
  public HoppingEvent getHoppingEventParam() {return _HoppingEventParam;}
  public void setGPIEventParam(GPIEvent p) {_GPIEventParam = p;}
  public GPIEvent getGPIEventParam() {return _GPIEventParam;}
  public void setROSpecEventParam(ROSpecEvent p) {_ROSpecEventParam = p;}
  public ROSpecEvent getROSpecEventParam() {return _ROSpecEventParam;}
  public void setReportBufferLevelWarningEventParam(ReportBufferLevelWarningEvent p) {_ReportBufferLevelWarningEventParam = p;}
  public ReportBufferLevelWarningEvent getReportBufferLevelWarningEventParam() {return _ReportBufferLevelWarningEventParam;}
  public void setReportBufferOverflowErrorEventParam(ReportBufferOverflowErrorEvent p) {_ReportBufferOverflowErrorEventParam = p;}
  public ReportBufferOverflowErrorEvent getReportBufferOverflowErrorEventParam() {return _ReportBufferOverflowErrorEventParam;}
  public void setReaderExceptionEventParam(ReaderExceptionEvent p) {_ReaderExceptionEventParam = p;}
  public ReaderExceptionEvent getReaderExceptionEventParam() {return _ReaderExceptionEventParam;}
  public void setRFSurveyEventParam(RFSurveyEvent p) {_RFSurveyEventParam = p;}
  public RFSurveyEvent getRFSurveyEventParam() {return _RFSurveyEventParam;}
  public void setAISpecEventParam(AISpecEvent p) {_AISpecEventParam = p;}
  public AISpecEvent getAISpecEventParam() {return _AISpecEventParam;}
  public void setAntennaEventParam(AntennaEvent p) {_AntennaEventParam = p;}
  public AntennaEvent getAntennaEventParam() {return _AntennaEventParam;}
  public void setConnectionAttemptEventParam(ConnectionAttemptEvent p) {_ConnectionAttemptEventParam = p;}
  public ConnectionAttemptEvent getConnectionAttemptEventParam() {return _ConnectionAttemptEventParam;}
  public void setConnectionCloseEventParam(ConnectionCloseEvent p) {_ConnectionCloseEventParam = p;}
  public ConnectionCloseEvent getConnectionCloseEventParam() {return _ConnectionCloseEventParam;}
  public int getNumCustomParams() { return _CustomParams.size();}
  public void addCustomParam(Custom p) {_CustomParams.add(p);}
  public Custom getCustomParam(int idx) {return _CustomParams.get(idx);}

  //Methods mandated by TLVParameter class
  public int getParamType() {return 246;}
  public void serializeBody(DataOutputStream out) throws IOException
  {
    if(_TimestampParam!=null) _TimestampParam.serialize(out);
    if(_HoppingEventParam!=null) _HoppingEventParam.serialize(out);
    if(_GPIEventParam!=null) _GPIEventParam.serialize(out);
    if(_ROSpecEventParam!=null) _ROSpecEventParam.serialize(out);
    if(_ReportBufferLevelWarningEventParam!=null) _ReportBufferLevelWarningEventParam.serialize(out);
    if(_ReportBufferOverflowErrorEventParam!=null) _ReportBufferOverflowErrorEventParam.serialize(out);
    if(_ReaderExceptionEventParam!=null) _ReaderExceptionEventParam.serialize(out);
    if(_RFSurveyEventParam!=null) _RFSurveyEventParam.serialize(out);
    if(_AISpecEventParam!=null) _AISpecEventParam.serialize(out);
    if(_AntennaEventParam!=null) _AntennaEventParam.serialize(out);
    if(_ConnectionAttemptEventParam!=null) _ConnectionAttemptEventParam.serialize(out);
    if(_ConnectionCloseEventParam!=null) _ConnectionCloseEventParam.serialize(out);
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
      if(Timestamp.class.isInstance(p))
        setTimestampParam((Timestamp)p);
      else if(HoppingEvent.class.isInstance(p))
        setHoppingEventParam((HoppingEvent)p);
      else if(GPIEvent.class.isInstance(p))
        setGPIEventParam((GPIEvent)p);
      else if(ROSpecEvent.class.isInstance(p))
        setROSpecEventParam((ROSpecEvent)p);
      else if(ReportBufferLevelWarningEvent.class.isInstance(p))
        setReportBufferLevelWarningEventParam((ReportBufferLevelWarningEvent)p);
      else if(ReportBufferOverflowErrorEvent.class.isInstance(p))
        setReportBufferOverflowErrorEventParam((ReportBufferOverflowErrorEvent)p);
      else if(ReaderExceptionEvent.class.isInstance(p))
        setReaderExceptionEventParam((ReaderExceptionEvent)p);
      else if(RFSurveyEvent.class.isInstance(p))
        setRFSurveyEventParam((RFSurveyEvent)p);
      else if(AISpecEvent.class.isInstance(p))
        setAISpecEventParam((AISpecEvent)p);
      else if(AntennaEvent.class.isInstance(p))
        setAntennaEventParam((AntennaEvent)p);
      else if(ConnectionAttemptEvent.class.isInstance(p))
        setConnectionAttemptEventParam((ConnectionAttemptEvent)p);
      else if(ConnectionCloseEvent.class.isInstance(p))
        setConnectionCloseEventParam((ConnectionCloseEvent)p);
      else if(Custom.class.isInstance(p))
        addCustomParam((Custom)p);
      else throw new IOException("ReaderEventNotificationData.deserializeBody(): Unhandled parameter class "+p.getClass().getName());
    }
  }


  public void toXML(StringBuffer sb, String indent)
  {
    sb.append(indent+"<ReaderEventNotificationData>\n");
    if(_TimestampParam!=null) _TimestampParam.toXML(sb, indent+"  ");
    if(_HoppingEventParam!=null) _HoppingEventParam.toXML(sb, indent+"  ");
    if(_GPIEventParam!=null) _GPIEventParam.toXML(sb, indent+"  ");
    if(_ROSpecEventParam!=null) _ROSpecEventParam.toXML(sb, indent+"  ");
    if(_ReportBufferLevelWarningEventParam!=null) _ReportBufferLevelWarningEventParam.toXML(sb, indent+"  ");
    if(_ReportBufferOverflowErrorEventParam!=null) _ReportBufferOverflowErrorEventParam.toXML(sb, indent+"  ");
    if(_ReaderExceptionEventParam!=null) _ReaderExceptionEventParam.toXML(sb, indent+"  ");
    if(_RFSurveyEventParam!=null) _RFSurveyEventParam.toXML(sb, indent+"  ");
    if(_AISpecEventParam!=null) _AISpecEventParam.toXML(sb, indent+"  ");
    if(_AntennaEventParam!=null) _AntennaEventParam.toXML(sb, indent+"  ");
    if(_ConnectionAttemptEventParam!=null) _ConnectionAttemptEventParam.toXML(sb, indent+"  ");
    if(_ConnectionCloseEventParam!=null) _ConnectionCloseEventParam.toXML(sb, indent+"  ");
    {
      int i;
      for(i=0;i<_CustomParams.size();i++)
        _CustomParams.get(i).toXML(sb, indent+"  ");
    }
    sb.append(indent+"</ReaderEventNotificationData>\n");
  }


}
