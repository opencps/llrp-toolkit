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

package org.llrp.ltk.util;


import jargs.gnu.CmdLineParser;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.FilenameFilter;
import java.io.IOException;

import org.apache.log4j.BasicConfigurator;
import org.apache.log4j.Level;
import org.apache.log4j.LogManager;
import org.apache.log4j.Logger;
import org.jdom.Document;
import org.jdom.JDOMException;
import org.jdom.output.Format;
import org.jdom.output.XMLOutputter;
//import org.llrp.ltk.generated.messages.LLRPMessageFactory;
import org.llrp.ltk.types.LLRPBitList;
import org.llrp.ltk.types.LLRPMessage;

/**
 * LLRPConverter is a (command line) tool to convert 
 * LLRP binary messages to LTK-XML messages and vice versa.<p>
 * 
 * Usage: java -jar LTKJava<Version>.jar  
 * [{-v,--verbose}]
 * [{-b,--binary}] input message(s) is in LLRP binary format<p>
 * [{-x,--xml}] input message(s) is in LTK XML format<p>
 * [{-d,--dir} directory path] directory with messages<p>
 * [{-f,--file} file path] single message to be converted<p>
 * [{-t,--targetDir} targetDirectory path] target directory for converted messages<p>
 * Example binary->xml file conversion to console: java -jar LTKJava<Version>.jar -b ADD_ROSPEC.bin<p>
 * Example xml->binary file conversion to console: java -jar LTKJava<Version>.jar -x ADD_ROSPEC.xml<p>
 * Example xml->binary file conversion of all files in a dir:
 * java -jar LTKJava<Version>.jar -x -d messages/xml -t messages/bin");<p>
	
	}                                         
 *
 */
public class LLRPConverter {

	LLRPMessage message;

	private static final Logger LOGGER = Logger.getLogger("LLRPMessageFactoryTest.class");

	public LLRPConverter(){
		super();

	}

	private void convertFilesInDirectory(String dir, String target, Boolean xml) {

		File testDir = new File(dir);

		int i;
		String filename;
		String targetFile;
		String targetDir;
		String[] filenames;
		FileWriter out;
		String output;
		FilenameFilter filter;

		// converted files are written to the destination directory if 
		// no other directory is specified
		if (target == null) {
			targetDir = dir;
		}
		else {
			targetDir = target; 
		}
			
		
		// find all binary or all xml files in the directory
		if (xml == Boolean.FALSE) {
			filter = new BinaryFilter();
		}
		else {
			filter = new XMLFilter();	
		}
		for (filenames = testDir.list(filter), i = 0;
		filenames != null && i < filenames.length; i++) {

			// for each file found, convert it, and write the result to the appropriate file
			filename = filenames[i];
			
			int dotPos = filename.lastIndexOf(".");
			
			try {
				if (xml == Boolean.TRUE) {
					output = convertMessageToBinary(dir + "/" + filename);
					filename = filename.substring(0, dotPos) + ".bin" ;
				}
				else {
					output = convertMessageToXML(dir + "/" + filename);
					filename = filename.substring(0, dotPos) + ".xml" ;
				}
				
				out = new FileWriter(new File(targetDir + "/" + filename));
				out.write(output);
				out.close();
			}
			catch (FileNotFoundException e) {
				System.err.println("File not found " + filename);
			}
			catch (IOException e) {
				System.err.println("File IO problem " + filename);
			}

			catch (JDOMException e) {
				System.err.println("Could not create XML document to instantiate LLRP Message " + filename);
				e.printStackTrace();	
			}

			
		}


	}

	private String convertMessageToXML(String file) throws FileNotFoundException, IOException {

		String bitstring = getFileContent(file);
		LLRPBitList bits = new LLRPBitList(bitstring);
		LOGGER.debug("Input binary message: " + bitstring);
//		message = LLRPMessageFactory.createLLRPMessage(bits);

		return message.toXMLString();
	}

	private String convertMessageToBinary(String file) throws FileNotFoundException, IOException, JDOMException {

		Document doc = new org.jdom.input.SAXBuilder().build(new
				FileReader(file));
		XMLOutputter outputter = new XMLOutputter(Format.getPrettyFormat());
		LOGGER.debug("Input XML Message: " + outputter.outputString(doc));
//		message = LLRPMessageFactory.createLLRPMessage(doc);

		return message.toBinaryString();
	}


	private void convert(Boolean xml, Boolean binary, String file, String dir, String targetDir) {


		try {
			if (binary != null) {
				if (file != null) {
					System.out.println(convertMessageToXML(file));
				}
				else if (dir != null) {
					convertFilesInDirectory(dir, targetDir, Boolean.FALSE);
				}
				else {
					System.err.println("This should never happen!");
				}

			}
			else if (xml != null) {
				if (file != null) {
					System.out.println(convertMessageToBinary(file));
				}
				else if (dir != null) {
					convertFilesInDirectory(dir, targetDir, Boolean.TRUE);
				}
				else {
					System.err.println("This should never happen!");
				}

			}
		}
		catch (FileNotFoundException e) {
			System.err.println("File not found");
			printUsage();                                                       
			System.exit(2); 
		}
		catch (IOException e) {
			System.err.println("File not found");
			printUsage();                                                       
			System.exit(2);
		}
		catch (JDOMException e) {
			System.err.println("Invalid XML document to instantiate LLRP Message" + file);
			System.exit(2);	
		}


	}


