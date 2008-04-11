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

import org.apache.mina.common.IdleStatus;
import org.apache.mina.common.IoSession;
import org.llrp.ltk.types.LLRPMessage;

public class LLRPIoHandlerAdapterImpl extends LLRPIoHandlerAdapter{

	private LLRPConnection connection;
	private IoSession replySession;

	public LLRPIoHandlerAdapterImpl(LLRPConnection connection) {
		this.connection = connection;
	}

	public void messageReceived(IoSession session, Object message)
			throws Exception {
		// message is already LLRPMessage
		replySession = session;
		connection.messageReceived(session,(LLRPMessage) message);
	}

	public void messageSent(IoSession session, Object message)	throws java.lang.Exception {
		connection.messageSent();
	}
	
	public void send(LLRPMessage message) {
		if(replySession.isConnected()){
		replySession.write(message);
		} else {
			connection.errorOccured("session not connected");
		}
		
	}
	
	public void exceptionCaught(IoSession session, Throwable cause)
			throws Exception {
		connection.errorOccured(cause.getClass().getName());
	}

	public void close() {
		replySession.close();
	}

    @Override
    public void sessionIdle( IoSession session, IdleStatus status ) throws Exception
    {
        System.out.println( "IDLE " + session.getIdleCount( status ));
    }
}
