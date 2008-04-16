package org.llrp.ltk.util;

import java.io.BufferedInputStream;
import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;

import org.llrp.ltk.types.LLRPBitList;

public class Util {

	public static LLRPBitList getBinaryFileContent(File file) throws IOException, FileNotFoundException{

		
		FileInputStream fis = new FileInputStream(file);
		BufferedInputStream bis = new BufferedInputStream(fis);
		
		
		LLRPBitList bitlist = new LLRPBitList();
		
        while (bis.available() > 0) {
        	byte[] bytes = new byte[bis.available()];
        	int length = bis.read(bytes);
        	bitlist.append(new LLRPBitList(bytes));
        }
    
        fis.close();
        bis.close();
		return bitlist;
        
	}
	
	public static String getTextFileContent(File file) throws IOException, FileNotFoundException{

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
	
	
	
}
