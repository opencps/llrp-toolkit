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
 * See the License for the specific language governing permissions and limitations under the License.
 */
package org.llrp.ltk.types;

import org.jdom.Content;
import org.jdom.Element;
import org.jdom.Namespace;
import org.llrp.ltk.exceptions.InvalidLLRPMessageException;


/**
 * Representing an LLRPParameter uniquely identified by its type number.
 * Call empty constructor to create new parameter. Use constructor taking
 * LLRPBitList or byte[] as a parameter to instantiate parameter from binary encoding. 
 * Use constructor taking JDOM element (www.jdom.org) to create parameter from XML encoding
 * See also TLVParameter and TVParameter
 *
 * @author Basil Gasser - ETH Zurich
 * @author Christian Floerkemeier - MIT
 */
public abstract class LLRPParameter {
    protected UnsignedShort bitLength = new UnsignedShort();

    /**
     * type number uniquely identifies a parameter.
     *
     * @return typeNum
     */
    public abstract SignedShort getTypeNum();

    /**
	 * name of parameter (same as class name)
	 * 
	 * @return String
	 */
	public abstract String getName();
    
    /**
     * create objects from binary.
     *
     * @param list - created by calling encodeBinary()
     */
    public abstract void decodeBinary(LLRPBitList list);

    /**
     * create binary representation of this parameter.
     *
     * @return LLRPBitList - a list of bits
     */
    public abstract LLRPBitList encodeBinary();

    /**
     * create xml representation of this parameter.
     *
     * @param name of element
     * @param ns Namespace of elements
     */
    public abstract Content encodeXML(String name, Namespace ns);

    /**
     * create objects from xml.
     *
     * @param element to be decoded
     */
    public abstract void decodeXML(Element element)  throws InvalidLLRPMessageException ;

    /**
     * length in bytes.
     *
     * @return length of message in bytes (octet)
     */
    public int getByteLength() {
        if ((bitLength == null) || bitLength.equals(new UnsignedShort(0))) {
            bitLength = new UnsignedShort(encodeBinary().length());
        }

        return (bitLength.toShort() / 8);
    }
}
