package org.llrp.ltk.types;

import java.util.LinkedList;

import org.jdom.Content;
import org.jdom.Element;
import org.jdom.Namespace;
import org.jdom.Text;
/**
 * Array of unsigned shorts encoded in HEX Format. Length encoded in first 16 bits of binary encoding
 * @author Basil Gasser - ETH Zurich
 *
 */
public class UnsignedShortArray_HEX extends UnsignedShortArray {
	public UnsignedShortArray_HEX(Element element) {
		super(element);
	}

	/**
	 * Creates a new UnsignedShortArray object.
	 * 
	 * @param length
	 *            of array
	 */
	public UnsignedShortArray_HEX(int length) {
		super(length);
	}

	public UnsignedShortArray_HEX(short[] data) {
		this.shorts = new UnsignedShort[data.length];
		for (int i = 0; i < data.length; i++) {
			shorts[i] = new UnsignedShort(data[i]);
		}
	}

	/**
	 * @param string
	 */
	public UnsignedShortArray_HEX(String string) {
		Element element = new Element("foo", "ns");
		element.setText(string);
		decodeXML(element);
	}

	/**
	 * first 16 bits of LLRPBitlist must indicate number of entries that follow
	 * 
	 * @param bits
	 *            to be decoded
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

		Element element = new Element(name, ns);
		element.setContent(new Text(toString()));

		return element;
	}

	@Override
	public void decodeXML(Element element) {
		String text = element.getText();
		if (!text.equals("")) {
			String[] strings = text.split(" ");
			shorts = new UnsignedShort[strings.length];

			for (int i = 0; i < strings.length; i++) {
				if (!strings[i].equals("")) {
					shorts[i] = new UnsignedShort(Integer.parseInt(strings[i],
							16));
				}
			}
		} else {
			shorts = new UnsignedShort[0];
		}
	}

	public String toString() {
		StringBuffer sb = new StringBuffer();
		for (UnsignedShort b : shorts) {
			if (b != null) {
				sb.append(" ");
				String s = b.toString(16);
				int check = s.length()%4;
				StringBuffer padding = new StringBuffer();
				if (check!=0){
					for (int i=0;i<4-check;i++){
						padding.append("0");
					}
				}
				sb.append(padding.append(s));
			}
		}
		// remove initial " " in the string
		if (sb.length() > 0 && sb.toString().startsWith(" ")) {
			sb.deleteCharAt(0);
		}
		
		return sb.toString();
	}
}
