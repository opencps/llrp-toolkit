package org.llrp.ltkGenerator;

import java.util.Collection;
import java.util.Date;
import java.util.HashMap;
import java.util.HashSet;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.Set;

import org.apache.commons.configuration.PropertiesConfiguration;
import org.apache.log4j.Logger;
import org.apache.xerces.dom.ElementNSImpl;
import org.llrp.ltkGenerator.generated.Annotation;
import org.llrp.ltkGenerator.generated.ChoiceDefinition;
import org.llrp.ltkGenerator.generated.ChoiceParameterReference;
import org.llrp.ltkGenerator.generated.CustomChoiceDefinition;
import org.llrp.ltkGenerator.generated.CustomMessageDefinition;
import org.llrp.ltkGenerator.generated.CustomParameterDefinition;
import org.llrp.ltkGenerator.generated.Description;
import org.llrp.ltkGenerator.generated.Documentation;
import org.llrp.ltkGenerator.generated.MessageDefinition;
import org.llrp.ltkGenerator.generated.ParameterDefinition;

public class Utility {
	private Map<String, String> superTypes;
	private List<ChoiceDefinition> choices;
	private List<CustomChoiceDefinition> customChoices;
	private Set<String> imports;
	private Map<String, List<String>> allowedIn;
	private Logger logger = CodeGenerator.logger;
	private PropertiesConfiguration properties;
	private int numberOfReserved;
	private Map<String, String> customEnumerationsMap;
	private Map<String, String> customChoicesMap;
	private Map<String, String> customParameterMap;
	private Map<String, String> customMessageMap;
	private Map<String, String> prefixForParameterMap;
	private Map<String, List<String>> choicesToImplementMap;
	private Map<String, List<String>> additionalSubTypesMap;
	private static final String NOTYPE = "NoType";
	private static final String HREF = "href";
	private static final String REFERENCE_LINK = "@link ";
	private static final String REFERENCE_TEXT = "See also ";

	public Utility(PropertiesConfiguration properties) {
		numberOfReserved = 0;
		superTypes = new HashMap<String, String>();
		imports = new HashSet<String>();
		allowedIn = new HashMap<String, List<String>>();
		this.properties = properties;
		customEnumerationsMap = new HashMap<String, String>();
		prefixForParameterMap = new HashMap<String, String>();
		customChoicesMap = new HashMap<String, String>();
		customParameterMap = new HashMap<String, String>();
		customMessageMap = new HashMap<String, String>();
		choicesToImplementMap = new HashMap<String, List<String>>();
		additionalSubTypesMap = new HashMap<String, List<String>>();
	}

	/**
	 * check if object o has type s
	 * 
	 * @param o
	 *            to check for type s
	 * @param s
	 *            name of type to be checked fo
	 * @return
	 */
	public boolean hasType(Object o, String s) {
		boolean b = false;

		try {
			Class sup = Class.forName(properties.getProperty("jaxBPackage")
					+ "." + s);
			Class cls = o.getClass();
			b = cls.equals(sup);
		} catch (Throwable e) {
			logger.warn("exception in utility.hasType:  "
					+ e.getLocalizedMessage());
		}

		return b;
	}

	/**
	 * match between types used in xml file and llrp types
	 * 
	 * @param xmlType
	 *            to convert
	 * @return
	 */
	public String convertType(String xmlType) {
		return convertType(xmlType, false);
	}

