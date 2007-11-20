package org.llrp.ltk.types;

import java.math.BigInteger;
import java.util.LinkedList;

import org.jdom.Content;
import org.jdom.Element;
import org.jdom.Namespace;
import org.jdom.Text;


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
    
    @Override
    public Content encodeXML(String name, Namespace ns) {
        String s = "";
        int i = 0;
        for (UnsignedByte b : bytes) {
        	if (i%4==0){
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
    
    
    
    /**
     * @Override
     * {@inheritDoc}
     */
    public void decodeXML(Element element) {
        String byteString = element.getText();
        LinkedList<UnsignedByte> tempList = new LinkedList<UnsignedByte>();

        int length = byteString.length();

        for (Integer i = 0; i < length; i = i + UnsignedByte.length()) {
        	String temp = byteString.substring(i,UnsignedByte.length());
        	Integer ti = Integer.decode("0x"+temp);
        	tempList.add(new UnsignedByte(ti));
        }
        UnsignedByte[] bs = new UnsignedByte[length];
        bytes = tempList.toArray(bs);
    }
}
