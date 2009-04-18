package org.llrp.ltkGenerator;

import java.io.BufferedWriter;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileWriter;
import java.io.IOException;
import java.io.InputStream;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;

import org.apache.commons.configuration.ConfigurationException;
import org.apache.commons.configuration.PropertiesConfiguration;
import org.apache.log4j.Logger;
import org.apache.log4j.PropertyConfigurator;
import org.apache.velocity.Template;
import org.apache.velocity.VelocityContext;
import org.apache.velocity.app.Velocity;
import org.apache.velocity.context.AbstractContext;
import org.apache.velocity.exception.MethodInvocationException;
import org.apache.velocity.exception.ParseErrorException;
import org.apache.velocity.exception.ResourceNotFoundException;
import org.llrp.ltkGenerator.generated.ChoiceDefinition;
import org.llrp.ltkGenerator.generated.CustomChoiceDefinition;
import org.llrp.ltkGenerator.generated.CustomEnumerationDefinition;
import org.llrp.ltkGenerator.generated.CustomMessageDefinition;
import org.llrp.ltkGenerator.generated.CustomParameterDefinition;
import org.llrp.ltkGenerator.generated.EnumerationDefinition;
import org.llrp.ltkGenerator.generated.LlrpDefinition;
import org.llrp.ltkGenerator.generated.MessageDefinition;
import org.llrp.ltkGenerator.generated.NamespaceDefinition;
import org.llrp.ltkGenerator.generated.ParameterDefinition;
import org.llrp.ltkGenerator.generated.VendorDefinition;

/**
 * generates LLRP messages, parameters, enumeration from the definitions in
 * llrpdef.xml and any extensions definitions in specified in the
 * generator.properties file.
 * 
 * the generations process uses apache velocity template engine. Each
 * MessageDefinition, ParameterDefinition, ChoiceDefinition, ... in llrpdef.xml
 * is applied to a template. The resulting Java classes are stored at the
 * locations specified in the propery file.
 * 
 * Extensions can be specified by listing the path to the extension
 * "extensionXMLs" propery in generator.properties e.g. extensionXMLs =
 * src/main/resources/customExtensions.xml;
 * 
 */

public class CodeGenerator {
	public static Logger logger = Logger.getLogger(Class.class.getName());
	public PropertiesConfiguration properties;
	private List<ParameterDefinition> parameters;
	private List<MessageDefinition> messages;
	private List<EnumerationDefinition> enumerations;
	private List<ChoiceDefinition> choices;
	private List<CustomParameterDefinition> customParams;
	private List<CustomMessageDefinition> customMessages;
	private List<CustomChoiceDefinition> customChoices;
	private List<CustomEnumerationDefinition> customEnumerations;
	private List<NamespaceDefinition> namespaces;
	private Map<String, String> schemaPaths;
	private Map<String, String> xmlFilePaths;
	private Utility utility;
	private Map<String, Long> vendorDefinitions;
	private String llrpPrefix;

