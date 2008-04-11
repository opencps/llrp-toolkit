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

import org.apache.mina.common.IoSession;
import org.llrp.ltk.types.LLRPMessage;

/**
 * LLRPConnection represents an interface for an LLRP connection at a LLRP reader or client. 
 * The actual implementation differ depending on whether it is a self-initiated connection 
 * or a remotely initiated connection.
 */

public interface LLRPConnection {

	/**
	 * is called by the IoHandlerAdapter whenever a message is received on the connection.
	 * 
	 * @param IoSession the session where the message was received
	 * @param message the LLRPMessage received
	 */
	public void messageReceived(IoSession session, LLRPMessage message);
	
	/**
	 * is called by the IoHandlerAdapter whenever a message is successfully transmitted.
	 * 
	 */
	public void messageSent();
	
	/**
	 * is called by the IoHandlerAdapter whenever an error ocurred.
	 * 
	 * @param message	error message
	 */
	public void errorOccured(String message);
	
	/**
	 * sends an LLRP message and returns the response message as defined in the 
	 * LLRP specification.
	 * 
	 * @param message LLRP message to be sent
	 * @returns message LLRP response message
	 */
	public LLRPMessage transact(LLRPMessage message);
	
	/**
	 * sends an LLRP message without waiting for a response message.
	 * 
	 * @param message LLRP message to be sent
	 */
	public void send(LLRPMessage message);
}
