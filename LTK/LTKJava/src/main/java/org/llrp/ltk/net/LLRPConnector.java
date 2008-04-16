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

import org.apache.log4j.Logger;
import org.apache.mina.common.CloseFuture;
import org.apache.mina.common.ConnectFuture;
import org.apache.mina.common.IoSession;
import org.apache.mina.common.ReadFuture;
import org.apache.mina.filter.codec.ProtocolCodecFilter;
import org.apache.mina.transport.socket.nio.NioSocketConnector;
import org.llrp.ltk.types.LLRPMessage;

/**
 * LLRPConnector implements a self-initiated LLRP connection. 
 * 
 * Here is a simple code example:
 * <p>
 * <code> LLRPConnection c = new LLRPConnector(endpoint,ip); </code> <p>
 * <code> c.connect(); </code> <p>
 * <code> // send message asynchronously </code> <p>
 * <code> c.send(llrpmessage); </code> <p>
 * <code> // send message synchronously </code> <p>
 * <code> LLRPMessage m = c.transact(llrpmessage); </code>
 */

public class LLRPConnector implements LLRPConnection{

	public static final int CONNECT_TIMEOUT = 3000;
	private static final String SYNC_MESSAGE_ANSWER = "synchronousMessageAnswer";
	private Logger log = Logger.getLogger(LLRPConnector.class);
	private String host;
	private int port = 5084;
	private org.apache.mina.transport.socket.SocketConnector connector;
	private LLRPIoHandlerAdapter handler;

	private ConnectFuture future;
	private LLRPEndpoint endpoint;

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
	public void connect(){
		connector = new NioSocketConnector();
		connector.getFilterChain().addLast("codec", new ProtocolCodecFilter(new LLRPProtocolCodecFactory(LLRPProtocolCodecFactory.BINARY_ENCODING)));
		connector.setHandler(handler);
		InetSocketAddress add = new InetSocketAddress(host, port);
		future = connector.connect(add);
		future.awaitUninterruptibly();
	}

	public void disconnect(){
		IoSession session = future.getSession();
		if (session != null && session.isConnected()){
			CloseFuture future = session.close();
			future.awaitUninterruptibly();
		}
	}

	/**
	 * {@inheritDoc}
	 */
	public void messageReceived(IoSession session, LLRPMessage message){
		String expectedSyncMessage = (String) session.getAttribute(SYNC_MESSAGE_ANSWER);
		// send message only if not already handled by synchronous call
		if (!message.getName().equals(expectedSyncMessage)){
			log.debug("message "+message.getClass()+" received in session "+session);
			endpoint.messageReceived(message);
		}
	}

	/**
	 * {@inheritDoc}
	 */
	public LLRPMessage transact(LLRPMessage message) {
		IoSession session = future.getSession();
		String returnMessageType = message.getResponseType();
		if (returnMessageType.equals("")){
			endpoint.errorOccured("message does not expect return message");
			return null;
		}
		session.setAttribute(SYNC_MESSAGE_ANSWER, returnMessageType);
		LLRPMessage returnMessage = null;
		if (!future.isConnected()){
			future = connector.connect();
			future.awaitUninterruptibly();
			session = future.getSession();
			log.info("new session created");
		}
		// useReadOperation must be enabled to use read operation.
		session.getConfig().setUseReadOperation(true);
		session.write(message);
		ReadFuture future = session.read();
		// Wait until a message is received.
		try {
			future.await();
			returnMessage = (LLRPMessage) future.getMessage();
			while(!returnMessage.getName().equals(returnMessageType)){
				future = session.read();
				future.await();
				returnMessage = (LLRPMessage) future.getMessage();
			}
			session.removeAttribute(SYNC_MESSAGE_ANSWER);
		} catch (Exception e) {
			e.printStackTrace();
		}
		return returnMessage;
	}

	/**
	 * {@inheritDoc}
	 */
	public void send(LLRPMessage message) {
		IoSession session = future.getSession();
		if (!future.isConnected()){
			future = connector.connect();
			future.awaitUninterruptibly();
			session = future.getSession();
			log.info("new session created");
		}
		session.write(message);
	}

	/**
	 * {@inheritDoc}
	 */
	public void errorOccured(String message) {
		throw new RuntimeException(message);
	}

	/**
	 * {@inheritDoc}
	 */
	public void messageSent() {
		log.debug("message transmitted");
	}

}
