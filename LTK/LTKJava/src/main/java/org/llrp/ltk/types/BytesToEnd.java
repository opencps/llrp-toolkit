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

import java.util.LinkedList;
import java.util.List;


/**
 * List of Bytes of unbound length.
 *
 * @author Basil Gasser - ETH Zurich
 */
public class BytesToEnd extends LLRPType {
    protected List<LLRPInteger> bytes = new LinkedList<LLRPInteger>();

    /**
         * Creates a new BytesToEnd object.
         */
    public BytesToEnd() {
        bytes = new LinkedList<LLRPInteger>();
    }

    /**
         * basically an endless array of bytes.
         * @param bits Integererpredet as block of bytes
         */
    public BytesToEnd(LLRPBitList bits) {
        decodeBinary(bits);
    }

    /**
     * Creates a new BytesToEnd object from jdom element - used for xml decoding.
     *
     * @param element to be decoded
     */
    public BytesToEnd(Element element) {
        decodeXML(element);
    }

    /**
     * add a byte to the end of all bytes.
     *
     * @param aByte to be added
     */
    public void add(LLRPInteger aByte) {
        bytes.add(aByte);
    }

    /**
     * overwritten decoding function.
     *
     * @param list to be decoded
     */
    public void decodeBinary(LLRPBitList list) {
        if (list.length() <= 16) {
            return;
        }

        Integer length = new SignedInteger(list.subList(0, 16)).toInteger();
        bytes = new LinkedList<LLRPInteger>();

        for (int i = 16; i < length; i = i + LLRPInteger.length()) {
            bytes.add(new LLRPInteger(list.subList(i, LLRPInteger.length())));
        }
    }

    /**
     * overwritten encoding function.
     *
     * @return LLRPBitList
     */
    public LLRPBitList encodeBinary() {
        LLRPBitList result = new LLRPBitList();

        for (LLRPInteger u : bytes) {
            result.append(u.encodeBinary());
        }

        return result;
    }

    /**
     * get a byte at a specified position.
     *
     * @param i position to be get
     *
     * @return LLRPInteger
     */
    public LLRPInteger get(Integer i) {
        return bytes.get(i);
    }

    /**
     * length in number of bytes (equal to number of bytes in list).
     *
     * @return Integer
     */
    public int getByteLength() {
        return bytes.size();
    }

    /**
     * length of BaseType - not of the array - for array length call size().
     *
     * @return  length of BaseType
     */
    public static int length() {
        return LLRPInteger.length();
    }

    /**
     * number of elements.
     *
     * @return number of elements
     */
    public int size() {
        return bytes.size();
    }

    /**
     * @Override
     * {@inheritDoc}
     */
    public Content encodeXML(String name, Namespace ns) {
        String s = "";

        for (LLRPInteger b : bytes) {
            s += b.toString();
        }

        Element element = new Element(name, ns);
        element.setContent(new Text(s));

        return element;
    }

    /**
     * @Override
     * {@inheritDoc}
     */
    public void decodeXML(Element element) {
        String byteString = element.getText();
        bytes = new LinkedList<LLRPInteger>();

        int length = byteString.length();

        for (Integer i = 0; i < length; i = i + LLRPInteger.length()) {
            bytes.add(new LLRPInteger(byteString.substring(i,
                        LLRPInteger.length())));
        }
    }
    
	
	public String toString() {
		return toString(2);
	}
	
	public String toString(int radix){
        String s = "";

        for (LLRPInteger b : bytes) {
            s += b.toString(radix);
        }
        return s;
	}
}
