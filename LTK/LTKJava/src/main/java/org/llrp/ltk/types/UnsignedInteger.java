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

import java.math.BigInteger;


/**
 * Unsigned 32 bit Integer
 *
 * @author Basil Gasser - ETH Zurich
 */
public class UnsignedInteger extends LLRPNumberType {
    // value interpreted as unsigned Integer
    // leading zero not sign but number
    private static final Integer LENGTH = 32;
    protected BigInteger value;

    /**
     * Creates a new UnsignedInteger object from Java Integer - interpreting signed bit as value
     *
     * @param value to set
     */
    public UnsignedInteger(Integer value) {
    	this.value = new BigInteger(Integer.toBinaryString(value.intValue()),2);
        signed = false;
    }
    
    /**
     * Creates a new UnsignedInteger object from Java Integer - interpreting signed bit as value
     *
     * @param value to set
     */
    public UnsignedInteger(int value){
    	this(new Integer(value));
    	if (!inRange(value)){
			throw new IllegalArgumentException("value "+value+" not in range allowed for UnsignedByte");
		}
    }
    
    /**
     * Creates a new UnsignedInteger object from Java long
     *
     * @param value to set
     */
    public UnsignedInteger(long value){
    	this.value = BigInteger.valueOf(value);
        signed = false;
        if (!inRange(value)){
			throw new IllegalArgumentException("Illegal Argument: value "+value+" not in range allowed for UnsignedInteger");
		}
    }

    /**
     * Creates a new UnsignedInteger object.
     *
     * @param valueString value as string
     */
    public UnsignedInteger(String valueString) {
        value = new BigInteger(valueString);
        if (!inRange(valueString)){
			throw new IllegalArgumentException("value "+valueString+" not in range");
		}
    }

    /**
     * String representation in specified radix.
     *
     */
    public UnsignedInteger(String valueString, int radix) {
        this(new BigInteger(valueString, radix).intValue());
    }
    /**
     * Creates a new UnsignedInteger object.
     */
    public UnsignedInteger() {
        value = new BigInteger("0");
        signed = false;
    }

    /**
     * Creates a new UnsignedInteger object.
     *
     * @param bitList to be decoded
     */
    public UnsignedInteger(LLRPBitList bitList) {
        decodeBinary(bitList);
        signed = false;
    }

    /**
     * Creates a new UnsignedInteger object from jdom element - used for xml decoding
     *
     * @param element to be decoded
     */
    public UnsignedInteger(Element element) {
        decodeXML(element);
    }

    /**
     * decode to binary representation
     *
     * @param bitList to be decoded
     */
    public void decodeBinary(LLRPBitList bitList) {
        value = new BigInteger(bitList.toString(), 2);
    }

    /**
     * number of bits used to represent this type
     *
     * @return int
     */
    public static int length() {
        return LENGTH;
    }

    /**
     * this might return a false value. Java Integer are signed and therefore
     * might not provide enough precision
     *
     * @return Integer
     */
    public Integer toInteger() {
        return value.intValue();
    }

    public int intValue(){
    	return value.intValue();
    }
    /**
     * Unsigned Integer wrapped Integero java long
     *
     * @return Long
     */
    public Long toLong() {
        return value.longValue();
    }

    @Override
    public void decodeXML(Element element) {
        value = new BigInteger(element.getText());
    }

    @Override
    public LLRPBitList encodeBinary() {
        LLRPBitList result = new LLRPBitList(value.toString(2));

        if (result.length() < LENGTH) {
            result.pad(LENGTH - result.length());
        }

        return result.subList(result.length() - LENGTH, LENGTH);
    }

    @Override
    public Content encodeXML(String name, Namespace ns) {
        Element element = new Element(name, ns);
        element.setContent(new Text(value.toString()));

        return element;
    }

    public String toString(int radix) {
        return value.toString(radix);
    }
    
    public String toString(){
    	return value.toString();
    }
    
    @Override
	public boolean inRange(long value) {
		return (value >= 0 && value <= 0xFFFFFFFFL);
	}
    
    public boolean inRange(String valueString){
		return inRange(new BigInteger(valueString).longValue());
	}
}
