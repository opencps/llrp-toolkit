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
    public static LlrpDefinition getLLRPDefinition(String jaxBPackage, String xmlPaths) {
        Object o = null;
        LlrpDefinition def = null;

        try {
            LOGGER.debug("set JaxBContext to " + jaxBPackage);

            JAXBContext context = JAXBContext.newInstance(jaxBPackage);
            LOGGER.debug("create Unmarshaller ");

            Unmarshaller unmarshaller = context.createUnmarshaller();
            SchemaFactory sf = SchemaFactory.newInstance(javax.xml.XMLConstants.W3C_XML_SCHEMA_NS_URI);
//            Schema schema = sf.newSchema(new File(llrpSchemaPath));
            LOGGER.debug("set schema file: " + xmlPaths);
//            unmarshaller.setSchema(schema);
            //o = unmarshaller.unmarshal(new File(llrpXMLPath));
            o = unmarshaller.unmarshal(createOne(xmlPaths));

            if (o instanceof LlrpDefinition) {
                def = (LlrpDefinition) o;
            } else {
                def = ((JAXBElement<LlrpDefinition>) o).getValue();
            }
        } catch (JAXBException e) {
            LOGGER.warn("exception caught: "+e.getMessage());
            e.printStackTrace();
        } 

        return def;
    }

    @SuppressWarnings("unchecked")
    public static InputStream createOne(String xmlPaths) {
        LOGGER.debug("combine llrp xml with vendor extensions");
        LOGGER.debug("paths are "+xmlPaths);
         try {
            // ---- Read XML file ----
            SAXBuilder builder = new SAXBuilder();
            LOGGER.debug("read llrp XML into Sax Document ");
            LOGGER.debug("llrp xml is " + xmlPaths);

            Document doc = null;

            // if string is not empty or holding semicolon only
            if (xmlPaths.length() > 1) {
                String[] paths = xmlPaths.split(";");
                // ---- Modify XML data ----
            	LOGGER.debug("reading file "+paths[0]);
                doc = builder.build(paths[0]);
                // start at 1 as we already read the first
                for (int i = 1; i < paths.length; i++) {
                    LOGGER.debug("add vendor extension " + paths[i]);

                    Document temp = builder.build(paths[i]);
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
