package org.llrp.ltk.net;

import junit.framework.TestCase;

import org.apache.log4j.BasicConfigurator;
import org.apache.log4j.Logger;
import org.llrp.ltk.exceptions.InvalidLLRPMessageException;
import org.llrp.ltk.generated.enumerations.ConnectionAttemptStatusType;
import org.llrp.ltk.generated.messages.DELETE_ROSPEC;
import org.llrp.ltk.generated.messages.DELETE_ROSPEC_RESPONSE;
import org.llrp.ltk.generated.messages.READER_EVENT_NOTIFICATION;
import org.llrp.ltk.types.LLRPMessage;
import org.llrp.ltk.types.UnsignedInteger;

public class LLRPConnectorTest extends TestCase implements LLRPEndpoint{

	LLRPConnection connection; 
	String READER_IP_ADDRESS = "10.78.0.108";
	LLRPMessage response;
	boolean ready = false;
	Logger logger;
	
	protected void setUp() throws Exception {
		
		BasicConfigurator.configure();
		logger = Logger.getRootLogger();
		
		if (System.getProperty("READER_IP_ADDRESS") != null) {
			String READER_IP_ADDRESS = System.getProperty("READER_IP_ADDRESS");
		}
		else {
			logger.warn("READER_IP_ADDRESS property not specified on command line using default value: "
			+ READER_IP_ADDRESS);
		}
		
		
		
		connection = new LLRPConnector(this, READER_IP_ADDRESS);
		((LLRPConnector) connection).connect();
				
	}

	
	protected void tearDown() {
		
		((LLRPConnector) connection).disconnect();
		
		
		
	}
	
	
	public final void testTransact() {
				
		
			
		try {
			DELETE_ROSPEC del = new DELETE_ROSPEC();
			del.setROSpecID(new UnsignedInteger(1));
			logger.debug(del.toXMLString());
			response = connection.transact(del);
			assertTrue("did not receive DELETE_ROSPEC_RESPONSE " +
					"after transact method sent DELETE_ROSPEC", (response instanceof DELETE_ROSPEC_RESPONSE));
		}
		catch (Exception e) {
			e.printStackTrace();				
		}

		
		
	}
	
	public final void testSend() throws Exception {
		
		
		try {
			DELETE_ROSPEC del = new DELETE_ROSPEC();
			del.setROSpecID(new UnsignedInteger(1));
			logger.debug(del.toXMLString());
			connection.send(del);
			
		}
		catch (Exception e) {
			e.printStackTrace();				
		}

		Thread.sleep(20000);
		
	}
	
	
	public void messageReceived(LLRPMessage message) {
		
		try {
			logger.debug("Received the following message: ");
			logger.debug(message.toXMLString());
		} catch (InvalidLLRPMessageException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	
	}
	
	public void errorOccured(String s) {
		
	}

}
