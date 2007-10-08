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
 * Integer with 96 Bits used to represent!
 *
 * @author Basil Gasser - ETH Zurich
 */
public class Integer96 extends LLRPNumberType {
    private static final Integer length = 96;
    protected BigInteger value;

    /**
         * Creates a new Integer96 object.
         *
         * @param value
         */
    public Integer96(BigInteger value) {
        this.value = value;
        signed = true;
    }

    /**
         * Creates a new Integer96 object.
         */
    public Integer96() {
        signed = true;
    }

    /**
         * Creates a new Integer96 object.
         *
         * @param bitList
         */
    public Integer96(LLRPBitList bitList) {
        decodeBinary(bitList);
        signed = true;
    }

    /**
     * Creates a new Integer96 object.
     *
     * @param bitList
     */
    public Integer96(Element element) {
        decodeXML(element);
    }

    /**
     *        overwrite decodeBinary as it ha to work differently
     *
     * @param bitList
     */
    public void decodeBinary(LLRPBitList bitList) {
        if (bitList.length() < length) {
            bitList.pad(length - bitList.length());
        }

        value = new BigInteger(bitList.toString(), 2);
    }

    @Override
    public LLRPBitList encodeBinary() {
        LLRPBitList result = new LLRPBitList(value.toString(2));

        if (result.length() < length) {
            result.pad(length - result.length());
        }

        return result.subList(result.length() - length, length);
    }

    /**
     * length in number of bits used to represent this type
     *
     * @return Integer
     */
    public static Integer length() {
        return length;
    }

    /**
     * this Integer converted to java Integerer - miht cause loss of precision
     *
     * @return Integer
     */
    public Integer toInteger() {
        return value.intValue();
    }

    /**
     * set bits
     *
     * @param value
     */
    protected void setBits(Number value) {
        this.value = new BigInteger(value.toString());
    }

    @Override
    public void decodeXML(Element element) {
        this.value = new BigInteger(element.getText());
    }

    @Override
    public Content encodeXML(String name) {
        Element element = new Element(name);
        element.setContent(new Text(value.toString()));

        return element;
    }

    public String toString() {
        return value.toString();
    }
}
