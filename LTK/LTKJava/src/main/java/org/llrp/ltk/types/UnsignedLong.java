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
 * UnsignedLong - type does not exist in java
 *
 * @author Basil Gasser - ETH Zurich
 */
public class UnsignedLong extends LLRPNumberType {
    private static final Integer LENGTH = 64;
    protected BigInteger value;

    /**
         * Creates a new UnsignedLong object.
         *
         * @param value to set
         */
    public UnsignedLong(Long value) {
        this.value = new BigInteger(value.toString());
        signed = false;
    }

    /**
     * Creates a new UnsignedLong object from jdom element - used for xml decoding
     *
     * @param element to be decoded
     */
    public UnsignedLong(Element element) {
        decodeXML(element);
    }

    /**
         * Creates a new UnsignedLong object.
         *
         * @param bitList to be decoded
         */
    public UnsignedLong(LLRPBitList bitList) {
        decodeBinary(bitList);
        signed = false;
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
     * compare
     *
     * @param other to compare
     *
     * @return boolean
     */
    public boolean equals(LLRPNumberType other) {
        if (other instanceof UnsignedLong) {
            return toBigInteger().equals(((UnsignedLong) other).toBigInteger());
        } else {
            return toBigInteger()
                       .equals(new BigInteger(other.encodeBinary().toString(), 2));
        }
    }

    /**
     * number of bits used to represent this type
     *
     * @return Integer
     */
    public static Integer length() {
        return LENGTH;
    }

    /**
     * UnsignedLong wrapped Integero BigInteger
     *
     * @return
     */
    public BigInteger toBigInteger() {
        return value;
    }

    /**
     * number of bits used to represent this type
     *
     * @return Integer
     */
    public Integer toInteger() {
        return new Long(toLong()).intValue();
    }

    /**
     * this might return a false value. Java long are signed and therefore might
     * not provide enough precision
     *
     * @return
     */
    public long toLong() {
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
    public Content encodeXML(String name) {
        Element element = new Element(name);
        element.setContent(new Text(value.toString()));

        return element;
    }

    public int hashCode() {
        return value.hashCode();
    }
}
