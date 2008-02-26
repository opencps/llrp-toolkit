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
 * UnsignedShort
 *
 * @author Basil Gasser - ETH Zurich
 */
public class UnsignedShort extends LLRPNumberType {
    private static final Integer LENGTH = 16;
    protected Integer value;

    /**
     * Creates a new UnsignedShort object - might loose precision
     *
     * @param value to set
     */
    public UnsignedShort(Short value) {
        this.value = new Integer(value);
        signed = false;
    }

    /**
     * Creates a new UnsignedShort object from jdom element - used for xml decoding
     *
     * @param element to be decoded
     */
    public UnsignedShort(Element element) {
        decodeXML(element);
    }

    /**
     * Creates a new UnsignedShort object.
     */
    public UnsignedShort() {
        value = 0;
        signed = false;
    }

    /**
     * Creates a new UnsignedShort object.
     *
     * @param value  to set
     */
    public UnsignedShort(Integer value) {
        this.value = value;
        signed = false;
    }

    /**
     * Creates a new UnsignedShort object.
     *
     * @param valueString value as string
     */
    public UnsignedShort(String valueString) {
        value = new Integer(valueString);
        signed = false;
    }

    /**
     * Creates a new UnsignedShort object.
     *
     * @param bitList to be decoded
     */
    public UnsignedShort(LLRPBitList bitList) {
        decodeBinary(bitList);
        signed = false;
    }

    /**
     * test
     *
     * @param bitList
     */
    public LLRPBitList encodeBinary() {
        LLRPBitList result = new LLRPBitList(Integer.toBinaryString(value));

        if (result.length() < LENGTH) {
            result.pad(LENGTH - result.length());
        }

        return result.subList(result.length() - LENGTH, LENGTH);
    }

    /**
     * test
     *
     * @return test
     */
    public static int length() {
        return LENGTH;
    }

    /**
     * wrap UnsignedShort Integero Integer
     *
     * @return
     */
    public Integer toInteger() {
        return value;
    }

    /**
     * this might return a false value. Java short are signed and therefore
     * might not provide enough precision
     *
     * @return
     */
    public short toShort() {
        return value.shortValue();
    }

    @Override
    public void decodeBinary(LLRPBitList list) {
        value = Integer.parseInt(list.toString(), 2);
    }

    @Override
    public void decodeXML(Element element) {
        value = new Integer(element.getText());
    }

    @Override
    public Content encodeXML(String name, Namespace ns) {
        Element element = new Element(name, ns);
        element.setContent(new Text(value.toString()));

        return element;
    }
}
