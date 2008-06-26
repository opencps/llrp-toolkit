package org.llrp.ltk.types;

import java.util.LinkedList;
import java.util.List;

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
     * @param binary String in Hexadecimal format
     */
    public BitArray_HEX(String hexString) {
    	Element element = new Element("foo","ns");
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

	@Override
	public Content encodeXML(String name, Namespace ns) {
		Element element = new Element(name, ns);
		
		element.setContent(new Text(result));

		return element;
	}

	/**
	 * @Override {@inheritDoc}
	 */
	public void decodeXML(Element element) {
		String hexString = element.getText();
		String countString = element.getAttributeValue("Count");
		Integer count;
		if (countString != null){
			count = Integer.parseInt(countString);
		} else {
			count = hexString.length()*4;
		}
		//count indicates that we only have to take a certain number of bits
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
		//take only the number of bits indicated by the count attribute
		List<Bit> shortenedList = tempList.subList(tempList.size()-count, tempList.size());
		Bit[] bs = new Bit[shortenedList.size()];
		bits = shortenedList.toArray(bs);
	}
	
	public String toString(){
		return encodeXML("foo", Namespace.getNamespace("foo")).getValue();
	}
}
