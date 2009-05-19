package org.llrp.ltk.util;

import java.io.BufferedInputStream;
import java.io.InputStream;

import org.apache.log4j.Logger;
import org.apache.xerces.dom.DOMInputImpl;
import org.llrp.ltk.generated.LLRPConstants;
import org.w3c.dom.ls.LSInput;
import org.w3c.dom.ls.LSResourceResolver;


public class LLRPExternalResourceResolver implements LSResourceResolver {
	private static final Logger LOGGER = Logger.getLogger(LLRPExternalResourceResolver.class);
	public LSInput resolveResource(String type, String namespaceURI, String publicId,
			String systemId, String baseURI) {
		if (LLRPConstants.EXTERNAL_LLRP_SCHEMA_PATH.equalsIgnoreCase(systemId) && LLRPConstants.REDIRECT_EXTERNAL_RESOURCES.booleanValue()){
			LOGGER.info("redirecting resource "+systemId+" to "+LLRPConstants.LLRPMESSAGESCHEMAPATH);
			LSInput lsInput = new DOMInputImpl();
			ClassLoader cl = getClass().getClassLoader();
	        InputStream s = new BufferedInputStream(cl
					.getResourceAsStream(LLRPConstants.LLRPMESSAGESCHEMAPATH)); 
	        lsInput.setByteStream(s);
			return  lsInput;
		}
		return null;
	}
}
