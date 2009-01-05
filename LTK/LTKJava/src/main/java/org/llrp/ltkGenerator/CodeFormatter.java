package org.llrp.ltkGenerator;

import de.hunsicker.jalopy.Jalopy;

import org.apache.log4j.Logger;
import org.apache.log4j.PropertyConfigurator;

import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.PrintStream;

import java.util.Properties;

public class CodeFormatter {
	private static Logger logger = Logger.getLogger(CodeFormatter.class);
	private Properties properties;

	public CodeFormatter(String propertiesFile) {
		try {
			properties = new Properties();
			properties.load(new FileInputStream(propertiesFile));
			PropertyConfigurator.configure(properties);
		} catch (FileNotFoundException e) {
			throw new IllegalArgumentException("file " + propertiesFile
					+ " not found");
		} catch (IOException e) {
			throw new IllegalArgumentException("file " + propertiesFile
					+ " can not be read");
		}
	}

	/**
	 * format code - provide file or directory.
	 * 
	 * @param file
	 *            to be formatted
	 */
	public void format(File file) {
		logger.debug("start formating at folder " + file.getName());
		formatDirectory(file);
		logger.debug("finished formatting");
	}

	/**
	 * format files within a directory.
	 * 
	 * @param directory
	 *            to be formatted
	 */
	private void formatDirectory(File directory) {
		String[] children = directory.list();

		if (children != null) {
			for (String name : children) {
				logger.debug("looking at " + directory.getAbsolutePath()
						+ File.separator + name);

				File temp = new File(directory.getAbsolutePath()
						+ File.separator + name);

				if (temp.isDirectory()) {
					formatDirectory(temp);
				} else if (name.endsWith(".java")) {
					formatFile(temp);
				} else {
					logger.debug("is nothing");
				}
			}
		}
	}

	/**
	 * format a file
	 * 
	 * @param file
	 *            to be formatted
	 */
	private void formatFile(File file) {
		// create a new Jalopy instance with the currently active code
		// convention settings
		Jalopy jalopy = new Jalopy();

		logger.debug("formating " + file.getAbsolutePath());

		// specify input and output target
		try {
			jalopy.setInput(file);
		} catch (FileNotFoundException e) {
			logger.debug("can not set input file: " + e.getLocalizedMessage());
		}
		logger.debug("input file set");
		jalopy.setOutput(file);
		logger.debug("output file set");

		// format and overwrite the given input file
		// jalopy has some ugly system out statements
		// temporary replace regular output stream by another output stream
		PrintStream orig = System.out;
		ByteArrayOutputStream ba = new ByteArrayOutputStream();
		PrintStream fo = null;
		fo = new PrintStream(ba);
		System.setOut(fo);
		jalopy.format();
		System.setOut(orig);
		if (jalopy.getState() == Jalopy.State.OK) {
			logger.debug(file + " successfully formatted");
		} else if (jalopy.getState() == Jalopy.State.WARN) {
			logger.debug(file + " formatted with warnings");
		} else if (jalopy.getState() == Jalopy.State.ERROR) {
			logger.debug(file + " could not be formatted");
		}

	}

	/**
	 * set up logger and read properties, then format files in source Folder
	 * (see properties file to specify source folder location)
	 * 
	 * @param args
	 *            of main
	 */
	public static void main(String[] args) {
		String propertiesFile = null;

		try {
			propertiesFile = args[0];
		} catch (ArrayIndexOutOfBoundsException e) {
			throw new IllegalArgumentException(
					"usage: provide path to properties file as first and only parammeter");
		}

		CodeFormatter formatter = new CodeFormatter(propertiesFile);
		File file = new File(formatter.properties.getProperty("sourceFolder"));
		formatter.formatDirectory(file);
	}
}
