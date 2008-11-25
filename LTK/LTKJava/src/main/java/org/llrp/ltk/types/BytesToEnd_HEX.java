package org.llrp.ltk.types;

import java.util.LinkedList;

import org.jdom.Content;
import org.jdom.Element;
import org.jdom.Namespace;
import org.jdom.Text;
import org.llrp.ltk.generated.LLRPConstants;

/**
 * BytesToEnd in HEX format.
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
	 * 
	 * @param list
	 */
	public BytesToEnd_HEX(LLRPBitList list) {
		super(list);
	}

	/**
	 * BytesToEnd created from XML element
	 * @param element 
	 */
	public BytesToEnd_HEX(Element element) {
		super(element);
	}

	/**
	 * @param hexString
	 *            String in Hexadecimal format
	 */
	public BytesToEnd_HEX(String hexString) {
		Element element = new Element("foo", "ns");
		element.setText(hexString);
		decodeXML(element);
	}

    /**
     * encode to XML
     *
     * @param name of element
     * @param ns Namespace
     */
	public Content encodeXML(String name, Namespace ns) {
		Element element = new Element(name, ns);
		element.setContent(new Text(toString()));

		return element;
	}

	/**
	 * decode JDOM Element
	 * @param element to decode
	 */
	public void decodeXML(Element element) {
		String hexString = element.getText().replaceAll(" ", "");
		bytes = new LinkedList<SignedByte>();

		for (int a = 0; a < hexString.length(); a = a + 2) {
			String twoHexDigits = "";
			if (a + 2 <= hexString.length()) {
				twoHexDigits = hexString.substring(a, a + 2);
			} else {
				twoHexDigits = "0" + hexString.substring(a, a + 1);
			}
			Integer hexInt = Integer.parseInt(twoHexDigits + "", 16);
			bytes.add(new SignedByte(hexInt));
		}
	}

	/**
	 * Bytes in HEX Format
	 * @return String 
	 */
	public String toString() {
		String s = "";
		int i = 0;
		for (SignedByte b : bytes) {

			if (b != null) {
				// a byte must always consitst of two hexadecimal digits. If
				// integer is smaller than 16 and therefore only needs one hex
				// digit,add 0
				String t = Integer.toHexString(b.value);
				if (t.length() < 2) {
					t = "0" + t;
				}
				s += t;
				i++;

			}
		}
		return s;
	}

}
