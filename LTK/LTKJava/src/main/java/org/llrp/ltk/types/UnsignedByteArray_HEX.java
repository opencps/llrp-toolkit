package org.llrp.ltk.types;

import java.math.BigInteger;
import java.util.LinkedList;

import org.jdom.Content;
import org.jdom.Element;
import org.jdom.Namespace;
import org.jdom.Text;

/**
 * Array of unsigned bytes encoded in HEX Format. Length encoded in first 16 bits of binary encoding
 * @author Basil Gasser - ETH Zurich
 *
 */
public class UnsignedByteArray_HEX extends UnsignedByteArray {
	/**
	 * Creates a new UnsignedByteArray object.
	 * 
	 * @param bytes
	 *            to create UnsignedByteArray
	 */
	public UnsignedByteArray_HEX(UnsignedByte[] bytes) {
		super(bytes);
	}

	/**
	 * Creates a new UnsignedByteArray object from jdom element - used for xml
	 * decoding
	 * 
	 * @param element
	 *            to be decoded
	 */
	public UnsignedByteArray_HEX(Element element) {
		super(element);
	}

	/**
	 * @param string
	 *            String in Hexadecimal format
	 */
	public UnsignedByteArray_HEX(String string) {
		Element element = new Element("foo", "ns");
		element.setText(string);
		decodeXML(element);
	}

	/**
	 * all values initially set to 0
	 * 
	 * @param length
	 *            of array
	 */
	public UnsignedByteArray_HEX(int length) {
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
	 * 
	 * @param list
	 *            to be decoded
	 */
	public UnsignedByteArray_HEX(LLRPBitList list) {
		super(list);
	}

	/**
	 * Creates a new UnsignedByteArray object.
	 * 
	 * @param bytes
	 *            to create UnsignedByteArray
	 */
	public UnsignedByteArray_HEX(byte[] bytes) {
		super(bytes);
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
	 * @param element to decode
	 */
	public void decodeXML(Element element) {
		String byteString = element.getText();
		if (byteString == null || byteString.equals("")){
			bytes = new UnsignedByte[0];
			return;
		}
		LinkedList<UnsignedByte> tempList = new LinkedList<UnsignedByte>();

		int length = byteString.length();

		for (int i = 0; i < length; i = i + 2) {
			String temp = byteString.substring(i, i+2);
			Integer ti = Integer.decode("0x" + temp);
			tempList.add(new UnsignedByte(ti));
		}
		UnsignedByte[] bs = new UnsignedByte[tempList.size()];
		bytes = tempList.toArray(bs);
	}

	public String toString() {
		String s = "";
		int i = 0;
		for (UnsignedByte b : bytes) {
			// U8v does not have spaces - this is a special case
			if (b != null) {
				String t = Integer.toHexString(b.value);
				if (t.length()==1){
					t = "0"+t;
				}
				s += t;
				
				i++;

			}
		}

		s = s.replaceFirst(" ", "");
		return s;
	}
}
