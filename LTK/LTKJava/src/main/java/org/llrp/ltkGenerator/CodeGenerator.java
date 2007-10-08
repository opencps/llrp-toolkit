package org.llrp.ltkGenerator;

import org.apache.log4j.Logger;
import org.apache.log4j.PropertyConfigurator;

import org.apache.velocity.Template;
import org.apache.velocity.VelocityContext;
import org.apache.velocity.app.Velocity;
import org.apache.velocity.exception.MethodInvocationException;
import org.apache.velocity.exception.ParseErrorException;
import org.apache.velocity.exception.ResourceNotFoundException;

import org.llrp.ltkGenerator.generated.ChoiceDefinition;
import org.llrp.ltkGenerator.generated.CustomMessageDefinition;
import org.llrp.ltkGenerator.generated.CustomParameterDefinition;
import org.llrp.ltkGenerator.generated.EnumerationDefinition;
import org.llrp.ltkGenerator.generated.LlrpDefinition;
import org.llrp.ltkGenerator.generated.MessageDefinition;
import org.llrp.ltkGenerator.generated.ParameterDefinition;

import java.io.BufferedWriter;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileWriter;
import java.io.IOException;

import java.util.LinkedList;
import java.util.List;
import java.util.Properties;


public class CodeGenerator {
    public static Logger logger = Logger.getLogger(Class.class.getName());
    public Properties properties;
    private List<ParameterDefinition> parameters;
    private List<MessageDefinition> messages;
    private List<EnumerationDefinition> enumerations;
    private List<ChoiceDefinition> choices;
    private List<CustomParameterDefinition> customParams;
    private List<CustomMessageDefinition> customMessages;
    private Utility utility;

    /**
     * instantiate new code generate - probably want to call generate after.
     */
    public CodeGenerator(String propertiesFile) {
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

        parameters = new LinkedList<ParameterDefinition>();
        messages = new LinkedList<MessageDefinition>();
        enumerations = new LinkedList<EnumerationDefinition>();
        choices = new LinkedList<ChoiceDefinition>();
        customParams = new LinkedList<CustomParameterDefinition>();
        customMessages = new LinkedList<CustomMessageDefinition>();
        utility = new Utility(properties);
        utility.setChoices(choices);
    }

    /**
     * generate code - definitons in generator.properties.
     */
    public void generate() {
        logger.debug("start code generation");
        logger.debug("retrieve llrp definitions");

        String jaxBPackage = properties.getProperty("jaxBPackage");
        String llrpSchemaPath = properties.getProperty("llrpSchema");
        String llrpXMLPath = properties.getProperty("llrpXML");
        LlrpDefinition llrp = LLRPUnmarshaller.getLLRPDefinition(jaxBPackage,
                llrpSchemaPath, llrpXMLPath);
        logger.debug("finished retrieving llrp definitions");
        logger.debug("start filling objects");
        fillObjects(llrp);
        logger.debug("finished filling objects");
        //generateCustom must be before Parameters because it sets the allowed in values
        logger.debug("finished filling objects");
        logger.debug("start generating custom parameters");
        generateCustomParameters();
        logger.debug("finished generating custom parameters");
        // generateMessages() and generateParameters must be executed before generateEnumerations because enumeration supertypes are determined in this methods
        logger.debug("start generating messages");
        generateMessages();
        logger.debug("finished generating messages");
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
        logger.debug("finished generatins custom messages");
    }

    private void generateMessages() {
        logger.debug(messages.size() + " messages to generate");
        logger.debug("using template " +
            properties.getProperty("messageTemplate"));
        logger.debug("generating files into " +
            properties.getProperty("generatedMessagePackage"));

        for (MessageDefinition m : messages) {
            try {
                VelocityContext context = new VelocityContext();
                context.put("message", m);
                context.put("utility", utility);

                Template template = Velocity.getTemplate(properties.getProperty(
                            "messageTemplate"));
                BufferedWriter writer = new BufferedWriter(new FileWriter(properties.getProperty(
                                "generatedMessagePackage") + m.getName() +
                            properties.getProperty("fileEnding")));
                template.merge(context, writer);
                writer.flush();
                writer.close();
            } catch (ResourceNotFoundException e) {
                logger.equals("Exception while generating code: " +
                    e.getLocalizedMessage() + " caused by " + e.getCause());
            } catch (ParseErrorException e) {
                logger.equals("Exception while generating code: " +
                    e.getLocalizedMessage() + " caused by " + e.getCause());
            } catch (MethodInvocationException e) {
                logger.equals("Exception while generating code: " +
                    e.getLocalizedMessage() + " caused by " + e.getCause());
            } catch (IOException e) {
                logger.equals("Exception while generating code: " +
                    e.getLocalizedMessage() + " caused by " + e.getCause());
            } catch (Exception e) {
                logger.equals("Exception while generating code: " +
                    e.getLocalizedMessage() + " caused by " + e.getCause());
            }
        }
    }

