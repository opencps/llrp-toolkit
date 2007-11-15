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
 * TwoBitField - a field consisting of two bits
 *
 * @author gasserb
 */
public class TwoBitField extends LLRPType {
    private static Integer length = 2;
    private Bit[] bits;

    /**
         * Generate a list of bits
         * @param bits to be decoded
         */
    public TwoBitField(Bit[] bits) {
        this.bits = new Bit[length];
        this.bits[0] = bits[0];
        this.bits[1] = bits[1];
    }

    /**
         * Creates a new TwoBitField object.
         *
         * @param list to be decoded
         */
    public TwoBitField(LLRPBitList list) {
        this.bits = new Bit[length];
        this.bits[0] = bits[0];
        this.bits[1] = bits[1];
        decodeBinary(list);
    }

    /**
         * Creates a new TwoBitField object.
         */
    public TwoBitField() {
        this.bits = new Bit[length];
        this.bits[0] = new Bit(0);
        this.bits[1] = new Bit(0);
    }

    /**
     * Creates a new TwoBitField object.
     * @param element to be decoded
     */
    public TwoBitField(Element element) {
        decodeXML(element);
    }

    /**
     * set Bit at specified position to 0
     *
     * @param i position to be set to 0
     */
    public void clear(Integer i) {
        if ((i < 0) || (i > bits.length)) {
            return;
        } else {
            bits[i] = new Bit(false);
        }
    }

    /**
     * just like BitArray but does not encode length before values
     *
     * @return LLRPBitList
     */
    public LLRPBitList encodeBinary() {
        LLRPBitList result = new LLRPBitList(bits.length);

        for (Integer i = 0; i < length; i++) {
            if (bits[i].toBoolean()) {
                result.set(i);
            } else {
                result.clear(i);
            }
        }

        return result;
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
     * decode bits from BitList. Length must not be provided
     *
     * @param list to be decoded
     */
    @Override
    public void decodeBinary(LLRPBitList list) {
        bits = new Bit[length];

        for (Integer i = 0; i < length; i++) {
            bits[i] = new Bit(list.get(i));
        }
    }

    /**
     * get bit at I
     *
     * @param i position to be returned
     *
     * @return bIT
     */
    public Bit get(Integer i) {
        return bits[i];
    }

    /**
     * set Bit at specified position to 1
     *
     * @param i position to be set to 1
     */
    public void set(Integer i) {
        if ((i < 0) || (i > bits.length)) {
            return;
        } else {
            bits[i] = new Bit(true);
        }
    }

    @Override
    public Content encodeXML(String name, Namespace ns) {
        
    	Integer s = bits[0].toInteger() + (bits[1].toInteger() * 2);

        Element element = new Element(name, ns);
        element.setContent(new Text(s.toString()));

        return element;
    }

    @Override
    public void decodeXML(Element element) {
        String text = element.getText();
        String[] bitStrings = text.split(" ");
        bits = new Bit[bitStrings.length];

        for (int i = 0; i < bits.length; i++) {
            bits[i] = new Bit(bitStrings[i]);
        }
    }
}
