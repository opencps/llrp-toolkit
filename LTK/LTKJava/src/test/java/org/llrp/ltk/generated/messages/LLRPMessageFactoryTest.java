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
 * See the License for the specific language governing permissions
 * and limitations under the License.
 *
 */

package org.llrp.ltk.generated.messages;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.FilenameFilter;
import java.io.IOException;
import java.io.StringReader;
import java.util.Properties;

import org.apache.log4j.Logger;
import org.apache.log4j.PropertyConfigurator;
import org.custommonkey.xmlunit.Diff;
import org.custommonkey.xmlunit.XMLTestCase;
import org.custommonkey.xmlunit.XMLUnit;
import org.jdom.Document;
import org.jdom.JDOMException;
import org.jdom.output.Format;
import org.jdom.output.XMLOutputter;
import org.junit.BeforeClass;
import org.junit.Test;
import org.llrp.ltk.exceptions.InvalidLLRPMessageException;
import org.llrp.ltk.generated.LLRPMessageFactory;
import org.llrp.ltk.types.LLRPBitList;
import org.llrp.ltk.types.LLRPMessage;
import org.llrp.ltk.util.Util;
import org.xml.sax.SAXException;


/**
 * LLRPMessageFactoryTest is a unit test that inputs different LTK XML 
 * messages and binary LLRP messages to the LLRPMessageFactory. The 
 * LLRPMessage objects returned by the LLRPMessageFactory
 * are encoded in binary and xml and compared against control messages.<p>
 * 
 * This test case also tests the individual LLRP Messages contained in 
 * org.llrp.ltk.generated.messages.<p>
 * 
 * The directory with the test messages is specified in generator.properties. 
 * The path to this propertis file is passed to the test case via a command 
 * line argument:<p>
 * 
 * -DpropertiesFile=src/main/resources/generator.properties<p>
 * 
 * The XML instances are compared using the xmlunit tool 
 * (see http://xmlunit.sourceforge.org). The binary messages are using 
 * a string comparator.
 * 
 * @author Christian Floerkemeier
 */
public class LLRPMessageFactoryTest extends XMLTestCase{

	LLRPMessage message; 

	Properties properties; 
	
	private static final Logger LOGGER = Logger.getLogger("LLRPMessageFactoryTest.class");

	String testDirName;

	/**
	 * load path from System to propertiesFile, set logger and get directory with test cases
	 * @throws Exception
	 */
	
	
	private void configure() {
		
		
		String propertiesFile = System.getProperty("propertiesFile");
		
		try {
            properties = new Properties();
            properties.load(new FileInputStream(propertiesFile));
            PropertyConfigurator.configure(properties);
            testDirName = properties.getProperty("testDirectory", "src/test/resources");
            LOGGER.debug("Directory with xml and binary test messages set to: " + testDirName);
            
        } catch (FileNotFoundException e) {
            throw new IllegalArgumentException("file " + propertiesFile +
                " not found");
        } catch (IOException e) {
            throw new IllegalArgumentException("file " + propertiesFile +
                " can not be read");
        }
		
		
	}
	


