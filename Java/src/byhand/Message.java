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

package edu.uark.csce.llrp;
import java.io.*;

// Message.java
//
// Base class for LLRP Message classes

public abstract class Message
{
    // List of message type values
    public static final int MT_GET_READER_CAPABILITIES=1;
    public static final int MT_GET_READER_CAPABILITIES_RESPONSE=11;
    public static final int MT_ADD_ROSPEC=20;
    public static final int MT_ADD_ROSPEC_RESPONSE=30;
    public static final int MT_DELETE_ROSPEC=21;
    public static final int MT_DELETE_ROSPEC_RESPONSE=31;
    public static final int MT_START_ROSPEC=22;
    public static final int MT_START_ROSPEC_RESPONSE=32;
    public static final int MT_STOP_ROSPEC=23;
    public static final int MT_STOP_ROSPEC_RESPONSE=33;
    public static final int MT_ENABLE_ROSPEC=24;
    public static final int MT_ENABLE_ROSPEC_RESPONSE=34;
    public static final int MT_DISABLE_ROSPEC=25;
    public static final int MT_DISABLE_ROSPEC_RESPONSE=35;
    public static final int MT_GET_ROSPECS=26;
    public static final int MT_GET_ROSPECS_RESPONSE=36;
    public static final int MT_ADD_ACCESSSPEC=40;
    public static final int MT_ADD_ACCESSSPEC_RESPONSE=50;
    public static final int MT_DELETE_ACCESSSPEC=41;
    public static final int MT_DELETE_ACCESSSPEC_RESPONSE=51;
    public static final int MT_ENABLE_ACCESSSPEC=42;
    public static final int MT_ENABLE_ACCESSSPEC_RESPONSE=52;
    public static final int MT_DISABLE_ACCESSSPEC=43;
    public static final int MT_DISABLE_ACCESSSPEC_RESPONSE=53;
    public static final int MT_GET_ACCESSSPECS=44;
    public static final int MT_GET_ACCESSSPECS_RESPONSE=54;
    public static final int MT_CLIENT_REQUEST_OP=45;
    public static final int MT_CLIENT_REQUEST_OP_RESPONSE=55;
    public static final int MT_GET_REPORT=60;
    public static final int MT_RO_ACCESS_REPORT=61;
    public static final int MT_KEEPALIVE=62;
    public static final int MT_KEEPALIVE_ACK=72;
    public static final int MT_READER_EVENT_NOTIFICATION=63;
    public static final int MT_ENABLE_EVENTS_AND_REPORTS=64;
    public static final int MT_ERROR_MESSAGE=100;
    public static final int MT_GET_READER_CONFIG=2;
    public static final int MT_GET_READER_CONFIG_RESPONSE=12;
    public static final int MT_SET_READER_CONFIG=3;
    public static final int MT_SET_READER_CONFIG_RESPONSE=13;
    public static final int MT_CLOSE_CONNECTION=14;
    public static final int MT_CLOSE_CONNECTION_RESPONSE=4;
    public static final int MT_CUSTOM_MESSAGE=1023;
    
    // member variables
    private int messageID = 0;
    
    // member variable accessors/mutators
    public void setMessageID(int id) {messageID = id;}
    public int getMessageID() {return messageID;}
    
    // abstract methods to be defined by deriving class
    public abstract int getMessageType();
    public abstract void serializeBody(DataOutputStream out) throws IOException;
    public abstract void deserializeBody(DataInputStream in) throws IOException;
    public abstract void toXML(StringBuffer sb, String indent);
    
    // Sends a message out an output stream
    public void send(OutputStream out) throws IOException
    {
        byte[] msgBytes = serialize();
        out.write(msgBytes);
    }
    
    // Serializes a message into bytes
    public byte[] serialize() throws IOException
    {
        // First serialize the body
        ByteArrayOutputStream bodyOut_ba = new ByteArrayOutputStream();
        DataOutputStream bodyOut_data = new DataOutputStream(bodyOut_ba);
        serializeBody(bodyOut_data);
        bodyOut_data.flush();
        byte[] bodyBytes = bodyOut_ba.toByteArray();
        
        // Now we can serialize the header
        ByteArrayOutputStream msgOut_ba = new ByteArrayOutputStream();
        DataOutputStream msgOut_data = new DataOutputStream(msgOut_ba);
        
        short firstShort = (short) ((getMessageType() & 0x03FF) | 0x0400);
        msgOut_data.writeShort(firstShort);
        msgOut_data.writeInt(bodyBytes.length + 10);
        msgOut_data.writeInt(messageID);
        msgOut_data.write(bodyBytes);
        msgOut_data.flush();
        
        return msgOut_ba.toByteArray();
    }
    
