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
 * UnsignedIntegerArray - length encoded as first 16 bits!
 *
 * @author gasserb
 */
public class UnsignedIntegerArray extends LLRPType {
    private UnsignedInteger[] integers;

    /**
         * Creates a new UnsignedIntegerArray object.
         *
         * @param Integers
         */
    public UnsignedIntegerArray(UnsignedInteger[] Integers) {
        this.integers = Integers.clone();
    }

    /**
     * Creates a new UnsignedIntegerArray object from jdom element - used for xml decoding
     *
     * @param bitList
     */
    public UnsignedIntegerArray(Element element) {
        decodeXML(element);
    }

    /**
         * Creates a new UnsignedIntegerArray object.
         *
         * @param length
         */
    public UnsignedIntegerArray(Integer length) {
        integers = new UnsignedInteger[length];
    }

    /**
         * first 16 bits of LLRPBitlist must indicate number of entries that follow
         */
    public UnsignedIntegerArray(LLRPBitList bits) {
        decodeBinary(bits);
    }

    /**
         * Creates a new UnsignedIntegerArray object.
         */
    public UnsignedIntegerArray() {
        integers = new UnsignedInteger[0];
    }

    /**
     * encodes length before encoding containing values
     *
     * @return
     */
    public LLRPBitList encodeBinary() {
        LLRPBitList result = new LLRPBitList();
        result.append(new UnsignedShort(integers.length).encodeBinary());

        for (Integer i = 0; i < integers.length; i++) {
            result.append(integers[i].encodeBinary());
        }

        return result;
    }

    /**
     * number of bytes used to represent this type
     *
     * @return Integer
     */
    public Integer getByteLength() {
        return integers.length * 2;
    }

    /**
     * length of BaseType not array - for array length call size()
     *
     * @return
     */
    public static Integer length() {
        return UnsignedInteger.length();
    }

    /**
     * first 16 bits of LLRPBitlist must indicate number of entries that follow
     *
     * @param list
     */
    public void decodeBinary(LLRPBitList list) {
        Integer length = new SignedInteger(list.subList(0,
                    SignedInteger.length())).toInteger();
        integers = new UnsignedInteger[length];

        for (Integer i = 1; i <= length; i++) {
            integers[i - 1] = new UnsignedInteger(list.subList(
                        i * SignedInteger.length(), SignedInteger.length()));
        }
    }

    /**
     * get UnsignedInteger at specified position
     *
     * @param i
     *
     * @return UnsignedInteger
     */
    public UnsignedInteger get(Integer i) {
        return integers[i];
    }

    /**
     * set UnsignedInteger at i to b
     *
     * @param i
     * @param b
     */
    public void set(Integer i, UnsignedInteger b) {
        if ((i < 0) || (i > integers.length)) {
            return;
        } else {
            integers[i] = b;
        }
    }

    /**
     * number of elements in array
     *
     * @return
     */
    public Integer size() {
        return integers.length;
    }

    @Override
    public Content encodeXML(String name) {
        String s = "";

        for (UnsignedInteger b : integers) {
            s += " ";
            s += b.toLong();
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
        integers = new UnsignedInteger[strings.length];

        for (int i = 0; i < strings.length; i++) {
            integers[i] = new UnsignedInteger(strings[i]);
        }
    }
}
