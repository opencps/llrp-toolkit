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

import java.io.IOException;
import java.net.InetSocketAddress;

import org.apache.log4j.Logger;
import org.apache.mina.common.IdleStatus;
import org.apache.mina.common.IoAcceptor;
import org.apache.mina.common.IoHandlerAdapter;
import org.apache.mina.common.IoSession;
import org.apache.mina.common.ReadFuture;
import org.apache.mina.filter.codec.ProtocolCodecFilter;
import org.apache.mina.filter.logging.LoggingFilter;
import org.apache.mina.transport.socket.nio.NioSocketAcceptor;
import org.llrp.ltk.types.LLRPMessage;


/**
 * LLRPAcceptor implements a remotely initiated LLRP connection. 
 * 
 * Here is a simple code example:
 * <p>
 * <code> LLRPConnection c = new LLRPAcceptor(endpoint); </code> <p>
 * <code> c.bind(); </code> <p>
 * <code> // send message asynchronously </code> <p>
 * <code> c.send(llrpmessage); </code> <p>
 * <code> // send message synchronously </code> <p>
 * <code> LLRPMessage m = c.transact(llrpmessage); </code>
 */
public class LLRPAcceptor extends IoHandlerAdapter implements LLRPConnection  {

	public static int IDLE_TIME = 20;
	private static final String SYNC_MESSAGE_ANSWER = "synchronousMessageAnswer";
	private Logger log = Logger.getLogger(LLRPAcceptor.class);
	private LLRPIoHandlerAdapter handler;
	private int port = 5084;
	private IoAcceptor acceptor;
	private InetSocketAddress socketAddress;
	private LLRPEndpoint endpoint;
	private IoSession session;



	public LLRPAcceptor(LLRPEndpoint endpoint){
		handler = new LLRPIoHandlerAdapterImpl(this);
		this.endpoint = endpoint;
	}

	public LLRPAcceptor(LLRPEndpoint endpoint, int port){
		this.endpoint = endpoint;
		this.port = port;
		handler = new LLRPIoHandlerAdapterImpl(this);
	}

	public LLRPAcceptor(LLRPEndpoint endpoint, int port, LLRPIoHandlerAdapter handler){
		this.endpoint = endpoint;
		this.port = port;
		this.handler = handler;
	}

	public LLRPAcceptor(LLRPEndpoint endpoint, LLRPIoHandlerAdapter handler){
		this.endpoint = endpoint;
		this.handler = handler;
	}

	public void bind(){
		acceptor = new NioSocketAcceptor();
		acceptor.getFilterChain().addLast( "logger", new LoggingFilter() );
		acceptor.getFilterChain().addLast("codec", new ProtocolCodecFilter(new LLRPProtocolCodecFactory(LLRPProtocolCodecFactory.BINARY_ENCODING)));
		acceptor.setHandler(handler);
		acceptor.getSessionConfig().setReadBufferSize( 2048 );
		acceptor.getSessionConfig().setIdleTime( IdleStatus.BOTH_IDLE, IDLE_TIME );
		try {        
			socketAddress = new InetSocketAddress(port);
			acceptor.bind(socketAddress);
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		log.info("server listening on port "+port);
	}

	/**
	 * {@inheritDoc}
	 */
	public void messageReceived(IoSession session, LLRPMessage message) {
		log.debug("message "+message.getClass()+" received in session "+session);
		this.session = session;
		endpoint.messageReceived(message);
	}


	public void close(){
		acceptor.unbind(socketAddress);
	}

	/**
	 * {@inheritDoc}
	 */
	public  void errorOccured(String message) {
		log.warn(message);
	}

	/**
	 * {@inheritDoc}
	 */
	public void messageSent() {
		log.debug("message transmitted");
	}

	/**
	 * {@inheritDoc}
	 */
	public LLRPMessage transact(LLRPMessage message) {
		String returnMessageType = message.getResponseType();
		if (returnMessageType.equals("")){
			endpoint.errorOccured("message does not expect return message");
			log.debug("message does not expect return message");
			return null;
		}
		if (session == null){
			log.warn("session is not yet established");
			endpoint.errorOccured("session is not yet established");
			return null;
		}
		session.setAttribute(SYNC_MESSAGE_ANSWER, returnMessageType);
		LLRPMessage returnMessage = null;
		if (!session.isConnected()){
			log.info("new session created");
			endpoint.errorOccured("new session created");
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
				log.info("message received does not match expected type; received "+returnMessage.getName()+", expected "+returnMessageType);
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
		handler.send(message);
	}


}