    public static Message receive(InputStream in) throws IOException
    {
        // Set up a data input stream
        DataInputStream msgIn_data = new DataInputStream(in);
        
        // Read in the first short to figure out message type
        int typeShort = msgIn_data.readUnsignedShort();
        typeShort = typeShort & 0x03FF; // strip off reserved, version bits
        
        // This will hold our return value
        Message rval = null;
        
        switch(typeShort)
        {
            case MT_GET_READER_CAPABILITIES: 
	      rval = new GetReaderCapabilities(); 
	      break;
            case MT_GET_READER_CAPABILITIES_RESPONSE: 
	      rval = new GetReaderCapabilitiesResponse(); 
	      break;
	    case MT_ADD_ROSPEC:
	      rval = new AddROSpec(); 
	      break;
            case MT_ADD_ROSPEC_RESPONSE:
	      rval = new AddROSpecResponse(); 
	      break;
            case MT_DELETE_ROSPEC:
	      rval = new DeleteROSpec(); 
	      break;
            case MT_DELETE_ROSPEC_RESPONSE:
	      rval = new DeleteROSpecResponse(); 
	      break;
            case MT_START_ROSPEC:
	      rval = new StartROSpec(); 
	      break;
            case MT_START_ROSPEC_RESPONSE:
	      rval = new StartROSpecResponse(); 
	      break;
            case MT_STOP_ROSPEC:
	      rval = new StopROSpec(); 
	      break;
            case MT_STOP_ROSPEC_RESPONSE:
	      rval = new StopROSpecResponse(); 
	      break;
            case MT_ENABLE_ROSPEC:
	      rval = new EnableROSpec(); 
	      break;
            case MT_ENABLE_ROSPEC_RESPONSE:
	      rval = new EnableROSpecResponse(); 
	      break;
            case MT_DISABLE_ROSPEC:
	      rval = new DisableROSpec(); 
	      break;
            case MT_DISABLE_ROSPEC_RESPONSE:
	      rval = new DisableROSpecResponse(); 
	      break;
            case MT_GET_ROSPECS:
	      rval = new GetROSpecs(); 
	      break;
            case MT_GET_ROSPECS_RESPONSE:
	      rval = new GetROSpecsResponse(); 
	      break;
            case MT_ADD_ACCESSSPEC:
	      rval = new AddAccessSpec(); 
	      break;
            case MT_ADD_ACCESSSPEC_RESPONSE:
	      rval = new AddAccessSpecResponse(); 
	      break;
            case MT_DELETE_ACCESSSPEC:
	      rval = new DeleteAccessSpec(); 
	      break;
            case MT_DELETE_ACCESSSPEC_RESPONSE:
	      rval = new DeleteAccessSpecResponse(); 
	      break;
            case MT_ENABLE_ACCESSSPEC:
	      rval = new EnableAccessSpec(); 
	      break;
            case MT_ENABLE_ACCESSSPEC_RESPONSE:
	      rval = new EnableAccessSpecResponse(); 
	      break;
            case MT_DISABLE_ACCESSSPEC:
	      rval = new DisableAccessSpec(); 
	      break;
            case MT_DISABLE_ACCESSSPEC_RESPONSE:
	      rval = new DisableAccessSpecResponse(); 
	      break;
            case MT_GET_ACCESSSPECS:
	      rval = new GetAccessSpecs(); 
	      break;
            case MT_GET_ACCESSSPECS_RESPONSE:
	      rval = new GetAccessSpecsResponse(); 
	      break;
            case MT_CLIENT_REQUEST_OP:
	      rval = new ClientRequestOp(); 
	      break;
            case MT_CLIENT_REQUEST_OP_RESPONSE:
	      rval = new ClientRequestOpResponse(); 
	      break;
            case MT_GET_REPORT:
	      rval = new GetReport(); 
	      break;
            case MT_RO_ACCESS_REPORT:
	      rval = new ROAccessReport(); 
	      break;
            case MT_KEEPALIVE:
	      rval = new KeepAlive(); 
	      break;
            case MT_KEEPALIVE_ACK:
	      rval = new KeepAliveAck(); 
	      break;
            case MT_READER_EVENT_NOTIFICATION:
	      rval = new ReaderEventNotification(); 
	      break;
            case MT_ENABLE_EVENTS_AND_REPORTS:
	      rval = new EnableEventsAndReports(); 
	      break;
            case MT_ERROR_MESSAGE:
	      rval = new ErrorMessage(); 
	      break;
            case MT_GET_READER_CONFIG:
	      rval = new GetReaderConfig(); 
	      break;
            case MT_GET_READER_CONFIG_RESPONSE:
	      rval = new GetReaderConfigResponse(); 
	      break;
            case MT_SET_READER_CONFIG:
	      rval = new SetReaderConfig(); 
	      break;
            case MT_SET_READER_CONFIG_RESPONSE:
	      rval = new SetReaderConfigResponse(); 
	      break;
            case MT_CLOSE_CONNECTION:
	      rval = new CloseConnection(); 
	      break;
            case MT_CLOSE_CONNECTION_RESPONSE:
	      rval = new CloseConnectionResponse(); 
	      break;
            case MT_CUSTOM_MESSAGE:
	      rval = new CustomMessage(); 
	      break;
            default: throw new IOException("Message.receive(): Could not handle type " + typeShort);
        }
        
        // Pull out the message length and message ID
        int mLen = msgIn_data.readInt();
        int mID = msgIn_data.readInt();
        
        // Read the entire body of the message into a byte array
        int bytesToRead = mLen - 10;
        byte[] bodyBytes = new byte[bytesToRead];
        msgIn_data.readFully(bodyBytes);
        
        // Record the message ID
        rval.setMessageID(mID);
        
        // Create a data input stream and call deserializeBody()
        ByteArrayInputStream body_ba = new ByteArrayInputStream(bodyBytes);
        DataInputStream bodyInput = new DataInputStream(body_ba);
        rval.deserializeBody(bodyInput);
        
        return rval;
    }
    
    public void show()
    {
        System.out.println(toXMLString());
    }

    public String toXMLString()
    {
	StringBuffer sb = new StringBuffer();
	toXML(sb,"");
	return sb.toString();
    }
      
}
