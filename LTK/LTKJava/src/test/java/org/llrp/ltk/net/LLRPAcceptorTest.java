package org.llrp.ltk.net;

import org.apache.log4j.BasicConfigurator;
import org.apache.log4j.Logger;
import org.llrp.ltk.exceptions.InvalidLLRPMessageException;
import org.llrp.ltk.generated.enumerations.ConnectionAttemptStatusType;
import org.llrp.ltk.generated.messages.DELETE_ROSPEC;
import org.llrp.ltk.generated.messages.DELETE_ROSPEC_RESPONSE;
import org.llrp.ltk.generated.messages.READER_EVENT_NOTIFICATION;
import org.llrp.ltk.types.LLRPMessage;
import org.llrp.ltk.types.UnsignedInteger;

import junit.framework.TestCase;

public class LLRPAcceptorTest extends TestCase implements LLRPEndpoint {

	LLRPConnection connection; 
	LLRPMessage response;
	boolean ready = false;
	Logger logger;
	
	protected void setUp() throws Exception {
		
		BasicConfigurator.configure();
		logger = Logger.getRootLogger();
		
		connection = new LLRPAcceptor(this);
		((LLRPAcceptor) connection).setPort(25084);
		((LLRPAcceptor) connection).bind(10000);
			
	}

	protected void tearDown() {
		
		((LLRPAcceptor) connection).close();
		
		
		
	}
	
	
	public final void testTransact() throws Exception {
				
		
			
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
	
	public final void testSend() {
		
		
		try {
			DELETE_ROSPEC del = new DELETE_ROSPEC();
			del.setROSpecID(new UnsignedInteger(1));
			logger.debug(del.toXMLString());
			connection.send(del);
			
		}
		catch (Exception e) {
			e.printStackTrace();				
		}

		
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
