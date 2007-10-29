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

import org.apache.log4j.Logger;

import org.jdom.Document;

import org.jdom.output.XMLOutputter;

import org.llrp.ltk.exceptions.IllegalBitListException;
import org.llrp.ltk.exceptions.LLRPException;
import org.llrp.ltk.exceptions.WrongParameterException;

import org.xml.sax.SAXException;

import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.IOException;
import java.io.InputStream;

import javax.xml.XMLConstants;
import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.transform.Source;
import javax.xml.transform.dom.DOMSource;
import javax.xml.transform.stream.StreamSource;
import javax.xml.validation.Schema;
import javax.xml.validation.SchemaFactory;
import javax.xml.validation.Validator;


/**
 * LLRPMessage is the abstract base class for all messages defined in the LLRP 
 * specification. It provides methods to encode/decode binary messages and LTK-XML 
 * messages.  
 * 
 * <p/>
 * LLRP Messages can be created in three different ways:
 * <ul>
 * <li>from a valid binary message
 * <li>from a valid LTK-XML message
 * <li>by using the individual get/set methods to construct a message
 * </ul>
 *  <p/>
 * 
 * <p/>
 * To generate the binary encoding or LTK-XML encoding from an object representation, 
 * use the <code>encodeBinary</code> or <code>encodeXML</code> methods respectively.
 *  <p/>
 * 
 * The abstract LLRPMessage methods implement the functionality that all LLRP 
 * messages have in common.
 * Each individual LTKJava message extends the LLRPMessage and implement the custom behaviour 
 * for each message type. For the binary encoding, subclasses specify the custom behaviour in a method
 * <code>encodeBinarySpecific</code> that is called from <code>encodeBinary</code>.
 *  <p/>
 * Some information on the binary message encoding defined in the LLRP specification:
 * Each binary message for example has the same format:<p/>
 *
 * [ Reserved (3 Bits) | Version (3 Bits) | Message Type (10 Bits) | Message Length (32 Bits) 
 * | Message ID (32 Bits) | other parameters ...]
 * <p/>
 * Reserved bits: The reserved bits are reserved for future extensions. 
 * All reserved bits in messages SHALL be set to 0 in outgoing messages.<p/>
 * Version: The version of LLRP. Implementations of LLRP based on the current 
 * specification are using the value 0x1. Other values are reserved for future use.<p/>
 * Message Type: The type of LLRP message being carried in the message.<p/>
 * Message Length: This value represents the size of the entire message 
 * in octets starting from bit offset 0 of the first word. 
 *
 */
public abstract class LLRPMessage {
    private static final Logger LOGGER = Logger.getLogger(LLRPMessage.class);
    public final int messageReservedLength = 3;

    // reserved length comes from parameter
    public final int reservedLength = 6;
    public final int versionLength = 3;
    public final int typeNumberLength = 10;
    public final int minHeaderLength = 80;
    protected BitList reserved = new BitList(messageReservedLength);
    protected BitList version;
    protected UnsignedInteger messageID = new UnsignedInteger();
    protected UnsignedInteger messageLength = new UnsignedInteger();

    /**
     * encodes this object in the binary format specified in EPCglobal LLRP spec.
     *
     * @return message in binary format
     */
    public final Byte[] encodeBinary() {
        LLRPBitList result = new LLRPBitList();
        result.append(reserved.encodeBinary());
        result.append(version.encodeBinary());
        // type number only last 10 bits of first two bytes. Bit 0-5 used for
        // reserved and version
        result.append(getTypeNum().encodeBinary()
                          .subList(messageReservedLength + versionLength,
                typeNumberLength));
        result.append(messageLength.encodeBinary());
        result.append(messageID.encodeBinary());
        // call the message specific encode function
        result.append(encodeBinarySpecific());
        // finalizeEncode sets the length correctly. It must be called after
        // encoding as length can only be set after message is encoded
        finalizeEncode(result);

        return result.toByteArray();
    }

