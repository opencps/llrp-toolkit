package org.llrp.ltk.types;

import org.jdom.Element;


public class Integer96_HEX extends Integer96 {
    public Integer96_HEX(LLRPBitList list) {
        super(list);
    }

    public Integer96_HEX(Element element) {
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
