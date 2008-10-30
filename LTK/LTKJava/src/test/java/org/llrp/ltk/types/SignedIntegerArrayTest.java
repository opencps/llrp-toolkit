package org.llrp.ltk.types;

import junit.framework.TestCase;

import org.jdom.Element;
import org.jdom.Namespace;
import org.junit.Before;
import org.junit.Test;

public class SignedIntegerArrayTest extends TestCase {

	private final String encodedXML = "1 2 3";
	// 0000000000000011 length = 3
	// 0000000000000001 ssa[0] = 1
	// 0000000000000010 ssa[1] = 2
	// 0000000000000011 ssa[2] = 3
	private final String encodedBinary = "0000000000000011000000000000000000000000000000010000000000000000000000000000001000000000000000000000000000000011";
	
	@Before
	public void setUp() throws Exception {
	}

	@Test
	public final void testEncodeXML() {
		
		SignedIntegerArray sia = new SignedIntegerArray();
		sia.add(new SignedInteger(1));
		sia.add(new SignedInteger(2));
		sia.add(new SignedInteger(3));
		assertEquals(sia.encodeXML("test", null).getValue(), encodedXML);
		
	}

	@Test
	public final void testEncodeBinary() {
		SignedIntegerArray sia = new SignedIntegerArray();
		sia.add(new SignedInteger(1));
		sia.add(new SignedInteger(2));
		sia.add(new SignedInteger(3));
		assertEquals(sia.encodeBinary().toString(), encodedBinary);
	}
	
	@Test
	public final void testCreationSignedInteger() {
		SignedInteger[] ss = new SignedInteger[3];
		ss[0] = new SignedInteger(1);
		ss[1] = new SignedInteger(2);
		ss[2] = new SignedInteger(3);

		SignedIntegerArray sib = new SignedIntegerArray(ss);
		SignedIntegerArray sia = new SignedIntegerArray();
		sia.add(new SignedInteger(1));
		sia.add(new SignedInteger(2));
		sia.add(new SignedInteger(3));
		assertTrue(sia.equals(sib));
	}
	
	@Test
	public final void testDecodeBinary() {
		SignedIntegerArray sib = new SignedIntegerArray(new LLRPBitList(encodedBinary));
		SignedIntegerArray sia = new SignedIntegerArray();
		sia.add(new SignedInteger(1));
		sia.add(new SignedInteger(2));
		sia.add(new SignedInteger(3));
		assertTrue(sia.equals(sib));
	}
	
	@Test
	public final void testDecodeXML() {
		Element xml = new Element("test",Namespace.getNamespace("foo"));
		xml.setText(encodedXML);
		SignedIntegerArray sib = new SignedIntegerArray(xml);
		SignedIntegerArray sia = new SignedIntegerArray();
		sia.add(new SignedInteger(1));
		sia.add(new SignedInteger(2));
		sia.add(new SignedInteger(3));
		assertTrue(sia.equals(sib));
	}
	
	@Test
	public final void testCreationInts() {
		int[] ss = new int[3];
		ss[0] = 1;
		ss[1] = 2;
		ss[2] = 3;

		SignedIntegerArray sib = new SignedIntegerArray(ss);
		SignedIntegerArray sia = new SignedIntegerArray();
		sia.add(new SignedInteger(1));
		sia.add(new SignedInteger(2));
		sia.add(new SignedInteger(3));
		assertTrue(sia.equals(sib));
	}

	@Test
	public final void testToString() {
		 // TODO
	}

}
