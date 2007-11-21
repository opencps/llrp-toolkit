

USING AND BUILDING THE LLRP Tool Kit for Java
Last updated 5 Oct 2007



INTRODUCTION
============

These are the instructions for using and building
the LLRP Tool Kit for Java (LTKJava).

The most recent versions of and news about the
LLRP Tool Kit is available on SourceForge.net

    http://sourceforge.net/projects/llrp-toolkit/




USER GUIDE
==========

1. Download the most recent version of the LLRP Tool Kit for Java and add it to your classpath. 
Daily build are currently available from 

	http://people.inf.ethz.ch/floerkem/ltk/

2. Add the runtime dependencies listed below to the classpath.

3. Check out one of the examples that will be available in the near future from the project wiki and use the javadoc


BUILDING FROM SOURCE
=====================

The LTKJava generates the LLRP java classes from the description of the LLRP binary binding 
in llrp-def.xml and possible extensions. A detailed description of the LLRPJava approach is available in the project wiki:

http://llrp-toolkit.wiki.sourceforge.net/ 

The LTKJava can be built ant with JDK1.5 and higher. (We plan to add maven support in the near future 
to facilitate the dependency management).

1. Obtain the sources from the CVS. 

You'll need the LTKJava directory and the Definitions directory

Using anonymous CVS, obtain the LLRP Tool Kit
distribution from SourceForge.net

cvs -d:pserver:anonymous@llrp-toolkit.cvs.sourceforge.net:/cvsroot/llrp-toolkit login

[press ENTER when prompted for the password]
 
cvs -z3 -d:pserver:anonymous@llrp-toolkit.cvs.sourceforge.net:/cvsroot/llrp-toolkit co -P LTK


2. Obtain the compile dependencies listed in the table below and store them in a directory LTKJava/lib

3. To build a jar file, change into LTKJava's root directory and run "ant jar". The result will be in the "target" subdirectory.

To build the Javadocs, run "ant javadoc". The result will be in "target/docs/apidocs". 


DEPENDENCIES
=====================

RUNTIME DEPENDENCIES

Name		Version		URL		

jdom		1.0			http://www.jdom.org	
log4j		1.2.15		http://logging.apache.org/log4j/1.2/download.html
jargs 		1.0			http://sourceforge.net/project/showfiles.php?group_id=33024

COMPILE DEPENDENCIES

Name		Version		URL

velocity-dep	1.5		http://velocity.apache.org	
jaxb-xjc	2.0			https://jaxb.dev.java.net/
jaxb-impl	2.0			https://jaxb.dev.java.net/
jaxb-api	2.0			https://jaxb.dev.java.net/
activation	2.0			https://jaxb.dev.java.net/
jsr173_1.0_api	2.0		https://jaxb.dev.java.net/
jalopy		0.3.1		http://jalopy.sourceforge.net
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
