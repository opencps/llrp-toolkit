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
import org.jdom.Text;

/**
 * Array of 16 bit signed shorts - length encoded in first 16 bits of binary encoding
 * 
 * @author gasserb
 */
public class SignedShortArray extends LLRPType {
	protected SignedShort[] shorts;

	/**
	 * Creates a new SignedShortArray object.
	 * 
	 * @param shorts
	 *            to create array from
	 */
	public SignedShortArray(SignedShort[] shorts) {
		this.shorts = shorts.clone();
	}

	/**
	 * Creates a new SignedShortArray object.
	 * 
	 * @param shortString
	 *            of shorts
	 */
	public SignedShortArray(String shortString) {
		if (shortString.equals("")) {
			shorts = new SignedShort[0];
		} else {
			String[] strings = shortString.split(" ");
			shorts = new SignedShort[strings.length];

			for (int i = 0; i < strings.length; i++) {
				shorts[i] = new SignedShort(strings[i]);
			}
		}
	}

	public SignedShortArray(short[] data) {
		this.shorts = new SignedShort[data.length];
		for (int i = 0; i < data.length; i++) {
			shorts[i] = new SignedShort(data[i]);
		}
	}

	/**
	 * Creates a new SignedShortArray object from jdom element - used for xml
	 * decoding
	 * 
	 * @param element
	 *            to be decoded
	 */
	public SignedShortArray(Element element) {
		decodeXML(element);
	}

	/**
	 * Creates a new SignedShortArray object.
	 * 
	 * @param length
	 *            of array
	 */
	public SignedShortArray(int length) {
		shorts = new SignedShort[length];
	}

	/**
	 * first 16 bits of LLRPBitlist must indicate number of entries that follow
	 * 
	 * @param bits
	 *            to be decoded
	 */
	public SignedShortArray(LLRPBitList bits) {
		decodeBinary(bits);
	}

	/**
	 * Creates an empty SignedShortArray. Do not call method 'set' on an empty
	 * array. Add SignedShorts by calling the add method
	 */
	public SignedShortArray() {
		shorts = new SignedShort[0];
	}

	/**
	 * first 16 bits of LLRPBitlist must indicate number of entries that follow
	 * 
	 * @param list
	 *            to be decoded
	 */
	public void decodeBinary(LLRPBitList list) {
		Integer length = new SignedShort(list.subList(0, SignedShort.length()))
				.toInteger();
		shorts = new SignedShort[length];

		for (int i = 0; i < length; i++) {
			shorts[i] = new SignedShort(list.subList(i
					* SignedShort.length() + SignedShort.length(),
					SignedShort.length()));
		}
	}

	/**
	 * encodes length before encoding containing values
	 * 
	 * @return LLRPBitList
	 */
	public LLRPBitList encodeBinary() {
		LLRPBitList result = new LLRPBitList();
		result.append(new SignedShort(shorts.length).encodeBinary());

		for (int i = 0; i < shorts.length; i++) {
			result.append(shorts[i].encodeBinary());
		}

		return result;
	}

	/**
	 * compare each element
	 * 
	 * @param other
	 *            to compare
	 * 
	 * @return boolean
	 */
	public boolean equals(LLRPType other) {
		SignedShortArray ba = (SignedShortArray) other;

		if (ba.size() != (this.size())) {
			return false;
		}

		for (int i = 0; i < shorts.length; i++) {
			if (!ba.get(i).equals(this.get(i))) {
				return false;
			}
		}

		return true;
	}

	/**
	 * get SignedShort at specified position
	 * 
	 * @param i
	 *            position
	 * 
	 * @return SignedShort
	 */
	public SignedShort get(int i) {
		return shorts[i];
	}

	/**
	 * lenght in bits
	 * 
	 * @return Integer
	 */
	public int getBitLength() {
		return shorts.length * SignedShort.length();
	}

	/**
	 * length in bytes
	 * 
	 * @return Integer
	 */
	public int getByteLength() {
		return shorts.length * 2;
	}

	/**
	 * length of BaseType not array - for array length call size()
	 * 
	 * @return int
	 */
	public static int length() {
		return SignedShort.length();
	}

	/**
	 * set SignedShort at given location
	 * 
	 * @param i
	 *            position
	 * @param b
	 *            SignedShort to be set
	 */
	public void set(int i, SignedShort b) {
		if ((i < 0) || (i > shorts.length)) {
			return;
		} else {
			shorts[i] = b;
		}
	}

	/**
	 * number of elements in array
	 * 
	 * @return int
	 */
	public int size() {
		return shorts.length;
	}

	@Override
	public Content encodeXML(String name, Namespace ns) {

		Element element = new Element(name, ns);
		element.setContent(new Text(toString()));

		return element;
	}

	@Override
	public void decodeXML(Element element) {
		String text = element.getText();
		if (text == null || text.equals("")){
			shorts = new SignedShort[0];
			return;
		}
		String[] strings = text.split(" ");
		shorts = new SignedShort[strings.length];

		for (int i = 0; i < strings.length; i++) {
			shorts[i] = new SignedShort(strings[i]);
		}
	}

	public void add(SignedShort aShort) {
		SignedShort[] newShorts = new SignedShort[shorts.length + 1];
		System.arraycopy(shorts, 0, newShorts, 0, shorts.length);
		newShorts[shorts.length] = aShort;
		shorts = newShorts;
	}

	public int hashCode() {
		return shorts.hashCode();
	}

	public short[] toShortArray() {
		short[] result = new short[shorts.length];
		for (int i = 0; i < shorts.length; i++) {
			result[i] = shorts[i].toShort();
		}
		return result;
	}

	@Override
	public String toString(int radix) {
		String s = "";
		for (SignedShort b : shorts) {
			if (b != null) {
				s += " " + b.toString(radix);
			}
		}
		return s;
	}

	public String toString() {
		String s = "";

		for (SignedShort b : shorts) {

			if (b != null) {
				s += " " + b.toInteger().toString();
			}
		}

		s = s.replaceFirst(" ", "");
		return s;
	}

	/**
	 * expects a string as formated for XML
	 */
	public boolean inRange(String valueString) {
		String[] strings = valueString.split(" ");
		// try do create each element. If one failes, the whole string is
		// illegal
		for (int i = 0; i < strings.length; i++) {
			try {
				new SignedShort(strings[i]);
			} catch (IllegalArgumentException e) {
				return false;
			}
		}
		return true;
	}
}
