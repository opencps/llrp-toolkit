package org.llrp.ltk.types;

import java.math.BigDecimal;
import java.math.BigInteger;
import java.text.SimpleDateFormat;
import java.util.Calendar;
import java.util.Date;
import java.util.GregorianCalendar;

import javax.xml.datatype.DatatypeConfigurationException;
import javax.xml.datatype.DatatypeFactory;
import javax.xml.datatype.XMLGregorianCalendar;

import org.jdom.Content;
import org.jdom.Element;
import org.jdom.Namespace;
import org.jdom.Text;

public class UnsignedLong_DATETIME extends UnsignedLong {
	/**
	 * unsigned long encoded in datetime Format.
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
     * @param string
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
	 * @return int
	 */
	public static int length() {
		return LENGTH;
	}

	@Override
	public void decodeXML(Element element) {
		//java.util.Calendar cal = DatatypeConverter.parseDateTime(element.getText());
		//value = BigInteger.valueOf(cal.getTimeInMillis());
		DatatypeFactory df;
		try {
			df = DatatypeFactory.newInstance();
			XMLGregorianCalendar cal = df.newXMLGregorianCalendar(element.getText());
			
			// convert to BigInteger format with value in microseconds 
			// the last three digits ("microseconds") are "000" at this stage
			value = BigInteger.valueOf(cal.toGregorianCalendar().getTimeInMillis()*1000);
			
			// compute microseconds value by subtracting milliseconds from return value
			// of XMLGregorianCalendar.getFractionalSecond()
			BigDecimal millisec = cal.getFractionalSecond().setScale(3, BigDecimal.ROUND_DOWN);
			BigDecimal microsec = (cal.getFractionalSecond().setScale(6,BigDecimal.ROUND_DOWN)).subtract(millisec);;
			
			// add microseconds 
			value = value.add(microsec.movePointRight(6).toBigInteger());
			
		} catch (DatatypeConfigurationException e) {
			throw new IllegalArgumentException(e.getMessage());
		}
		
	}

	@Override
	public Content encodeXML(String name, Namespace ns) {
		
		GregorianCalendar cal = new GregorianCalendar();
		
		// initialize calendar after removing the last 
		// three digits that represent microseconds 
		final long milliseconds = value.divide(new BigInteger("1000")).longValue();
		cal.setTimeInMillis(milliseconds);
		
		StringBuffer sb = new StringBuffer();
		DatatypeFactory df;
		try {
			df = DatatypeFactory.newInstance();
			XMLGregorianCalendar xmlcal = df.newXMLGregorianCalendar(cal);
		    sb.append(xmlcal.toXMLFormat());
		    
		} catch (DatatypeConfigurationException e) {
			throw new IllegalArgumentException(e.getMessage());
		}
		
		int indexOfT = sb.indexOf("T");
		int l = value.toString().length();
		String microseconds = value.toString().substring(l-3, l);
		sb.insert(indexOfT+13, microseconds);
		
		Element element = new Element(name, ns);
		
		// to convert to xsd:dateTime it is necessary to insert a ":" 
		// in the timezone generated using the "Z" in the 
		// SimpleDateFormat expression above
	
		//sb.append(dateFormat.format(date));
		//sb.insert(sb.length()-2, ":");
		
		element.setContent(new Text(sb.toString()));

		return element;
	}
	
	
	public String toString(){
		return encodeXML("foo", Namespace.getNamespace("foo")).getValue();
	}
}
