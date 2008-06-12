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
    public UnsignedShortArray_HEX(int length) {
        super(length);
    }
    
    public UnsignedShortArray_HEX(short[] data){
    	this.shorts = new UnsignedShort[data.length];
    	for (int i = 0; i<data.length;i++){
    		shorts[i] = new UnsignedShort(data[i]);
    	}
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
        StringBuffer sb = new StringBuffer();
        for (UnsignedShort b : shorts) {
        	if (b != null) {
        		sb.append(" ");
            	sb.append(Integer.toHexString(b.value)); 
            } 
        }
        // remove initial " " in the string 
        sb.deleteCharAt(0);

        Element element = new Element(name, ns);
        element.setContent(new Text(sb.toString()));

        return element;
    }


    
    @Override
    public void decodeXML(Element element) {
        String text = element.getText();
        String[] strings = text.split(" ");
        shorts = new UnsignedShort[strings.length];

        for (int i = 0; i < strings.length; i++) {
            shorts[i] = new UnsignedShort(Integer.parseInt(strings[i],16));
        }
    }
    
    public String toString(){
    	return toString(16);
    }
}
