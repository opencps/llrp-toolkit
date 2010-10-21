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
import java.util.concurrent.TimeUnit;
import java.util.concurrent.TimeoutException;

import org.apache.log4j.Logger;
import org.apache.mina.common.IoSession;
import org.apache.mina.common.WriteFuture;
import org.llrp.ltk.generated.enumerations.ConnectionAttemptStatusType;
import org.llrp.ltk.generated.parameters.ConnectionAttemptEvent;
import org.llrp.ltk.types.LLRPMessage;

/**
 * LLRPConnection represents an abstract interface for an LLRP connection at a LLRP reader or client. 
 * The actual implementation differ depending on whether it is a self-initiated connection 
 * or a remotely initiated connection.
 */

public abstract class LLRPConnection {
	public static final int CONNECT_TIMEOUT = 10000;
	public static final String SYNC_MESSAGE_ANSWER = "synchronousMessageAnswer";	
	protected LLRPEndpoint endpoint;
	protected LLRPIoHandlerAdapter handler;
	protected IoSession session;
	private Logger log = Logger.getLogger(LLRPConnection.class);
	
	public LLRPConnection(){
		handler = new LLRPIoHandlerAdapterImpl(this);
	}
	
	/**
	 * check whether ConnectionAttemptStatus in READER_NOTIFICATION message was set by reader
	 * to 'Success'. 
	 * 
	 * @param timeout	the wait time before reader replies with a status report
	 * @throws LLRPConnectionAttemptFailedException
	 */
	protected void checkLLRPConnectionAttemptStatus(long timeout)throws LLRPConnectionAttemptFailedException{
		try{
			BlockingQueue<ConnectionAttemptEvent> connectionAttemptEventQueue = handler.getConnectionAttemptEventQueue();
			ConnectionAttemptEvent connectionAttemptEvent = connectionAttemptEventQueue.poll(timeout, TimeUnit.MILLISECONDS);
			if(connectionAttemptEvent != null){
				ConnectionAttemptStatusType status = connectionAttemptEvent.getStatus();
				if(status.intValue() == ConnectionAttemptStatusType.Success){
					log.info("LLRP reader reported successfull connection attempt (ConnectionAttemptEvent.Status = " + status.toString() + ")");
				}else{
					log.info("LLRP reader reported failed connection attempt (ConnectionAttemptStatus = " + status.toString() + ")");
					throw new LLRPConnectionAttemptFailedException(status.toString());
				}
			} else{
				throw new LLRPConnectionAttemptFailedException("Connection request timed out after " + timeout + " ms.");
			}
		}catch(InterruptedException e){
			e.printStackTrace();
			throw new LLRPConnectionAttemptFailedException(e.getMessage());
		}
	}

	/**
	 * reconnect to existing connection
	 * 
	 * @return boolean indicating failure (false) or success (true)
	 */
	public abstract boolean reconnect();
	
	/**
	 * sends an LLRP message without waiting for a response message.
	 * 
	 * @param message LLRP message to be sent
	 */
	public void send(LLRPMessage message){
		if (session == null){
			log.warn("session is not yet established");
			endpoint.errorOccured("session is not yet established");
			return;
		}
		
		if(!session.isConnected()){
			if(reconnect()){
				session.write(message);
			}else{
				log.info("session is not yet connected");
				endpoint.errorOccured("session is not yet connected");
			}
		}else{
			session.write(message);
		}
		
	}
	
	/**
	 * sends an LLRP message and returns the response message as defined in the 
	 * LLRP specification. 
	 * 
	 * @param message LLRP message to be sent
	 * @return message LLRP response message
	 */
	public LLRPMessage transact(LLRPMessage message) throws TimeoutException{
		return transact(message,0);
	}
	/**
	 * sends an LLRP message and returns the response message as defined in the 
	 * LLRP specification timing out after the time interval specified.
	 * 
	 * @param message LLRP message to be sent
	 * @param transactionTimeout  timeout
	 * @return message LLRP response message
	 */
	public LLRPMessage transact(LLRPMessage message,long transactionTimeout) throws TimeoutException{
		String returnMessageType = message.getResponseType();
		if (returnMessageType.equals("")){
			endpoint.errorOccured("message does not expect return message");
			return null;
		}
		if (session == null){
			log.warn("session is not yet established");
			endpoint.errorOccured("session is not yet established");
			return null;
		}
		
		LLRPMessage returnMessage = null;
		if (!session.isConnected()){
			if(!reconnect()){//reconnect failed
				log.info("session is not yet connected");
				endpoint.errorOccured("session is not yet connected");
				return null;
			}
		}

		// move setAttribute here from above block to avoid the risk of overwriting session where SYNC_MESSAGE_ANSWER is already set
		session.setAttribute(SYNC_MESSAGE_ANSWER, returnMessageType);
		
		WriteFuture writeFuture = session.write(message);
		log.info(message.getName() + " transact ....");
		writeFuture.join();
		
		
		// Wait until a message is received.
		try {
			BlockingQueue<LLRPMessage> synMessageQueue = handler.getSynMessageQueue();
			returnMessage = transactionTimeout==0?synMessageQueue.take():synMessageQueue.poll(transactionTimeout, TimeUnit.MILLISECONDS);
			// if message received was not expected message, wait for next message (restart timer)
			while(returnMessage!=null && !returnMessage.getName().equals(returnMessageType)){
				returnMessage = transactionTimeout==0?synMessageQueue.take():synMessageQueue.poll(transactionTimeout, TimeUnit.MILLISECONDS);
			}
			session.removeAttribute(SYNC_MESSAGE_ANSWER);
			if (returnMessage == null){
				throw new TimeoutException("Request timed out after " + transactionTimeout + " ms.");
			}
		} catch (InterruptedException e) {
			e.printStackTrace();
		}
		return returnMessage;
	}
	
	/**
	 * returns the endpoint which receives incoming LLRPMessages
	 * 
	 * @return the endpoint
	 */
	public LLRPEndpoint getEndpoint() {
		return endpoint;
	}

	/**
	 * sets the endpoint which receives incoming LLRPMessages
	 * 
	 * @param endpoint the endpoint to set
	 */
	public void setEndpoint(LLRPEndpoint endpoint) {
		this.endpoint = endpoint;
	}
	
	/**
	 * returns the handler that handles incoming LLRPMessages and forwards them to LLRPEndpoint registered.
	 * 
	 * @return the handler
	 */
	public LLRPIoHandlerAdapter getHandler() {
		return handler;
	}

	/**
	 * sets the handler that handles incoming LLRPMessages and forwards them to LLRPEndpoint registered.
	 * 
	 * @param handler the handler to set
	 */
	public void setHandler(LLRPIoHandlerAdapter handler) {
		this.handler = handler;
	}
	
	
}
