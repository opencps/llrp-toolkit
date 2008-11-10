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
 * unsigned 16bit short
 * 
 * @author Basil Gasser - ETH Zurich
 */
public class UnsignedShort extends LLRPNumberType {
	private static final Integer LENGTH = 16;
	protected Integer value;

	/**
	 * Creates a new UnsignedShort object - might loose precision
	 * 
	 * @param value
	 *            to set
	 */
	public UnsignedShort(Short value) {
		this.value = value & 0xFFFFFFFF;
		signed = false;
	}

	public UnsignedShort(int value) {
		if (value < 0) {
			this.value = (2*Short.MAX_VALUE+value+2);
		} else {
			this.value = value;
		}
		signed = false;
		if (!inRange(this.value)) {
			throw new IllegalArgumentException("value " + value
					+ " not in range allowed for UnsignedShort");
		}
	}

	/**
	 * Creates a new UnsignedShort object from jdom element - used for xml
	 * decoding
	 * 
	 * @param element
	 *            to be decoded
	 */
	public UnsignedShort(Element element) {
		decodeXML(element);
	}

	/**
	 * Creates a new UnsignedShort object.
	 */
	public UnsignedShort() {
		value = 0;
		signed = false;
	}

	/**
	 * String representation in specified radix.
	 * 
	 */
	public UnsignedShort(String valueString, int radix) {
		this(new BigInteger(valueString, radix).intValue());
	}

	/**
	 * Creates a new UnsignedShort object.
	 * 
	 * @param value
	 *            to set
	 */
	public UnsignedShort(Integer value) {
		this(value.intValue());
	}

	/**
	 * Creates a new UnsignedShort object.
	 * 
	 * @param valueString
	 *            value as string
	 */
	public UnsignedShort(String valueString) {
		value = new Integer(valueString);
		signed = false;
		if (!inRange(valueString)) {
			throw new IllegalArgumentException("value " + valueString
					+ " not in range");
		}
	}

	/**
	 * Creates a new UnsignedShort object.
	 * 
	 * @param bitList
	 *            to be decoded
	 */
	public UnsignedShort(LLRPBitList bitList) {
		decodeBinary(bitList);
		signed = false;
	}

	/**
	 * @return LLRPBitList
	 * 
	 */
	public LLRPBitList encodeBinary() {
		LLRPBitList result = new LLRPBitList(Integer.toBinaryString(value));

		if (result.length() < LENGTH) {
			result.pad(LENGTH - result.length());
		}

		return result.subList(result.length() - LENGTH, LENGTH);
	}

	/**
	 * 
	 * 
	 * @return int
	 */
	public static int length() {
		return LENGTH;
	}

	/**
	 * wrap UnsignedShort Integero Integer
	 * 
	 * @return Integer
	 */
	public Integer toInteger() {
		return value;
	}

	public int intValue() {
		return value.intValue();
	}

	/**
	 * this might return a false value. Java short are signed and therefore
	 * might not provide enough precision
	 * 
	 * @return short
	 */
	public short toShort() {
		return value.shortValue();
	}

	@Override
	public void decodeBinary(LLRPBitList list) {
		value = Integer.parseInt(list.toString(), 2);
	}

	@Override
	public void decodeXML(Element element) {
		value = new Integer(element.getText());
	}

	@Override
	public Content encodeXML(String name, Namespace ns) {
		Element element = new Element(name, ns);
		element.setContent(new Text(value.toString()));

		return element;
	}

	public String toString(int radix) {
		return Integer.toString(value & 0xFFFF, radix);
	}

	@Override
	public boolean inRange(long value) {
		int max = Short.MAX_VALUE + Short.MAX_VALUE + 1;
		return (value >= 0 && value <= max);
	}

	public boolean inRange(String valueString) {
		return inRange(new BigInteger(valueString).longValue());
	}
}
