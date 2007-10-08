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
import org.jdom.Text;

import java.math.BigInteger;


/**
 * UnsignedInteger - type does not exist in java
 *
 * @author Basil Gasser - ETH Zurich
 */
public class UnsignedInteger extends LLRPNumberType {
    // value interpreted as unsigned Integer
    // leading zero not sign but number
    private static final Integer length = 32;
    protected BigInteger value;

    /**
     * Creates a new UnsignedInteger object from Java Integer - might loose precision
     *
     * @param value
     */
    public UnsignedInteger(Integer value) {
        this.value = new BigInteger(value.toString());
        signed = false;
    }

    /**
     * Creates a new UnsignedInteger object.
     *
     * @param value
     */
    public UnsignedInteger(String valueString) {
        value = new BigInteger(valueString);
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
     * @param bitList
     */
    public UnsignedInteger(LLRPBitList bitList) {
        decodeBinary(bitList);
        signed = false;
    }

    /**
     * Creates a new UnsignedInteger object from jdom element - used for xml decoding
     *
     * @param bitList
     */
    public UnsignedInteger(Element element) {
        decodeXML(element);
    }

    /**
     * decode to binary representation
     *
     * @param bitList
     */
    public void decodeBinary(LLRPBitList bitList) {
        value = new BigInteger(bitList.toString(), 2);
    }

    /**
     * number of bits used to represent this type
     *
     * @return Integer
     */
    public static Integer length() {
        return length;
    }

    /**
     * this might return a false value. Java Integer are signed and therefore
     * might not provide enough precision
     *
     * @return
     */
    public Integer toInteger() {
        return value.intValue();
    }

    /**
     * Unsigned Integer wrapped Integero java long
     *
     * @return
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

        if (result.length() < length) {
            result.pad(length - result.length());
        }

        return result.subList(result.length() - length, length);
    }

    @Override
    public Content encodeXML(String name) {
        Element element = new Element(name);
        element.setContent(new Text(value.toString()));

        return element;
    }

    public String toString() {
        return toLong().toString();
    }
}
