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
 * 8 Bit Integer!
 *
 * @author Basil Gasser - ETH Zurich
 */
public class LLRPInteger extends LLRPNumberType {
    private static final Integer length = 8;
    protected Integer value;

    /**
     * Creates a new LLRPInteger object.
     *
     * @param value
     */
    public LLRPInteger(Byte value) {
        this.value = new Integer(value);
        signed = false;
    }

    /**
     * Creates a new LLRPInteger object.
     *
     * @param value
     */
    public LLRPInteger(String valueString) {
        this(new Byte(valueString));
    }

    /**
     * Creates a new LLRPInteger object from Integer - might loose precision
     *
     * @param value
     */
    public LLRPInteger(Integer value) {
        this(value.byteValue());
    }

    /**
     * Creates a new LLRPInteger object.
     *
     * @param bitList
     */
    public LLRPInteger(LLRPBitList bitList) {
        decodeBinary(bitList);
        signed = false;
    }

    /**
     * Creates a new LLRPInteger object.
     *
     * @param bitList
     */
    public LLRPInteger(Element element) {
        decodeXML(element);
        signed = false;
    }

    /**
     * Creates a new LLRPInteger object.
     */
    public LLRPInteger() {
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
     * this represented as byte - no loss of precision
     *
     * @return Byte
     */
    public Byte toByte() {
        return value.byteValue();
    }

    /**
     * this represented as Integereger - no loss of precision
     *
     * @return Integer
     */
    public Integer toInteger() {
        return new Integer(toByte());
    }

    @Override
    public void decodeXML(Element element) {
        this.value = new Integer(element.getText());
    }

    @Override
    public Content encodeXML(String name) {
        Element element = new Element(name);
        element.setContent(new Text(value.toString()));
        LLRPBitList.class.getName()
                         .replaceAll(LLRPBitList.class.getPackage().getName(),
            "");

        return element;
    }

    @Override
    public void decodeBinary(LLRPBitList list) {
        value = Integer.valueOf(list.toString(), 2);
    }

    @Override
    public LLRPBitList encodeBinary() {
        LLRPBitList result = new LLRPBitList(Integer.toBinaryString(value));

        if (result.length() < length) {
            result.pad(length - result.length());
        }

        return result.subList(result.length() - length, length);
    }

    public String toString() {
        return Integer.toString(value);
    }
}
