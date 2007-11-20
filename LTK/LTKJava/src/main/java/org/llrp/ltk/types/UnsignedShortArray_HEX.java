package org.llrp.ltk.types;

import java.util.LinkedList;

import org.jdom.Content;
import org.jdom.Element;
import org.jdom.Namespace;
import org.jdom.Text;


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
    
    @Override
    public Content encodeXML(String name, Namespace ns) {
        String s = "";
        int i = 0;
        for (UnsignedShort b : shorts) {
        	if ((i)%4==0){
        		// add space every 4th element
        		s+=" ";
        	}
        	if (b != null) {
                s += Integer.toHexString(b.value);
            	i++;
                
            } 
        }

        s = s.replaceFirst(" ", "");

        Element element = new Element(name, ns);
        element.setContent(new Text(s));

        return element;
    }


    
    @Override
    public void decodeXML(Element element) {
        String text = element.getText();
        String[] strings = text.split(" ");
        shorts = new UnsignedShort[strings.length];

        for (int i = 0; i < strings.length; i++) {
            shorts[i] = new UnsignedShort(Integer.decode("0x"+strings[i]));
        }
    }
    
}
