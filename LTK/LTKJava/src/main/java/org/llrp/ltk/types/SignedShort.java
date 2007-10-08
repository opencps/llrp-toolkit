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
 * SignedShort
 *
 * @author Basil Gasser - ETH Zurich
 */
public class SignedShort extends LLRPNumberType {
    private static final Integer length = 16;
    protected Integer value;

    /**
     * Creates a new SignedShort object.
     *
     * @param value
     */
    public SignedShort(Short value) {
        this.value = new Integer(value);
        signed = true;
    }

    /**
     * Creates a new SignedShort object from jdom element - used for xml decoding
     *
     * @param bitList
     */
    public SignedShort(Element element) {
        decodeXML(element);
    }

    /**
     * Creates a new SignedShort object - might loose precision.
     *
     * @param value
     */
    public SignedShort(Integer value) {
        this.value = value;
        signed = true;
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
     * @param bitList
     */
    public SignedShort(LLRPBitList bitList) {
        decodeBinary(bitList);
        signed = true;
    }

    /**
     * decode from binary
     *
     * @param bitList
     */
    public void decodeBinary(LLRPBitList bitList) {
        value = Integer.parseInt(bitList.toString(), 2);
    }

    /**
     * get number of bits used to represent this type
     *
     * @return Integer
     */
    public static Integer length() {
        return length;
    }

    /**
     * Integereger representation - no loss in precision
     *
     * @return Integer
     */
    public Integer toInteger() {
        return new Integer(toShort());
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
