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
		bits10 = new Bit[2];
		this.bits10[0] = new Bit(1);
		this.bits10[1] = new Bit(0);
		bits00 = new Bit[2];
		this.bits00[0] = new Bit(0);
		this.bits00[1] = new Bit(0);
		bits11 = new Bit[2];
		this.bits11[0] = new Bit(1);
		this.bits11[1] = new Bit(1);
		
	}
	
	public void testDecodeXML() {		
		TwoBitField twoBitA = new TwoBitField(bits00);
		Element element = new Element("foo", "ns");
		element.setText("0");
		TwoBitField twoBitB = new TwoBitField();
		twoBitB.decodeXML(element);
		assertEquals("Two bit field is not decoded correctly", twoBitB.toString(), twoBitA.toString());
		
		twoBitA = new TwoBitField(bits01);
		element = new Element("foo", "ns");
		element.setText("1");
		twoBitB = new TwoBitField();
		twoBitB.decodeXML(element);
		assertEquals("Two bit field is not decoded correctly", twoBitB.toString(), twoBitA.toString());
		
		twoBitA = new TwoBitField(bits10);
		element = new Element("foo", "ns");
		element.setText("2");
		twoBitB = new TwoBitField();
		twoBitB.decodeXML(element);
		assertEquals("Two bit field is not decoded correctly", twoBitB.toString(), twoBitA.toString());
		
		twoBitA = new TwoBitField(bits11);
		element = new Element("foo", "ns");
		element.setText("3");
		twoBitB = new TwoBitField();
		twoBitB.decodeXML(element);
		assertEquals("Two bit field is not decoded correctly", twoBitB.toString(), twoBitA.toString());
	}
	

	public void testEncodeXML() {
		String name = "name";
		Namespace ns = Namespace.getNamespace("www.llrp.org");
		TwoBitField twoBitA = new TwoBitField(bits01);
		Content e = twoBitA.encodeXML(name, ns);
		assertEquals("Two bit field is not encoded correctly", e.getValue(), "1");
		
		twoBitA = new TwoBitField(bits10);
		e = twoBitA.encodeXML(name, ns);
		assertEquals("Two bit field is not encoded correctly", e.getValue(), "2");
		
		twoBitA = new TwoBitField(bits00);
		e = twoBitA.encodeXML(name, ns);
		assertEquals("Two bit field is not encoded correctly", e.getValue(), "0");
		
		twoBitA = new TwoBitField(bits11);
		e = twoBitA.encodeXML(name, ns);
		assertEquals("Two bit field is not encoded correctly", e.getValue(), "3");	
		
	}

	public void testEncodeBinary() {
		TwoBitField twoBitA = new TwoBitField(bits01);
		LLRPBitList e = twoBitA.encodeBinary();
		assertEquals("Two bit field is not encoded correctly", e.toString(), "01");
		twoBitA = new TwoBitField(bits10);
		e = twoBitA.encodeBinary();
		assertEquals("Two bit field is not encoded correctly", e.toString(), "10");
		twoBitA = new TwoBitField(bits00);
		e = twoBitA.encodeBinary();
		assertEquals("Two bit field is not encoded correctly", e.toString(), "00");
		twoBitA = new TwoBitField(bits11);
		e = twoBitA.encodeBinary();
		assertEquals("Two bit field is not encoded correctly", e.toString(), "11");				
	}
	
	public void testDecodeBinary() {
		LLRPBitList e = new LLRPBitList("00");
		TwoBitField twoBitField = new TwoBitField(e);
		assertEquals("Two Bit field not decoded from binary correctly",twoBitField.intValue(),0);
		e = new LLRPBitList("01");
		twoBitField = new TwoBitField(e);
		assertEquals("Two Bit field not decoded from binary correctly",twoBitField.intValue(),1);
		e = new LLRPBitList("10");
		twoBitField = new TwoBitField(e);
		assertEquals("Two Bit field not decoded from binary correctly",twoBitField.intValue(),2);
		e = new LLRPBitList("11");
		twoBitField = new TwoBitField(e);
		assertEquals("Two Bit field not decoded from binary correctly",twoBitField.intValue(),3);
		
		
		
	}
}
