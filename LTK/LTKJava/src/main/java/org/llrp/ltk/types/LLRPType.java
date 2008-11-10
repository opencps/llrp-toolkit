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

import org.jdom.Content;
import org.jdom.Document;
import org.jdom.Element;
import org.jdom.Namespace;
import org.jdom.output.Format;
import org.jdom.output.XMLOutputter;


/**
 * super type for all types in LLRP.
 *
 * @author Basil Gasser - ETH Zurich
 */
public abstract class LLRPType {
    /**
     * decode
     *
     * @param list to be decoded
     */
    public abstract void decodeBinary(LLRPBitList list);

    /**
     * encode this value
     *
     * @return LLRPBitList
     */
    public abstract LLRPBitList encodeBinary();

    /**
     * create xml representation of this parameter.
     *
     * @param name returned content should have
     * @param ns Namespace of elements
     */
    public abstract Content encodeXML(String name, Namespace ns);

    /**
     * create objects from xml.
     *
     * @param element to be decoded
     */
    public abstract void decodeXML(Element element);
    
    /**
     * create string as found in xml encoding
     * @return String
     */ 
    public String toXMLString() {
        Content c = encodeXML(this.getClass().getName(), Namespace.getNamespace(""));
        XMLOutputter outputter = new XMLOutputter();
        outputter.setFormat(Format.getPrettyFormat());
        Document d = new Document();
        d.addContent(c);
        return outputter.outputString(d);
    }
    
    /**
     * String representation
     * @return String
     */ 
    public String toString(){
		return toString(2);
	}
    public abstract String toString(int radix);
    
	public abstract boolean inRange(String valueString);
}
