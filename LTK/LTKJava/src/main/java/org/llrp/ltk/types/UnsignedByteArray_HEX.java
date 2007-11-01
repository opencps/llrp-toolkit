package org.llrp.ltk.types;

import org.jdom.Element;


public class UnsignedByteArray_HEX extends UnsignedByteArray {
    /**
    * Creates a new UnsignedByteArray object.
    *
    * @param bytes to create UnsignedByteArray
    */
    public UnsignedByteArray_HEX(UnsignedByte[] bytes) {
        super(bytes);
    }

    /**
     * Creates a new UnsignedByteArray object from jdom element - used for xml decoding
     *
     * @param element to be decoded
     */
    public UnsignedByteArray_HEX(Element element) {
        super(element);
    }

    /**
         * all values initially set to 0
         * @param length of array
         */
    public UnsignedByteArray_HEX(Integer length) {
        super(length);
    }

    /**
         * Creates a new UnsignedByteArray object.
         */
    public UnsignedByteArray_HEX() {
        super();
    }

    /**
         * create ByteArray from BitList. First 16 Bits must be length of ByteArray
         * @param list to be decoded
         */
    public UnsignedByteArray_HEX(LLRPBitList list) {
        super(list);
    }

    /**
         * Creates a new UnsignedByteArray object.
         *
         * @param bytes to create UnsignedByteArray
         */
    public UnsignedByteArray_HEX(byte[] bytes) {
        super(bytes);
    }
}
