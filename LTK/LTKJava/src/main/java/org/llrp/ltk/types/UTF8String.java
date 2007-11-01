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
 * Strig with UT8 Encoding!
 *
 * @author gasserb
 */
public class UTF8String extends LLRPType {
    //8 because we encode it as a list of bytes
    protected static Integer LENGTH = 8;
    protected String string;

    protected UTF8String() {
        string = "";
    }

    /**
         * Creates a new UTF8String object.
         *
         * @param subList to be decoded
         */
    public UTF8String(LLRPBitList subList) {
        decodeBinary(subList);
    }

    /**
     * Creates a new UTF8String object from jdom element - used for xml decoding
     *
     * @param element to be decoded
     */
    public UTF8String(Element element) {
        decodeXML(element);
    }

    /**
         * Creates a new UTF8String object.
         *
         * @param string to be represented
         */
    public UTF8String(String string) {
        this.string = string;
    }

    /**
     * decode bits from BitList.
     *
     * @param list to be decoded
     */
    public void decodeBinary(LLRPBitList list) {
        LLRPBitList subList = list.subList(UnsignedShort.length(),
                list.length() - UnsignedShort.length());
        Byte[] bigBytes = subList.toByteArray();
        byte[] bytes = new byte[bigBytes.length];

        for (int i = 0; i < bytes.length; i++) {
            bytes[i] = bigBytes[i];
        }

        string = new String(bytes);
    }

    /**
     * return number of bits used to represent this type
     *
     * @return Integer
     */
    public static Integer length() {
        // 8 because we encod it as bytes and a byte has 8 bits
        return 8;
    }

    /**
     * encode to binary representation
     *
     * @return LLRPBitList
     */
    public LLRPBitList encodeBinary() {
        LLRPBitList result = new LLRPBitList();
        result.append(new UnsignedShort(string.toCharArray().length).encodeBinary());

        byte[] bytes = string.getBytes();
        Byte[] bigBytes = new Byte[bytes.length];

        for (int i = 0; i < bytes.length; i++) {
            bigBytes[i] = bytes[i];
        }

        result.append(new LLRPBitList(bigBytes));

        return result;
    }

    /**
     * compare two UTF8Strings
     *
     * @param other to compare
     *
     * @return boolean
     */
    public boolean equals(LLRPType other) {
        UTF8String u = (UTF8String) other;

        return string.equals(u.string);
    }

    /**
     * number of bytes used to represent this type
     *
     * @return Integer
     */
    public Integer getByteLength() {
        return (string.length() / 8);
    }

    @Override
    public void decodeXML(Element element) {
        string = element.getText();
    }

    @Override
    public Content encodeXML(String name, Namespace ns) {
        Element element = new Element(name, ns);
        element.setContent(new Text(string));

        return element;
    }

    public String toString() {
        return string;
    }

    public int hashCode() {
        return string.hashCode();
    }
}
