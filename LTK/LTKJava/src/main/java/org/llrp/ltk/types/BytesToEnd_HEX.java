package org.llrp.ltk.types;

import org.jdom.Element;


/**
 * class representing BytesToEnd in Hex format.
 *
 */
public class BytesToEnd_HEX extends BytesToEnd {
    /**
     * class representing BytesToEnd in Hex format.
     */
    public BytesToEnd_HEX() {
        super();
    }

    /**
     * bits interpreted to be in hexadecimal format.
     * @param list
     */
    public BytesToEnd_HEX(LLRPBitList list) {
        super(list);
    }

    /**
     * {@inheritDoc}
     */
    public BytesToEnd_HEX(Element element) {
        super(element);
    }
}
