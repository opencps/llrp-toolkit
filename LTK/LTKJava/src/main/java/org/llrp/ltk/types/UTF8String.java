/*
 * Copyright 2007 ETH Zurich
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 *
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and limitations under the License.
 */
package org.llrp.ltk.types;

import java.math.BigInteger;

import org.jdom.Content;
import org.jdom.Element;
import org.jdom.IllegalDataException;
import org.jdom.Namespace;
import org.jdom.Text;
import org.jdom.Verifier;


/**
 * Strig with UT8 Encoding!
 *
 * @author Basil Gasser - ETH Zurich
 */
public class UTF8String extends LLRPType {
    //8 because we encode it as a list of bytes
    protected static Integer LENGTH = 8;
    protected String string;

    protected UTF8String() {
        string = "";
    }

    /**
         * Creates a new UTF8String object.
         *
         * @param subList to be decoded
         */
    public UTF8String(LLRPBitList subList) {
        decodeBinary(subList);
    }

    /**
     * Creates a new UTF8String object from jdom element - used for xml decoding
     *
     * @param element to be decoded
     */
    public UTF8String(Element element) {
        decodeXML(element);
    }

    /**
         * Creates a new UTF8String object.
         *
         * @param string to be represented
         */
    public UTF8String(String string) {
        this.string = string;
    }

    /**
     * decode bits from BitList.
     *
     * @param list to be decoded
     */
    public void decodeBinary(LLRPBitList list) {
        LLRPBitList subList = list.subList(UnsignedShort.length(),
                list.length() - UnsignedShort.length());
        byte[] bigBytes = subList.toByteArray();
        byte[] bytes = new byte[bigBytes.length];

        for (int i = 0; i < bytes.length; i++) {
            bytes[i] = bigBytes[i];
        }

        string = new String(bytes);
    }

    /**
     * return number of bits used to represent this type
     *
     * @return Integer
     */
    public static int length() {
        // 8 because we encod it as bytes and a byte has 8 bits
        return 8;
    }

    /**
     * encode to binary representation
     *
     * @return LLRPBitList
     */
    public LLRPBitList encodeBinary() {
        LLRPBitList result = new LLRPBitList();
        result.append(new UnsignedShort(string.toCharArray().length).encodeBinary());

        byte[] bytes = string.getBytes();
        byte[] bigBytes = new byte[bytes.length];

        for (int i = 0; i < bytes.length; i++) {
            bigBytes[i] = bytes[i];
        }

        result.append(new LLRPBitList(bigBytes));

        return result;
    }

    /**
     * compare two UTF8Strings
     *
     * @param other to compare
     *
     * @return boolean
     */
    public boolean equals(LLRPType other) {
        UTF8String u = (UTF8String) other;

        return string.equals(u.string);
    }

    /**
     * number of bytes used to represent this type
     *
     * @return Integer
     */
    public Integer getByteLength() {
        return (string.length() / 8);
    }

    @Override
    public void decodeXML(Element element) {
        string = element.getText();
    }

    /**
     * create xml representation of this parameter. 
     * 
     * This method will check the supplied string to see if it only contains 
     * characters allowed by the XML 1.0 specification. The C0 controls 
     * (e.g. null, vertical tab, formfeed, etc.) are removed 
     * except for carriage return, linefeed and the horizontal tab. 
     * Surrogates will throw an IllegalDataException. 
     * 
     * Note that characters like " and 
     * < are allowed in attribute values and element content. 
     * They will simply be escaped when the value is serialized. 
     *
     * @param name returned content should have
     * @param ns Namespace of elements
     */
    public Content encodeXML(String name, Namespace ns) {
        Element element = new Element(name, ns);
        
        // org.jdom.Text includes a method call to org.jdom.Verifier.checkCharacterData
        // if this check fails, the IllegalDataException is thrown. 
        // Our code then eliminates the illegal XML Character.

        try {
        	element.setContent(new Text(string));
        }
        catch (IllegalDataException e) {
        	
        	StringBuffer buffer = new StringBuffer();
        	for (int i = 0, len = string.length(); i<len; i++) {

                int ch = string.charAt(i);
                
                if (Verifier.isXMLCharacter(ch)) {
                	buffer.append(string.charAt(i));
                }
                else {
                	// non XML Characters are eliminated
                }
        	}
        	element.setContent(new Text(buffer.toString()));
        }
        

        return element;
    }

    public String toString() {
        return string;
    }
    
    public String toString(int radix){
    	return toString();
    }

    public int hashCode() {
        return string.hashCode();
    }
    
    public boolean inRange(String valueString){
    	// everyting allowed
		return true;
	}
}