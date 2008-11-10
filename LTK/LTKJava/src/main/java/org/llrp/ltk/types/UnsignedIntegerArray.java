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
 * Array of unsigned 32bit integers. Length encoded in first 16 bits of binary encoding
 * 
 * @author gasserb
 */
public class UnsignedIntegerArray extends LLRPType {
	private UnsignedInteger[] integers;

	/**
	 * Creates a new UnsignedIntegerArray object.
	 * 
	 * @param ints
	 *            to create UsnignedIntegerArray from
	 */
	public UnsignedIntegerArray(UnsignedInteger[] ints) {
		this.integers = ints.clone();
	}

	/**
	 * Creates a new UnsignedIntegerArray object from jdom element - used for
	 * xml decoding
	 * 
	 * @param element
	 *            to be decoded
	 */
	public UnsignedIntegerArray(Element element) {
		decodeXML(element);
	}

	/**
	 * Creates a new UnsignedIntegerArray object.
	 * 
	 * @param length
	 *            of array
	 */
	public UnsignedIntegerArray(int length) {
		integers = new UnsignedInteger[length];
	}

	/**
	 * first 16 bits of LLRPBitlist must indicate number of entries that follow
	 * 
	 * @param bits
	 *            to be decoded
	 */
	public UnsignedIntegerArray(LLRPBitList bits) {
		decodeBinary(bits);
	}

    /**
     * @param string
     */
    public UnsignedIntegerArray(String string) {
    	Element element = new Element("foo","ns");
    	element.setText(string);
        decodeXML(element);
    }
	
	/**
	 * Creates an empty UnsignedIntegerArray. Do not call methood 'set' on an
	 * empty array. Add UnsignedInteger by calling the add method
	 */
	public UnsignedIntegerArray() {
		integers = new UnsignedInteger[0];
	}

	/**
	 * encodes length before encoding containing values
	 * 
	 * @return LLRPBitList
	 */
	public LLRPBitList encodeBinary() {
		LLRPBitList result = new LLRPBitList();
		result.append(new UnsignedShort(integers.length).encodeBinary());

		for (int i = 0; i < integers.length; i++) {
			result.append(integers[i].encodeBinary());
		}

		return result;
	}

	/**
	 * number of bytes used to represent this type
	 * 
	 * @return Integer
	 */
	public int getByteLength() {
		return integers.length * 2;
	}

	/**
	 * length of BaseType not array - for array length call size()
	 * 
	 * @return int
	 */
	public static int length() {
		return UnsignedInteger.length();
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
		integers = new UnsignedInteger[length];

		for (int i = 0; i < length; i++) {
			integers[i] = new UnsignedInteger(list.subList(i
					* UnsignedInteger.length() + SignedShort.length(),
					SignedInteger.length()));
		}
	}

	/**
	 * get UnsignedInteger at specified position
	 * 
	 * @param i
	 *            position
	 * 
	 * @return UnsignedInteger
	 */
	public UnsignedInteger get(int i) {
		return integers[i];
	}

	/**
	 * set UnsignedInteger at i to b
	 * 
	 * @param i
	 *            position
	 * @param b
	 *            unsignedInteger to be set
	 */
	public void set(int i, UnsignedInteger b) {
		if ((i < 0) || (i > integers.length)) {
			return;
		} else {
			integers[i] = b;
		}
	}

	/**
	 * number of elements in array
	 * 
	 * @return int
	 */
	public int size() {
		return integers.length;
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
		if (!text.equals("")){
		String[] strings = text.split(" ");
		integers = new UnsignedInteger[strings.length];

		for (int i = 0; i < strings.length; i++) {
			integers[i] = new UnsignedInteger(strings[i]);
		}
		} else {
			integers = new UnsignedInteger[0];
		}
	}

	public void add(UnsignedInteger aInteger) {
		UnsignedInteger[] newIntegers = new UnsignedInteger[integers.length + 1];
		System.arraycopy(integers, 0, newIntegers, 0, integers.length);
		newIntegers[integers.length] = aInteger;
		integers = newIntegers;
	}

	public String toString(int radix) {
		String s = "";

		for (UnsignedInteger b : integers) {
			s += b.toString(radix);
		}
		return s;

	}
	
	public String toString(){
		String s = "";

		for (UnsignedInteger b : integers) {
			s += " ";
			s += b.toLong();
		}

		s = s.replaceFirst(" ", "");
		return s;
	}
	
	/**
	 * expects a string as formated for XML
	 */
	public boolean inRange(String valueString){
		String[] strings = valueString.split(" ");
		// try do create each element. If one failes, the whole string is illegal
		for (int i = 0; i < strings.length; i++) {
			try {
				new UnsignedInteger(strings[i]);
			} catch (IllegalArgumentException e){
				return false;
			}
		}
		return true;
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
		if (!(other instanceof UnsignedIntegerArray)) {
			throw new IllegalArgumentException(
					"Argument not UnsignedIntegerArray");
		}
		UnsignedIntegerArray ba = (UnsignedIntegerArray) other;

		if (ba.size() != (this.size())) {
			return false;
		}

		for (int i = 0; i < integers.length; i++) {
			if (!ba.get(i).equals(this.get(i))) {
				return false;
			}
		}

		return true;
	}
}