	@Test
	public final void testCreateLLRPMessageLLRPBitList() {
       
		configure();
		String filename; 
		File testDir = new File(testDirName);

		int i;
		String[] filenames;
		FilenameFilter filter = new BinaryFilter();
		for (filenames = testDir.list(filter), i = 0;
		filenames != null && i < filenames.length; i++) {

			filename = testDirName + "/" + filenames[i];
			LOGGER.debug("File name loaded: " + filename);
			try {
				LLRPBitList bits = Util.loadBinaryFileContent(new File (filename));
				LOGGER.debug("Binary Message used to create Java message: " + bits);
				message = LLRPMessageFactory.createLLRPMessage(bits);
				
				// assert that the binary encoding of the java object is identical to the original binary message
				LOGGER.debug("Binary Message after decoding/encoding: " + message.toHexString());
				assertEquals("Binary Input not equal to Binary Output for " + message.getClass(), bits.toString(), message.toBinaryString());

				
				// assert that the xml encoding of the java object is "identical" to the control xml message		
				
			    int dotPos = filename.lastIndexOf(".");
			    filename = filename.substring(0, dotPos) + ".xml" ;
				String xmlstring = Util.loadTextFileContent(new File (filename));
				LOGGER.debug("Expected XML representation: " + xmlstring);
				LOGGER.debug("XML represenation generated: " + message.toXMLString());

				XMLUnit.setIgnoreWhitespace(true);   
				Diff diff = new Diff(new FileReader(
						new File(filename)),
						new StringReader(message.toXMLString()));

				assertTrue("Encoded XML not equal to control XML instance" + message.getClass() , diff.similar());

			}
			catch (FileNotFoundException e) {
				LOGGER.error("File not found " + filename);
				fail("Could not open message:" + filename);
			}
			catch (InvalidLLRPMessageException e) {
				LOGGER.error("LLRP message not valid " + filename);
				LOGGER.error(e.getMessage());
				fail("LLRP message not valid:" + filename);
			}
			catch (IOException e) {
				LOGGER.error("Could not open file " + filename);
				fail("Could not open message: " + filename);
			}
			catch (SAXException e) {
				LOGGER.error("Could not xml build file during assertXMLEqual ");
				e.printStackTrace();
				fail("xml comparison failed: ");
			}


		}

	}


	@Test
	public final void testCreateLLRPMessageByteArray() {
		 // TODO
	}

	@Test
	public final void testCreateLLRPMessageDocument() {

		configure();
		String filename;
		File testDir = new File(testDirName);

		int i;
		String[] filenames;
		FilenameFilter filter = new XMLFilter();
		for (filenames = testDir.list(filter), i = 0;
		filenames != null && i < filenames.length; i++) {

			filename = testDirName + "/" + filenames[i];

			try {
				
				Document doc = new org.jdom.input.SAXBuilder().build(new
						FileReader(filename));
				
				XMLOutputter outputter = new XMLOutputter(Format.getPrettyFormat());
				LOGGER.debug("XML Message used to create Java message: " + outputter.outputString(doc));
			
				message = LLRPMessageFactory.createLLRPMessage(doc);
				
				// assert that the xml encoding of the java object is "identical" to the control xml message	
				
				LOGGER.debug("XML represenation generated: " + message.toXMLString());
				
				XMLUnit.setIgnoreWhitespace(true);   
				Diff diff = new Diff(new FileReader(
						new File(filename)),
						new StringReader(message.toXMLString()));

				assertTrue("XML Input not equal to XML Output for " + message.getClass() , diff.similar());
				
				// assert that the binary encoding of the java object is identical to the binary message
				
				int dotPos = filename.lastIndexOf(".");
			    filename = filename.substring(0, dotPos) + ".bin" ;
				String bitstring = Util.loadBinaryFileContent(new File(filename)).toString();
				
				LOGGER.debug("Expected binary representation: " + bitstring);
				LOGGER.debug("Binary Message after decoding/encoding: " + message.toBinaryString());
				assertEquals("Encoded binary message not equal to control binary message " + message.getClass(), bitstring, message.toBinaryString());


			}
			catch (FileNotFoundException e) {
				LOGGER.error("File not found " + filename);
				fail("Could not open message:" + filename);
			}
			catch (InvalidLLRPMessageException e) {
				LOGGER.error("LLRP message not valid " + filename);
				LOGGER.error(e.getMessage());
				fail("LLRP message not valid:" + filename);
			}
			catch (IOException e) {
				LOGGER.error("Could not open file " + filename);
				fail("Could not open message: " + filename);
			}
			catch (SAXException e) {
				LOGGER.error("Could not xml build file during assertXMLEqual ");
				e.printStackTrace();
				fail("xml comparison failed: ");
			}
			catch (JDOMException e) {
				LOGGER.error("Could not create XML document to instantiate LLRP Message" + filename);
				e.printStackTrace();
				fail("Could not create XML document to instantiate LLRP Message " + filename);
				
			}


		}
		
	}





	
	
	class BinaryFilter implements FilenameFilter {
		public boolean accept(File dir, String name) {
			return (name.endsWith(".bin"));
		}
	}
	
	class XMLFilter implements FilenameFilter {
		public boolean accept(File dir, String name) {
			return (name.endsWith(".xml"));
		}
	}
	
	
	


}
