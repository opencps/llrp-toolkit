package org.llrp.ltk.types;

import java.math.BigInteger;

import org.jdom.Content;
import org.jdom.Element;
import org.jdom.Namespace;
import org.jdom.Text;


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
    
    @Override
    public Content encodeXML(String name, Namespace ns) {
        String s = "";
        int i = 0;
        byte[] bytes = value.toByteArray();
        for (byte b : bytes) {
        	if (i%4==0){
        		// add space every 4th element
        		s+=" ";
        	}
            s += Integer.toHexString(b);
        	i++;
         }
        Element element = new Element(name, ns);
        element.setContent(new Text(s));

        return element;
    }
    
    
    @Override
    public void decodeXML(Element element) {
    
        this.value = new BigInteger(element.getText(),16);
    }
}
