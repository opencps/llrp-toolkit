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


/**
 * UnsignedByteArray - length encoded as first 16 bits!
 *
 * @author gasserb
 */
public class UnsignedByteArray extends LLRPType {
    protected UnsignedByte[] bytes;

    /**
         * Creates a new UnsignedByteArray object.
         *
         * @param bytes to create UnsignedByteArray
         */
    public UnsignedByteArray(UnsignedByte[] bytes) {
        this.bytes = bytes.clone();
    }

    /**
     * Creates a new UnsignedByteArray object from jdom element - used for xml decoding
     *
     * @param element to be decoded
     */
    public UnsignedByteArray(Element element) {
        decodeXML(element);
    }

    /**
         * all values initially set to 0
         * @param length of array
         */
    public UnsignedByteArray(Integer length) {
        bytes = new UnsignedByte[length];
    }

	/**
	 * Creates an empty UnsignedByteArray. Do not call methood 'set' on an empty array.
	 * Add an UnsignedByte by calling the add method
	 */
    public UnsignedByteArray() {
        bytes = new UnsignedByte[0];
    }

    /**
         * create ByteArray from BitList. First 16 Bits must be length of ByteArray
         * @param list to be decoded
         */
    public UnsignedByteArray(LLRPBitList list) {
        decodeBinary(list);
    }

    /**
         * Creates a new UnsignedByteArray object.
         *
         * @param bytes to create UnsignedByteArray
         */
    public UnsignedByteArray(byte[] bytes) {
        this.bytes = new UnsignedByte[bytes.length];

        for (Integer i = 0; i < bytes.length; i++) {
            this.bytes[i] = new UnsignedByte(bytes[i]);
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
     * @param list to be decoded
     */
    @Override
    public void decodeBinary(LLRPBitList list) {
        Integer length = new SignedShort(list.subList(0, SignedShort.length())).toInteger();
        bytes = new UnsignedByte[length];

        for (Integer i = 1; i <= length; i++) {
            bytes[i - 1] = new UnsignedByte(list.subList(
                        i * UnsignedByte.length(), UnsignedByte.length()));
        }
    }

    /**
     * get UnsignedByte at specified position
     *
     * @param i position
     *
     * @return LLRPInteger
     */
    public UnsignedByte get(Integer i) {
        return bytes[i];
    }

    /**
     * set Byte at provided position to provided byte
     *
     * @param i position
     * @param b byte to be set
     */
    public void set(Integer i, UnsignedByte b) {
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
    public Content encodeXML(String name, Namespace ns) {
        String s = "";

        for (UnsignedByte b : bytes) {
            s += b.toInteger().toString();
        }

        s = s.replaceFirst(" ", "");

        Element element = new Element(name, ns);
        element.setContent(new Text(s));

        return element;
    }

    @Override
    public void decodeXML(Element element) {
        String text = element.getText();
        String[] strings = text.split(" ");
        bytes = new UnsignedByte[strings.length];

        for (int i = 0; i < strings.length; i++) {
            bytes[i] = new UnsignedByte(strings[i]);
        }
    }

    public void add(UnsignedByte aByte) {
        UnsignedByte[] newBytes = new UnsignedByte[bytes.length + 1];
        System.arraycopy(bytes, 0, newBytes, 0, bytes.length);
        newBytes[bytes.length] = aByte;
        bytes = newBytes;
    }
}
