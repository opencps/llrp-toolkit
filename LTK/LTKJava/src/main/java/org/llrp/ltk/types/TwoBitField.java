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
 * TwoBitField - a field consisting of two bits
 * 
 * @author gasserb
 */
public class TwoBitField extends LLRPType {
	private static int length = 2;
	private Bit[] bits;

	/**
	 * Creates a TwoBitField. MSB at position 0, LSB at position 1
	 * 
	 * @param bits
	 *            to be decoded
	 */
	public TwoBitField(Bit[] bits) {
		this.bits = new Bit[length];
		if (bits.length<2){
			this.bits[0] = new Bit(0);
			this.bits[1] = new Bit(0);
		} else {
			this.bits[0] = bits[0];
			this.bits[1] = bits[1];
		}
	}

	/**
	 * Creates a TwoBitField. LSB at position 1, MSB at position 0
	 * 
	 * @param list
	 *            to be decoded
	 */
	public TwoBitField(LLRPBitList list) {
		this.bits = new Bit[length];
		this.bits[0] = new Bit(0);
		this.bits[1] = new Bit(0);
		decodeBinary(list);
	}

	/**
	 * create new TwoBitField from String where the String is assumed to be in Decimal format
	 * @param string
	 */
	public TwoBitField(String string) {
		Element element = new Element("foo", "ns");
		element.setText(string);
		decodeXML(element);
		if (!inRange(toString(10))){
			throw new IllegalArgumentException(string+" not in range allowed for TwoBitField");
		}
	}

	/**
	 * Creates a new TwoBitField object.
	 */
	public TwoBitField() {
		this.bits = new Bit[length];
		this.bits[0] = new Bit(0);
		this.bits[1] = new Bit(0);
	}

	/**
	 * Creates a new TwoBitField object.
	 * 
	 * @param element
	 *            to be decoded
	 */
	public TwoBitField(Element element) {
		decodeXML(element);
	}

	/**
	 * set Bit at specified position to 0
	 * 
	 * @param i
	 *            position to be set to 0
	 */
	public void clear(Integer i) {
		if ((i < 0) || (i > bits.length)) {
			return;
		} else {
			bits[i] = new Bit(0);
		}
	}

	/**
	 * just like BitArray but does not encode length before values
	 * MSB first
	 * 
	 * @return LLRPBitList
	 */
	public LLRPBitList encodeBinary() {
		LLRPBitList result = new LLRPBitList(bits.length);

		for (Integer i = 0; i < length; i++) {
			if (bits[i].toBoolean()) {
				result.set(i);
			} else {
				result.clear(i);
			}
		}

		return result;
	}

	/**
	 * number of bits used to represent this type
	 * 
	 * @return Integer
	 */
	public static int length() {
		return length;
	}

	/**
	 * decode bits from BitList. Length must not be provided, MSB first
	 * 
	 * @param list
	 *            to be decoded
	 */
	@Override
	public void decodeBinary(LLRPBitList list) {
		bits = new Bit[length];

		for (Integer i = 0; i < length; i++) {
			bits[i] = new Bit(list.get(i));
		}
	}

	/**
	 * get bit at I
	 * 
	 * @param i
	 *            position to be returned
	 * 
	 * @return bIT
	 */
	public Bit get(Integer i) {
		return bits[i];
	}

	/**
	 * two bit field interpreted as two bit number
	 * 
	 * @return int
	 */
	public int intValue() {
		String s = bits[0].toString() + "" + bits[1].toString();
		return new BigInteger(s, 2).intValue();
	}

	/**
	 * set Bit at specified position to 1
	 * 
	 * @param i
	 *            position to be set to 1
	 */
	public void set(Integer i) {
		if ((i < 0) || (i > bits.length)) {
			return;
		} else {
			bits[i] = new Bit(true);
		}
	}

	@Override
	public Content encodeXML(String name, Namespace ns) {

		Element element = new Element(name, ns);
		element.setContent(new Text(toString()));

		return element;
	}

	/**
	 * decode XML element. The contained Text is assumed to be a string representing a number in Decimal Format.
	 */
	public void decodeXML(Element element) {

		this.bits = new Bit[length];
		if (!element.getText().equals("")) {
			int i = Integer.parseInt(element.getText());
			if (i == 0) {
				this.bits[1] = new Bit(0);
				this.bits[0] = new Bit(0);
			} else if (i == 1) {
				this.bits[1] = new Bit(1);
				this.bits[0] = new Bit(0);
			} else if (i == 2) {
				this.bits[1] = new Bit(0);
				this.bits[0] = new Bit(1);
			} else if (i == 3) {
				this.bits[1] = new Bit(1);
				this.bits[0] = new Bit(1);
			} else {
				throw new IllegalArgumentException(element.getText()+" not in range");
			}
		} else {
			this.bits[0] = new Bit(0);
			this.bits[1] = new Bit(0);

		}

	}
	/**
	 * encodes TwoBitField to a string where the string represents the value in decimal format and bits are interpreted MSB first
	 */
	public String toString() {
		Integer s = bits[1].toInteger() + (bits[0].toInteger() * 2);
		return s.toString();

	}

	/**
	 * encodes the TwoBitField to a string where the String represents the value in the given radix. If the radix is two, the string is in MSB first format
	 */
	public String toString(int radix) {
		if (radix == 2){
			return bits[1].toString() + "" + bits[0].toString();
		} else {
			return Integer.toString(intValue(), radix);
		}
	}

	/**
	 * expects a string as formated for XML
	 */
	public boolean inRange(String valueString) {
		int i = Integer.parseInt(valueString);
		return (i >= 0 && i <= 3);
	}
}
