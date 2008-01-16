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

import org.llrp.ltk.exceptions.LLRPException;

import java.util.BitSet;


/**
 * A list of bits used for binary representation of messages.
 *
 * @author Basil Gasser - ETH Zurich
 */
public class LLRPBitList {
    private BitSet bits;
    private Integer length;

    /**
     * Creates a new LLRPBitList object.
     */
    public LLRPBitList() {
        bits = new BitSet(0);
        bits.clear(0);
        length = 0;
    }

    /**
     * bytes interpreted in order they appear in array.
     *
     * @param bytes interpreted in order they appear in array
     */
    public LLRPBitList(byte[] bytes) {
        bits = new BitSet(bytes.length * 8);
        length = bytes.length * 8;

        // iterate over all bytes
        for (Integer i = 0; i < bytes.length; i++) {
            Integer position = 8 * (i + 1);
            byte b = bytes[i];

            // iterate over each bit of one byte
            for (Integer j = 0; j < 8; j++) {
                // use bitarithmetic to set (1) or clear (0) a bit
                if ((b & (1 << j)) > 0) {
                    bits.set(position - j - 1);
                } else {
                    bits.clear(position + j);
                }
            }
        }
    }

    /**
     * create BitList from String. Everything but '0' is interpreted as 1.
     *
     * @param bitString to be decoded
     */
    public LLRPBitList(String bitString) {
        bits = new BitSet(bitString.length());
        length = bitString.length();

        for (Integer i = 0; i < bitString.length(); i++) {
            if (bitString.charAt(i) == '0') {
                bits.clear(i);
            } else {
                bits.set(i);
            }
        }
    }

    /**
     * creates lit list with all bits set to 0.
     *
     * @param n
     *            length of bit list
     */
    public LLRPBitList(Integer n) {
        bits = new BitSet(n);
        length = n;
    }

    /**
     * add a bit to bit list. Length is increased by 1.
     *
     * @param bit to be added
     */
    public void add(boolean bit) {
        if (bit) {
            bits.set(length);
        } else {
            bits.clear(length);
        }

        length++;
    }

    /**
     * appends other bit list to this. This list gets changed.
     *
     * @param other
     *            bit list
     */
    public void append(LLRPBitList other) {
        Integer oldLength = length;

        for (Integer i = 0; i < other.length; i++) {
            length++;

            // the used BiSet adjust the length of the underlying BitSet
            // automatically
            if (other.bits.get(i)) {
                bits.set(oldLength + i);
            } else {
                bits.clear(oldLength + i);
            }
        }
    }

    /**
     * clear bit at specified position.
     *
     * @param position to clear
     */
    public void clear(Integer position) {
        bits.clear(position);
    }

    /**
     * clone.
     *
     * @return cloned object
     */
    public Object clone() {
        try {
            super.clone();
        } catch (CloneNotSupportedException e) {
            // we just don't clone super
        }

        LLRPBitList n = new LLRPBitList();
        n.bits = (BitSet) bits.clone();
        n.length = length;

        return n;
    }

    /**
     * bitwise comparison.
     *
     * @param other to compare
     *
     * @return boolean
     */
    public boolean equals(LLRPBitList other) {
        if (other.length.compareTo(length) != 0) {
            return false;
        }

        for (Integer i = 0; i < length; i++) {
            if (this.get(i) ^ other.get(i)) {
                return false;
            }
        }

        return true;
    }

    /**
     * get bit as boolean value at specified position.
     *
     * @param position
     *            of bit
     *
     * @return returns true (bit set) or false
     */
    public boolean get(Integer position) {
        return bits.get(position);
    }

    /**
     * returns number of bits in this list.
     *
     * @return
     */
    public Integer length() {
        return length;
    }

    /**
     * add a list of 0s to front of list.
     *
     * @param number
     *            of bits to add at front
     */
    public void pad(Integer number) {
        // new bitset of length number
        BitSet n = new BitSet(number);

        for (Integer i = 0; i < length; i++) {
            if (bits.get(i)) {
                n.set(number + i);
            } else {
                n.clear(number + i);
            }
        }

        length += number;
        bits = n;
    }

    /**
     * set bit at specified position to true.
     *
     * @param position
     *            start at index 0
     */
    public void set(Integer position) {
        if (position > length) {
            length = position + 1;
        }

        bits.set(position);
    }

    /**
     * return a list containing a copy of the elements starting at from, having
     * length length.
     *
     * @param from
     *            where sublist starts, list start at Index 0
     * @param subLength
     *            long sublist is
     *
     * @return
     *
     * @throws LLRPException thrown
     */
    public LLRPBitList subList(Integer from, Integer subLength)
        throws LLRPException {
        if (from < 0) {
            // logger.error("try to start sublist at negative position - this is
            // not possible");
            throw new LLRPException(
                "try to start sublist at negative position - this is not possible");
        }

        if (length < (from + subLength)) {
            // logger.error("list not long enough. List has "+length+" elements,
            // tried to get sublist from "+from+" with length "+subLength);
            throw new LLRPException("list not long enough");
        }

        // return a new bitlist containing copies of the elements
        LLRPBitList b = new LLRPBitList(subLength);

        for (Integer i = 0; i < subLength; i++) {
            if (bits.get(from + i)) {
                b.set(i);
            } else {
                b.clear(i);
            }
        }

        return b;
    }

    /**
     * 8 bits bundled Integero one byte.
     *
     * @return byte Array
     */
    public byte[] toByteArray() {
        Integer nOFb = (length / 8);
        byte[] result = new byte[nOFb];

        for (Integer i = 0; i < nOFb; i++) {
            result[i] = new Byte(new LLRPInteger(subList(8 * i, 8)).toByte());
        }

        return result;
    }

    /**
     * encoded message as a string.
     *
     * @return String
     */
    public String toString() {
        String s = "";

        for (Integer i = 0; i < length; i++) {
            if (bits.get(i)) {
                s = s + "1";
            } else {
                s = s + "0";
            }
        }

        return s;
    }

    public int hashCode() {
        return bits.hashCode();
    }
}