	/**
	 * match between types used in xml file and llrp types - specify if it is an
	 * enumeration
	 * 
	 * @param xmlType
	 *            found in xml
	 * @param isEnum
	 *            indicate if type is enumeration
	 * @return
	 */
	public String convertType(String xmlType, boolean isEnum) {
		// two bit enumerations must be handled seperately
		logger.debug("Utility.convertType: get llrp type for " + xmlType);
		if (xmlType == null) {
			return "null";
		}
		if (isEnum) {
			if (xmlType.equalsIgnoreCase("u2")) {
				return "TwoBitEnumeration";
			}
		}

		if (xmlType.equalsIgnoreCase("u1")) {
			return "Bit";
		}

		if (xmlType.equalsIgnoreCase("u2")) {
			return "TwoBitField";
		}

		if (xmlType.equalsIgnoreCase("u1v")) {
			return "BitArray";
		}

		if (xmlType.equalsIgnoreCase("u8")) {
			return "UnsignedByte";
		}

		if (xmlType.equalsIgnoreCase("s8")) {
			return "SignedByte";
		}

		if (xmlType.equalsIgnoreCase("u8v")) {
			return "UnsignedByteArray";
		}

		if (xmlType.equalsIgnoreCase("s8v")) {
			return "SignedByteArray";
		}

		if (xmlType.equalsIgnoreCase("utf8v")) {
			return "UTF8String";
		}

		if (xmlType.equalsIgnoreCase("u16")) {
			return "UnsignedShort";
		}

		if (xmlType.equalsIgnoreCase("s16")) {
			return "SignedShort";
		}

		if (xmlType.equalsIgnoreCase("u16v")) {
			return "UnsignedShortArray";
		}

		if (xmlType.equalsIgnoreCase("s16v")) {
			return "SignedShortArray";
		}

		if (xmlType.equalsIgnoreCase("u32")) {
			return "UnsignedInteger";
		}

		if (xmlType.equalsIgnoreCase("s32")) {
			return "SignedInteger";
		}

		if (xmlType.equalsIgnoreCase("u32v")) {
			return "UnsignedIntegerArray";
		}

		if (xmlType.equalsIgnoreCase("s32v")) {
			return "SignedIntegerArray";
		}

		if (xmlType.equalsIgnoreCase("u64")) {
			return "UnsignedLong";
		}

		if (xmlType.equalsIgnoreCase("s64")) {
			return "SignedLong";
		}

		if (xmlType.equalsIgnoreCase("u64v")) {
			return "UnsignedLongArray";
		}

		if (xmlType.equalsIgnoreCase("s64v")) {
			return "SignedLongArray";
		}

		if (xmlType.equalsIgnoreCase("u96")) {
			return "Integer96";
		}

		if (xmlType.equalsIgnoreCase("bytesToEnd")) {
			return "BytesToEnd";
		}

		logger.debug("Utility.convertType: No type found for " + xmlType);

		return NOTYPE;
	}

	public String formatType(String baseType, String format) {
		return convertType(baseType, false) + "_" + format;
	}

	public String getBaseType(String arrayType) {
		return arrayType.replace("Array", "");
	}

	public boolean isArray(String type) {
		if (type == null) {
			return false;
		}

		return type.toLowerCase().contains("array")
				|| type.toLowerCase().contains("utf8string");
	}

	public boolean isByteToEnd(String type) {
		if (type == null) {
			return false;
		}
		return type.toLowerCase().contains("bytestoend");
	}

	/**
	 * get base type of an array
	 * 
	 * @param arrayType
	 *            name
	 * @return
	 */
	public String arrayBaseType(String arrayType) {
		return arrayType.replace("Array", "");
	}

	public List<String> getSubTypes(String type) {
		List<String> result = new LinkedList<String>();
		for (ChoiceDefinition cd : choices) {
			if (cd.getName().equalsIgnoreCase(type)) {
				for (ChoiceParameterReference ref : cd.getParameter()) {
					result.add(ref.getType());
				}
			}
		}

		for (CustomChoiceDefinition cd : customChoices) {
			if (cd.getName().equalsIgnoreCase(type)) {
				for (ChoiceParameterReference ref : cd.getParameter()) {
					result.add(ref.getType());
				}
			}
		}
		List<String> additionalSubTypes = additionalSubTypesMap.get(type);
		if (additionalSubTypes != null){
			result.addAll(additionalSubTypes);
		}
		String subs = "";
		for (String s : result) {
			subs = subs + s + ", ";
		}

		logger.debug(type + " has following subtypes: " + subs);
		return result;
	}

	public List<String> getInterfaces(String type) {
		List<String> result = new LinkedList<String>();

		for (ChoiceDefinition cd : choices) {
			for (ChoiceParameterReference ref : cd.getParameter()) {
				if (ref.getType().equalsIgnoreCase(type)) {
					result.add(cd.getName());
				}
			}
		}
		for (CustomChoiceDefinition cd : customChoices) {
			for (ChoiceParameterReference ref : cd.getParameter()) {
				if (ref.getType().equalsIgnoreCase(type)) {
					result.add(cd.getName());
				}
			}
		}
		List<String> allowedInChoices = choicesToImplementMap.get(type);
		if (allowedInChoices != null){
			result.addAll(allowedInChoices);
		}
		return result;
	}

	public void clearImports() {
		imports = new HashSet<String>();
	}

	public void addImport(String i) {
		imports.add(i);
	}

	public boolean hasImport(String i) {
		boolean res = imports.contains(i);

		return res;
	}

	public String firstToLowerCase(String s) {
		return s.substring(0, 1).toLowerCase() + s.substring(1);
	}

