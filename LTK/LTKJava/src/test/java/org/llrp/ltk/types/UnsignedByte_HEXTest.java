package org.llrp.ltk.types;

import org.jdom.Element;
import org.jdom.Namespace;

import junit.framework.TestCase;

public class UnsignedByte_HEXTest extends TestCase {

	final String UBYTE1 = "4";
	final String UBYTE2= "f";
	
	protected void setUp() throws Exception {
		super.setUp();
	}

	public void testEncodeXML() {
		
		UnsignedByte_HEX ubyte;
		String xmlubyte;
		
		ubyte = new UnsignedByte_HEX(UBYTE1);
		
		xmlubyte = ubyte.encodeXML("foo", Namespace.getNamespace("foo")).getValue();
		assertEquals("UnsignedByte_HEX is not encoded correctly as xsd hexByte: " , UBYTE1, xmlubyte);
		
		ubyte = new UnsignedByte_HEX(new LLRPBitList("00000100"));
		
		xmlubyte = ubyte.encodeXML("foo", Namespace.getNamespace("foo")).getValue();
		assertEquals("UnsignedByte_HEX is not encoded correctly as xsd hexByte: " , UBYTE1, xmlubyte);
		
		ubyte = new UnsignedByte_HEX(UBYTE2);
		
		xmlubyte = ubyte.encodeXML("foo", Namespace.getNamespace("foo")).getValue();
		assertEquals("UnsignedByte_HEX is not encoded correctly as xsd hexByte: " , UBYTE2, xmlubyte);
		
		ubyte = new UnsignedByte_HEX(new LLRPBitList("00001111"));
		
		xmlubyte = ubyte.encodeXML("foo", Namespace.getNamespace("foo")).getValue();
		assertEquals("UnsignedByte_HEX is not encoded correctly as xsd hexByte: " , UBYTE2, xmlubyte);
		
		
	}

	public void testDecodeXML() {
		
		UnsignedByte_HEX ubyte;
		String xmlubyte;
		
		Element e = new Element("Foo", "http://www.llrp.org/ltk/schema/core/encoding/xml/1.0");
		ubyte = new UnsignedByte_HEX();
		
		e.setText(UBYTE1);
		ubyte.decodeXML(e);	
		assertEquals("XML encoded UnsignedByte_HEX is not decoded correctly: " , UBYTE1, ubyte.toString());
		
		e.setText(UBYTE2);
		ubyte.decodeXML(e);	
		assertEquals("XML encoded UnsignedByte_HEX is not decoded correctly: " , UBYTE2, ubyte.toString());
		
		
	}

}