    private void generateParameters() {
        logger.debug(parameters.size() + " parameters to generate");

        logger.debug("using template " +
            properties.getProperty("parameterTemplate"));
        logger.debug("generating files into " +
            properties.getProperty("parameterMessagePackage"));

        for (ParameterDefinition p : parameters) {
            try {
                VelocityContext context = new VelocityContext();
                context.put("parameter", p);
                context.put("choices", choices);
                context.put("utility", utility);

                Template template = Velocity.getTemplate(properties.getProperty(
                            "parameterTemplate"));
                BufferedWriter writer = new BufferedWriter(new FileWriter(properties.getProperty(
                                "generatedParameterPackage") + p.getName() +
                            properties.getProperty("fileEnding")));
                template.merge(context, writer);
                writer.flush();
                writer.close();
            } catch (ResourceNotFoundException e) {
                logger.equals("Exception while generating code: " +
                    e.getLocalizedMessage() + " caused by " + e.getCause());
            } catch (ParseErrorException e) {
                logger.equals("Exception while generating code: " +
                    e.getLocalizedMessage() + " caused by " + e.getCause());
            } catch (MethodInvocationException e) {
                logger.equals("Exception while generating code: " +
                    e.getLocalizedMessage() + " caused by " + e.getCause());
            } catch (IOException e) {
                logger.equals("Exception while generating code: " +
                    e.getLocalizedMessage() + " caused by " + e.getCause());
            } catch (Exception e) {
                logger.equals("Exception while generating code: " +
                    e.getLocalizedMessage() + " caused by " + e.getCause());
            }
        }
    }

    private void generateInterfaces() {
        logger.debug(choices.size() + " interfaces to generate");
        logger.debug("using template " +
            properties.getProperty("interfaceTemplate"));
        logger.debug("generating files into " +
            properties.getProperty("generatedInterfacePackage"));

        for (ChoiceDefinition cd : choices) {
            try {
                VelocityContext context = new VelocityContext();
                context.put("interface", cd);
                context.put("utility", utility);

                Template template = Velocity.getTemplate(properties.getProperty(
                            "interfaceTemplate"));
                BufferedWriter writer = new BufferedWriter(new FileWriter(properties.getProperty(
                                "generatedInterfacePackage") + cd.getName() +
                            properties.getProperty("fileEnding")));
                template.merge(context, writer);
                writer.flush();
                writer.close();
            } catch (ResourceNotFoundException e) {
                logger.equals("Exception while generating code: " +
                    e.getLocalizedMessage() + " caused by " + e.getCause());
            } catch (ParseErrorException e) {
                logger.equals("Exception while generating code: " +
                    e.getLocalizedMessage() + " caused by " + e.getCause());
            } catch (MethodInvocationException e) {
                logger.equals("Exception while generating code: " +
                    e.getLocalizedMessage() + " caused by " + e.getCause());
            } catch (IOException e) {
                logger.equals("Exception while generating code: " +
                    e.getLocalizedMessage() + " caused by " + e.getCause());
            } catch (Exception e) {
                logger.equals("Exception while generating code: " +
                    e.getLocalizedMessage() + " caused by " + e.getCause());
            }
        }
    }

    private void generateEnumerations() {
        logger.debug(enumerations.size() + " enumerations to generate");
        logger.debug("using template " +
            properties.getProperty("enumerationTemplate"));
        logger.debug("generating files into " +
            properties.getProperty("generatedEnumerationPackage"));

        for (EnumerationDefinition enu : enumerations) {
            try {
                VelocityContext context = new VelocityContext();
                context.put("enum", enu);
                context.put("utility", utility);

                Template template = Velocity.getTemplate(properties.getProperty(
                            "enumerationTemplate"));
                BufferedWriter writer = new BufferedWriter(new FileWriter(properties.getProperty(
                                "generatedEnumerationPackage") + enu.getName() +
                            properties.getProperty("fileEnding")));
                template.merge(context, writer);
                writer.flush();
                writer.close();
            } catch (ResourceNotFoundException e) {
                logger.equals("Exception while generating code: " +
                    e.getLocalizedMessage() + " caused by " + e.getCause());
            } catch (ParseErrorException e) {
                logger.equals("Exception while generating code: " +
                    e.getLocalizedMessage() + " caused by " + e.getCause());
            } catch (MethodInvocationException e) {
                logger.equals("Exception while generating code: " +
                    e.getLocalizedMessage() + " caused by " + e.getCause());
            } catch (IOException e) {
                logger.equals("Exception while generating code: " +
                    e.getLocalizedMessage() + " caused by " + e.getCause());
            } catch (Exception e) {
                logger.equals("Exception while generating code: " +
                    e.getLocalizedMessage() + " caused by " + e.getCause());
            }
        }
    }

