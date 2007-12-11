package org.llrp.ltk.types;

import org.jdom.Content;
import org.jdom.Element;
import org.jdom.Namespace;
import org.jdom.Text;

import java.math.BigInteger;


public class Integer96_HEX extends Integer96 {
    public Integer96_HEX(LLRPBitList list) {
        super(list);
    }

    public Integer96_HEX(Element element) {
        super(element);
    }

    public Integer96_HEX(String string) {
        super(new BigInteger(string, 16));
    }

    /**
    * length in number of bits used to represent this type.
    *
    * @return
    */
    public static Integer length() {
        return LENGTH;
    }

    @Override
    public Content encodeXML(String name, Namespace ns) {
        Element element = new Element(name, ns);

        // need even number of digits
        String s = value.toString(16);

        if ((s.length() % 2) != 0) {
            s = "0" + s;
        }

        element.setContent(new Text(s));

        return element;
    }

    @Override
    public void decodeXML(Element element) {
        this.value = new BigInteger(element.getText(), 16);
    }
}
