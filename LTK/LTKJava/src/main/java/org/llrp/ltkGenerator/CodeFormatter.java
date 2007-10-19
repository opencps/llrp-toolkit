package org.llrp.ltkGenerator;

import de.hunsicker.jalopy.Jalopy;

import org.apache.log4j.ConsoleAppender;
import org.apache.log4j.FileAppender;
import org.apache.log4j.Level;
import org.apache.log4j.Logger;
import org.apache.log4j.PropertyConfigurator;
import org.apache.log4j.SimpleLayout;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;

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
            throw new IllegalArgumentException("file " + propertiesFile +
                " not found");
        } catch (IOException e) {
            throw new IllegalArgumentException("file " + propertiesFile +
                " can not be read");
        }
    }

    /**
     * format code - provide file or directory.
     * @param file
     */
    public void format(File file) {
        logger.debug("start formating at folder " + file.getName());
        formatDirectory(file);
        logger.debug("finished formatting");
    }

    /**
     * format files within a directory.
     * @param directory
     */
    private void formatDirectory(File directory) {
        String[] children = directory.list();

        if (children != null) {
            for (String name : children) {
                logger.debug("looking at " + directory.getAbsolutePath() +
                    "\\" + name);

                File temp = new File(directory.getAbsolutePath() + "\\" + name);

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
     */
    private void formatFile(File file) {
        // create a new Jalopy instance with the currently active code convention settings
        Jalopy jalopy = new Jalopy();

        try {
            Jalopy.setConvention(properties.getProperty("formatDefinitions"));
        } catch (IOException e1) {
            // just catch - we just use default formatting
            logger.warn("formatting file not correctly set");
        }

        logger.debug("formating " + file.getAbsolutePath());

        // specify input and output target
        try {
            jalopy.setInput(file);
        } catch (FileNotFoundException e) {
            logger.debug("can not set input file: " + e.getLocalizedMessage());
        }

        jalopy.setOutput(file);

        // format and overwrite the given input file
        jalopy.format();

        if (jalopy.getState() == Jalopy.State.OK) {
            logger.debug(file + " successfully formatted");
        } else if (jalopy.getState() == Jalopy.State.WARN) {
            logger.debug(file + " formatted with warnings");
        } else if (jalopy.getState() == Jalopy.State.ERROR) {
            logger.debug(file + " could not be formatted");
        }
    }

    /**
     * set up logger and read properties, then format files in source Folder (see properties file to specify source folder location)
     *
     * @param args
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
        File file = new File(formatter.properties.getProperty("generatedBase"));
        formatter.formatDirectory(file);
    }
}
