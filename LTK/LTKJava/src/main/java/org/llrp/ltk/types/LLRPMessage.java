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

import org.jdom.output.Format;
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
import javax.xml.transform.Source;
import javax.xml.transform.stream.StreamSource;
import javax.xml.validation.Schema;
import javax.xml.validation.SchemaFactory;
import javax.xml.validation.Validator;


/**
 * representing a message in LLRP. * The binary encoding Is always: Reserved(3
 * Bits) | Version (3 Bits) | Message Type (10 Bits) | Message Length (32 Bits) |
 * Parameters
 *
 * call empty constructor to create new message. Use constructor taking
 * LLRPBitList or Byte[] to create message from binary encoded message. Use
 * constructor taking JDOM document to create message from XML encoding
 *
 */
public abstract class LLRPMessage {
    private static final Logger LOGGER = Logger.getLogger(LLRPMessage.class);

    // reserved length comes from parameter
    public static final int RESERVEDLENGTH = 6;
    public static final int VERSIONLENGTH = 3;
    public static final int TYPENUMBERLENGTH = 10;
    public static final int MINHEADERLENGTH = 80;
    public final int messageReservedLength = 3;
    protected BitList reserved = new BitList(messageReservedLength);
    protected BitList version;
    protected UnsignedInteger messageID = new UnsignedInteger();
    protected UnsignedInteger messageLength = new UnsignedInteger();

    /**
     * encode this message to binary formate.
     *
     * @return Byte[] which can directly be sent over a stream
     */
    public final Byte[] encodeBinary() {
        LLRPBitList result = new LLRPBitList();
        result.append(reserved.encodeBinary());
        result.append(version.encodeBinary());
        // type number only last 10 bits of first two bytes. Bit 0-5 used for
        // reserved and version
        result.append(getTypeNum().encodeBinary()
                          .subList(messageReservedLength + VERSIONLENGTH,
                TYPENUMBERLENGTH));
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
     * encoding function - has to be implemented by each message.
     *
     * @return LLRPBitList
     */
    protected abstract LLRPBitList encodeBinarySpecific();

    /**
     * create message from Byte[]. Will also be called from Constructor taking a
     * Byte[] Argument
     *
     * @param byteArray
     *            representing message
     *
     * @throws LLRPException
     *             if bitstring is not well formatted or has any other error
     * @throws IllegalBitListException
     */
    public final void decodeBinary(Byte[] byteArray) {
        LLRPBitList bits = new LLRPBitList(byteArray);

        // message must have at least 80 bits for header
        if (bits.length() < MINHEADERLENGTH) {
            LOGGER.error("Bit String too short, must be at least 80, is " +
                bits.length());
            throw new IllegalBitListException("Bit String too short");
        }

        Short messageType = new SignedShort(bits.subList(messageReservedLength +
                    VERSIONLENGTH,
                    SignedShort.length() -
                    (messageReservedLength + VERSIONLENGTH))).toShort();

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
        version = new BitList(VERSIONLENGTH);

        // version is a BitList of length VERSIONLENGTH
        for (int i = 0; i < VERSIONLENGTH; i++) {
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

    // /**
    /**
     * Message ID to distinguish messages of same type.
     *
     * @return UnsignedInteger
     */
    public UnsignedInteger getMessageID() {
        return messageID;
    }

    /**
     * number of bytes of encoded message.
     *
     * @return UnsignedInteger
     */
    public UnsignedInteger getMessageLength() {
        return messageLength;
    }

    /**
     * type number uniquely identifies message.
     *
     * @return SignedShort
     */
    public abstract SignedShort getTypeNum();

    /**
     * version of llrp.
     *
     * @return BitList
     */
    public BitList getVersion() {
        return version;
    }

    /**
     * setMessageID.
     *
     * @param messageID
     *            of type UnsignedInteger
     */
    public void setMessageID(UnsignedInteger messageID) {
        this.messageID = messageID;
    }

    /**
     * create BitList easiest is to use variadic argument. for example
     * BitList(0,1,1) for value 3.
     *
     * @param version
     *            as bit array
     *
     * @throws WrongParameterException
     */
    public void setVersion(BitList version) {
        if (version.length() != VERSIONLENGTH) {
            // LLRPMessage.logger.warn("wrong length of version - must be bit
            // array of length 3");
            throw new LLRPException("wrong length of version");
        }

        this.version = version;
    }

    /**
     * Message as Bitstring.
     *
     * @return String
     */
    public String toString() {
        return encodeBinary().toString();
    }

    /**
     * to be implemented by specific message.
     *
     * @param bits
     *            without header
     *
     * @throws LLRPException
     */
    protected abstract void decodeBinarySpecific(LLRPBitList bits);

    /**
     * finalizeEncode sets the length of the message.
     *
     * @param result
     *            LLRPBitList to finalize
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
                result.set(VERSIONLENGTH + messageReservedLength +
                    TYPENUMBERLENGTH + i);
            } else {
                result.clear(VERSIONLENGTH + messageReservedLength +
                    TYPENUMBERLENGTH + i);
            }
        }
    }

    /**
     * create xml representation of this parameter.
     *
     * @return Dom Document
     */
    public abstract Document encodeXML();

    /**
     * create objects from xml.
     *
     * @param xml document as jdom document
     *
     */
    public abstract void decodeXML(Document xml);

    /**
     * Check xml file against xml schema.
     * @param jdomDoc to be checked
     * @param schemaPath path to xml schema file
     *
     * @return boolean true if valid
     */
    public boolean isValidXMLMessage(Document jdomDoc, String schemaPath) {
        try {
            //create input stream of jdomDoc
            XMLOutputter output = new XMLOutputter();
            ByteArrayOutputStream stream = new ByteArrayOutputStream();
            output.output(jdomDoc, stream);

            byte[] a = stream.toByteArray();
            InputStream is = new ByteArrayInputStream(a);

            // create a SchemaFactory capable of understanding WXS schemas
            SchemaFactory factory = SchemaFactory.newInstance(XMLConstants.W3C_XML_SCHEMA_NS_URI);

            // load a WXS schema, represented by a Schema instance
            Source schemaFile = new StreamSource(new File(schemaPath));
            Schema schema = factory.newSchema(schemaFile);

            // create a Validator instance, which can be used to validate an instance document
            Validator validator = schema.newValidator();

            // validate the DOM tree
            validator.validate(new StreamSource(is));
        } catch (SAXException e) {
            XMLOutputter output = new XMLOutputter();
            output.setFormat(Format.getPrettyFormat());
            LOGGER.warn("LTK XML message can not be validated against schema " +
                schemaPath + output.outputString(jdomDoc));

            return false;
        } catch (IOException e) {
            LOGGER.warn("LLRP.xsd schema cannot be found " + schemaPath);

            return false;
        }

        return true;
    }

    public String toXMLString() {
        Document d = this.encodeXML();
        XMLOutputter outputter = new XMLOutputter();
        outputter.setFormat(Format.getPrettyFormat());

        return outputter.outputString(d);
    }

    public String toBinaryString() {
        return new LLRPBitList(this.encodeBinary()).toString();
    }
}
