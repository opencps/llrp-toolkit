package org.llrp.ltk.net;

import junit.framework.TestCase;

import org.apache.log4j.BasicConfigurator;
import org.apache.log4j.Logger;
import org.llrp.ltk.exceptions.InvalidLLRPMessageException;
import org.llrp.ltk.generated.enumerations.KeepaliveTriggerType;
import org.llrp.ltk.generated.messages.KEEPALIVE;
import org.llrp.ltk.generated.messages.SET_READER_CONFIG;
import org.llrp.ltk.generated.parameters.KeepaliveSpec;
import org.llrp.ltk.types.Bit;
import org.llrp.ltk.types.LLRPMessage;
import org.llrp.ltk.types.UnsignedInteger;

public class LLRPIoHandlerAdapterImplTest extends TestCase implements LLRPEndpoint {

	LLRPConnection connection; 
	String READER_IP_ADDRESS = "127.0.0.1";
	LLRPMessage response;
	Logger logger;
	int keep_alives_received = 0;
	
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
				
	}
	
	
	
public void testSetKeepAliveForward() throws Exception {
	
	LLRPIoHandlerAdapter handler = new LLRPIoHandlerAdapterImpl();
	handler.setKeepAliveForward(true);
	connection = new LLRPConnector(this, READER_IP_ADDRESS,handler);
	handler.setConnection(connection);
	((LLRPConnector) connection).connect();
	
	SET_READER_CONFIG m = new SET_READER_CONFIG();
	KeepaliveSpec s = new KeepaliveSpec();
	s.setKeepaliveTriggerType(new KeepaliveTriggerType(KeepaliveTriggerType.Periodic));
	s.setPeriodicTriggerValue(new UnsignedInteger(1000));
	m.setKeepaliveSpec(s);
	m.setResetToFactoryDefault(new Bit(0));
	
	LLRPMessage r = connection.transact(m); 
	
	
	Thread.sleep(10000);
	
	assertTrue("KEEP_ALIVE MESSAGES were not forwarded", keep_alives_received > 0);
	
/*
		connection = new LLRPConnector(this, READER_IP_ADDRESS);
		LLRPIoHandlerAdapter handler = new LLRPIoHandlerAdapterImpl(connection);
		handler.setKeepAliveForward(true);
		connection.setHandler(handler);
		*/
		
	((LLRPConnector) connection).disconnect();
	
	keep_alives_received = 0;
	handler.setKeepAliveForward(false);
	connection = new LLRPConnector(this, READER_IP_ADDRESS,handler);
	handler.setConnection(connection);
	((LLRPConnector) connection).connect();
	
	r = connection.transact(m); 
	
	Thread.sleep(10000);
	
	assertTrue("KEEP_ALIVE MESSAGES were forwarded by mistake", keep_alives_received == 0);
	
/*
		connection = new LLRPConnector(this, READER_IP_ADDRESS);
		LLRPIoHandlerAdapter handler = new LLRPIoHandlerAdapterImpl(connection);
		handler.setKeepAliveForward(true);
		connection.setHandler(handler);
		*/
		
	((LLRPConnector) connection).disconnect();
	
		
		
	}

public void messageReceived(LLRPMessage message) {
	

	if(message instanceof KEEPALIVE){
		keep_alives_received++;
	}
	
}

public void errorOccured(String s) {
	
}


}
