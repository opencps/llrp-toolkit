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

import java.io.BufferedInputStream;
import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.util.HashMap;
import java.util.Map;

import javax.xml.XMLConstants;
import javax.xml.transform.stream.StreamSource;
import javax.xml.validation.Schema;
import javax.xml.validation.SchemaFactory;
import javax.xml.validation.Validator;

import org.apache.log4j.Logger;
import org.jdom.Document;
import org.jdom.output.Format;
import org.jdom.output.XMLOutputter;
import org.llrp.ltk.exceptions.InvalidLLRPMessageException;
import org.llrp.ltk.exceptions.MissingParameterException;
import org.llrp.ltk.util.LLRPExternalResourceResolver;
import org.xml.sax.SAXException;

/**
 * LLRPMessage represents an LLRP message in LTKJava. 
 * <p>
 * The binary encoding Is always: Reserved(3
 * Bits) | Version (3 Bits) | Message Type (10 Bits) | Message Length (32 Bits) |
 * Parameters
 * 
 * call empty constructor to create new message. Use constructor taking
 * LLRPBitList or Byte[] to create message from binary encoded message. Use
 * constructor with JDOM document as a parameter to create message from XML encoding
 * 
 * @author Basil Gasser - ETH Zurich
 * @author Christian Floerkemeier - MIT
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
	
	private Validator validator;
	private static Map<String,Validator> validators = new HashMap<String,Validator>();
	private static byte[] mutex = new byte[0];

	/**
	 * encode this message to binary formate.
	 * 
	 * @return Byte[] which can directly be sent over a stream
	 */
	public final byte[] encodeBinary() throws InvalidLLRPMessageException {

		try {

			LLRPBitList result = new LLRPBitList();
			result.append(reserved.encodeBinary());
			if (version != null) {
				result.append(version.encodeBinary());
			} else {
				throw new MissingParameterException("version not set");
			}
			// type number only last 10 bits of first two bytes. Bit 0-5 used
			// for
			// reserved and version
			result.append(getTypeNum().encodeBinary().subList(
					messageReservedLength + VERSIONLENGTH, TYPENUMBERLENGTH));
			result.append(messageLength.encodeBinary());
			result.append(messageID.encodeBinary());
			// call the message specific encode function
			result.append(encodeBinarySpecific());
			// finalizeEncode sets the length correctly. It must be called after
			// encoding as length can only be set after message is encoded
			finalizeEncode(result);

			return result.toByteArray();

		} catch (IllegalArgumentException e) {
			throw new InvalidLLRPMessageException(e.getMessage(),e);
		} catch (MissingParameterException e) {
			throw new InvalidLLRPMessageException(e.getMessage(),e);
		}
	}

	/**
	 * encoding function - has to be implemented by each message.
	 * 
	 * @return LLRPBitList
	 */
	protected abstract LLRPBitList encodeBinarySpecific()
			throws InvalidLLRPMessageException;

	/**
	 * create message from byte[]. Will also be called from Constructor taking a
	 * byte[] Argument
	 * 
	 * @param byteArray
	 *            representing message
	 * 
	 * @throws InvalidLLRPMessageException
	 *             if bitstring is not well formatted or has any other error
	 */
	public final void decodeBinary(byte[] byteArray)
			throws InvalidLLRPMessageException {

		try {

			LLRPBitList bits = new LLRPBitList(byteArray);

			// message must have at least 80 bits for header
			if (bits.length() < MINHEADERLENGTH) {
				LOGGER.error("Bit String too short, must be at least 80, is "
						+ bits.length());
				throw new InvalidLLRPMessageException(
						"Invalid binary message: Bit String is too short");
			}

			Short messageType = new SignedShort(bits.subList(
					messageReservedLength + VERSIONLENGTH, SignedShort.length()
							- (messageReservedLength + VERSIONLENGTH)))
					.toShort();

			// this should never occur. Implies an error in Message Decoder,
			// since it is the one responsible for calling the appropriate
			// class constructors according to message type
			if (!messageType.equals(getTypeNum().toShort())) {
				LOGGER.error("incorrect type. Message of Type "
						+ getTypeNum().toShort()
						+ " expected, but message indicates " + messageType);
				throw new InvalidLLRPMessageException(
						"incorrect type. Message of Type "
								+ getTypeNum().toShort()
								+ " expected, but message indicates "
								+ messageType);
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
			// call decodeSpecific which is implemented by the class subtyping
			// this
			// one.
			// pass only data bits, not header
			if (messageLength.intValue()==byteArray.length){
			decodeBinarySpecific(bits.subList(position, bits.length()
					- position));
			} else {
				throw new InvalidLLRPMessageException("message length not equal to length given in message ");
			}

		} catch (IllegalArgumentException e) {
			throw new InvalidLLRPMessageException(e.getMessage(),e);
		} catch (MissingParameterException e) {
			throw new InvalidLLRPMessageException(e.getMessage(),e);
		}

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
	 * response message type awaited
	 * 
	 * @return String
	 */
	public abstract String getResponseType();

	/**
	 * name of message (same as class name)
	 * 
	 * @return String
	 */
	public abstract String getName();

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
	 */
	public void setVersion(BitList version) {
		if (version.length() != VERSIONLENGTH) {
			// LLRPMessage.logger.warn("wrong length of version - must be bit
			// array of length 3");
			throw new IllegalArgumentException("wrong length of version");
		}

		this.version = version;
	}

	/**
	 * to be implemented by specific message.
	 * 
	 * @param bits
	 *            without header
	 * 
	 * @throws InvalidLLRPMessageException
	 */
	protected abstract void decodeBinarySpecific(LLRPBitList bits)
			throws InvalidLLRPMessageException;

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
				result.set(VERSIONLENGTH + messageReservedLength
						+ TYPENUMBERLENGTH + i);
			} else {
				result.clear(VERSIONLENGTH + messageReservedLength
						+ TYPENUMBERLENGTH + i);
			}
		}
	}

	/**
	 * create xml representation of this parameter.
	 * 
	 * @return Dom Document
	 * @throws InvalidLLRPMessageException
	 */
	public abstract Document encodeXML() throws InvalidLLRPMessageException;

	/**
	 * create objects from xml.
	 * 
	 * @param xml
	 *            document as jdom document
	 * @throws InvalidLLRPMessageException
	 * 
	 */
	public abstract void decodeXML(Document xml)
			throws InvalidLLRPMessageException;

	/**
	 * Check xml file against xml schema.
	 * 
	 * @param jdomDoc
	 *            to be checked
	 * @param schemaPath
	 *            path to xml schema file
	 * 
	 * @return boolean true if valid
	 * @throws InvalidLLRPMessageException
	 */
	public boolean isValidXMLMessage(Document jdomDoc, String schemaPath)
			throws InvalidLLRPMessageException {
		try {
			// create input stream of jdomDoc
			XMLOutputter output = new XMLOutputter();
			ByteArrayOutputStream stream = new ByteArrayOutputStream();
			output.output(jdomDoc, stream);

			byte[] a = stream.toByteArray();
			InputStream is = new ByteArrayInputStream(a);
			
			synchronized (mutex) {
				
				// get the validator for the schemaPath specified
				validator = validators.get(schemaPath);
				
				if(validator == null){//first access
					//create a SchemaFactory capable of understanding WXS schemas
					SchemaFactory factory = SchemaFactory
							.newInstance(XMLConstants.W3C_XML_SCHEMA_NS_URI);
					factory.setResourceResolver(new LLRPExternalResourceResolver());
					// load a WXS schema, represented by a Schema instance
					ClassLoader cl = getClass().getClassLoader();
					InputStream s = new BufferedInputStream(cl
							.getResourceAsStream(schemaPath));
					Schema schema = factory.newSchema(new StreamSource(s));
					// create a Validator instance, which can be used to validate an
					// instance document
					validator = schema.newValidator();
					// add validator to Map of existing validators
					validators.put(schemaPath, validator);
				}
				//validate the DOM tree
				validator.validate(new StreamSource(is));
			}
		} catch (SAXException e) {

			XMLOutputter output = new XMLOutputter();
			output.setFormat(Format.getPrettyFormat());
			LOGGER.warn("LTK XML message can not be validated against schema "
					+ schemaPath + output.outputString(jdomDoc) + "because "
					+ e.getMessage());
			throw new InvalidLLRPMessageException(
					"LTK XML message can not be validated against schema "
							+ schemaPath + output.outputString(jdomDoc)
							+ "because " + e.getMessage(),e);

		} catch (IOException e) {
			LOGGER.warn("LLRP.xsd schema cannot be found " + schemaPath);

			throw new InvalidLLRPMessageException(
					"LLRP.xsd schema cannot be found " + schemaPath,e);
		}

		return true;
	}

	/**
	 * Return LLRP message as string in LTK XML format
	 * 
	 * If there is an error during message encoding, the error message is
	 * returned.
	 * 
	 * @return LRRP message in LTK XML encoding
	 * @throws InvalidLLRPMessageException
	 */
	public String toXMLString() throws InvalidLLRPMessageException {

		Document d = this.encodeXML();
		XMLOutputter outputter = new XMLOutputter();
		outputter.setFormat(Format.getPrettyFormat());

		return outputter.outputString(d);

	}

	/**
	 * Return LLRP message as binary string in LLRP binary encoding.
	 * 
	 * If there is an error during message encoding, the error message is
	 * returned.
	 * 
	 * @return LRRP message in LLRP binary encoding
	 * @throws InvalidLLRPMessageException
	 */
	public String toBinaryString() throws InvalidLLRPMessageException {

		return new LLRPBitList(this.encodeBinary()).toString();

	}

	/**
	 * Return LLRP message as hex string in LLRP binary encoding.
	 * 
	 * If there is an error during message encoding, the error message is
	 * returned.
	 * 
	 * @return LRRP message in LLRP binary encoding
	 * @throws InvalidLLRPMessageException
	 */
	public String toHexString() throws InvalidLLRPMessageException {

		byte[] bytes = this.encodeBinary();
		StringBuilder sb = new StringBuilder();
		for (byte b : bytes) {
			// encode each byte as a 2character hex with a one-space separation
			sb
					.append((Integer.toString((b & 0xff) + 0x100, 16)
							.substring(1) + " ").toUpperCase());
		}
		return sb.toString();

	}

}
