package org.llrp.ltk.types;

import junit.framework.TestCase;

import org.jdom.Element;
import org.jdom.Namespace;
import org.junit.Before;
import org.junit.Test;

public class UnsignedLong_DATETIMETest extends TestCase {

	final String DATE1 = "2001-10-26T05:32:52.123678-04:00";
	final long DATE1L = 1004088772123678L;
	
	final String DATE2 = "2001-10-26T05:32:52.999999-04:00";
	final long DATE2L = 1004088772999999L;
	
	final String DATE3 = "3000-12-12T12:12:12.123Z";
	final String DATE3ALT = "3000-12-12T07:12:12.123000-05:00";
	final long DATE3L = 32533531932123000L;
	
	final String DATE4 = "2001-10-26T05:32:52.00000001-04:00";
	final String DATE4ALT = "2001-10-26T05:32:52.000000-04:00";
	final long DATE4L = 1004088772000000L;
	
	
	@Before
	public void setUp() throws Exception {
	}

	@Test
	public final void testEncodeXML() {
		
		UnsignedLong_DATETIME dateTime;
		String xmlDateTime;
		
		dateTime = new UnsignedLong_DATETIME(DATE1L);
		xmlDateTime = dateTime.encodeXML("foo", Namespace.getNamespace("foo")).getValue();
		assertEquals("UnsignedLong_DATETIME is not encoded correctly as xsd dateTime: " , DATE1, xmlDateTime);
		
		dateTime = new UnsignedLong_DATETIME(DATE2L);
		xmlDateTime = dateTime.encodeXML("foo", Namespace.getNamespace("foo")).getValue();
		assertEquals("UnsignedLong_DATETIME is not encoded correctly as xsd dateTime: " , DATE2, xmlDateTime);
		
		dateTime = new UnsignedLong_DATETIME(DATE3L);
		xmlDateTime = dateTime.encodeXML("foo", Namespace.getNamespace("foo")).getValue();
		assertEquals("UnsignedLong_DATETIME is not encoded correctly as xsd dateTime: " , DATE3ALT, xmlDateTime);
		
		dateTime = new UnsignedLong_DATETIME(DATE4L);
		xmlDateTime = dateTime.encodeXML("foo", Namespace.getNamespace("foo")).getValue();
		assertEquals("UnsignedLong_DATETIME is not encoded correctly as xsd dateTime: " , DATE4ALT, xmlDateTime);
		
		
	}

	@Test
	public final void testDecodeXML() {
		
		Element e = new Element("Microseconds", "http://www.llrp.org/ltk/schema/core/encoding/xml/1.0");
		UnsignedLong_DATETIME dateTime = new UnsignedLong_DATETIME();
		
		
		e.setText(DATE1);
		dateTime.decodeXML(e);	
		assertEquals("XML encoded UnsignedLong_DATETIME is not decoded correctly: " , DATE1L, dateTime.toLong());
	
		e.setText(DATE2);
		dateTime.decodeXML(e);	
		assertEquals("XML encoded UnsignedLong_DATETIME is not decoded correctly: " , DATE2L, dateTime.toLong());
	
		e.setText(DATE3);
		dateTime.decodeXML(e);	
		assertEquals("XML encoded UnsignedLong_DATETIME is not decoded correctly: " , DATE3L, dateTime.toLong());
		
		e.setText(DATE4);
		dateTime.decodeXML(e);	
		assertEquals("XML encoded UnsignedLong_DATETIME is not decoded correctly: " , DATE4L, dateTime.toLong());
	
	
	}

	@Test
	public final void testToString() {
		 // TODO
	}

}
