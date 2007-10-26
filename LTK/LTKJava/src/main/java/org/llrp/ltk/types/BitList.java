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

import java.math.BigInteger;

import java.util.ArrayList;


/**
 * List of Bits - length not encoded!
 *
 * @author gasserb
 */
public class BitList extends LLRPType {
    private Integer length;
    private Bit[] bits;

    /**
         * Generate a list of bits - bit[0] is the least significant bit.
         * @param bits to be decoded
         */
    public BitList(Bit[] bits) {
        //the bits are provided with least significant bit first. However internally
        // they are stored with most significant bit at positon 0
        length = bits.length;
        this.bits = new Bit[bits.length];

        for (int i = 0; i < length; i++) {
            this.bits[length - i - 1] = new Bit(bits[i].value);
        }
    }

    /**
     * create BitList - String must consist of numbers only. Everything but 0 is interpreted as 1
     * @param bitString to be decoded
     */
    public BitList(String bitString) {
        this();
        setValue(Integer.parseInt(bitString, 2));
    }

    /**
         * Creates a new BitList object.
         *
         * @param list to be decoded
         */
    public BitList(LLRPBitList list) {
        decodeBinary(list);
    }

    public BitList(Integer... integers) {
        ArrayList<Bit> arrayList = new ArrayList<Bit>();

        for (Integer i : integers) {
            arrayList.add(new Bit(i));
        }

        bits = new Bit[arrayList.size()];

        int j = 0;

        for (Bit b : arrayList) {
            bits[j] = b;
            j++;
        }

        length = bits.length;
    }

    /**
         * Generate a list of bits.
         * All bits initially set to 0
         * @param l length
         */
    public BitList(Integer l) {
        bits = new Bit[l];

        for (Integer i = 0; i < l; i++) {
            bits[i] = new Bit(false);
        }

        length = l;
    }

    /**
         * Creates a new BitList object.
         */
    public BitList() {
        bits = new Bit[1];
        length = 1;
    }

    public void setValue(Integer value) {
        String bitString = Integer.toBinaryString(value);
        int diff = 0;

        if (length > bitString.length()) {
            // don't loose the length initally set with constructor
            diff = length - bitString.length();
        } else {
            // however if value is bigger, automatically make BitList longer
            length = bitString.length();
            bits = new Bit[length];
        }

        for (int i = 0; i < bitString.length(); i++) {
            bits[diff + i] = new Bit("" + bitString.charAt(i));
        }
    }

    /**
     * set Bit at specified position to 0.
     *
     * @param i position to clear
     */
    public void clear(Integer i) {
        if ((i < 0) || (i > bits.length)) {
            return;
        } else {
            bits[i] = new Bit(false);
        }
    }

    /**
     * just like BitArray but does not encode length before values.
     *
     * @return
     */
    public LLRPBitList encodeBinary() {
        LLRPBitList result = new LLRPBitList(bits.length);

        for (Integer i = 0; i < bits.length; i++) {
            if (bits[i].toBoolean()) {
                result.set(i);
            } else {
                result.clear(i);
            }
        }

        return result;
    }

    /**
     * length of the list.
     *
     * @return Integer
     */
    public Integer length() {
        return length;
    }

    /**
     * decode bits from BitList. Length must not be provided
     *
     * @param list to be decoded
     */
    @Override
    public void decodeBinary(LLRPBitList list) {
        bits = new Bit[list.length()];

        for (Integer i = 0; i < list.length(); i++) {
            bits[i] = new Bit(list.get(i));
        }
    }

    /**
     * get bit at specified position.
     *
     * @param i position to get
     *
     * @return Bit
     */
    public Bit get(Integer i) {
        return bits[i];
    }

    /**
     * set Bit at specified position to 1.
     *
     * @param i position to be set
     */
    public void set(Integer i) {
        if ((i < 0) || (i > bits.length)) {
            return;
        } else {
            bits[i] = new Bit(true);
        }
    }

    @Override
    public Content encodeXML(String name) {
        String s = "";

        for (Bit b : bits) {
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
        String[] bitStrings = text.split(" ");
        bits = new Bit[bitStrings.length];

        for (int i = 0; i < bits.length; i++) {
            bits[i] = new Bit(bitStrings[i]);
        }
    }

    public BigInteger toInteger() {
        return new BigInteger(toBinaryString(), 2);
    }

    public String toBinaryString() {
        String s = "";

        for (Bit b : bits) {
            s += b.toInteger().toString();
        }

        return s;
    }

    public String toString() {
        return toInteger().toString();
    }
}