	public String firstToUpperCase(String s) {
		return s.substring(0, 1).toUpperCase() + s.substring(1);
	}

	public String getSuperType(String sub) {
		String sup = superTypes.get(sub.toLowerCase());

		if (sup == null) {
			logger.debug("Utility.getSuperType: nothing found for " + sub);

			return "";
		}

		return sup;
	}

	public void setSuperType(String sub, String sup) {
		// use the converted types
		String s = convertType(sup, true);

		if (!s.equalsIgnoreCase(NOTYPE)) {
			// if it already contains the key but a different value, something
			// is not ok!
			if (superTypes.containsKey(sub.toLowerCase())
					&& !superTypes.get(sub.toLowerCase()).equalsIgnoreCase(s)) {
				throw new RuntimeException(
						"Invalid definition in LLRPdef.xml or extension: trying to set "
								+ s + " as super type for " + sub
								+ " but super type is already set to "
								+ superTypes.get(sub.toLowerCase()));
			}
			superTypes.put(sub.toLowerCase(), s);
			logger.debug("Utility.setSupertype: set " + s + " for "
					+ sub.toLowerCase());
		} else {
			if (superTypes.containsKey(sub.toLowerCase())
					&& !superTypes.get(sub.toLowerCase()).equalsIgnoreCase(sup)) {
				throw new RuntimeException(
						"Invalid definition in LLRPdef.xml or extension: trying to set "
								+ sup + " as super type for "
								+ sub.toLowerCase()
								+ " but super type is already set to "
								+ superTypes.get(sub.toLowerCase()));
			}
			superTypes.put(sub.toLowerCase(), sup);
			logger.debug("Utility.setSupertype: set " + sup + " for "
					+ sub.toLowerCase());
		}
	}

	public boolean isCustom(String type) {
		return type.equalsIgnoreCase("custom");
	}

	/**
	 * return a list of custom parameters that are allowed for this parameter
	 * 
	 * @param type
	 *            for which to get custom parameters
	 * @return
	 */
	public List<String> allowedCustom(String type) {
		List<String> result = allowedIn.get(type);

		if (result == null) {
			result = new LinkedList<String>();
		}

		return result;
	}

	public int getLength(Collection<Object> l) {
		return l.size();
	}

	public boolean isEmptyCollection(Collection<Object> c) {
		return c.isEmpty();
	}

	/**
	 * store in which parameter custom is allowed in.
	 * Relates to a <allowedIn type="<Parameter>" element in Custom definition
	 * @param parameter
	 * @param custom
	 */
	public void addAllowedIn(String parameter, String custom) {
		if (isChoice(parameter) || isCustomChoice(parameter)){
			// parameter is actually a choice so we have to remember so custom can implement parameter
			List<String> list = choicesToImplementMap.get(custom);
			if (list == null){
				list = new LinkedList<String>();
				choicesToImplementMap.put(custom,list);
			}
			list.add(parameter);
			// remember reverse mapping so when retrieving subTypes this can also easily be done
			List<String> subTypesList = additionalSubTypesMap.get(parameter);
			if (subTypesList == null){
				subTypesList = new LinkedList<String>();
				additionalSubTypesMap.put(parameter,subTypesList);
			}			
			subTypesList.add(custom);

		}
		// add custom to allowed in parameter
		List<String> result = allowedIn.get(parameter);		
		if (result == null) {
			result = new LinkedList<String>();
			allowedIn.put(parameter, result);
		}
		result.add(custom);		
	}

	public List<ChoiceDefinition> getChoices() {
		return choices;
	}

	public List<CustomChoiceDefinition> getCustomChoices() {
		return customChoices;
	}

	public void setChoices(List<ChoiceDefinition> choices) {
		this.choices = choices;
	}

	public void setCustomChoices(List<CustomChoiceDefinition> choices) {
		this.customChoices = choices;
	}

	public Map<String, List<String>> getAllowedIn() {
		return allowedIn;
	}

	public void setAllowedIn(Map<String, List<String>> allowedIn) {
		this.allowedIn = allowedIn;
	}

	public String getDateNTime() {
		Date date = new Date();

		return date.toString();
	}

