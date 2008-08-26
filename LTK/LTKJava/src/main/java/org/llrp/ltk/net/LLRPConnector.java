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

import java.net.InetSocketAddress;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.LinkedBlockingQueue;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.TimeoutException;

import org.apache.log4j.Logger;
import org.apache.mina.common.CloseFuture;
import org.apache.mina.common.ConnectFuture;
import org.apache.mina.common.IoSession;
import org.apache.mina.common.WriteFuture;
import org.apache.mina.filter.codec.ProtocolCodecFilter;
import org.apache.mina.transport.socket.nio.SocketConnector;
import org.llrp.ltk.types.LLRPMessage;

/**
 * LLRPConnector implements a self-initiated LLRP connection. 
 * 
 * Here is a simple code example:
 * <p>
 * <code> LLRPConnector c = new LLRPConnector(endpoint,ip); </code> <p>
 * <code> c.connect(); </code> <p>
 * <code> // send message asynchronously - response needs to handled via  </code> <p>
 * <code> the message received method of the endpoint </code> <p>
 * <code> c.send(llrpmessage); </code> <p>
 * <code>  </code> <p>
 * <code> // send message synchronously </code> <p>
 * <code> LLRPMessage m = c.transact(llrpmessage); </code>
 */

public class LLRPConnector implements LLRPConnection{

	public static final int CONNECT_TIMEOUT = 3000;
	private static final String SYNC_MESSAGE_ANSWER = "synchronousMessageAnswer";
	private Logger log = Logger.getLogger(LLRPConnector.class);
	private String host;
	private int port = 5084;
	private org.apache.mina.transport.socket.nio.SocketConnector connector;
	private LLRPIoHandlerAdapter handler;

	private ConnectFuture future;
	private LLRPEndpoint endpoint;
	
	private BlockingQueue<LLRPMessage> synMessageQueue = new LinkedBlockingQueue<LLRPMessage>();
	private long synMessageTimeout = 10000;

	public LLRPConnector() {
		super();
	}

	public LLRPConnector(LLRPEndpoint endpoint, String host, int port) {
		this.host = host;
		this.port = port;
		this.endpoint = endpoint;
		handler = new LLRPIoHandlerAdapterImpl(this);
	}

	public LLRPConnector(LLRPEndpoint endpoint, String host) {
		this.host = host;
		this.endpoint = endpoint;
		handler = new LLRPIoHandlerAdapterImpl(this);
	}
	
	public LLRPConnector(LLRPEndpoint endpoint, String host, LLRPIoHandlerAdapter handler) {
		this.host = host;
		this.endpoint = endpoint;
		this.handler = handler;
	} 
	
	public LLRPConnector(LLRPEndpoint endpoint, String host, int port, LLRPIoHandlerAdapter handler) {
		this.host = host;
		this.port = port;
		this.endpoint = endpoint;
		this.handler = handler;
	} 
	
	public void connect(){
		connector = new SocketConnector();
		connector.getFilterChain().addLast("codec", new ProtocolCodecFilter(new LLRPProtocolCodecFactory(LLRPProtocolCodecFactory.BINARY_ENCODING)));
		// MINA 2.0 method 
		//connector.setHandler(handler);
		InetSocketAddress add = new InetSocketAddress(host, port);
		future = connector.connect(add,handler);
		// MINA 2.0
		//future.awaitUninterruptibly();
	}

	public void disconnect(){
		IoSession session = future.getSession();
		if (session != null && session.isConnected()){
			CloseFuture future = session.close();
			// MINA 2.0
			// future.awaitUninterruptibly();
		}
	}

	/**
	 * {@inheritDoc}
	 */
	public void messageReceived(IoSession session, LLRPMessage message){
		String expectedSyncMessage = (String) session.getAttribute(SYNC_MESSAGE_ANSWER);
		log.debug("message "+message.getClass()+" received in session "+session);
		// send message only if not already handled by synchronous call
		if (!message.getName().equals(expectedSyncMessage)){
			log.debug("Calling messageReceived of endpoint ... "+session);
			endpoint.messageReceived(message);
		}else{
			synMessageQueue.add(message);
			log.debug("Adding message "+message.getClass()+" to transaction queue "+session);
		}
	}


