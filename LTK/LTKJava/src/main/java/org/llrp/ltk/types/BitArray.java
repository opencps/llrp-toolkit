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


/**
 * Array of bits
 *
 * @author gasserb
 */
public class BitArray extends LLRPType {
    protected static final int LENGTH = 1;
    protected Bit[] bits;

    /**
         * create a new BitArray.
         * When encoded, BitArray also encodes its length.
         * @param bits to be decoded
         */
    public BitArray(Bit[] bits) {
        this.bits = bits.clone();
    }

    /**
         * create a new BitArray.
         * When encoded, BitArray also encodes its length.
         * @param list to be decoded
         */
    public BitArray(LLRPBitList list) {
        decodeBinary(list);
    }

    /**
         * create a new BitArray.
         * When encoded, BitArray also encodes its length.
         * Initially all bits set to 0
         * @param length of array
         */
    public BitArray(int length) {
        bits = new Bit[length];

        for (int i = 0; i < length; i++) {
            bits[i] = new Bit(false);
        }
    }

	/**
	 * Creates an empty BitArray. Do not call method 'set' on an empty array.
	 * Add a Bit by calling the add method
	 */
    public BitArray() {
        bits = new Bit[0];
    }

    /**
     * @param element to be decoded
     */
    public BitArray(Element element) {
        decodeXML(element);
    }

    /**
     * @param binaryString
     */
    public BitArray(String binaryString) {
    	Element element = new Element("foo","ns");
    	element.setText(binaryString);
        decodeXML(element);
    }
    
    /**
     * set bit at provided positionto 0.
     *
     * @param i to be cleared
     */
    public void clear(int i) {
        if ((i < 0) || (i > bits.length)) {
            return;
        } else {
            bits[i] = new Bit(false);
        }
    }

    /**
     * encodes length before encoding containing values.
     *
     * @return LLRPBitList
     */
    public LLRPBitList encodeBinary() {
        // add bits so that length of result is multiple of 8
        LLRPBitList padding = null;

        if ((bits.length % 8) > 0) {
            padding = new LLRPBitList(8 - (bits.length % 8));
        } else {
            padding = new LLRPBitList();
        }

        // length before bits, in number of bytes
        int len = bits.length+padding.length()/8;
        LLRPBitList result = new UnsignedShort(len).encodeBinary();

        for (int i = 0; i < bits.length; i++) {
            result.add(bits[i].toBoolean());
        }

        result.append(padding);

        return result;
    }

    /**
     * length of BaseType - not the array - for array length call size().
     *
     *
     * @return Integer representing number of bits nedded for this type
     */
    public static int length() {
        return Bit.length();
    }

    /**
     * Create BitArray from BitList. Must provide length with first 16 bits.
     *
     * @param list to be decoded
     */
    @Override
    public void decodeBinary(LLRPBitList list) {
        Integer length = new SignedInteger(list.subList(0, SignedShort.length())).toInteger();
        bits = new Bit[length];
        for (Integer i = 1; i <= length; i++) {
            bits[i - 1] = new Bit(list.get(15 + i));
        }
    }

    /**
     * get bit at specified position.
     *
     * @param i to get
     *
     * @return Bit
     */
    public Bit get(int i) {
        return bits[i];
    }

    /**
     * set bit at provided position to 1.
     *
     * @param i to be set to 1
     */
    public void set(int i) {
        if ((i < 0) || (i > bits.length)) {
            return;
        } else {
            bits[i] = new Bit(true);
        }
    }

    /**
     * number of elements in array.
     *
     * @return number of elements in array
     */
    public int size() {
        return bits.length;
    }

    @Override
    public Content encodeXML(String name, Namespace ns) {
        Element element = new Element(name, ns);
        element.setText(toString());
        return element;
    }

    @Override
    public void decodeXML(Element element) {
        String text = element.getText();
        if (!text.equals("")){
        String[] bitStrings = text.split(" ");
        bits = new Bit[bitStrings.length];

        for (int i = 0; i < bits.length; i++) {
            bits[i] = new Bit(bitStrings[i]);
        }
        } else {
        	bits = new Bit[0];
        }
    }

    public void add(Bit aBit) {
        Bit[] newBits = new Bit[bits.length + 1];
        System.arraycopy(bits, 0, newBits, 0, bits.length);
        newBits[bits.length] = aBit;
        bits = newBits;
    }
    
	public String toString() {
		 String s = "";

	        for (Bit b : bits) {
	            s += b.toInteger().toString();
	        }

	        s = s.replaceFirst(" ", "");
	       return s;
	}
	
	public String toString(int radix){
        String s = "";

        for (Bit b : bits) {
            s += Integer.toString(b.toInteger(),radix);
        }
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
				new Bit(strings[i]);
			} catch (IllegalArgumentException e){
				return false;
			}
		}
		return true;
	}
    
}
