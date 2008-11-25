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
 * 8 Bit Integer
 *
 * @author Basil Gasser - ETH Zurich
 */
public class SignedByte extends LLRPNumberType {
    private static final Integer LENGTH = 8;
    protected Integer value;

    /**
     * Creates a new SignedByte object.
     *
     * @param value to set
     */
    public SignedByte(byte value) {
        this.value = new Integer(value);
        signed = false;
    }

    /**
     * Creates a new SignedByte object.
     *
     * @param stringValue value as string
     */
    public SignedByte(String stringValue) {
        this(new Byte(stringValue));
        if (!inRange(stringValue)){
			throw new IllegalArgumentException("value "+stringValue+" not in range allowed for SignedByte");
		}
    }

    /**
     * Creates a new SignedByte object from Integer. One might loose precision.
     *
     * @param value to set
     */
    public SignedByte(Integer value) {
        this(value.byteValue());
    }

    
    public SignedByte(int value){
    	this(new Integer(value));
    	if (!inRange(value)){
			throw new IllegalArgumentException("value "+value+" not in range");
		}
    }

    /**
     * String representation in specified radix.
     *
     */
    public SignedByte(String valueString, int radix) {
        this(new BigInteger(valueString, radix).intValue());
    }

    
    /**
     * Creates a new SignedByte object.
     *
     * @param bitList to be decoded
     */
    public SignedByte(LLRPBitList bitList) {
        decodeBinary(bitList);
        signed = false;
    }

    /**
     * Creates a new SignedByte object.
     *
     * @param element to be decoded
     */
    public SignedByte(Element element) {
        decodeXML(element);
        signed = false;
    }

    /**
     * Creates a new SignedByte object.
     */
    public SignedByte() {
        value = 0;
        signed = false;
    }

    /**
     *
     * number of bits used to represent this type.
     *
     * @return Integer
     */
    public static int length() {
        return LENGTH;
    }

    /**
     * this represented as byte - no loss of precision.
     *
     * @return Byte
     */
    public byte toByte() {
        return value.byteValue();
    }

    /**
     * this represented as Integereger - no loss of precision.
     *
     * @return Integer
     */
    public Integer toInteger() {
        return Integer.valueOf(toByte());
    }
    
    public int intValue(){
    	return toInteger().intValue();
    }

    @Override
    public void decodeXML(Element element) {
        this.value = new Integer(element.getText());
    }

    @Override
    public Content encodeXML(String name, Namespace ns) {
        Element element = new Element(name, ns);
        element.setContent(new Text(value.toString()));

        return element;
    }

    @Override
    public void decodeBinary(LLRPBitList list) {
        String bitString = list.toString();

        //if first bit is set and list is exactly length bits long, its negative
        // number is in 2's complement format
        if ((bitString.length() == LENGTH) && (bitString.charAt(0) == '1')) {
            //flip all bits
            // add one
            bitString = bitString.replaceAll("0", "#");
            bitString = bitString.replaceAll("1", "0");
            bitString = bitString.replaceAll("#", "1");
            bitString = bitString.replaceFirst("0", "");
            value = Integer.parseInt(bitString, 2) + 1;
            value = -value;
        } else {
            value = Integer.parseInt(bitString, 2);
        }

        signed = true;
    }

    @Override
    public LLRPBitList encodeBinary() {
        LLRPBitList result = new LLRPBitList(Integer.toBinaryString(value));

        if (result.length() < LENGTH) {
            result.pad(LENGTH - result.length());
        }

        return result.subList(result.length() - LENGTH, LENGTH);
    }

    public String toString() {
        return Integer.toString(value);
    }
    
    public String toString(int radix){
    	return Integer.toString(value, radix);
    }
    
    @Override
	public boolean inRange(long value) {
		return (value >= Byte.MIN_VALUE && value <= Byte.MAX_VALUE);
	}
    
    public boolean inRange(String valueString){
		return inRange(new BigInteger(valueString).longValue());
	}
}
