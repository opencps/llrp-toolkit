package org.llrp.ltk.types;

import org.jdom.Content;
import org.jdom.Element;
import org.jdom.Namespace;
import org.jdom.Text;

import java.math.BigInteger;

import java.text.SimpleDateFormat;

import java.util.Date;


public class UnsignedLong_DATETIME extends UnsignedLong {
    /**
     * {@inheritDoc}
     * @param aLong
     */
    public UnsignedLong_DATETIME(Long aLong) {
        super(aLong);
    }

    public UnsignedLong_DATETIME() {
        this.value = BigInteger.ZERO;
    }

    /**
     * Creates a new UnsignedLong_DATETIME object from jdom element - used for xml decoding
     *
     * @param element to be decoded
     */
    public UnsignedLong_DATETIME(Element element) {
        super(element);
    }

    /**
         * Creates a new UnsignedLong_DATETIME object.
         *
         * @param bitList to be decoded
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
        value = new BigInteger(element.getText());
    }

    @Override
    public Content encodeXML(String name, Namespace ns) {
        SimpleDateFormat dateFormat = new SimpleDateFormat(
                "yyyy-MM-dd'T'hh:mm:ss.S'Z'");
        Date date = new Date(value.longValue());
        Element element = new Element(name, ns);
        element.setContent(new Text(dateFormat.format(date)));

        return element;
    }
}
