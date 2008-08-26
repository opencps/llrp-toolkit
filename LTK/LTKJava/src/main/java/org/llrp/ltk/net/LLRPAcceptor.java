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
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.LinkedBlockingQueue;
import java.util.concurrent.TimeUnit;

import org.apache.log4j.Logger;
import org.apache.mina.common.IdleStatus;
import org.apache.mina.common.IoAcceptor;
import org.apache.mina.common.IoHandlerAdapter;
import org.apache.mina.common.IoSession;
import org.apache.mina.common.WriteFuture;
import org.apache.mina.filter.LoggingFilter;
import org.apache.mina.filter.codec.ProtocolCodecFilter;
import org.apache.mina.transport.socket.nio.SocketAcceptor;
import org.apache.mina.transport.socket.nio.SocketAcceptorConfig;
import org.llrp.ltk.types.LLRPMessage;


/**
 * LLRPAcceptor implements a remotely initiated LLRP connection. 
 * 
 * Here is a simple code example:
 * <p>
 * <code> LLRPAcceptor c = new LLRPAcceptor(endpoint); </code> <p>
 * <code> c.bind(); </code> <p>
 * <code> // send message asynchronously </code> <p>
 * <code> c.send(llrpmessage); </code> <p>
 * <code> // send message synchronously </code> <p>
 * <code> LLRPMessage m = c.transact(llrpmessage); </code>
 * 
 * @author Basil Gasser - ETH Zurich
 * @author Christian Floerkemeier - MIT
 * 
 */
public class LLRPAcceptor extends IoHandlerAdapter implements LLRPConnection  {

	public static final int IDLE_TIME = 20;
	private static final String SYNC_MESSAGE_ANSWER = "synchronousMessageAnswer";
	private static final Logger log = Logger.getLogger(LLRPAcceptor.class);
	private LLRPIoHandlerAdapter handler;
	private int port = 5084;
	private IoAcceptor acceptor;
	private InetSocketAddress socketAddress;
	private LLRPEndpoint endpoint;
	private IoSession session;
	private BlockingQueue<LLRPMessage> synMessageQueue = new LinkedBlockingQueue<LLRPMessage>();
	private long synMessageTimeout = 0;

	public LLRPAcceptor() {
		super();
	}

	/**
	 * creates a remotely initiated LLRP connection on default PORT 5084 
	 * and uses LLRPIoHandlerAdapterImpl by default
	 * 
	 * @param LLRPEndpoint endpoint that handles incoming, asynchronous LLRP messages
	 */
	
	public LLRPAcceptor(LLRPEndpoint endpoint){
		handler = new LLRPIoHandlerAdapterImpl(this);
		this.endpoint = endpoint;
	}
	
	/**
	 * creates a remotely initiated LLRP connection and uses LLRPIoHandlerAdapterImpl by default
	 * 
	 * @param LLRPEndpoint endpoint that handles incoming, asynchronous LLRP messages
	 * @param Port on which LLRPAcceptor is waiting for incoming connections
	 */

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
		acceptor = new SocketAcceptor();
		acceptor.getFilterChain().addLast( "logger", new LoggingFilter() );
		acceptor.getFilterChain().addLast("codec", new ProtocolCodecFilter(new LLRPProtocolCodecFactory(LLRPProtocolCodecFactory.BINARY_ENCODING)));
		// MINA 2.0
		// acceptor.setHandler(handler);
		//acceptor.getSessionConfig().setReadBufferSize( 2048 );
		//acceptor.getSessionConfig().setIdleTime( IdleStatus.BOTH_IDLE, IDLE_TIME );
		
		// MINA 1.1
		SocketAcceptorConfig cfg = new SocketAcceptorConfig();
		cfg.getSessionConfig().setReceiveBufferSize(2048);
		
		try {        
			socketAddress = new InetSocketAddress(port);
			acceptor.bind(socketAddress, handler);
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
//		log.debug("message "+message.getClass()+" received in session "+session);
		this.session = session;
//		endpoint.messageReceived(message);
		
		String expectedSyncMessage = (String) session.getAttribute(SYNC_MESSAGE_ANSWER);
		// send message only if not already handled by synchronous call
		if (!message.getName().equals(expectedSyncMessage)){
			log.debug("message "+message.getClass()+" received in session "+session);
			endpoint.messageReceived(message);
		}else{
			synMessageQueue.add(message);
		}
	}


	public void close(){
		acceptor.unbind(socketAddress);
	}

	/**
	 * {@inheritDoc}
	 */
	public  void errorOccured(String message) {
		log.warn(message);
		endpoint.errorOccured(message);
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
			log.info("session is not yet connected");
			endpoint.errorOccured("session is not yet connected");
			return null;
		}

		WriteFuture writeFuture = session.write(message);
		writeFuture.join();

		// Wait until a message is received.
		try {
			returnMessage = synMessageTimeout==0?synMessageQueue.take():synMessageQueue.poll(synMessageTimeout, TimeUnit.MILLISECONDS);
			while(returnMessage!=null && !returnMessage.getName().equals(returnMessageType)){
				returnMessage = synMessageTimeout==0?synMessageQueue.take():synMessageQueue.poll(synMessageTimeout, TimeUnit.MILLISECONDS);
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

	public long getTransactionTimeout() {
		return synMessageTimeout;
	}

	public void setTransactionTimeout(long synMessageTimeout) {
		this.synMessageTimeout = synMessageTimeout;
	}


}
