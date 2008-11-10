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
 * 16bit signed Short
 *
 * @author Basil Gasser - ETH Zurich
 */
public class SignedShort extends LLRPNumberType {
    private static final Integer LENGTH = 16;
    protected Integer value;

    /**
     * Creates a new SignedShort object.
     *
     * @param value to set
     */
    public SignedShort(Short value) {
        this.value = new Integer(value);
        signed = true;
    }

    /**
     * Creates a new SignedShort object from jdom element - used for xml decoding.
     *
     * @param element to be decoded
     */
    public SignedShort(Element element) {
        decodeXML(element);
    }

    /**
     * Creates a new SignedShort object - might loose precision.
     *
     * @param value to set
     */
    public SignedShort(Integer value) {
        this.value = value;
        signed = true;
    }
    
   

    /**
     * String representation in specified radix.
     *
     */
    public SignedShort(String valueString, int radix) {
        this(new BigInteger(valueString, radix).intValue());
    }

    public SignedShort(int value) {
       this(new Integer(value));
       if (!inRange(value)){
			throw new IllegalArgumentException("value "+value+" not in range allowed for SignedShort");
		}
    }
    
    /**
     * Creates a new SignedShort object.
     */
    public SignedShort() {
        value = 0;
        signed = true;
    }

    /**
     * Creates a new SignedShort object.
     *
     * @param bitList to be decoded
     */
    public SignedShort(LLRPBitList bitList) {
        decodeBinary(bitList);
        signed = true;
    }

    /**
     * String representation in radix 10.
     *
     */
    public SignedShort(String valueString) {
        this(valueString,10);
        if (!inRange(valueString)){
			throw new IllegalArgumentException("value "+valueString+" not in range");
		}
    }
    
    /**
     * decode from binary
     *
     * @param bitList to be decoded
     */
    public void decodeBinary(LLRPBitList bitList) {
        String bitString = bitList.toString();

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
    }

    /**
     * get number of bits used to represent this type
     *
     * @return Integer
     */
    public static int length() {
        return LENGTH;
    }

    /**
     * Integereger representation - no loss in precision
     *
     * @return Integer
     */
    public Integer toInteger() {
        return new Integer(toShort());
    }
    
    public int intValue(){
    	return toInteger().intValue();
    }

    /**
     * short representation - no loss in precision
     *
     * @return short
     */
    public short toShort() {
        return value.shortValue();
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
        element.setContent(new Text(value.toString()));

        return element;
    }
    
    public String toString() {
        return Integer.toString(value);
    }
    
    public String toString(int radix){
    	return Integer.toString(value, radix);
    }
    
    @Override
	public boolean inRange(long value) {
		return (value >= Short.MIN_VALUE && value <= Short.MAX_VALUE);
	}
    
    public boolean inRange(String valueString){
		return inRange(new BigInteger(valueString).longValue());
	}
}
