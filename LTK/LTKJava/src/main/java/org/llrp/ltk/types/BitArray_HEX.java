package org.llrp.ltk.types;

import org.jdom.Element;


public class BitArray_HEX extends BitArray {
    /**
    * create a new BitArray_HEX.
    * When encoded, BitArray_HEX also encodes its length.
    * @param bits to be decoded
    */
    public BitArray_HEX(Bit[] bits) {
        super(bits);
    }

    /**
         * create a new BitArray_HEX.
         * When encoded, BitArray_HEX also encodes its length.
         * @param list to be decoded
         */
    public BitArray_HEX(LLRPBitList list) {
        super(list);
    }

    /**
         * create a new BitArray_HEX.
         * When encoded, BitArray_HEX also encodes its length.
         * Initially all bits set to 0
         * @param length of array
         */
    public BitArray_HEX(Integer length) {
        super(length);
    }

    /**
         * empty bit array.
         */
    public BitArray_HEX() {
        super();
    }

    /**
     * @param element to be decoded
     */
    public BitArray_HEX(Element element) {
        super(element);
    }

    /**
     * length in number of bits used to represent this type.
     *
     * @return
     */
    public static Integer length() {
        return LENGTH;
    }
}
