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
		((LLRPAcceptor) connection).bind();
		
		// Wait for reader initated connection and reader event notification
		Thread.sleep(20000);
		
	}

	public final void testTransact() {
				
		if (ready == true) {
			
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
		else {
			logger.error("Connection to reader unsuccessful");
			throw new RuntimeException("Connection to reader unsuccessful");
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
		
		// check whether received message is of type READER_EVENT_NOTIFICATION
		// and whether connection attempt was successful
		if (message instanceof READER_EVENT_NOTIFICATION) {
			READER_EVENT_NOTIFICATION m = (READER_EVENT_NOTIFICATION) message;
			logger.debug("ConnectioAttempt Status: " + m.getReaderEventNotificationData()
					.getConnectionAttemptEvent().getStatus());
			if (m.getReaderEventNotificationData()
					.getConnectionAttemptEvent().getStatus()
					.intValue() == (ConnectionAttemptStatusType.Success)) {
				ready = true;
				logger.debug("Setting READY flag to " + ready);
				
			}
			else {
				logger.error("Connection to reader unsuccessful");
				throw new RuntimeException("Connection to reader unsuccessful");
			}
		}
		else {
			logger.error("Expected READER_EVENT_NOTIFICATION but received " + message.getName());
			throw new RuntimeException("Expected READER_EVENT_NOTIFICATION but received " + message.getName());
			
		}
		
		
		
		
		
		
		
	}
	
	public void errorOccured(String s) {
		
	}


}
