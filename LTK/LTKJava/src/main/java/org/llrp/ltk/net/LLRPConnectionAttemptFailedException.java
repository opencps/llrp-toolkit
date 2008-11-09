package org.llrp.ltk.net;

/**
	 * LLRPConnectionAttemptFailedException is thrown whenever a connection with the reader could not be established
 */

public class LLRPConnectionAttemptFailedException extends Exception{
	  public LLRPConnectionAttemptFailedException(final String message) {
	        super(message);
	    }
}
