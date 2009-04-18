package org.llrp.ltk.util;

import org.apache.log4j.Logger;
import org.llrp.ltk.generated.LLRPConstants;
import org.w3c.dom.ls.LSInput;
import org.w3c.dom.ls.LSResourceResolver;

import com.sun.org.apache.xerces.internal.dom.DOMInputImpl;


public class LLRPExternalResourceResolver implements LSResourceResolver {
	private static final Logger LOGGER = Logger.getLogger(LLRPExternalResourceResolver.class);
	public LSInput resolveResource(String type, String namespaceURI, String publicId,
			String systemId, String baseURI) {
		if (LLRPConstants.EXTERNAL_LLRP_SCHEMA_PATH.equalsIgnoreCase(systemId) && LLRPConstants.REDIRECT_EXTERNAL_RESOURCES.booleanValue()){
			LOGGER.info("redirecting resource "+systemId+" to "+LLRPConstants.LLRPMESSAGESCHEMAPATH);
			return  new DOMInputImpl(publicId,LLRPConstants.LLRPMESSAGESCHEMAPATH,baseURI);
		}
		return null;
	}
}