    /**
     * encodes the parts of the binary message that are specific for each LLRP 
     * message. This method is implemented by each message class that extends 
     * {@link LLRPMessage} and is called by the encodeBinary method.
     * 
     * @see #encodeBinary()
     *
     * @return list of bits that represent the part of the binary message
     */
    protected abstract LLRPBitList encodeBinarySpecific();

    /**
     * decodes LLRP message in binary format
     * <p/>
     * This method is also called by the constructor when a new LLRP Message
     * of this type is instantiated from a binary message.
     * <p/>
     * The result is available via a number of <code>get</code> methods and 
     * can also be encoded in XML.
     *
     * @param byteArray binary LLRP message
     *            
     *
     * @throws LLRPException
     *             if bitstring is not well formatted or has any other error
     * @throws IllegalBitListException
     */
    public final void decodeBinary(Byte[] byteArray) {
        LLRPBitList bits = new LLRPBitList(byteArray);

        // message must have at least 80 bits for header
        if (bits.length() < minHeaderLength) {
            LOGGER.error("Bit String too short, must be at least 80, is " +
                bits.length());
            throw new IllegalBitListException("Bit String too short");
        }

        Short messageType = new SignedShort(bits.subList(messageReservedLength +
                    versionLength,
                    SignedShort.length() -
                    (messageReservedLength + versionLength))).toShort();

        // this should never occur. Implies an error in Message Decoder,
        // since it is the one responsible for calling the appropriate
        // class constructors according to message type
        if (!messageType.equals(getTypeNum().toShort())) {
            LOGGER.error("incorrect type. Message of Type " +
                getTypeNum().toShort() + " expected, but message indicates " +
                messageType);
            throw new LLRPException("incorrect type. Message of Type " +
                getTypeNum().toShort() + " expected, but message indicates " +
                messageType);
        }

        // skip first three bits as they are reserved
        int position = messageReservedLength;
        version = new BitList(versionLength);

        // version is a BitList of length versionLength
        for (int i = 0; i < versionLength; i++) {
            // read bits starting at position (position==reservedLength)
            if (bits.get(position + i)) {
                version.set(i);
            } else {
                version.clear(i);
            }
        }

        // skip message type - length starts at position 16 - it is a
        // signedShort
        position = SignedShort.length();
        messageLength = new UnsignedInteger(bits.subList(position,
                    UnsignedInteger.length()));
        position += UnsignedInteger.length();
        messageID = new UnsignedInteger(bits.subList(position,
                    UnsignedInteger.length()));
        position += UnsignedInteger.length();
        // call decodeSpecific which is implemented by the class subtyping this
        // one.
        // pass only data bits, not header
        decodeBinarySpecific(bits.subList(position, bits.length() - position));
    }


    /**
     * get Message ID to distinguish messages of the same type.
     * 
     * @return message ID of current message
     */
    public UnsignedInteger getMessageID() {
        return messageID;
    }

    /**
     * get length of encoded binary message. This value represents the size of the entire message 
     * in octets starting from bit offset 0 of the first word. 
     *
     * @return length of message in bytes
     */
    public UnsignedInteger getMessageLength() {
        return messageLength;
    }

    /**
     * get type number as defined in Table 4 of EPCglobal LLRP specification.
     *
     * @return message type
     */
    public abstract SignedShort getTypeNum();

    /**
     * get version of LLRP used. Implementations of LLRP based on the current LLRP 
     * specification are using the value 0x1. Other values are reserved for future use.
     *
     * @return lists of bits representing version. e.g. (0,0,1) for Ox1
     */
    public BitList getVersion() {
        return version;
    }

    /**
     * set message ID of current message. 
     *
     * @param messageID
     *            of type UnsignedInteger
     */
    public void setMessageID(UnsignedInteger messageID) {
        this.messageID = messageID;
    }

