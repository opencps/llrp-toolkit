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


/**
 * UnsignedShort
 *
 * @author Basil Gasser - ETH Zurich
 */
public class UnsignedByte extends LLRPNumberType {
    private static final Integer length = 8;
    protected Integer value;

    /**
         * Creates a new UnsignedByte object.
         *
         * @param value
         */
    public UnsignedByte(Integer value) {
        this.value = value;
        signed = false;
    }

    /**
     * Creates a new UnsignedByte object from byte - might loose information
     *
     * @param value
     */
    public UnsignedByte(byte value) {
        this.value = new Integer(value);
        signed = false;
    }

    /**
         * Creates a new UnsignedByte object.
         *
         * @param bitList
         */
    public UnsignedByte(LLRPBitList bitList) {
        decodeBinary(bitList);
        signed = false;
    }

    /**
         * Creates a new UnsignedByte object.
         */
    public UnsignedByte() {
        value = 0;
        signed = false;
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
     * to java Byte - might loose precision
     *
     * @return Byte
     */
    public Byte toByte() {
        return value.byteValue();
    }

    /**
     * to java Integer
     *
     * @return Integer
     */
    public Integer toInteger() {
        return value;
    }

    @Override
    public void decodeBinary(LLRPBitList list) {
        value = Integer.parseInt(list.toString(), 2);
    }

    @Override
    public void decodeXML(Element element) {
        value = Integer.parseInt(element.getText(), 2);
    }

    @Override
    public LLRPBitList encodeBinary() {
        LLRPBitList result = new LLRPBitList(Integer.toBinaryString(value));

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
}
