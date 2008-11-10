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
 * Array of UnsignedShorts - length encoded with first 16 bits in binary encoding
 * 
 * @author gasserb
 */
public class UnsignedShortArray extends LLRPType {
	protected UnsignedShort[] shorts;

	/**
	 * Creates a new UnsignedShortArray object.
	 * 
	 * @param shorts
	 *            to create array from
	 */
	public UnsignedShortArray(UnsignedShort[] shorts) {
		this.shorts = shorts.clone();
	}

	/**
	 * Creates a new UnsignedShortArray object.
	 * 
	 * @param shortString
	 *           string of shorts
	 */
	public UnsignedShortArray(String shortString) {
		if (shortString.equals("")) {
			shorts = new UnsignedShort[0];
		} else {
			String[] strings = shortString.split(" ");
			shorts = new UnsignedShort[strings.length];

			for (int i = 0; i < strings.length; i++) {
				shorts[i] = new UnsignedShort(strings[i]);
			}
		}
	}

	public UnsignedShortArray(short[] data) {
		this.shorts = new UnsignedShort[data.length];
		for (int i = 0; i < data.length; i++) {
			shorts[i] = new UnsignedShort(data[i]);
		}
	}

	/**
	 * Creates a new UnsignedShortArray object from jdom element - used for xml
	 * decoding
	 * 
	 * @param element
	 *            to be decoded
	 */
	public UnsignedShortArray(Element element) {
		decodeXML(element);
	}

	/**
	 * Creates a new UnsignedShortArray object.
	 * 
	 * @param length
	 *            of array
	 */
	public UnsignedShortArray(int length) {
		shorts = new UnsignedShort[length];
	}

	/**
	 * first 16 bits of LLRPBitlist must indicate number of entries that follow
	 * 
	 * @param bits
	 *            to be decoded
	 */
	public UnsignedShortArray(LLRPBitList bits) {
		decodeBinary(bits);
	}

	/**
	 * Creates an empty UnsignedShortArray. Do not call method 'set' on an empty
	 * array. Add UnsignedShorts by calling the add method
	 */
	public UnsignedShortArray() {
		shorts = new UnsignedShort[0];
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
		shorts = new UnsignedShort[length];

		for (int i = 0; i < length; i++) {
			shorts[i] = new UnsignedShort(list.subList(i
					* UnsignedShort.length() + SignedShort.length(),
					UnsignedShort.length()));
		}
	}

	/**
	 * encodes length before encoding containing values
	 * 
	 * @return LLRPBitList
	 */
	public LLRPBitList encodeBinary() {
		LLRPBitList result = new LLRPBitList();
		result.append(new UnsignedShort(shorts.length).encodeBinary());

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
		UnsignedShortArray ba = (UnsignedShortArray) other;

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
	 * get UnsignedShort at specified position
	 * 
	 * @param i
	 *            position
	 * 
	 * @return UnsignedShort
	 */
	public UnsignedShort get(int i) {
		return shorts[i];
	}

	/**
	 * lenght in bits
	 * 
	 * @return Integer
	 */
	public int getBitLength() {
		return shorts.length * UnsignedShort.length();
	}

	/**
	 * length in bytes
	 * 
	 * @return int
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
		return UnsignedShort.length();
	}

	/**
	 * set UnsignedShort at given location
	 * 
	 * @param i
	 *            position
	 * @param b
	 *            UnsignedShort to be set
	 */
	public void set(int i, UnsignedShort b) {
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
			shorts = new UnsignedShort[0];
			return;
		}
		String[] strings = text.split(" ");
		shorts = new UnsignedShort[strings.length];

		for (int i = 0; i < strings.length; i++) {
			shorts[i] = new UnsignedShort(strings[i]);
		}
	}

	public void add(UnsignedShort aShort) {
		UnsignedShort[] newShorts = new UnsignedShort[shorts.length + 1];
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
		for (UnsignedShort b : shorts) {
			if (b != null) {
				s += " " + b.toString(radix);
			}
		}
		return s;
	}

	public String toString() {
		String s = "";

		for (UnsignedShort b : shorts) {

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
				new UnsignedShort(strings[i]);
			} catch (IllegalArgumentException e) {
				return false;
			}
		}
		return true;
	}
}
