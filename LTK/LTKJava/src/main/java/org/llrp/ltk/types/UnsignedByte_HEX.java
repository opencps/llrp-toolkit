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

import java.math.BigInteger;

import org.jdom.Content;
import org.jdom.Element;
import org.jdom.Namespace;
import org.jdom.Text;

/**
 * unsigned 16 bit short
 * 
 * @author Basil Gasser - ETH Zurich
 */
public class UnsignedByte_HEX extends UnsignedByte {
	private static final Integer LENGTH = 8;

	/**
	 * Creates a new UnsignedByte_HEX object.
	 * 
	 * @param value
	 *            to set
	 */
	public UnsignedByte_HEX(int value) {
		if (value < 0) {
			this.value = (2*Byte.MAX_VALUE+value+2);
		} else {
			this.value = value;
		}
		signed = false;
		if (!inRange(this.value)) {
			throw new IllegalArgumentException("value " + value
					+ " not in range allowed for UnsignedByte_HEX");
		}
	}

	/**
	 * Creates a new UnsignedByte_HEX object from byte
	 * 
	 * @param value
	 *            interpreted as unsigned byte
	 */
	public UnsignedByte_HEX(byte value) {
		this((int) value);
	}

	/**
	 * Creates a new UnsignedByte_HEX object.
	 * 
	 * @param valueString
	 *            value as string
	 */
	public UnsignedByte_HEX(String valueString) {
		this(Integer.parseInt(valueString,16));
		if (!inRange(value)) {
			throw new IllegalArgumentException("value " + valueString
					+ " not in range");
		}
	}

	/**
	 * Creates a new UnsignedByte_HEX object.
	 * 
	 * @param bitList
	 *            to be decoded
	 */
	public UnsignedByte_HEX(LLRPBitList bitList) {
		decodeBinary(bitList);
		signed = false;
	}

	/**
	 * Creates a new UnsignedByte_HEX object.
	 */
	public UnsignedByte_HEX() {
		value = 0;
		signed = false;
	}

	/**
	 * String representation in specified radix.
	 * 
	 */
	public UnsignedByte_HEX(String valueString, int radix) {
		this(new BigInteger(valueString, radix).intValue());
	}

	public UnsignedByte_HEX(Element element) {
		decodeXML(element);
	}

	@Override
	public void decodeXML(Element element) {
		value = Integer.parseInt(element.getText(), 16);
	}

	

	@Override
	public Content encodeXML(String name, Namespace ns) {
		Element element = new Element(name, ns);
		element.setContent(new Text(Integer.toHexString(value)));

		return element;
	}

	public String toString(int radix) {
		return Integer.toString(value, radix).replace("-", "");
	}

	public String toString() {
		return toString(16);
	}

}