	/**
	 * {@inheritDoc}
	 */
	public void send(LLRPMessage message) {
		IoSession session = future.getSession();
		if (!future.isConnected()){
			future = connector.connect(session.getServiceAddress(),session.getHandler());
			// MINA 2.0
			// future = connector.connect();
			// future.awaitUninterruptibly();
			session = future.getSession();
			log.info("new session created");
		}
		session.write(message);
	}

	/**
	 * {@inheritDoc}
	 */
	public void errorOccured(String message) {
		//throw new RuntimeException(message);
		endpoint.errorOccured(message);
	}

	/**
	 * {@inheritDoc}
	 */
	public void messageSent() {
		log.debug("message transmitted");
	}

	/**
	 * @return the endpoint
	 */
	public LLRPEndpoint getEndpoint() {
		return endpoint;
	}

	/**
	 * @param endpoint the endpoint to set
	 */
	public void setEndpoint(LLRPEndpoint endpoint) {
		this.endpoint = endpoint;
	}

	/**
	 * @return the handler
	 */
	public LLRPIoHandlerAdapter getHandler() {
		return handler;
	}

	/**
	 * @param handler the handler to set
	 */
	public void setHandler(LLRPIoHandlerAdapter handler) {
		this.handler = handler;
	}

	/**
	 * @return the host
	 */
	public String getHost() {
		return host;
	}

	/**
	 * @param host the host to set
	 */
	public void setHost(String host) {
		this.host = host;
	}

	/**
	 * @return the port
	 */
	public int getPort() {
		return port;
	}

	/**
	 * @param port the port to set
	 */
	public void setPort(int port) {
		this.port = port;
	}
	
	/**
	 * {@inheritDocs}
	 */
	public LLRPMessage transact(LLRPMessage message) throws TimeoutException {
		IoSession session = future.getSession();
		if(session == null){
			endpoint.errorOccured("session is not yet established");
			return null;
		}
		String returnMessageType = message.getResponseType();
		if (returnMessageType.equals("")){
			endpoint.errorOccured("message does not expect return message");
			return null;
		}
		session.setAttribute(SYNC_MESSAGE_ANSWER, returnMessageType);
		LLRPMessage returnMessage = null;
		if (!future.isConnected()){
			future = connector.connect(session.getServiceAddress(),session.getHandler());
			session = future.getSession();
			log.info("new session created");
		}
		
		
		WriteFuture writeFuture = session.write(message);
		log.info(message.getName() + " sent ....");
		writeFuture.join();

		// Wait until a message is received.
		try {
			returnMessage = synMessageTimeout==0?synMessageQueue.take():synMessageQueue.poll(synMessageTimeout, TimeUnit.MILLISECONDS);
			// if message received was not expected message, wait for next message (restart timer)
			while(returnMessage!=null && !returnMessage.getName().equals(returnMessageType)){
				returnMessage = synMessageTimeout==0?synMessageQueue.take():synMessageQueue.poll(synMessageTimeout, TimeUnit.MILLISECONDS);
			}
			session.removeAttribute(SYNC_MESSAGE_ANSWER);
			if (returnMessage == null){
				throw new TimeoutException("Request timed out after " + synMessageTimeout + " ms.");
			}
		} catch (InterruptedException e) {
			e.printStackTrace();
		}
				
		return returnMessage;

	}
	
	/**
	 * {@inheritDocs}
	 */
	public long getTransactionTimeout() {
		return synMessageTimeout;
	}

	/**
	 * {@inheritDocs}
	 */
	public void setTransactionTimeout(long synMessageTimeout) {
		this.synMessageTimeout = synMessageTimeout;
	}

}
