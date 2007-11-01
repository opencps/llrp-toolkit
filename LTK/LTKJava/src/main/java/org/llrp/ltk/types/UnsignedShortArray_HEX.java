package org.llrp.ltk.types;

import org.jdom.Element;


public class UnsignedShortArray_HEX extends UnsignedShortArray {
    public UnsignedShortArray_HEX(Element element) {
        super(element);
    }

    /**
         * Creates a new UnsignedShortArray object.
         *
         * @param length of array
         */
    public UnsignedShortArray_HEX(Integer length) {
        super(length);
    }

    /**
         * first 16 bits of LLRPBitlist must indicate number of entries that follow
         * @param bits  to be decoded
         */
    public UnsignedShortArray_HEX(LLRPBitList bits) {
        super(bits);
    }

    /**
         * Creates a new UnsignedShortArray_HEX object.
         */
    public UnsignedShortArray_HEX() {
        super();
    }
}
