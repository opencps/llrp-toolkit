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
 * Integer with 96 Bits used to represent.
 *
 * @author Basil Gasser - ETH Zurich
 */
public class Integer96 extends LLRPNumberType {
    protected static final Integer LENGTH = 96;
    protected BigInteger value;

    /**
         * Creates a new Integer96 object.
         *
         * @param value to set
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
         * @param bitList to be decoded
         */
    public Integer96(LLRPBitList bitList) {
        decodeBinary(bitList);
        signed = true;
    }

    /**
     * Creates a new Integer96 object.
     *
     * @param element to be decoded
     */
    public Integer96(Element element) {
        decodeXML(element);
    }

    /**
     *        overwrite decodeBinary as it ha to work differently.
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
            value = new BigInteger(bitString, 2);
            value = value.add(new BigInteger("1"));
            value = value.negate();
        } else {
            value = new BigInteger(bitString, 2);
        }
    }

    @Override
    public LLRPBitList encodeBinary() {
        LLRPBitList result = new LLRPBitList(value.toString(2));

        if (result.length() < LENGTH) {
            result.pad(LENGTH - result.length());
        }

        return result.subList(result.length() - LENGTH, LENGTH);
    }

    /**
     * length in number of bits used to represent this type.
     *
     * @return Integer
     */
    public static Integer length() {
        return LENGTH;
    }

    /**
     * this Integer converted to java Integerer - miht cause loss of precision.
     *
     * @return Integer
     */
    public Integer toInteger() {
        return value.intValue();
    }

    /**
     * set bits.
     *
     * @param value to set
     */
    protected void setBits(Number value) {
        this.value = new BigInteger(value.toString());
    }

    @Override
    public void decodeXML(Element element) {
        this.value = new BigInteger(element.getText());
    }

    @Override
    public Content encodeXML(String name, Namespace ns) {
        Element element = new Element(name, ns);
        element.setContent(new Text(value.toString()));

        return element;
    }

    public String toString() {
        return value.toString();
    }
 
}
