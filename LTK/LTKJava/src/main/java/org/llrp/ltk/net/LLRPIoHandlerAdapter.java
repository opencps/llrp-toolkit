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
import org.apache.mina.common.IoHandlerAdapter;
import org.apache.mina.common.IoSession;
import org.llrp.ltk.generated.messages.KEEPALIVE;
import org.llrp.ltk.generated.messages.KEEPALIVE_ACK;
import org.llrp.ltk.generated.messages.READER_EVENT_NOTIFICATION;
import org.llrp.ltk.generated.parameters.ConnectionAttemptEvent;
import org.llrp.ltk.types.LLRPMessage;

public class LLRPIoHandlerAdapter extends IoHandlerAdapter{
	private Logger log = Logger.getLogger(LLRPConnection.class);	
	private LLRPConnection connection;
	private BlockingQueue<LLRPMessage> synMessageQueue = new LinkedBlockingQueue<LLRPMessage>();
	private BlockingQueue<ConnectionAttemptEvent> connectionAttemptEventQueue = new LinkedBlockingQueue<ConnectionAttemptEvent>(1);
	private boolean keepAliveAck = true;

	public LLRPIoHandlerAdapter(LLRPConnection connection) {
		this.connection = connection;
	}
	public void sessionOpened(IoSession session) throws Exception {
		log.debug("session is opened:"+session);
		this.connection.session = session;
	}

	public void messageReceived(IoSession session, Object message)
			throws Exception {		
		LLRPMessage llrpMessage = (LLRPMessage) message;
		log.debug("message "+message.getClass()+" received in session "+session);
		if(message instanceof KEEPALIVE){
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

	public void messageSent(IoSession session, Object message)	throws java.lang.Exception {
		log.debug("message transmitted");
	}
	
	
	public void exceptionCaught(IoSession session, Throwable cause)
			throws Exception {
		connection.getEndpoint().errorOccured(cause.getClass().getName());
	}

    @Override
    public void sessionIdle( IoSession session, IdleStatus status ) throws Exception
    {
        System.out.println( "IDLE " + session.getIdleCount( status ));
    }

	public BlockingQueue<LLRPMessage> getSynMessageQueue() {
		return synMessageQueue;
	}
	
	public BlockingQueue<ConnectionAttemptEvent> getConnectionAttemptEventQueue() {
		return connectionAttemptEventQueue;
	}
	
	public boolean isKeepAliveAck() {
		return keepAliveAck;
	}
	public void setKeepAliveAck(boolean keepAliveAck) {
		this.keepAliveAck = keepAliveAck;
	}
}
