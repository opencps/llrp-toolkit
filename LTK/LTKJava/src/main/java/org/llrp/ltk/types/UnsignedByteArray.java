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
 * UnsignedByteArray - length encoded as first 16 bits!
 *
 * @author gasserb
 */
public class UnsignedByteArray extends LLRPType {
    private LLRPInteger[] bytes;

    /**
         * Creates a new UnsignedByteArray object.
         *
         * @param bytes
         */
    public UnsignedByteArray(LLRPInteger[] bytes) {
        this.bytes = bytes.clone();
    }

    /**
     * Creates a new UnsignedByteArray object from jdom element - used for xml decoding
     *
     * @param bitList
     */
    public UnsignedByteArray(Element element) {
        decodeXML(element);
    }

    /**
         * all values initially set to 0
         * @param length
         */
    public UnsignedByteArray(Integer length) {
        bytes = new LLRPInteger[length];
    }

    /**
         * Creates a new UnsignedByteArray object.
         */
    public UnsignedByteArray() {
        bytes = new LLRPInteger[0];
    }

    /**
         * create ByteArray from BitList. First 16 Bits must be length of ByteArray
         */
    public UnsignedByteArray(LLRPBitList list) {
        decodeBinary(list);
    }

    /**
         * Creates a new UnsignedByteArray object.
         *
         * @param bytes
         */
    public UnsignedByteArray(byte[] bytes) {
        this.bytes = new LLRPInteger[bytes.length];

        for (Integer i = 0; i < bytes.length; i++) {
            this.bytes[i] = new LLRPInteger(bytes[i]);
        }
    }

    /**
     * encodes length before encoding containing values
     *
     * @return LLRPBitList
     */
    public LLRPBitList encodeBinary() {
        LLRPBitList result = new LLRPBitList();
        result.append(new UnsignedShort(bytes.length).encodeBinary());

        for (Integer i = 0; i < bytes.length; i++) {
            result.append(bytes[i].encodeBinary());
        }

        return result;
    }

    /**
     * number of bytes used to represent this type
     *
     * @return Integer
     */
    public Integer getByteLength() {
        return bytes.length;
    }

    /**
     * length of BaseType - not of the array - for array length call size()
     *
     * @return
     */
    public static Integer length() {
        return LLRPInteger.length();
    }

    /**
     * first 16 bits must be number of Bytes that follow
     *
     * @param list
     */
    @Override
    public void decodeBinary(LLRPBitList list) {
        Integer length = new SignedShort(list.subList(0, SignedShort.length())).toInteger();
        bytes = new LLRPInteger[length];

        for (Integer i = 1; i <= length; i++) {
            bytes[i - 1] = new LLRPInteger(list.subList(
                        i * LLRPInteger.length(), LLRPInteger.length()));
        }
    }

    /**
     * get UnsignedByte at specified position
     *
     * @param i
     *
     * @return LLRPInteger
     */
    public LLRPInteger get(Integer i) {
        return bytes[i];
    }

    /**
     * set Byte at provided position to provided byte
     *
     * @param i
     * @param b
     */
    public void set(Integer i, LLRPInteger b) {
        if ((i < 0) || (i > bytes.length)) {
            return;
        } else {
            bytes[i] = b;
        }
    }

    /**
     * number of elements in array
     *
     * @return
     */
    public Integer size() {
        return bytes.length;
    }

    @Override
    public Content encodeXML(String name) {
        String s = "";

        for (LLRPInteger b : bytes) {
            s += " ";
            s += b.toInteger().toString();
        }

        s = s.replaceFirst(" ", "");

        Element element = new Element(name);
        element.setContent(new Text(s));

        return element;
    }

    @Override
    public void decodeXML(Element element) {
        String text = element.getText();
        String[] strings = text.split(" ");
        bytes = new LLRPInteger[strings.length];

        for (int i = 0; i < strings.length; i++) {
            bytes[i] = new LLRPInteger(strings[i]);
        }
    }
}
