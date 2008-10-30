package org.llrp.ltk.types;

import junit.framework.TestCase;

import org.jdom.Element;
import org.jdom.Namespace;
import org.junit.Before;
import org.junit.Test;

public class SignedShortArrayTest extends TestCase {

	private final String encodedXML = "1 2 3";
	// 0000000000000011 length = 3
	// 0000000000000001 ssa[0] = 1
	// 0000000000000010 ssa[1] = 2
	// 0000000000000011 ssa[2] = 3
	private final String encodedBinary = "0000000000000011000000000000000100000000000000100000000000000011";
	
	@Before
	public void setUp() throws Exception {
	}

	@Test
	public final void testEncodeXML() {
		
		SignedShortArray ssa = new SignedShortArray();
		ssa.add(new SignedShort(1));
		ssa.add(new SignedShort(2));
		ssa.add(new SignedShort(3));
		assertEquals(ssa.encodeXML("test", null).getValue(), encodedXML);
		
	}

	@Test
	public final void testEncodeBinary() {
		SignedShortArray ssa = new SignedShortArray();
		ssa.add(new SignedShort(1));
		ssa.add(new SignedShort(2));
		ssa.add(new SignedShort(3));
		assertEquals(ssa.encodeBinary().toString(), encodedBinary);
	}
	
	@Test
	public final void testCreationSignedShort() {
		SignedShort[] ss = new SignedShort[3];
		ss[0] = new SignedShort(1);
		ss[1] = new SignedShort(2);
		ss[2] = new SignedShort(3);

		SignedShortArray ssb = new SignedShortArray(ss);
		SignedShortArray ssa = new SignedShortArray();
		ssa.add(new SignedShort(1));
		ssa.add(new SignedShort(2));
		ssa.add(new SignedShort(3));
		assertTrue(ssa.equals(ssb));
	}
	
	@Test
	public final void testDecodeBinary() {
		SignedShortArray ssb = new SignedShortArray(new LLRPBitList(encodedBinary));
		SignedShortArray ssa = new SignedShortArray();
		ssa.add(new SignedShort(1));
		ssa.add(new SignedShort(2));
		ssa.add(new SignedShort(3));
		assertTrue(ssa.equals(ssb));
	}
	
	@Test
	public final void testDecodeXML() {
		Element xml = new Element("test",Namespace.getNamespace("foo"));
		xml.setText(encodedXML);
		SignedShortArray ssb = new SignedShortArray(xml);
		SignedShortArray ssa = new SignedShortArray();
		ssa.add(new SignedShort(1));
		ssa.add(new SignedShort(2));
		ssa.add(new SignedShort(3));
		assertTrue(ssa.equals(ssb));
	}
	
	@Test
	public final void testCreationShort() {
		short[] ss = new short[3];
		ss[0] = 1;
		ss[1] = 2;
		ss[2] = 3;

		SignedShortArray ssb = new SignedShortArray(ss);
		SignedShortArray ssa = new SignedShortArray();
		ssa.add(new SignedShort(1));
		ssa.add(new SignedShort(2));
		ssa.add(new SignedShort(3));
		assertTrue(ssa.equals(ssb));
	}

	@Test
	public final void testToString() {
		 // TODO
	}

}