    private void generateCustomParameters() {
        logger.debug(customParams.size() + " custom parameters to generate");
        logger.debug("using template " +
            properties.getProperty("customParameterTemplate"));
        logger.debug("generating files into " +
            properties.getProperty("generatedCustomParameterPackage"));

        for (CustomParameterDefinition cd : customParams) {
            try {
                VelocityContext context = new VelocityContext();
                context.put("custom", cd);
                context.put("utility", utility);

                Template template = Velocity.getTemplate(properties.getProperty(
                            "customParameterTemplate"));
                BufferedWriter writer = new BufferedWriter(new FileWriter(properties.getProperty(
                                "generatedCustomParameterPackage") +
                            cd.getName() +
                            properties.getProperty("fileEnding")));
                template.merge(context, writer);
                writer.flush();
                writer.close();
            } catch (ResourceNotFoundException e) {
                logger.equals("Exception while generating code: " +
                    e.getLocalizedMessage() + " caused by " + e.getCause());
            } catch (ParseErrorException e) {
                logger.equals("Exception while generating code: " +
                    e.getLocalizedMessage() + " caused by " + e.getCause());
            } catch (MethodInvocationException e) {
                logger.equals("Exception while generating code: " +
                    e.getLocalizedMessage() + " caused by " + e.getCause());
            } catch (IOException e) {
                logger.equals("Exception while generating code: " +
                    e.getLocalizedMessage() + " caused by " + e.getCause());
            } catch (Exception e) {
                logger.equals("Exception while generating code: " +
                    e.getLocalizedMessage() + " caused by " + e.getCause());
            }
        }
    }

    private void generateCustomMessages() {
        logger.debug(customMessages.size() + " custom messages to generate");
        logger.debug("using template " +
            properties.getProperty("customMessageTemplate"));
        logger.debug("generating files into " +
            properties.getProperty("generatedCustomMessagePackage"));

        for (CustomMessageDefinition cd : customMessages) {
            try {
                VelocityContext context = new VelocityContext();
                context.put("message", cd);
                context.put("utility", utility);

                Template template = Velocity.getTemplate(properties.getProperty(
                            "customMessageTemplate"));
                BufferedWriter writer = new BufferedWriter(new FileWriter(properties.getProperty(
                                "generatedCustomMessagePackage") +
                            cd.getName() +
                            properties.getProperty("fileEnding")));
                template.merge(context, writer);
                writer.flush();
                writer.close();
            } catch (ResourceNotFoundException e) {
                logger.equals("Exception while generating code: " +
                    e.getLocalizedMessage() + " caused by " + e.getCause());
            } catch (ParseErrorException e) {
                logger.equals("Exception while generating code: " +
                    e.getLocalizedMessage() + " caused by " + e.getCause());
            } catch (MethodInvocationException e) {
                logger.equals("Exception while generating code: " +
                    e.getLocalizedMessage() + " caused by " + e.getCause());
            } catch (IOException e) {
                logger.equals("Exception while generating code: " +
                    e.getLocalizedMessage() + " caused by " + e.getCause());
            } catch (Exception e) {
                logger.equals("Exception while generating code: " +
                    e.getLocalizedMessage() + " caused by " + e.getCause());
            }
        }
    }

    private void fillObjects(LlrpDefinition llrp) {
        List<Object> childs = llrp.getMessageDefinitionOrParameterDefinitionOrChoiceDefinition();

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
            } else {
                logger.warn("type not used: " + o.getClass() +
                    " in CodeGenerator.fillObjects");
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

        CodeGenerator cg = new CodeGenerator(propertiesFile);
        cg.generate();

        //        CodeFormatter.formatDirectory(new File(properties.getProperty("generatedBase")));;
    }
}
