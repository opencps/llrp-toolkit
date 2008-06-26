package org.llrp.ltk.types;

import java.math.BigInteger;
import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.Date;

import javax.xml.bind.DatatypeConverter;

import org.jdom.Content;
import org.jdom.Element;
import org.jdom.Namespace;
import org.jdom.Text;

public class UnsignedLong_DATETIME extends UnsignedLong {
	/**
	 * {@inheritDoc}
	 * 
	 * @param aLong
	 */
	public UnsignedLong_DATETIME(long aLong) {
		super(aLong);
	}

	public UnsignedLong_DATETIME() {
		this.value = BigInteger.ZERO;
	}
	

    /**
     * @param String
     */
    public UnsignedLong_DATETIME(String string) {
    	Element element = new Element("foo","ns");
    	element.setText(string);
        decodeXML(element);
    }
    
    

	/**
	 * Creates a new UnsignedLong_DATETIME object from jdom element - used for
	 * xml decoding
	 * 
	 * @param element
	 *            to be decoded
	 */
	public UnsignedLong_DATETIME(Element element) {
		super(element);
	}

	/**
	 * Creates a new UnsignedLong_DATETIME object.
	 * 
	 * @param bitList
	 *            to be decoded
	 */
	public UnsignedLong_DATETIME(LLRPBitList bitList) {
		super(bitList);
	}

	/**
	 * length in number of bits used to represent this type.
	 * 
	 * @return
	 */
	public static int length() {
		return LENGTH;
	}

	@Override
	public void decodeXML(Element element) {
		java.util.Calendar cal = DatatypeConverter.parseDateTime(element.getText());
		value = BigInteger.valueOf(cal.getTimeInMillis());
	}

	@Override
	public Content encodeXML(String name, Namespace ns) {
		SimpleDateFormat dateFormat = new SimpleDateFormat(
				"yyyy-MM-dd'T'hh:mm:ss.SZ");
		Date date = new Date(value.longValue());
		Element element = new Element(name, ns);
		
		// to convert to xsd:dateTime it is necessary to insert a ":" 
		// in the timezone generated using the "Z" in the 
		// SimpleDateFormat expression above
		StringBuffer sb = new StringBuffer();
		sb.append(dateFormat.format(date));
		sb.insert(sb.length()-2, ":");
		
		element.setContent(new Text(sb.toString()));

		return element;
	}
}
