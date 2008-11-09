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

import java.math.BigInteger;

import org.apache.log4j.Logger;
import org.apache.mina.common.ByteBuffer;
import org.apache.mina.common.IoSession;
import org.apache.mina.filter.codec.CumulativeProtocolDecoder;
import org.apache.mina.filter.codec.ProtocolDecoderOutput;
import org.llrp.ltk.generated.LLRPMessageFactory;
import org.llrp.ltk.types.LLRPMessage;

/**
	 * LLRPBinaryDecoder decodes incoming binary LLRP messages to LLRPMessage objects.
 */

public class LLRPBinaryDecoder extends CumulativeProtocolDecoder {

	private static final String MESSAGE_VERSION_KEY = "MessageVersion";
	private static final String MESSAGE_LENGTH_ARRAY = "LengthArray";
	private static final String MESSAGE_LENGTH_KEY = "MessageLength";
	private Logger log = Logger.getLogger(LLRPBinaryDecoder.class);

	
	@Override
	protected boolean doDecode(IoSession session, ByteBuffer in,
			ProtocolDecoderOutput out) throws Exception {
		// if 6 bytes in the buffer we can determine the next length to see
		// if buffer contains a completely delivered message.
		// in.getInt(2) will throw a BufferUnderflowException if there are not
		// even enough bytes to determine length
		int length = -1;
		byte[] lengthArray = null;
		byte[] version = null;
		if (in.remaining() >= 6
				&& session.getAttribute(MESSAGE_LENGTH_KEY) == null) {
			// enough bytes to decode length
			log.debug("determine length of message");
			version = new byte[2];
			version[0] = in.get();
			version[1] = in.get();
			lengthArray = new byte[4];
			lengthArray[0] = in.get();
			lengthArray[1] = in.get();
			lengthArray[2] = in.get();
			lengthArray[3] = in.get();
			length = new BigInteger(lengthArray).intValue();
			session.setAttribute(MESSAGE_LENGTH_ARRAY, lengthArray);
			session.setAttribute(MESSAGE_LENGTH_KEY, new Integer(length));
			session.setAttribute(MESSAGE_VERSION_KEY, version);
			// if the entire message is already available, call doDecode again.
			return (in.remaining()>=length-6);
		} else if (session.getAttribute(MESSAGE_LENGTH_KEY) != null) {
			log.debug("length already determined, see if enough bytes are available");
			length = ((Integer)session.getAttribute(MESSAGE_LENGTH_KEY)).intValue();
			version = (byte[]) session.getAttribute(MESSAGE_VERSION_KEY);
			lengthArray = (byte[]) session.getAttribute(MESSAGE_LENGTH_ARRAY);
			if (in.remaining() >= length-6) {
				// all bytes received to decode message
				byte[] msg = new byte[length];
				msg[0] = version[0];
				msg[1] = version[1];
				msg[2] = lengthArray[0];
				msg[3] = lengthArray[1];
				msg[4] = lengthArray[2];
				msg[5] = lengthArray[3];
				for (int i = 6; i < length; i++) {
					msg[i] = (byte) in.get();
				}
				log.debug("message completely received");
				log.debug("start decoding message");
				LLRPMessage message = LLRPMessageFactory.createLLRPMessage(msg);
				log.debug("message decoded: " + message.getClass());
				out.write(message);
				session.removeAttribute(MESSAGE_LENGTH_ARRAY);
				session.removeAttribute(MESSAGE_LENGTH_KEY);
				session.removeAttribute(MESSAGE_VERSION_KEY);
				// there might be an other message to be decoded

				// see if there's another completly delivered message in the
				// buffer
				// in this case, we would have to return true
				try {
					if (in.remaining() >= 6) {
						version = new byte[2];
						version[0] = in.get();
						version[1] = in.get();
						lengthArray = new byte[4];
						lengthArray[0] = in.get();
						lengthArray[1] = in.get();
						lengthArray[2] = in.get();
						lengthArray[3] = in.get();
						length = new BigInteger(lengthArray).intValue();
						session.setAttribute(MESSAGE_LENGTH_ARRAY, lengthArray);
						session.setAttribute(MESSAGE_LENGTH_KEY, new Integer(
								length));
						session.setAttribute(MESSAGE_VERSION_KEY, version);
						if (in.remaining() - in.markValue() >= length-6) {
							log.debug("another message already in the buffer");
							return true;
						} else {
							log.debug("message not yet completly delivered");
							return false;
						}
					}
				} catch (Exception e) {
					// not enough bytes to determine length
					log.debug("not enough bytes to determine message length");
					return false;
				}
			} else {
				// not enough bytes to determin length

				log.debug("not enough bytes to determine message length");
				return false;
			}
		} else {
			log.debug("not enough bytes to determine length");
			return false;
		}
		return false;
	}

}
