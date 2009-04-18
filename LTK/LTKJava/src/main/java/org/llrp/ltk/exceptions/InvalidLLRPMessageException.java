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
package org.llrp.ltk.exceptions;

import java.io.IOException;

import org.xml.sax.SAXException;


/**
 * Main LLRP Exception - this exception is thrown whenever an invalid
 * LLRP message is detected during binary or XML message encoding and decoding. 
 * The validation of a binary message is based on the 
 * rules specified in the llrpdef.xml. When encoding and decoding LLRP
 * messages in the LTK-XML message format, this exception is also thrown
 * if the message cannot be validated against the LLRP.xsd schema.
 *
 * @author Basil Gasser - ETH Zurich
 * @author Christian Floerkemeier - MIT
 */
public class InvalidLLRPMessageException extends Exception {
    /**
         * Creates a new LLRPException object.
         *
         * @param message string
         */
    public InvalidLLRPMessageException(final String message) {
        super(message);
    }

	public InvalidLLRPMessageException(String string, Exception e) {
		super(string,e);
	}

}