	/**
	 * instantiate new code generator - probably want to call generate after.
	 * 
	 * @param propertiesFile
	 *            path to properties file
	 */
	public CodeGenerator(String propertiesFile) {
		try {
			InputStream is = new FileInputStream(propertiesFile);
			properties = new PropertiesConfiguration();
			properties.load(is);
		} catch (ConfigurationException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (FileNotFoundException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		vendorDefinitions = new HashMap<String, Long>();
		parameters = new LinkedList<ParameterDefinition>();
		messages = new LinkedList<MessageDefinition>();
		enumerations = new LinkedList<EnumerationDefinition>();
		choices = new LinkedList<ChoiceDefinition>();
		customParams = new LinkedList<CustomParameterDefinition>();
		customMessages = new LinkedList<CustomMessageDefinition>();
		customChoices = new LinkedList<CustomChoiceDefinition>();
		customEnumerations = new LinkedList<CustomEnumerationDefinition>();
		xmlFilePaths = new HashMap<String, String>();
		schemaPaths = new HashMap<String, String>();
		namespaces = new LinkedList<NamespaceDefinition>();
		utility = new Utility(properties);
		utility.setChoices(choices);
		utility.setCustomChoices(customChoices);
	}

	public CodeGenerator() {
		// nothing to do
	}

	/**
	 * generates LLRP messages, parameters, enumeration from the definitions in
	 * llrpdef.xml and any extensions definitions in specified in the
	 * generator.properties file.
	 */
	private void generate() {
		logger.debug("start code generation");
		String jaxBPackage = properties.getString("jaxBPackage");
		String[] extensions = properties.getStringArray("definition");
		String jarSchema = properties.getString("JarSchemaPath");
		boolean first = true;
		for (String s : extensions) {
			String[] oneExt = s.split(";");
			String path = oneExt[2];
			String[] splitted = path.split("/");
			if (first) {
				llrpPrefix = oneExt[0];
				first = false;
			}
			schemaPaths.put(oneExt[0].toLowerCase(), jarSchema
					+ splitted[splitted.length - 1]);
			xmlFilePaths.put(oneExt[0].toLowerCase(), oneExt[1]);

		}
		LlrpDefinition llrp = getLLRPDefinition(jaxBPackage, extensions);
		logger.debug("finished retrieving llrp definitions");
		logger.debug("start filling objects");
		fillObjects(llrp);
		createLookupMaps();
		logger.debug("finished filling objects");
		// generateCustom must be before Parameters because it sets the allowed
		// in values
		logger.debug("start generating custom parameters");
		generateCustomParameters();
		logger.debug("finished generating custom parameters");
		// generateMessages() and generateParameters must be executed before
		// generateEnumerations because enumeration supertypes are determined in
		// this methods
		logger.debug("start generating messages");
		generateMessages();
		logger.debug("finished generating messages");
		logger.debug("start generating messageFactory");
		generateMessageFactory();
		logger.debug("finished generating messageFactory");
		logger.debug("start generating parameters");
		generateParameters();
		logger.debug("finished generating parameters");
		logger.debug("start generating interfaces");
		generateInterfaces();
		logger.debug("finished generating interfaces");
		logger.debug("start generating enumerations");
		generateEnumerations();
		logger.debug("finished generating enumerations");
		logger.debug("start generating custom messages");
		generateCustomMessages();
		logger.debug("finished generating custom messages");
		logger.debug("start generating custom enumerations");
		generateCustomEnumerations();
		logger.debug("finished generating custom enumerations");
		logger.debug("start generating custom choices");
		generateCustomInterfaces();
		logger.debug("finished generating custom choices");
		logger.debug("start generating constants");
		generateConstants();
		logger.debug("finished object generator");

		logger.debug("finished generatins constants");
	}

	/**
	 * returns LLRPDefinition object that is the root of llrp xml
	 * 
	 * @param jaxBPackage
	 *            package definition of classes generated by jax B
	 * @param paths
	 *            to xml and xsd files. Each entry must be formatted as
	 *            "nameSpacePrefix;PathToXML;PathToXSD" see also definition
	 *            elements in generator.properties file
	 * @return
	 */
	public LlrpDefinition getLLRPDefinition(String jaxBPackage, String[] paths) {
		logger.debug("retrieve llrp definitions");
		String filePaths = "";
		try {
			filePaths = "";
			for (String s : paths) {
				logger.debug("extension: " + s);
				String[] oneExt = s.split(";");
				logger.debug("adding: " + oneExt[0].toLowerCase() + ", "
						+ oneExt[1] + ", " + oneExt[2]);
				filePaths += oneExt[1] + ";";
			}
		} catch (Exception e) {
			logger.error("error processing properties file! error message: "
					+ e.getMessage());
			System.exit(1);
		}

		LlrpDefinition llrp = LLRPUnmarshaller.getLLRPDefinition(jaxBPackage,
				filePaths);
		return llrp;
	}

	/**
	 * generates LLRPMessageFactory.java using the MessageFactoryTemplate.
	 */
	private void generateMessageFactory() {
		logger.debug("using template "
				+ properties.getString("messageFactoryTemplate"));
		logger.debug("generating MessageFactory");

		try {
			VelocityContext context = new VelocityContext();
			context.put("messages", messages);
			context.put("customs", customMessages);
			Template template = Velocity.getTemplate(properties
					.getString("messageFactoryTemplate"));
			BufferedWriter writer = new BufferedWriter(new FileWriter(
					properties.getString("generatedBase")
							+ "LLRPMessageFactory"
							+ properties.getString("fileEnding")));
			template.merge(context, writer);
			writer.flush();
			writer.close();
		} catch (ResourceNotFoundException e) {
			logger.error("Exception while generating code: "
					+ e.getLocalizedMessage() + " caused by " + e.getCause());
		} catch (ParseErrorException e) {
			logger.error("Exception while generating code: "
					+ e.getLocalizedMessage() + " caused by " + e.getCause());
		} catch (MethodInvocationException e) {
			logger.error("Exception while generating code: "
					+ e.getLocalizedMessage() + " caused by " + e.getCause());
		} catch (IOException e) {
			logger.error("Exception while generating code: "
					+ e.getLocalizedMessage() + " caused by " + e.getCause());
		} catch (Exception e) {
			logger.error("Exception while generating code: "
					+ e.getLocalizedMessage() + " caused by " + e.getCause());
		}
	}

	/**
	 * generates LLRP Messages using the MessageTemplate and the definitions in
	 * llrpdef.xml.
	 */
	private void generateMessages() {
		// set xml schema location in LLRPMessage. This is necessary to validate
		// xml messages
		logger.debug(messages.size() + " messages to generate");
		logger.debug("using template "
				+ properties.getString("messageTemplate"));
		logger.debug("generating files into "
				+ properties.getString("generatedMessagePackage"));

		for (MessageDefinition m : messages) {
			try {
				VelocityContext context = new VelocityContext();
				context.put("message", m);
				context.put("utility", utility);
				context.put("namespaces", namespaces);
				context.put("vendor", llrpPrefix);
				context.put("XMLSCHEMALOCATION", properties
						.getString("messageSchema"));

				Template template = Velocity.getTemplate(properties
						.getString("messageTemplate"));
				BufferedWriter writer = new BufferedWriter(new FileWriter(
						properties.getString("generatedMessagePackage")
								+ m.getName()
								+ properties.getString("fileEnding")));
				template.merge(context, writer);
				writer.flush();
				writer.close();
			} catch (ResourceNotFoundException e) {
				logger.error("Exception while generating code: "
						+ e.getLocalizedMessage() + " caused by "
						+ e.getCause());
			} catch (ParseErrorException e) {
				logger.error("Exception while generating code: "
						+ e.getLocalizedMessage() + " caused by "
						+ e.getCause());
			} catch (MethodInvocationException e) {
				logger.error("Exception while generating code: "
						+ e.getLocalizedMessage() + " caused by "
						+ e.getCause());
			} catch (IOException e) {
				logger.error("Exception while generating code: "
						+ e.getLocalizedMessage() + " caused by "
						+ e.getCause());
			} catch (Exception e) {
				logger.error("Exception while generating code: "
						+ e.getLocalizedMessage() + " caused by "
						+ e.getCause());
			}
		}
	}

	/**
	 * generates LLRP Parameters using the ParameterTemplate and the definitions
	 * in llrpdef.xml.
	 */
	private void generateParameters() {
		logger.debug(parameters.size() + " parameters to generate");

		logger.debug("using template "
				+ properties.getString("parameterTemplate"));
		logger.debug("generating files into "
				+ properties.getString("parameterMessagePackage"));

		for (ParameterDefinition p : parameters) {
			try {
				VelocityContext context = new VelocityContext();
				context.put("parameter", p);
				context.put("choices", choices);
				context.put("utility", utility);
				context.put("vendor", llrpPrefix);

				Template template = Velocity.getTemplate(properties
						.getString("parameterTemplate"));
				BufferedWriter writer = new BufferedWriter(new FileWriter(
						properties.getString("generatedParameterPackage")
								+ p.getName()
								+ properties.getString("fileEnding")));
				template.merge(context, writer);
				writer.flush();
				writer.close();
			} catch (ResourceNotFoundException e) {
				logger.error("Exception while generating code: "
						+ e.getLocalizedMessage() + " caused by "
						+ e.getCause());
			} catch (ParseErrorException e) {
				logger.error("Exception while generating code: "
						+ e.getLocalizedMessage() + " caused by "
						+ e.getCause());
			} catch (MethodInvocationException e) {
				logger.error("Exception while generating code: "
						+ e.getLocalizedMessage() + " caused by "
						+ e.getCause());
			} catch (IOException e) {
				logger.error("Exception while generating code: "
						+ e.getLocalizedMessage() + " caused by "
						+ e.getCause());
			} catch (Exception e) {
				logger.error("Exception while generating code: "
						+ e.getLocalizedMessage() + " caused by "
						+ e.getCause());
			}
		}
	}

	/**
	 * generates interfaces that represent the choice constructs in llrpdef.xml
	 * using the InterfaceTemplate.
	 */

	private void generateInterfaces() {
		logger.debug(choices.size() + " interfaces to generate");
		logger.debug("using template "
				+ properties.getString("interfaceTemplate"));
		logger.debug("generating files into "
				+ properties.getString("generatedInterfacePackage"));

		for (ChoiceDefinition cd : choices) {
			try {
				VelocityContext context = new VelocityContext();
				context.put("interface", cd);
				context.put("utility", utility);

				Template template = Velocity.getTemplate(properties
						.getString("interfaceTemplate"));
				BufferedWriter writer = new BufferedWriter(new FileWriter(
						properties.getString("generatedInterfacePackage")
								+ cd.getName()
								+ properties.getString("fileEnding")));
				template.merge(context, writer);
				writer.flush();
				writer.close();
			} catch (ResourceNotFoundException e) {
				logger.error("Exception while generating code: "
						+ e.getLocalizedMessage() + " caused by "
						+ e.getCause());
			} catch (ParseErrorException e) {
				logger.error("Exception while generating code: "
						+ e.getLocalizedMessage() + " caused by "
						+ e.getCause());
			} catch (MethodInvocationException e) {
				logger.error("Exception while generating code: "
						+ e.getLocalizedMessage() + " caused by "
						+ e.getCause());
			} catch (IOException e) {
				logger.error("Exception while generating code: "
						+ e.getLocalizedMessage() + " caused by "
						+ e.getCause());
			} catch (Exception e) {
				logger.error("Exception while generating code: "
						+ e.getLocalizedMessage() + " caused by "
						+ e.getCause());
			}
		}
	}

	/**
	 * generates custom interfaces that represent the choice constructs in a
	 * vendor extension xml using the CustomInterfaceTemplate.
	 */

	private void generateCustomInterfaces() {
		logger.debug(choices.size() + " interfaces to generate");
		logger.debug("using template "
				+ properties.getString("customInterfaceTemplate"));
		logger.debug("generating files into "
				+ properties.getString("generatedCustomInterfacePackage"));

		for (CustomChoiceDefinition cd : customChoices) {
			try {
				VelocityContext context = new VelocityContext();
				context.put("interface", cd);
				context.put("utility", utility);

				Template template = Velocity.getTemplate(properties
						.getString("customInterfaceTemplate"));
				BufferedWriter writer = new BufferedWriter(new FileWriter(
						properties.getString("generatedCustomInterfacePackage")
								+ cd.getName()
								+ properties.getString("fileEnding")));
				template.merge(context, writer);
				writer.flush();
				writer.close();
			} catch (ResourceNotFoundException e) {
				logger.error("Exception while generating code: "
						+ e.getLocalizedMessage() + " caused by "
						+ e.getCause());
			} catch (ParseErrorException e) {
				logger.error("Exception while generating code: "
						+ e.getLocalizedMessage() + " caused by "
						+ e.getCause());
			} catch (MethodInvocationException e) {
				logger.error("Exception while generating code: "
						+ e.getLocalizedMessage() + " caused by "
						+ e.getCause());
			} catch (IOException e) {
				logger.error("Exception while generating code: "
						+ e.getLocalizedMessage() + " caused by "
						+ e.getCause());
			} catch (Exception e) {
				logger.error("Exception while generating code: "
						+ e.getLocalizedMessage() + " caused by "
						+ e.getCause());
			}
		}
	}

	/**
	 * generates enumerations that represent the enumerations defined in
	 * llrpdef.xml using the EnumerationTemplate.
	 */

	private void generateEnumerations() {
		logger.debug(enumerations.size() + " enumerations to generate");
		logger.debug("using template "
				+ properties.getString("enumerationTemplate"));
		logger.debug("generating files into "
				+ properties.getString("generatedEnumerationPackage"));

		for (EnumerationDefinition enu : enumerations) {
			createOneEnumeration(enu, utility.getSuperType(enu.getName()),
					false);

			if (!utility.getSuperType(enu.getName() + "Array").equals("")) {
				logger.debug("create enumerations array for " + enu.getName()
						+ "Array");
				createOneEnumeration(enu, utility.getSuperType(enu.getName()
						+ "Array"), true);
			}
		}
	}

	private void createOneEnumeration(EnumerationDefinition enu,
			String superType, boolean isArray) {
		try {
			VelocityContext context = new VelocityContext();
			context.put("enum", enu);
			context.put("utility", utility);
			context.put("superType", superType);
			context.put("vendor", llrpPrefix);
			if (isArray) {
				context.put("className", enu.getName() + "Array");
			} else {
				context.put("className", enu.getName());
			}
			Template template = Velocity.getTemplate(properties
					.getString("enumerationTemplate"));
			String name;
			if (isArray) {

				name = properties.getString("generatedEnumerationPackage")
						+ enu.getName() + "Array"
						+ properties.getString("fileEnding");
			} else {
				name = properties.getString("generatedEnumerationPackage")
						+ enu.getName() + properties.getString("fileEnding");
			}
			BufferedWriter writer = new BufferedWriter(new FileWriter(name));
			template.merge(context, writer);
			writer.flush();
			writer.close();
		} catch (ResourceNotFoundException e) {
			logger.error("Exception while generating code: "
					+ e.getLocalizedMessage() + " caused by " + e.getCause());
		} catch (ParseErrorException e) {
			logger.error("Exception while generating code: "
					+ e.getLocalizedMessage() + " caused by " + e.getCause());
		} catch (MethodInvocationException e) {
			logger.error("Exception while generating code: "
					+ e.getLocalizedMessage() + " caused by " + e.getCause());
		} catch (IOException e) {
			logger.error("Exception while generating code: "
					+ e.getLocalizedMessage() + " caused by " + e.getCause());
		} catch (Exception e) {
			logger.error("Exception while generating code: "
					+ e.getLocalizedMessage() + " caused by " + e.getCause());
		}
	}

	/**
	 * create maps to look up if a enumeration or choice is a custom
	 * enumeration, choice respectively. Call before creating java files
	 */
	private void createLookupMaps() {
		for (CustomEnumerationDefinition cust : customEnumerations) {
			utility.addCustomEnumeration(cust.getName());
		}
		for (CustomChoiceDefinition cust : customChoices) {
			utility.addCustomChoice(cust.getName());
		}
		for (CustomMessageDefinition cust : customMessages) {
			utility.addCustomMessage(cust.getName());
		}
		for (CustomParameterDefinition cust : customParams) {
			utility.addCustomParameter(cust.getName());
		}
	}

	/**
	 * generates custom enumerations that represent the custom enumerations
	 * defined in a vendor xml using the CustomEnumerationTemplate.
	 */

	private void generateCustomEnumerations() {
		logger.debug(enumerations.size() + " custom enumerations to generate");
		logger.debug("using template "
				+ properties.getString("customEnumerationTemplate"));
		logger.debug("generating files into "
				+ properties.getString("generatedCustomEnumerationPackage"));

		for (CustomEnumerationDefinition enu : customEnumerations) {
			createOneCustomEnumeration(enu,
					utility.getSuperType(enu.getName()), false);

			if (!utility.getSuperType(enu.getName() + "Array").equals("")) {
				createOneCustomEnumeration(enu, utility.getSuperType(enu
						.getName()
						+ "Array"), true);
			}

		}
	}

	private void createOneCustomEnumeration(CustomEnumerationDefinition enu,
			String superType, boolean isArray) {
		try {
			logger.debug("generating custom enumeration " + enu.getName()
					+ " with super type " + superType + " and is array = "
					+ isArray);
			VelocityContext context = new VelocityContext();
			context.put("enum", enu);
			context.put("superType", superType);
			context.put("utility", utility);
			context.put("vendor", enu.getNamespace());
			if (isArray) {
				context.put("className", enu.getName() + "Array");
			} else {
				context.put("className", enu.getName());
			}

			Template template = Velocity.getTemplate(properties
					.getString("customEnumerationTemplate"));
			String name;
			if (isArray) {
				name = properties
						.getString("generatedCustomEnumerationPackage")
						+ enu.getName()
						+ "Array"
						+ properties.getString("fileEnding");
			} else {
				name = properties
						.getString("generatedCustomEnumerationPackage")
						+ enu.getName() + properties.getString("fileEnding");
			}
			BufferedWriter writer = new BufferedWriter(new FileWriter(name));
			template.merge(context, writer);
			writer.flush();
			writer.close();
		} catch (ResourceNotFoundException e) {
			logger.error("Exception while generating code: "
					+ e.getLocalizedMessage() + " caused by " + e.getCause());
		} catch (ParseErrorException e) {
			logger.error("Exception while generating code: "
					+ e.getLocalizedMessage() + " caused by " + e.getCause());
		} catch (MethodInvocationException e) {
			logger.error("Exception while generating code: "
					+ e.getLocalizedMessage() + " caused by " + e.getCause());
		} catch (IOException e) {
			logger.error("Exception while generating code: "
					+ e.getLocalizedMessage() + " caused by " + e.getCause());
		} catch (Exception e) {
			logger.error("Exception while generating code: "
					+ e.getLocalizedMessage() + " caused by " + e.getCause());
		}
	}

	/**
	 * generates custom parameters using the CustomParameterTemplate and user
	 * defined parameters in an xml file that validates against llrpdef.xsd
	 */

	private void generateCustomParameters() {
		logger.debug(customParams.size() + " custom parameters to generate");
		logger.debug("using template "
				+ properties.getString("customParameterTemplate"));
		logger.debug("generating files into "
				+ properties.getString("generatedCustomParameterPackage"));

		for (CustomParameterDefinition cd : customParams) {
			try {
				VelocityContext context = new VelocityContext();
				context.put("custom", cd);
				context.put("utility", utility);
				context.put("vendor", cd.getVendor());
				context.put("vendorID", vendorDefinitions.get(cd.getVendor()
						.toLowerCase()));

				Template template = Velocity.getTemplate(properties
						.getString("customParameterTemplate"));
				BufferedWriter writer = new BufferedWriter(new FileWriter(
						properties.getString("generatedCustomParameterPackage")
								+ cd.getName()
								+ properties.getString("fileEnding")));
				template.merge(context, writer);
				writer.flush();
				writer.close();
			} catch (ResourceNotFoundException e) {
				logger.error("Exception while generating code: "
						+ e.getLocalizedMessage() + " caused by "
						+ e.getCause());
			} catch (ParseErrorException e) {
				logger.error("Exception while generating code: "
						+ e.getLocalizedMessage() + " caused by "
						+ e.getCause());
			} catch (MethodInvocationException e) {
				logger.error("Exception while generating code: "
						+ e.getLocalizedMessage() + " caused by "
						+ e.getCause());
			} catch (IOException e) {
				logger.error("Exception while generating code: "
						+ e.getLocalizedMessage() + " caused by "
						+ e.getCause());
			} catch (Exception e) {
				logger.error("Exception while generating code: "
						+ e.getLocalizedMessage() + " caused by "
						+ e.getCause());
			}
		}
	}

	/**
	 * generates custom messages using the CustomMessageTemplate and user
	 * defined messages from an xml file that validates against llrpdef.xsd
	 */

	private void generateCustomMessages() {
		logger.debug(customMessages.size() + " custom messages to generate");
		logger.debug("using template "
				+ properties.getString("customMessageTemplate"));
		logger.debug("generating files into "
				+ properties.getString("generatedCustomMessagePackage"));

		for (CustomMessageDefinition cd : customMessages) {
			try {
				VelocityContext context = new VelocityContext();
				context.put("message", cd);
				context.put("namespaces", namespaces);
				context.put("utility", utility);
				context.put("vendor", cd.getVendor());
				context.put("vendorID", vendorDefinitions.get(cd.getVendor()
						.toLowerCase()));
				Template template = Velocity.getTemplate(properties
						.getString("customMessageTemplate"));
				BufferedWriter writer = new BufferedWriter(new FileWriter(
						properties.getString("generatedCustomMessagePackage")
								+ cd.getName()
								+ properties.getString("fileEnding")));
				template.merge(context, writer);
				writer.flush();
				writer.close();
			} catch (ResourceNotFoundException e) {
				logger.error("Exception while generating code: "
						+ e.getLocalizedMessage() + " caused by "
						+ e.getCause());
			} catch (ParseErrorException e) {
				logger.error("Exception while generating code: "
						+ e.getLocalizedMessage() + " caused by "
						+ e.getCause());
			} catch (MethodInvocationException e) {
				logger.error("Exception while generating code: "
						+ e.getLocalizedMessage() + " caused by "
						+ e.getCause());
			} catch (IOException e) {
				logger.error("Exception while generating code: "
						+ e.getLocalizedMessage() + " caused by "
						+ e.getCause());
			} catch (Exception e) {
				logger.error("Exception while generating code: "
						+ e.getLocalizedMessage() + " caused by "
						+ e.getCause());
			}
		}
	}

	/**
	 * generates LLRPConstants.java from properties defined in
	 * generator.properties this properties include NamespacePrefix for XML
	 * encoding, namespace etc.
	 */

	private void generateConstants() {
		logger.debug("using template "
				+ properties.getString("constantsTemplate"));
		logger.debug("generating files into "
				+ properties.getString("generateConstantsPackage"));

		try {
			VelocityContext context = new VelocityContext();
			context.put("XMLEncodingSchema", properties
					.getString("XMLEncodingSchema"));
			context.put("externalLLRPSchema", properties
					.getString("externalLLRPSchema"));
			context.put("redirectExternalResources", properties
					.getString("redirectExternalResources"));
			context.put("extensionSchemas", schemaPaths);
			context.put("namespaces", namespaces);
			context.put("llrpprefix", llrpPrefix);
			Template template = Velocity.getTemplate(properties
					.getString("constantsTemplate"));
			BufferedWriter writer = new BufferedWriter(new FileWriter(
					properties.getString("generateConstantsPackage")
							+ "LLRPConstants"
							+ properties.getString("fileEnding")));
			template.merge(context, writer);
			writer.flush();
			writer.close();
		} catch (ResourceNotFoundException e) {
			logger.error("Exception while generating code: "
					+ e.getLocalizedMessage() + " caused by " + e.getCause());
		} catch (ParseErrorException e) {
			logger.error("Exception while generating code: "
					+ e.getLocalizedMessage() + " caused by " + e.getCause());
		} catch (MethodInvocationException e) {
			logger.error("Exception while generating code: "
					+ e.getLocalizedMessage() + " caused by " + e.getCause());
		} catch (IOException e) {
			logger.error("Exception while generating code: "
					+ e.getLocalizedMessage() + " caused by " + e.getCause());
		} catch (Exception e) {
			logger.error("Exception while generating code: "
					+ e.getLocalizedMessage() + " caused by " + e.getCause());
		}
	}

	/**
	 * add all messages, parameters, or choice defintions to corresponding
	 * lists. Each item in these lists will later be passed to the velocity
	 * context which generates the corresponding java class.
	 * 
	 * @param llrp
	 *            java objects of definitions from llrpdef.xml
	 */

	private void fillObjects(LlrpDefinition llrp) {
		List<Object> childs = llrp
				.getMessageDefinitionOrParameterDefinitionOrChoiceDefinition();

		for (Object o : childs) {
			if (o instanceof ParameterDefinition) {
				parameters.add((ParameterDefinition) o);
			} else if (o instanceof MessageDefinition) {
				messages.add((MessageDefinition) o);
			} else if (o instanceof EnumerationDefinition) {
				enumerations.add((EnumerationDefinition) o);
			} else if (o instanceof ChoiceDefinition) {
				choices.add((ChoiceDefinition) o);
			} else if (o instanceof CustomParameterDefinition) {
				customParams.add((CustomParameterDefinition) o);
			} else if (o instanceof CustomMessageDefinition) {
				customMessages.add((CustomMessageDefinition) o);
			} else if (o instanceof CustomEnumerationDefinition) {
				customEnumerations.add((CustomEnumerationDefinition) o);
			} else if (o instanceof CustomChoiceDefinition) {
				customChoices.add((CustomChoiceDefinition) o);
			} else if (o instanceof NamespaceDefinition) {
				NamespaceDefinition nsDef = (NamespaceDefinition) o;
				namespaces.add(nsDef);
			} else if (o instanceof VendorDefinition) {
				VendorDefinition vd = (VendorDefinition) o;
				vendorDefinitions.put(vd.getName().toLowerCase(), vd
						.getVendorID());
			} else {
				logger.warn("type not used: " + o.getClass()
						+ " in CodeGenerator.fillObjects");
			}
		}
	}

	public static void main(String[] args) {
		String propertiesFile = null;

		try {
			propertiesFile = args[0];
		} catch (ArrayIndexOutOfBoundsException e) {
			throw new IllegalArgumentException(
					"usage: provide path to properties file as first and only parammeter");
		}

		PropertyConfigurator.configure(propertiesFile);
		CodeGenerator cg = new CodeGenerator(propertiesFile);
		cg.generate();
	}

	public List<CustomChoiceDefinition> getCustomChoices() {
		return customChoices;
	}

	public void setCustomChoices(List<CustomChoiceDefinition> customChoices) {
		this.customChoices = customChoices;
	}
}
