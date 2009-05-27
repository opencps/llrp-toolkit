

USING AND BUILDING THE LLRP Tool Kit for Java
Last updated 20 May 2009



INTRODUCTION
============

These are the instructions for using and building
the LLRP Tool Kit for Java (LTKJava).

The most recent versions of and news about the
LLRP Tool Kit is available on SourceForge.net

    http://sourceforge.net/projects/llrp-toolkit/

LTKJava was developed and is being maintained by:
Christian Floerkemeier (MIT) & Basil Gasser (ETH Zurich)


FEATURES 
==========

- codec for binary LLRP messages
- codec for LTK XML messages incl. XSD schema validation
- support for LTK structured extension approach (binary & LTK XML)
- synchronous and asynchronous messaging
- client and reader-initiated connections
- automatic acknowledgement of KEEP_ALIVE messages

CHANGELOg
==========

A changelog is available at:

http://llrp-toolkit.cvs.sourceforge.net/viewvc/llrp-toolkit/LTK/LTKJava/CHANGELOG.txt



USER GUIDE
==========

See wiki for additional examples: http://llrp-toolkit.wiki.sourceforge.net/

1. Download the most recent version of the LLRP Tool Kit for Java and add it to your classpath. 

It is available from the download section at

http://sourceforge.net/projects/llrp-toolkit/

2. Add the runtime dependencies listed below to the classpath if you are not using the
LTKJava jar with the dependencies (LTKJava_with_dep_<version>.jar)

3. Start using the library:

A. from the command line: convert binary messages to XML messages and vice versa.
at the command line type the following command for usage info:

java -jar LTKJava_with_dep_<version>.jar 

B. in a java application

instantiate an LTK Java Message object from an LTK-XML file and convert it to 
the binary representation: 

// build JDOM document from file<p>
org.jdom.Document doc = new org.jdom.input.SAXBuilder().build(new
                                 FileReader(filename));
// create LTK-Java object<p>
LLRPMessage message = LLRPMessageFactory.createLLRPMessage(doc);

// output as a binary message<p>
Byte[] output = message.encodeBinary();

// open a client initiated connection
LLRPConnection connection = new LLRPConnector(this, READER_IP_ADDRESS);
((LLRPConnector) connection).connect();
		
// send the message synchronously
LLRPMessage response = connection.transact(message);

See wiki for more detailed examples: http://llrp-toolkit.wiki.sourceforge.net/

BUILDING FROM SOURCE
=====================

The LTKJava generates the LLRP java classes from the description of the LLRP binary binding 
in llrp-def.xml and any extensions specified. A detailed description of the LTKJava approach is available in the project wiki:

http://llrp-toolkit.wiki.sourceforge.net/ 

The LTKJava can be built using ant with JDK1.5 and higher. 

1. Obtain the sources from the CVS. 

You'll need the LTKJava directory and the Definitions directory

Using anonymous CVS, obtain the LLRP Tool Kit
distribution from SourceForge.net

cvs -d:pserver:anonymous@llrp-toolkit.cvs.sourceforge.net:/cvsroot/llrp-toolkit login

[press ENTER when prompted for the password]
 
cvs -z3 -d:pserver:anonymous@llrp-toolkit.cvs.sourceforge.net:/cvsroot/llrp-toolkit co -P LTK


2. Obtain the compile and runtime dependencies listed in the table below and store them in a directory LTKJava/lib

3. To build the jar files, change into LTKJava's root directory and run "ant package". The result will be in the "target" subdirectory.

To build the Javadocs, run "ant javadoc". The result will be in "target/docs/apidocs". 


DEPENDENCIES
=====================

RUNTIME DEPENDENCIES

Name		Version		URL		

jdom		1.0			http://www.jdom.org	
log4j		1.2.15		http://logging.apache.org/log4j/1.2/download.html
jargs 		1.0			http://sourceforge.net/project/showfiles.php?group_id=33024
mina 		1.1.7 		http://mina.apache.org
slf4j-log4j12 1.5.0 		http://www.slf4j.org
slf4j-api 1.5.0 		http://www.slf4j.org
xerces-j  2.4.0			http://xerces.apache.org/


COMPILE DEPENDENCIES

Name		Version			URL


velocity-dep 		1.5 	http://velocity.apache.org
jalopy 				0.3.1 	http://jalopy.sourceforge.net
jdom 				1.0 	http://www.jdom.org
log4j 				1.2.15 	http://logging.apache.org/log4j/1.2/download.html
commons-collection 	3.2		http://commons.apache.org
commons-configuration 1.5 	http://commons.apache.org
commons-lang 		2.3 	http://commons.apache.org
common-logging 		1.1.1   http://commons.apache.org
JAXB RI dependencies including:
jaxb-xjc 			2.0 	https://jaxb.dev.java.net/
jaxb-impl 			2.0 	https://jaxb.dev.java.net/
jaxb-api 			2.0 	https://jaxb.dev.java.net/
activation 			2.0 	https://jaxb.dev.java.net/
jsr173_1.0_api 		2.0 	https://jaxb.dev.java.net/
The five above jaxb dependencies are available in a single jar "JAXB RI" from https://jaxb.dev.java.net/. Execute this jar (doubleclick windows, "java -jar <name>" all other platforms) and copy the individual jars to the LTKJava/lib directory)
plus above runtime dependencies

TEST DEPENDENCIES 

Name		Version		URL

JUnit		4.4			http://sourceforge.net/project/showfiles.php?group_id=15278
plus above dependencies
note that the junit dependency needs to be placed in the ANT_HOME/lib directory

FOR FURTHER INFORMATION OR TO GET HELP
======================================

Visit the LLRP Tool Kit project on SourceForge.net and use the information in the wiki

http://llrp-toolkit.wiki.sourceforge.net/ 

Visit the mailing list archive:

http://sourceforge.net/mailarchive/forum.php?forum_name=llrp-toolkit-devel

Send an email to the LTK mailing list

llrp-toolkit-devel@lists.sourceforge.net
