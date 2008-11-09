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

import org.apache.log4j.Logger;
import org.apache.mina.common.ByteBuffer;
import org.apache.mina.common.IoSession;
import org.apache.mina.filter.codec.ProtocolEncoder;
import org.apache.mina.filter.codec.ProtocolEncoderOutput;
import org.llrp.ltk.exceptions.InvalidLLRPMessageException;
import org.llrp.ltk.types.LLRPMessage;

/**
	 * LLRPBinaryEncoder encodes LLRPMessage objects to the LLRP binary format.
 */

public class LLRPBinaryEncoder implements ProtocolEncoder {

	Logger log = Logger.getLogger(LLRPBinaryEncoder.class);

	public void dispose(IoSession session) throws Exception {
		// nothing to dispose
	}

	/**
	 * convert LLRPMessage object to binary format
	*/
	
	public void encode(IoSession session, Object message,
			ProtocolEncoderOutput out) throws Exception {
		LLRPMessage llrp = (LLRPMessage) message;
		log.debug("encoding message " + llrp.getClass());
		byte[] byteMsg;
		try {
			byteMsg = llrp.encodeBinary();
		} catch (InvalidLLRPMessageException me) {
			log.warn("no message written because error occured: "
					+ me.getMessage());
			return;
		}
		// Note: ByteBuffer is renamed in MINA to IOBuffer
		ByteBuffer buffer = ByteBuffer.allocate(byteMsg.length, false);
		buffer.put(byteMsg);
		buffer.flip();
		out.write(buffer);
	}

}
