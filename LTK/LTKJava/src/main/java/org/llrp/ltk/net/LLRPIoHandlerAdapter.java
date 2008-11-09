/*
 * Copyright 2007 ETH Zurich
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 *
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions
 * and limitations under the License.
 */

package org.llrp.ltk.net;

import java.util.concurrent.BlockingQueue;

import org.apache.mina.common.IoHandlerAdapter;
import org.llrp.ltk.generated.parameters.ConnectionAttemptEvent;
import org.llrp.ltk.types.LLRPMessage;


/**
 * LLRPIoHandlerAdapter defines abstract methods that need to be implemented by any LLRPIoHandlerAdaptor implementation in addition to
 * the methods defined by the Apache MINA IoHandler interface. 
 *
 */

public abstract class LLRPIoHandlerAdapter extends IoHandlerAdapter{

	/**
	 * returns true if incoming KEEP_ALIVE messages are being acknowledged.
	 */
	
	public abstract boolean isKeepAliveAck();
	
	/**
	 * set whether incoming KEEP_ALIVE messages should be acknowledged. Default case is that 
	 * KEEP_ALIVE messages are acknowledged.
	 * 
	 * @param keepAliveAck true if KEEP_ALIVE messages are to be acknowledged
	 */
	
	public abstract void setKeepAliveAck(boolean keepAliveAck);
	
	/**
	 * returns true if incoming KEEP_ALIVE messages are being forwarded to the LLRPEndpoint.
	 * 
	 * 
	 * @return keepAliveForward true if KEEP_ALIVE messages are forwarded, false otherwise
	 */
	
	public abstract boolean isKeepAliveForward();
	
	
	/**
	 * set whether incoming KEEP_ALIVE messages are being forwarded to the LLRPEndpoint.
	 * Default is with forwarding off.
	 * 
	 * @param keepAliveForward true if KEEP_ALIVE messages are to be forwarded
	 */
	
	public abstract void setKeepAliveForward(boolean keepAliveForward);
	
	/**
	 * returns queue of all incoming messages where the messages type is equal to the one specified 
	 * in the IoSession parameter LLRPConnection.SYNC_MESSAGE_ANSWER. This method is required by
	 * the transact (synchronous message sending) of the LLRP connections. 
	 **/

	public abstract BlockingQueue<LLRPMessage> getSynMessageQueue();

	/** 
	 * returns queue with all incoming ConnectionAttemptEvent parameters which were embedded in 
	 * READER_NOTIFICATION messages. 
	 * The getConnectionAttemptEventQueue method is used
	 * to fetch any ConnectionAttemptEvent that arrived in READER_NOTIFICATION messages. 
	 * These events are used by LLRPConnection objects
	 * to check whether the connection could be established successfully. 
	 **/ 
	
	public abstract BlockingQueue<ConnectionAttemptEvent> getConnectionAttemptEventQueue();

	/** 
	 * gets connection on which handler is operating
	 * 
	 * @return connection
	 **/ 
	
	public abstract LLRPConnection getConnection();
	
	/** 
	 * sets connection on which handler is operating
	 * 
	 * @param connection
	 **/
	
	public abstract void setConnection(LLRPConnection connection);
	
	
	

}
