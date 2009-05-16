package org.llrp.ltkGenerator.ant;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;

import org.apache.commons.configuration.ConfigurationException;
import org.apache.commons.configuration.PropertiesConfiguration;
import org.apache.tools.ant.Project;
import org.apache.tools.ant.Task;

public class CopySchemaFiles extends Task {

	private static final String FOLDER_SEPARATOR = "/";
	private String propertyPath;
	private String destination;
	private String statusMessage;
	private PropertiesConfiguration properties;

	public void execute() {
		if (propertyPath == null || propertyPath.equals("")) {
			log("Cannot read properties file as path is not specified",
					Project.MSG_ERR);
			return;
		}
		String msg = "Successfully copied the following files: ";
		try {
			InputStream is = new FileInputStream(propertyPath);
			properties = new PropertiesConfiguration();
			properties.load(is);
			String[] extensions = properties.getStringArray("definition");
			String paths = "";
			for (String s : extensions) {
				String[] oneExt = s.split(";");
				String path = oneExt[2];
				String[] splittedPath = path.split(FOLDER_SEPARATOR);
				String file = splittedPath[splittedPath.length - 1];
				boolean ok = copyFile(path, destination + FOLDER_SEPARATOR
						+ file);
				if (ok) {
					msg += path + ";";
				}
			}
		} catch (ConfigurationException e) {
			msg = "Error copying schemas: " + e.getMessage();
			log("Configuration Exception " + e.getMessage(), Project.MSG_ERR);
		} catch (FileNotFoundException e) {
			log("Properties File can not be found", Project.MSG_ERR);
			msg = "Error copying schemas: " + e.getMessage();
			// } catch (Exception e){
			// log("General Exception "+e.getMessage(), Project.MSG_ERR);
			// msg = "Error copying schemas: "+e.getMessage();
		}

		Project project = getProject();
		project.setProperty(statusMessage, msg);
	}

	// Private Methods

	private boolean copyFile(String source, String destination) {
		log("copy from " + source + " to " + destination, Project.MSG_INFO);
		boolean success = true;
		FileInputStream from = null;
		FileOutputStream to = null;
		try {
			File fromFile = new File(source);
			File toFile = new File(destination);
			if (!fromFile.exists()) {
				log("source " + source + " does not exist", Project.MSG_ERR);
				success = false;
			}
			if (!fromFile.isFile()) {
				log("source " + source + " is not a file", Project.MSG_ERR);
				success = false;
			}
			if (!fromFile.canRead()) {
				log("source " + source + " can not be read", Project.MSG_ERR);
				success = false;
			}

			if (toFile.isDirectory()) {
				toFile = new File(toFile, fromFile.getName());
				success = false;
			}

			if (toFile.exists()) {
				if (!toFile.canWrite()) {
					log("destination is not writable", Project.MSG_ERR);
					return false;
				}
			} else {
				String parent = toFile.getParent();
				if (parent == null)
					parent = System.getProperty("user.dir");
				File dir = new File(parent);
				if (!dir.exists()) {
					dir.mkdirs();
					log(dir.getAbsolutePath()+ " created", Project.MSG_DEBUG);
				}
				if (dir.isFile()) {
					log("destination folder is a file", Project.MSG_ERR);
					return false;
				}
				if (!dir.canWrite()) {
					log("can not write to destination directory",
							Project.MSG_ERR);
					return false;
				}
			}

			from = new FileInputStream(fromFile);
			to = new FileOutputStream(toFile);
			byte[] buffer = new byte[4096];
			int bytesRead;

			while ((bytesRead = from.read(buffer)) != -1)
				to.write(buffer, 0, bytesRead); // write
		} catch (FileNotFoundException ffe) {
			log("can not copy file " + ffe.getMessage(), Project.MSG_ERR);
			success = false;
		} catch (IOException e) {
			log("can not copy file " + e.getMessage(), Project.MSG_ERR);
			success = false;
		} finally {
			if (from != null)
				try {
					from.close();
				} catch (IOException e) {
					;
				}
			if (to != null)
				try {
					to.close();
				} catch (IOException e) {
					;
				}
		}
		return success;
	}

	// Getters and Setters

	public void setStatusMessage(String statusMessage) {
		this.statusMessage = statusMessage;
	}

	public String getStatusMessage() {
		return statusMessage;
	}

	public String getPropertyPath() {
		return propertyPath;
	}

	public void setPropertyPath(String propertyPath) {
		this.propertyPath = propertyPath;
	}

	public String getDestination() {
		return destination;
	}

	public void setDestination(String destination) {
		this.destination = destination;
	}

}
