package org.llrp.ltk.types;


import org.jdom.Content;
import org.jdom.Element;
import org.jdom.Namespace;

import junit.framework.TestCase;

public class TwoBitFieldTest extends TestCase {

	Bit[] bits00;
	Bit[] bits01;
	Bit[] bits10;
	Bit[] bits11;
	LLRPBitList result;
	

	public void setUp() throws Exception {
		
		bits01 = new Bit[2];
		this.bits01[0] = new Bit(0);
		this.bits01[1] = new Bit(1);
		
		
		
		
	}
	
	public void testDecodeXML() {

		/* the following test code does not work yet */
		
		TwoBitField twoBitA = new TwoBitField(bits01);
		Element element = new Element("foo", "ns");
		element.setText("01");
		TwoBitField twoBitB = new TwoBitField();
		twoBitB.decodeXML(element);
		assertEquals("Two bit field is not decoded correctly", twoBitB.toString(), twoBitA.toString());

		}
	

	public void testEncodeXML(String name, Namespace ns) {
		
	
		
		
		
	}

	public void testEncodeBinary() {
		
	

		
	}
	
	public void testDecodeBinary() {
	
		
		
		
	}
}
