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
import java.util.concurrent.LinkedBlockingQueue;

import org.apache.log4j.Logger;
import org.apache.mina.common.IdleStatus;
import org.apache.mina.common.IoSession;
import org.llrp.ltk.generated.messages.KEEPALIVE;
import org.llrp.ltk.generated.messages.KEEPALIVE_ACK;
import org.llrp.ltk.generated.messages.READER_EVENT_NOTIFICATION;
import org.llrp.ltk.generated.parameters.ConnectionAttemptEvent;
import org.llrp.ltk.types.LLRPMessage;

/**
 * 
 * LLRPIoHandlerAdapterImpl is the default implementation of the LLRPIoHandlerAdapter.
 * It handles incoming messages: routes incoming asynchronous messages 
 * to the LLRPEndpoint registered, replies to KEEP_ALIVE messages and handles incoming READER_NOTIFICATION 
 * messages and responses to synchronous calls. 
 *
 */

public class LLRPIoHandlerAdapterImpl extends LLRPIoHandlerAdapter{
	private Logger log = Logger.getLogger(LLRPIoHandlerAdapterImpl.class);	
	private LLRPConnection connection;
	private BlockingQueue<LLRPMessage> synMessageQueue = new LinkedBlockingQueue<LLRPMessage>();
	private BlockingQueue<ConnectionAttemptEvent> connectionAttemptEventQueue = new LinkedBlockingQueue<ConnectionAttemptEvent>(1);
	private boolean keepAliveAck = true;
	private boolean keepAliveForward = false;

	
	public LLRPIoHandlerAdapterImpl(LLRPConnection connection) {
		this.connection = connection;
	}
	
	public LLRPIoHandlerAdapterImpl() {
	}
	
	/**
	 * {@inheritDoc}
	 */
	
	public void sessionOpened(IoSession session) throws Exception {
		log.debug("session is opened:"+session);
		this.connection.session = session;
	}

	/**
	 * is called whenever an LLRP Message is received. The method replies to incoming
	 * KEEP_ALIVE messages by sending an KEEP_ALIVE_ACK when the keepAliveAck flag is set. 
	 * ConnectionAttemptEvents of incoming are stored in a queue that can be retrieved using 
	 * the getConnectionAttemptEventQueue method. messageReceived also checks whether the 
	 * incoming message is a response to previously method sent via the LLRPConnection.transact 
	 * method. Matching messages are stored in a queue that can be retrieved via the 
	 * getSynMessageQueue() method. All incoming messages except KEEP_ALIVE and those identified
	 * as synchronous responses to the LLRPConnection.transact method are passed to the
	 * LLRPEndpoint registered.
	 */
	
	public void messageReceived(IoSession session, Object message)
			throws Exception {		
		LLRPMessage llrpMessage = (LLRPMessage) message;
		log.info("message "+message.getClass()+" received in session "+session);
		if (log.isDebugEnabled()) {
			log.debug(llrpMessage.toXMLString());
		}
		if(message instanceof KEEPALIVE){
			if (keepAliveForward) {
				connection.getEndpoint().messageReceived(llrpMessage);
			}		
			if(keepAliveAck){
				session.write(new KEEPALIVE_ACK());
				return;
			}
			
		}
		
		if (llrpMessage instanceof READER_EVENT_NOTIFICATION) {
			 ConnectionAttemptEvent connectionAttemptEvent = ((READER_EVENT_NOTIFICATION)message).getReaderEventNotificationData().getConnectionAttemptEvent();
			 if(connectionAttemptEvent != null){
				 connectionAttemptEventQueue.add(connectionAttemptEvent);
				 connection.getEndpoint().messageReceived(llrpMessage);
				 return;
			 }
		}
		
		String expectedSyncMessage = (String) session.getAttribute(LLRPConnection.SYNC_MESSAGE_ANSWER);
		// send message only if not already handled by synchronous call
		if (!llrpMessage.getName().equals(expectedSyncMessage)){
			log.debug("Calling messageReceived of endpoint ... "+session);
			connection.getEndpoint().messageReceived(llrpMessage);
		}else{
			synMessageQueue.add(llrpMessage);
			log.debug("Adding message "+message.getClass()+" to transaction queue "+session);
		}
	}

	/**
	 * {@inheritDoc}
	 */
	
	public void messageSent(IoSession session, Object message)	throws java.lang.Exception {
		if (log.isInfoEnabled()) {
			log.info( "Message " + ((LLRPMessage)message).getName() + " successfully transmitted");
		}
		if (log.isDebugEnabled()) {
			log.debug(((LLRPMessage)message).toXMLString());
		}
		
	}
	
	/**
	 * {@inheritDoc}
	 */
	
	public void exceptionCaught(IoSession session, Throwable cause)
			throws Exception {
		connection.getEndpoint().errorOccured(cause.getClass().getName());
	}

	/**
	 * {@inheritDoc}
	 */
	
    public void sessionIdle( IoSession session, IdleStatus status ) throws Exception
    {
        System.out.println( "IDLE " + session.getIdleCount( status ));
    }

    /**
	 * {@inheritDoc}
	 */
    
	public BlockingQueue<LLRPMessage> getSynMessageQueue() {
		return synMessageQueue;
	}
	
	/**
	 * {@inheritDoc}
	 */
	
	public BlockingQueue<ConnectionAttemptEvent> getConnectionAttemptEventQueue() {
		return connectionAttemptEventQueue;
	}
	
	/**
	 * {@inheritDoc}
	 */
	
	public boolean isKeepAliveAck() {
		return keepAliveAck;
	}
	
	/**
	 * {@inheritDoc}
	 */
	
	public void setKeepAliveAck(boolean keepAliveAck) {
		this.keepAliveAck = keepAliveAck;
	}
	
	/**
	 * {@inheritDoc}
	 */
	
	public boolean isKeepAliveForward() {
		return keepAliveForward;
	}
	
	
	/**
	 * {@inheritDoc}
	 */
	
	public void setKeepAliveForward(boolean keepAliveForward) {
		this.keepAliveForward = keepAliveForward;
	}

	/**
	 * {@inheritDoc}
	 */
	
	public LLRPConnection getConnection() {
		return connection;
	}

	/**
	 * {@inheritDoc}
	 */
	
	public void setConnection(LLRPConnection connection) {
		this.connection = connection;
	}
	
}
