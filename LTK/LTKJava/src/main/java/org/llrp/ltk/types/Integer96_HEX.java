package org.llrp.ltk.types;

import org.jdom.Content;
import org.jdom.Element;
import org.jdom.Namespace;
import org.jdom.Text;

import java.math.BigInteger;

/**
 * 96 bit Integer encoded in HEX Format
 * @author Basil Gasser - ETH Zurich
 *
 */
public class Integer96_HEX extends UnsignedInteger96 {
    public Integer96_HEX() {
        super();
    }

	public Integer96_HEX(LLRPBitList list) {
        super(list);
    }
	
	public Integer96_HEX(long value){
		super(value);
	}

    public Integer96_HEX(Element element) {
        super(element);
    }

    public Integer96_HEX(String string) {
        super(new BigInteger(string, 16));
    }
    
    public Integer96_HEX(String string, int radix){
    	super(new BigInteger(string, radix));
    }

    /**
    * length in number of bits used to represent this type.
    *
    */
    public static int length() {
        return LENGTH;
    }

    @Override
    public Content encodeXML(String name, Namespace ns) {
        Element element = new Element(name, ns);

      
        element.setContent(new Text(toString()));

        return element;
    }

    @Override
    public void decodeXML(Element element) {
        this.value = new BigInteger(element.getText(), 16);
    }
    
	
	public String toString() {
		  // need even number of digits
        String s = value.toString(16);

        if ((s.length() % 2) != 0) {
            s = "0" + s;
        }
        return s;
	}
	

}
