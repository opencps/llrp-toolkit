package org.llrp.ltk.types;

import java.util.LinkedList;
import java.util.List;

import org.jdom.Content;
import org.jdom.Element;
import org.jdom.Namespace;
import org.jdom.Text;

public class BitArray_HEX extends BitArray {
	/**
	 * Array of Bits encoded in HEX Format.
	 * XML element has count attribute if number of bits is not evenly divisible my 8
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
	public BitArray_HEX(int length) {
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
	 * @param hexString
	 *            String in Hexadecimal format
	 */
	public BitArray_HEX(String hexString) {
		Element element = new Element("foo", "ns");
		element.setText(hexString);
		decodeXML(element);
	}

	/**
	 * length in number of bits used to represent this type.
	 * 
	 * @return Integer representing number of bits used for this type
	 */
	public static int length() {
		return LENGTH;
	}

    /**
     * encode to XML
     *
     * @param name of element
     * @param ns Namespace
     */
	public Content encodeXML(String name, Namespace ns) {
		Element element = new Element(name, ns);
		// xs:hexBinary only lets you define a number of bits evenly divisible
		// by 8
		// if you don't have a number of bits evenly divisible by 8, then put a
		// Count attribute to indicate the actual total number of bits you are
		// attempting to represent.
		String bitString = "";
		for (int i = 0; i < bits.length; i++) {
			bitString += bits[i];
		}
		int mod = bitString.length() % 8;
		if (mod != 0) {
			element.setAttribute("Count", new Integer(bitString.length())
					.toString());
			// add zeros to front
			for (int a = 0; a < 8 - mod; a++) {
				bitString = "0" + bitString;
			}
		}
		String result = "";
		for (int i = 0; i < bitString.length(); i = i + 4) {
			result += Integer.toHexString(Integer.parseInt(bitString.substring(
					i, i + 4), 2));
		}
		element.setContent(new Text(result));

		return element;
	}

	/**
	 * decode JDOM Element
	 * @param element to decode
	 */
	public void decodeXML(Element element) {
		String hexString = element.getText();
		String countString = element.getAttributeValue("Count");
		Integer count;
		if (countString != null) {
			count = Integer.parseInt(countString);
		} else {
			count = hexString.length() * 4;
		}
		// count indicates that we only have to take a certain number of bits
		LinkedList<Bit> tempList = new LinkedList<Bit>();

		int length = hexString.length();

		for (int a = 0; a < length; a++) {
			char hexDigit = hexString.charAt(a);
			Integer hexInt = Integer.parseInt(hexDigit + "", 16);
			String bitString = Integer.toBinaryString(hexInt);
			// add always four bits, if bitString is too short, at zeros
			String padding = "";
			for (int j = 0; j < 4 - bitString.length(); j++) {
				padding += "0";
			}
			bitString = padding + bitString;
			for (int j = 0; j < bitString.length(); j++) {
				tempList.add(new Bit(bitString.charAt(j) + ""));
			}
		}
		// take only the number of bits indicated by the count attribute
		List<Bit> shortenedList = tempList.subList(tempList.size() - count,
				tempList.size());
		Bit[] bs = new Bit[shortenedList.size()];
		bits = shortenedList.toArray(bs);
	}

	public String toString() {
		return encodeXML("foo", Namespace.getNamespace("foo")).getValue();
	}
}
