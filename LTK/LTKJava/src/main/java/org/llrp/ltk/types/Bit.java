/*
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
 * A single bit
 * 
 * @author Basil Gasser - ETH Zurich
 */
public class Bit extends LLRPNumberType {
	private static final Integer LENGTH = 1;
	protected Integer value;

	/**
	 * initially set to 0.
	 */
	public Bit() {
		value = 0;
		signed = false;
	}

	/**
	 * create a bit.
	 * 
	 * @param x -
	 *            everything but 0 is interpreted as 1
	 */
	public Bit(int x) {
		if (!inRange(x)){
			throw new IllegalArgumentException("value "+x+" not in range allowed for Bit");
		}
		if (x != 0) {
			value = 1;
		} else {
			value = 0;
		}

		signed = false;
	}

	/**
	 * create a 7bit.
	 * 
	 * @param x
	 */
	public Bit(boolean x) {
		signed = x;
		if (x) {
			value = 1;
		} else {
			value = 0;
		}
	}

	public Bit(Integer i) {
		this(i.intValue());
	}

	/**
	 * create a bit.
	 * 
	 * @param x -
	 *            everything but 0 is interpreted as 1
	 */
	public Bit(String x) {
		if (!inRange(x)){
			throw new IllegalArgumentException("value "+x+" not in range");
		}
		if (x.equalsIgnoreCase("0")) {
			value = 0;
		} else {
			value = 1;
		}

		signed = false;
	}

	/**
	 * create a bit. Look at first bit of list only.
	 * 
	 * @param bitList -
	 *            everything but 0 is interpreted as 1
	 */
	public Bit(LLRPBitList bitList) {
		if (bitList.get(0)) {
			value = 1;
		} else {
			value = 0;
		}

		signed = false;
	}

	/**
	 * everything but 0 is interpreted as 1.
	 * 
	 * @param bool
	 *            initial value
	 */
	public Bit(Boolean bool) {
		if (bool) {
			value = 1;
		} else {
			value = 0;
		}

		signed = false;
	}

	/**
	 * Creates a new Bit object from jdom element - used for xml decoding.
	 * 
	 * @param element
	 *            to be decoded
	 */
	public Bit(Element element) {
		decodeXML(element);
	}

	/**
	 * length in number of bits used to represent this type.
	 * 
	 * @return Integer representing number of bits nedded for this type
	 */
	public static int length() {
		return LENGTH;
	}

	/**
	 * 0 is false, 1 is true.
	 * 
	 * @return boolean
	 */
	public boolean toBoolean() {
		return !value.equals(0);
	}

	/**
	 * bit as byte - the last bit is set or not.
	 * 
	 * @return byte
	 */
	public byte toByte() {
		return value.byteValue();
	}

	/**
	 * either 0 or 1.
	 * 
	 * @return either 0 or 1!
	 */
	public Integer toInteger() {
		return new Integer(toByte());
	}
	
	public int intValue(){
		return toInteger().intValue();
	}

	@Override
	public void decodeXML(Element element) {
		if (element.getText().equalsIgnoreCase("0")) {
			value = 0;
		} else {
			value = 1;
		}
	}

	@Override
	public Content encodeXML(String name, Namespace ns) {
		Element element = new Element(name, ns);
		element.setContent(new Text(value.toString()));

		return element;
	}

	@Override
	public void decodeBinary(LLRPBitList list) {
		if (list.get(0)) {
			value = 1;
		} else {
			value = 0;
		}
	}

	@Override
	public LLRPBitList encodeBinary() {
		LLRPBitList result = new LLRPBitList(1);

		if (value != 0) {
			result.set(0);
		} else {
			result.clear(0);
		}

		return result;
	}

	public String toString() {
		if (value == 0) {
			return "0";
		} else {
			return "1";
		}
	}
	
	public String toString(int radix){
		return toString();
	}

	@Override
	public boolean inRange(long value) {
		return (value >= 0 && value <= 1);
	}
	
	public boolean inRange(String valueString){
		return inRange(new BigInteger(valueString).longValue());
	}
}
