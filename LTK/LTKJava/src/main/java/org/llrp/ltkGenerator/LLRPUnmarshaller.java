package org.llrp.ltkGenerator;

import org.apache.log4j.Logger;

import org.jdom.Document;
import org.jdom.Element;
import org.jdom.JDOMException;

import org.jdom.input.SAXBuilder;

import org.jdom.output.XMLOutputter;

import org.llrp.ltkGenerator.generated.LlrpDefinition;

import org.xml.sax.SAXException;

import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;

import java.util.LinkedList;
import java.util.List;
import java.util.Properties;

import javax.xml.bind.JAXBContext;
import javax.xml.bind.JAXBElement;
import javax.xml.bind.JAXBException;
import javax.xml.bind.Unmarshaller;
import javax.xml.validation.Schema;
import javax.xml.validation.SchemaFactory;


public class LLRPUnmarshaller {
    private static final String JAXP_SCHEMA_LANGUAGE = "http://java.sun.com/xml/jaxp/properties/schemaLanguage";
    private static final String JAXP_SCHEMA_LOCATION = "http://java.sun.com/xml/jaxp/properties/schemaSource";
    private static final String W3C_XML_SCHEMA = "http://www.w3.org/2001/XMLSchema";
    private static final Logger LOGGER = Logger.getLogger(LLRPUnmarshaller.class);

    @SuppressWarnings("unchecked")
    public static LlrpDefinition getLLRPDefinition(String jaxBPackage,
        String llrpSchemaPath, String llrpXMLPath, String extensionsPath) {
        Object o = null;
        LlrpDefinition def = null;

        try {
            LOGGER.debug("set JaxBContext to " + jaxBPackage);

            JAXBContext context = JAXBContext.newInstance(jaxBPackage);
            LOGGER.debug("create Unmarshaller ");

            Unmarshaller unmarshaller = context.createUnmarshaller();
            SchemaFactory sf = SchemaFactory.newInstance(javax.xml.XMLConstants.W3C_XML_SCHEMA_NS_URI);
            Schema schema = sf.newSchema(new File(llrpSchemaPath));
            LOGGER.debug("set schema file: " + llrpSchemaPath);
            unmarshaller.setSchema(schema);
            //o = unmarshaller.unmarshal(new File(llrpXMLPath));
            o = unmarshaller.unmarshal(createOne(llrpXMLPath, extensionsPath));

            if (o instanceof LlrpDefinition) {
                def = (LlrpDefinition) o;
            } else {
                def = ((JAXBElement<LlrpDefinition>) o).getValue();
            }
        } catch (JAXBException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        } catch (SAXException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }

        return def;
    }

    @SuppressWarnings("unchecked")
    public static InputStream createOne(String llrpXMLPath,
        String extensionsPath) {
        LOGGER.debug("combine llrp xml with vendor extensions");

        try {
            // ---- Read XML file ----
            SAXBuilder builder = new SAXBuilder();
            LOGGER.debug("read llrp XML into Sax Document ");
            LOGGER.debug("llrp xml is " + llrpXMLPath);

            Document doc = builder.build(llrpXMLPath);

            // if string is not empty or holding semicolon only
            if (extensionsPath.length() > 1) {
                // ---- Modify XML data ----
                String[] extensions = extensionsPath.split(";");

                for (int i = 0; i < extensions.length; i++) {
                    LOGGER.debug("add vendor extension " + extensions[i]);

                    Document temp = builder.build(extensions[i]);
                    List<Element> children = new LinkedList<Element>(temp.getRootElement()
                                                                         .getChildren());

                    for (Element child : children) {
                        temp.getRootElement().removeContent(child);
                        doc.getRootElement().addContent(child);
                    }
                }
            }

            XMLOutputter output = new XMLOutputter();
            ByteArrayOutputStream stream = new ByteArrayOutputStream();
            output.output(doc, stream);
            LOGGER.debug("finished combining xml - writing to stream");

            byte[] a = stream.toByteArray();
            InputStream is = new ByteArrayInputStream(a);

            return is;
        } catch (JDOMException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        } catch (IOException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }

        byte[] b = new byte[0];

        return new ByteArrayInputStream(b);
    }
}