    /**
     * set version of LLRP Message. Implementations of LLRP based on the current LLRP 
     * specification are using the value 0x1. Other values are reserved for future use.
     * 
     * set version 0x1 by using the variadic argument for the BitList constructor. 
     * e.g. <code>setVersion(new BitList(0,0,1)).
     *
     * @param version
     *            as bit array
     *
     * @throws WrongParameterException
     */
    public void setVersion(BitList version) {
        if (version.length() != versionLength) {
            // LLRPMessage.logger.warn("wrong length of version - must be bit
            // array of length 3");
            throw new LLRPException("wrong length of version");
        }

        this.version = version;
    }

    /**
     * get binary representation of message as a string
     *
     * @return message in binary format as a string
     */
    public String toString() {
        return encodeBinary().toString();
    }

    /**
     * decodes the parts of the binary message that are specific for each LLRP 
     * message. This method is implemented by each message class that extends 
     * {@link LLRPMessage} and is called by the decodeBinary method.
     * 
     * @see #decodeBinary()
     *
     * @param list of bits in message without header (reserved, version, ...)
     *
     * @throws LLRPException
     */
    protected abstract void decodeBinarySpecific(LLRPBitList bits);

    /**
     * finalizeEncode computes the length of the binary message. 
     * The method is called at the end of <code>encodeBinary</code> method
     * to set the message length appropriately.
     *
     * @param result binary message which was just encoded
     */
    private void finalizeEncode(LLRPBitList result) {
        // get length of BitList
        int lengthBits = result.length();

        // calculte number of bytes
        // BitList must already be a multiple of 8
        int lengthBytes = lengthBits / 8;
        LLRPBitList binLength = new UnsignedInteger(lengthBytes).encodeBinary();

        for (int i = 0; i < binLength.length(); i++) {
            // replace bits starting from bit 16 up to bit 48
            if (binLength.get(i)) {
                result.set(versionLength + messageReservedLength +
                    typeNumberLength + i);
            } else {
                result.clear(versionLength + messageReservedLength +
                    typeNumberLength + i);
            }
        }
    }

    /**
     * encodes/serializes this object in the LTK-XML format specified LTK project. The corresponding
     * XML Schema LLRP.xsd is available from {@link www.llrp.org}.
     *
     * @return message in LTK-XML format
     */
    public abstract Document encodeXML();

    /**
     * decodes/deserializes LLRP message from LTK-XML format
     * <p/>
     * This method is also called by the constructor when a new LLRP Message
     * of this type is instantiated from a LTK-XML message.
     * <p/>
     * The result is available via a number of <code>get</code> methods and 
     * can also be encoded in binary.
     *
     * @param xml LTK-XML message as a jdom document
     */
    public abstract void decodeXML(Document xml);

    /**
     * Check xml file against xml schema.
     * @param jdomDoc to be checked
     * @param XMLSCHEMALOCATION path to xml schema file
     * @return boolean ture if valid
     */
    public boolean isValidXMLMessage(Document jdomDoc, String XMLSCHEMALOCATION) {
        try {
            //create input stream of jdomDoc 
            XMLOutputter output = new XMLOutputter();
            ByteArrayOutputStream stream = new ByteArrayOutputStream();
            output.output(jdomDoc, stream);
            LOGGER.debug("finished combining xml - writing to stream");

            byte[] a = stream.toByteArray();
            InputStream is = new ByteArrayInputStream(a);

            // create a SchemaFactory capable of understanding WXS schemas
            SchemaFactory factory = SchemaFactory.newInstance(XMLConstants.W3C_XML_SCHEMA_NS_URI);

            // load a WXS schema, represented by a Schema instance
            Source schemaFile = new StreamSource(new File(XMLSCHEMALOCATION));
            Schema schema = factory.newSchema(schemaFile);

            // create a Validator instance, which can be used to validate an instance document
            Validator validator = schema.newValidator();

            // validate the DOM tree
            validator.validate(new StreamSource(is));
        } catch (SAXException e) {
            LOGGER.warn("document can not be validated against schema " +
                XMLSCHEMALOCATION);

            return false;
        } catch (IOException e) {
            LOGGER.warn("document can not be validated against schema " +
                XMLSCHEMALOCATION);

            return false;
        }

        return true;
    }
}