	private static void printUsage() {                                          
		System.err.println(                                                     
				"Usage: java -jar LTKJava<Version>.jar  [{-v,--verbose}]\n" +
				"                      [{-b,--binary}] input message(s) is in LLRP binary format\n" +
				"                      [{-x,--xml}] input message(s) is in LTK XML format\n" +
				"                      [{-d,--dir} directory path] directory with messages\n" +
				"                      [{-f,--file} file path] single message to be converted\n" +
		"                      [{-t,--targetDir} targetDirectory path] target directory for converted messages\n\n" +
	    "Example binary->xml file conversion to console:\n java -jar LTKJava<Version>.jar -b ADD_ROSPEC.bin\n" +
	    "Example xml->binary file conversion to console:\n java -jar LTKJava<Version>.jar -x ADD_ROSPEC.xml\n" +
	    "Example xml->binary file conversion of all files in a dir:\n" +
	    "       java -jar LTKJava<Version>.jar -x -d messages/xml -t messages/bin\n");
	
	}                                                                           

	public static void main( String[] args ) {                                  

		// First, you must create a CmdLineParser, and add to it the            
		// appropriate Options.                                                 

		BasicConfigurator.configure();
		Logger rootLogger = LogManager.getRootLogger();
		rootLogger.setLevel(Level.WARN);
		
		CmdLineParser parser = new CmdLineParser();                             
		CmdLineParser.Option verbose = parser.addBooleanOption('v', "verbose");     

		CmdLineParser.Option binary = parser.addBooleanOption('b',"binary");              
		CmdLineParser.Option xml = parser.addBooleanOption('x',"xml");             
		CmdLineParser.Option dir = parser.addStringOption('d',"dir"); 
		CmdLineParser.Option file = parser.addStringOption('f',"file");
		CmdLineParser.Option targetDir = parser.addStringOption('t',"targetDir");

		// Next, you must parse the user-provided command line arguments, and   
		// catch any errors therein.                                            

		// Options may appear on the command line in any order, and may even    
		// appear after some or all of the non-option arguments.                


		try {                                                                   
			parser.parse(args);                                                 
		}                                                                       
		catch ( CmdLineParser.OptionException e ) {                             
			System.err.println(e.getMessage());                                 
			printUsage();                                                       
			System.exit(2);                                                     
		}       



		// For options that may be specified only zero or one time, the value   
		// of that option is extracted.  If the options      
		// were not specified, the corresponding values will be null.           

		Boolean verboseValue = (Boolean)parser.getOptionValue(verbose, Boolean.FALSE);  
		Boolean xmlValue = (Boolean)parser.getOptionValue(xml);
		Boolean binaryValue = (Boolean)parser.getOptionValue(binary);

		String dirValue = (String)parser.getOptionValue(dir);     
		String fileValue = (String)parser.getOptionValue(file);
		String targetDirValue = (String)parser.getOptionValue(targetDir);

		if ((xmlValue == null) && (binaryValue == null) && (fileValue == null) && (dirValue == null)){
			printUsage();                                                       
			System.exit(2);
		}

		if ((xmlValue == null) && (binaryValue == null)){
			System.err.println("Specify the type of input message format (either binary or xml)");
			printUsage();                                                       
			System.exit(2);
		}
		if ((xmlValue != null) && (binaryValue != null)){
			System.err.println("Specify the type of input message format (either binary or xml)");
			printUsage();                                                       
			System.exit(2);
		}

		if ((fileValue == null) && (dirValue == null)){
			System.err.println("Specify a file or directory for conversion");
			printUsage();                                                       
			System.exit(2);
		}

		if ((fileValue != null) && (dirValue != null)){
			System.err.println("Specify either a file or a directory for conversion");
			printUsage();                                                       
			System.exit(2);
		}
		
		if ((targetDirValue != null) && (!(new File(targetDirValue)).isDirectory())){
			System.err.println("Target Directory does not exist: " + targetDirValue);
			printUsage();                                                       
			System.exit(2);
		}
		
		if ((dirValue != null) && (!(new File(dirValue)).isDirectory())){
			System.err.println("Directory does not exist: " + dirValue);
			printUsage();                                                       
			System.exit(2);
		}
		if ((fileValue != null) && (!(new File(fileValue)).isFile())){
			System.err.println("File does not exist: " + fileValue);
			printUsage();                                                       
			System.exit(2);
		}

		LLRPConverter converter = new LLRPConverter();

		converter.convert(xmlValue, binaryValue, fileValue, dirValue, targetDirValue);

	}


	private String getFileContent(String filename) throws IOException, FileNotFoundException{

		File file = new File(filename);
		FileReader fis = null;
		BufferedReader bis = null;
		StringBuffer buffer = new StringBuffer();
		String line; 

		fis = new FileReader(file);

		// Here BufferedReader is added for fast reading.
		bis = new BufferedReader(fis);
		line = bis.readLine();
		while ( line != null ) { // co

			// this statement reads the line from the file and print it to
			// the console.
			buffer.append(line);
			line = bis.readLine();
		}

		return buffer.toString();
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

