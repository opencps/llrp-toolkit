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
public class UnsignedByte extends LLRPNumberType {
	private static final Integer LENGTH = 8;
	protected int value;

	/**
	 * Creates a new UnsignedByte object.
	 * 
	 * @param value
	 *            to set
	 */
	public UnsignedByte(int value) {
		if (value < 0) {
			this.value = (2*Byte.MAX_VALUE+value+2);
		} else {
			this.value = value;
		}
		signed = false;
		if (!inRange(this.value)) {
			throw new IllegalArgumentException("value " + value
					+ " not in range allowed for UnsignedByte");
		}
	}

	/**
	 * Creates a new UnsignedByte object from byte
	 * 
	 * @param value
	 *            interpreted as unsigned byte
	 */
	public UnsignedByte(byte value) {
		this((int) value);
	}

	/**
	 * Creates a new UnsignedByte object.
	 * 
	 * @param valueString
	 *            value as string
	 */
	public UnsignedByte(String valueString) {
		this(new Integer(valueString));
		if (!inRange(valueString)) {
			throw new IllegalArgumentException("value " + valueString
					+ " not in range");
		}
	}

	/**
	 * Creates a new UnsignedByte object.
	 * 
	 * @param bitList
	 *            to be decoded
	 */
	public UnsignedByte(LLRPBitList bitList) {
		decodeBinary(bitList);
		signed = false;
	}

	/**
	 * Creates a new UnsignedByte object.
	 */
	public UnsignedByte() {
		value = 0;
		signed = false;
	}

	/**
	 * String representation in specified radix.
	 * 
	 */
	public UnsignedByte(String valueString, int radix) {
		this(new BigInteger(valueString, radix).intValue());
	}

	public UnsignedByte(Element element) {
		decodeXML(element);
	}
	
	
	/**
	 * number of bits used to represent this type
	 * 
	 * @return Integer
	 */
	public static int length() {
		return LENGTH;
	}

	/**
	 * to java byte - might loose precision
	 * 
	 * @return byte
	 */
	public byte toByte() {
		return (byte) value;
	}

	/**
	 * to java Integer
	 * 
	 * @return Integer
	 */
	public Integer toInteger() {
		return value;
	}

	@Override
	public void decodeBinary(LLRPBitList list) {
		value = Integer.parseInt(list.toString(), 2);
	}

	@Override
	public void decodeXML(Element element) {
		value = Integer.parseInt(element.getText());
	}

	@Override
	public LLRPBitList encodeBinary() {
		LLRPBitList result = new LLRPBitList(Integer.toBinaryString(value));

		if (result.length() < LENGTH) {
			result.pad(LENGTH - result.length());
		}

		return result.subList(result.length() - LENGTH, LENGTH);
	}

	@Override
	public Content encodeXML(String name, Namespace ns) {
		Element element = new Element(name, ns);
		element.setContent(new Text(Integer.toString(value)));

		return element;
	}

	public String toString(int radix) {
		return Integer.toString(value, radix).replace("-", "");
	}

	public String toString() {
		return toString(10);
	}

	@Override
	public boolean inRange(long value) {
		return (value >= 0 && value <= 255);
	}

	public boolean inRange(String valueString) {
		return inRange(new BigInteger(valueString).longValue());
	}
}