	public String getAnnotation(Object o) {
		String doc = "";

		if (o instanceof MessageDefinition) {
			MessageDefinition m = (MessageDefinition) o;
			if (!m.getAnnotation().isEmpty()) {
				Annotation a = m.getAnnotation().get(0);
				List<Object> list = a.getDocumentationOrDescription();
				doc = extractAnnotation(list);
			}
		}
		if (o instanceof ParameterDefinition) {
			ParameterDefinition m = (ParameterDefinition) o;
			if (!m.getAnnotation().isEmpty()) {
				Annotation a = m.getAnnotation().get(0);
				List<Object> list = a.getDocumentationOrDescription();
				doc = extractAnnotation(list);
			}
		}
		if (o instanceof CustomMessageDefinition) {
			CustomMessageDefinition m = (CustomMessageDefinition) o;
			if (!m.getAnnotation().isEmpty()) {
				Annotation a = m.getAnnotation().get(0);
				List<Object> list = a.getDocumentationOrDescription();
				doc = extractAnnotation(list);
			}
		}

		if (o instanceof CustomParameterDefinition) {
			CustomParameterDefinition m = (CustomParameterDefinition) o;
			if (!m.getAnnotation().isEmpty()) {
				Annotation a = m.getAnnotation().get(0);
				List<Object> list = a.getDocumentationOrDescription();
				doc = extractAnnotation(list);
			}	
		}
		if (doc.equals("")) {
			return "no documentation found";
		} else {
			return doc;
		}
	}

	private String extractAnnotation(List<Object> list) {
		String description  = "";
		String documentation = "";
		for (Object ob : list) {

			if (ob instanceof Description) {
				Description descOb = (Description) ob;
				for (Object ox : descOb.getContent()) {
					// allowed are only Element or String
					if (ox instanceof ElementNSImpl) {
						ElementNSImpl el = (ElementNSImpl) ox;
						description += el.getTextContent();
					} else {
						description += ox.toString();
					}
				}
				description += '\n';
			}
			if (ob instanceof Documentation) {
				Documentation descOb = (Documentation) ob;
				String temp_doc = "";
				boolean first = true;
				for (Object ox : descOb.getContent()) {
					// allowed are only Element or String
					if (ox instanceof ElementNSImpl) {
						if (!first){
							temp_doc += " and ";
						} else {
							first = false;
						}
						ElementNSImpl el = (ElementNSImpl) ox;
						String uri = el.getAttribute(HREF);
						String linkText = el.getChildNodes().item(0).getTextContent().replace('\n', ' ');
						temp_doc += "{"+REFERENCE_LINK +"<a href=\""+uri+"\">"+linkText+"</a>}"+'\n';
					} else {
						temp_doc += ox.toString();
					}
				}
				if (!temp_doc.equals("")){
					documentation += REFERENCE_TEXT;
					documentation += temp_doc;
				}
			}
		}
		return description+documentation;
	}

	/**
	 * helper method to store number of reserved fields. Increases after each
	 * call automatically
	 * 
	 * @return number of reserved definitions found to moment of calling
	 */
	public int getCurrentNumerOfReserved() {
		return numberOfReserved++;
	}

	/**
	 * reset the counter of number of reserved definitions to 0
	 */
	public void clearNumberOfReserved() {
		numberOfReserved = 0;
	}

	public boolean isCustomEnumeration(String name) {
		return customEnumerationsMap.containsKey(name.toLowerCase());
	}

	public void addCustomEnumeration(String name) {
		customEnumerationsMap.put(name.toLowerCase(), name.toLowerCase());
	}

	public void addCustomChoice(String name) {
		customChoicesMap.put(name.toLowerCase(), name.toLowerCase());
	}

	public boolean isCustomChoice(String name) {
		if (name == null) {
			return false;
		}
		return customChoicesMap.containsKey(name.toLowerCase());
	}

	public boolean isChoice(String name) {
		if (name == null) {
			return false;
		}
		for(ChoiceDefinition c : choices){
			if (c.getName().equalsIgnoreCase(name)) return true;
		}
		return choices.contains(name);
	}
	
	public void addCustomMessage(String name) {
		customMessageMap.put(name.toLowerCase(), name.toLowerCase());
	}

	public void addCustomParameter(String name) {
		customParameterMap.put(name.toLowerCase(), name.toLowerCase());
	}

	public boolean isCustomParameter(String name) {
		return customParameterMap.containsKey(name.toLowerCase());
	}

	public boolean isCustomMessage(String name) {
		return customMessageMap.containsKey(name.toLowerCase());
	}

	public void addPrefixForParameter(String parameter, String ns) {
		prefixForParameterMap.put(parameter, ns);
	}

	public String getPrefixForParameter(String parameter) {
		if (prefixForParameterMap.containsKey(parameter)) {
			return prefixForParameterMap.get(parameter);
		} else {
			return "";
		}
	}
}
