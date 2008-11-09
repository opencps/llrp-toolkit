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
import org.apache.mina.filter.codec.ProtocolCodecFactory;
import org.apache.mina.filter.codec.ProtocolDecoder;
import org.apache.mina.filter.codec.ProtocolEncoder;

/**
 * Codec Factory for LLRP protocol
 */

public class LLRPProtocolCodecFactory implements ProtocolCodecFactory {

	public static final int BINARY_ENCODING = 1;
	private final ProtocolEncoder encoder;
	private final ProtocolDecoder decoder;


	/**
	 * LLRPProtocolCodecFactory supports a single encoder/decoder type only: 
	 * The binary message binding in the LLRP specification.
	 * 
	 * @param type Codec to be used (currently only LLRPProtocolCodecFactory.BINARY_ENCODING is supported)
	 */

	public LLRPProtocolCodecFactory(int type) {
		switch (type) {
		case BINARY_ENCODING:

			encoder = new LLRPBinaryEncoder();
			decoder = new LLRPBinaryDecoder();
			break;
		default:
			throw new IllegalArgumentException(
			"only BINARY_ENCODING type supported");
		}
	}

	/**
	 * get protocol decoder for session specified. There is currently only a LLRPBinaryDecoder supported. 
	 *
	 * @param session
	 * @return ProtocolDecoder
	 */
	public ProtocolDecoder getDecoder(IoSession session) throws Exception {
		return decoder;
	}

	/**
	 * get protocol encoder for session specified. There is currently only a LLRPBinaryEncoder supported. 
	 *
	 * @param session
	 * @return ProtocolEncoder
	 */
	public ProtocolEncoder getEncoder(IoSession session) throws Exception {
		return encoder;
	}

	/**
	 * get protocol decoder. There is currently only a LLRPBinaryDecoder supported. 
	 *
	 * @return ProtocolDecoder
	 */
	public ProtocolDecoder getDecoder() throws Exception {
		return decoder;
	}

	/**
	 * get protocol encoder. There is currently only a LLRPBinaryEncoder supported. 
	 *
	 * @return ProtocolEncoder
	 */
	public ProtocolEncoder getEncoder() throws Exception {
		return encoder;
	}
	

}
