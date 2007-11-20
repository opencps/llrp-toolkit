package org.llrp.ltk.types;

import java.util.LinkedList;

import org.jdom.Content;
import org.jdom.Element;
import org.jdom.Namespace;
import org.jdom.Text;

public class BitArray_HEX extends BitArray {
	/**
	 * create a new BitArray_HEX. When encoded, BitArray_HEX also encodes its
	 * length.
	 * 
	 * @param bits
	 *            to be decoded
	 */
	public BitArray_HEX(Bit[] bits) {
		super(bits);
	}

	/**
	 * create a new BitArray_HEX. When encoded, BitArray_HEX also encodes its
	 * length.
	 * 
	 * @param list
	 *            to be decoded
	 */
	public BitArray_HEX(LLRPBitList list) {
		super(list);
	}

	/**
	 * create a new BitArray_HEX. When encoded, BitArray_HEX also encodes its
	 * length. Initially all bits set to 0
	 * 
	 * @param length
	 *            of array
	 */
	public BitArray_HEX(Integer length) {
		super(length);
	}

	/**
	 * empty bit array.
	 */
	public BitArray_HEX() {
		super();
	}

	/**
	 * @param element
	 *            to be decoded
	 */
	public BitArray_HEX(Element element) {
		super(element);
	}

	/**
	 * length in number of bits used to represent this type.
	 * 
	 * @return Integer representing number of bits used for this type
	 */
	public static Integer length() {
		return LENGTH;
	}

	@Override
	public Content encodeXML(String name, Namespace ns) {
		String s = "";
		String result = "";
		for (int i = 0; i < bits.length; i++) {
			s += bits[i].toString();
			// stop after 8 elements. we start at 0 so it is one off to 8
			if (i % 8 == 7 && i > 0) {
				Integer t = Integer.parseInt(s, 2);
				result += Integer.toHexString(t);
				s = "";
			}
		}
		// if s is not empty string we didn't have a number of bits divisible by
		// 8 so we need padding.
		if (!s.equals("") && s.length() < 8) {
			for (int i = s.length(); i < 8; i++) {
				s += "0";
			}
			Integer t = Integer.parseInt(s, 2);
			result += Integer.toHexString(t);
		}
		Element element = new Element(name, ns);
		element.setContent(new Text(result));

		return element;
	}

	/**
	 * @Override {@inheritDoc}
	 */
	public void decodeXML(Element element) {
		 String hexString = element.getText();
		 LinkedList<Bit> tempList = new LinkedList<Bit>();
		
		 int length = hexString.length();
		
		 for (int a = 0; a < length; a++) {
			 char hexDigit = hexString.charAt(a);
			 Integer hexInt = Integer.parseInt(hexDigit+"",16);
			 String bitString = Integer.toBinaryString(hexInt);
			 // add always four bits, if bitString is too short, at zeros
			 String padding = "";
			 for (int j=0;j<4-bitString.length();j++){
				 padding += "0";
			 }
			 bitString = padding+bitString;
			 for (int j=0;j<bitString.length();j++){
				 tempList.add(new Bit(bitString.charAt(j)+""));
			 }
		 }
		 Bit[] bs = new Bit[tempList.size()];
		 bits = tempList.toArray(bs);
	}
}
