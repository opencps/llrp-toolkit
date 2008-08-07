BUILD NOTES
===========

1. First, you must copy llrp-1x0-def.xml to the LLRP directory

2. To build vendor extensions (the LLRPVendorExt project), you must copy
   your VendorExt.xml to the LLRPVendorExt directory.

3. If you don't want to build the LLRPVendorExt project, delete the line
   containing LLRPVendorExt from the LTKNet.sln file.

   Note: The namespace of the vendor extension is LLRP.xxxxxx. The dll
	 file name should be LLRP.xxxxxx.dll accordingly. xxxxxx is set
	 via the assembly name accessible from the application tab in
	 the properties window for the LLRPVendorExt project. 

4. If loading the solution file in Visual Studio, read the warning and
   choose "Load Normally."
   Note that the IDE shows several missed files. They will be generated
   once you build the project.

5. To build from the command line type

	msbuild

   The Clean and Rebuild targets also work

	msbuild /t:clean
	msbuild /t:rebuild



INTRODUCTION
============

This is a high level overview of and orientation to
the LLRP Tool Kit for .Net (LTKNet) on Windows.

The most recent versions of and news about the
LLRP Tool Kit is available on SourceForge.net

    http://sourceforge.net/projects/llrp-toolkit/

This document is organized:
    OVERVIEW 			-- a summary for an apps programmer
    COMPATIBILITY		-- compatibility information
    LTKNet KIT STRUCTURE 	-- LTKNet structure
    MESSAGE STATES 		-- message state transition diagram
    USE SOURCE CODE 		-- how to use this source code
    TODO LIST			--	


OVERVIEW
==========

    - There is a .Net class definition for each LLRP
      message and parameter type
    - Each class contains methods including FromBitArray, ToBitArray, 
      FromString(XmlNode), ToString. Thus it can be decoded/encoded by itself
    - LLRPEndPoint supports end-to-end programming mode
    - LLRPClient supports client programming mode. It uses event and 
      synchronized message/response
    - Messages from the reader are decoded into object trees. It can then be
      encoded into XML string
    - An application constructs messages as "object trees" or from a XML string
    - Applications can traverse the object trees to retrieve results and tag data
    - Message factory is used to generate message from xml string
    - Type check in IDE is enforced


PREREQUESTS
===========

The source codes are compiled and tested in Microsoft Visual Studio 2005 standard 
version.

COMPATIBILITY
=============

It requires .Net framework 2.0 or up. 

 



LTKNet KIT STRUCTURE
====================

LTKNet kit contains five major modules. 

      - LLRP is the LLRP base library. It contains definitions of all LLRP messages and parameters. 
	Interfaces, base class of custom parameters and message are defined. Vendors are encoraged 
        to derive their extension from these interfaces and base classes.
      - LLRPTest is a GUI based LLRP example and test utility. It contains ROSpec operation commands,
        ACCESSSpec operation commands, reader configuration, capability, and XML encoding/decoding.
      - TestData contains four test files.
      - LLRPVendorExt is optional vendor extension implementation. 


* represents code generated code

LTKNet-
      |- LTKNet.sln           	: Solution file
      |- README.TXT		: This file
      |- RELEASE NOTES.TXT	: Release notes
      |- LLRP 			: LLRP base libary	
      |- LLRPTest		: LLRP .Net example & test utility
      |- LLRPEndPointServerTest : LLRP xml test data
      |- LLRPVendorExt		: LLRP Vendor extension



LLRP--
      |- LLRPParam.xslt 	  : XSL transform stylesheet for parameters
      |- LLRPMsg.xslt	  	  : XSL transform stylesheet for messages
      |- LLRPEndPoint.xslt	  : XSL transform stylesheet for LLRPEndPoint
      |- LLRPClient.xslt	  : XSL transform stylesheet for parameters
      |- LLRPXmlParser.xslt       : XSL transform stylesheet for general xml parser
      |- LLRPEnum.xslt      	  : XSL transform stylesheet for enumerations
      |- templates.xslt		  : XSL transform general templates
      |- Transaction.cs		  : Simple TCPIP send and receive class
      |- TCPIPConnection.cs       : LLRP TCPIP client and Server implementation
      |- LLRPUtil.cs		  : Data type conversion
      |- LLRPDataType.cs	  : LLRP supporting data types
      |- Customparameters.cs	  : interfaces and base classes for LLRP custom parameters
      |- CommunicationInterface.cs: Communication interface base class
      |- LLRP.csproj		  : project file
      |- LLRPHelper.cs		  : Helper classes
      |- CodeGenerator.dll	  : Code generator



LLRPVendorExt-
      |- VendorExt.xml     	  : Vendor LLRP extension definitions
      |- templates.xslt		  : XSL transform general templates
      |- VendorExt.xslt	  	  : Vendor LLRP extension XSL tranform stylesheet
      |- CodeGenerator.dll	  : Code generator
      |- LLRPImpinjExt.csproj     : project file


LLRPTest
      |- MainFrm.cs		  : GUI 
      |- MainFrm.resx		  : Resource
      |- MainFrm.Designer.cs	  : GUI layout
      |- Program.cs		  : Program entry
      |- LLRPTest.csproj 	  : Project file   

LLRPEndPointServerTest
      |- MainFrm.cs		  : GUI 
      |- MainFrm.resx		  : Resource
      |- MainFrm.Designer.cs	  : GUI layout
      |- Program.cs		  : Program entry
      |- LLRPEndPointServerTest.csproj : Project file
   



LLRP MESSAGE STATES
===================


LLRP .Net library supports both xml-based or object(binary)-based programming
approaches. messages that flows from reader to application or from application 
to reader have following states.  XML state is optional in an application.

message states: from application to reader

----------      ----------      ----------
|        |      |        |      |        |
|   XML  | ---> |   Obj  | ---->|  Bin   |
|        |      |        |      |        |
----------      ----------      ----------

message states: from reader to application

----------      ----------      ----------
|        |      |        |      |        |
|  Bin   | ---> |  Obj   | ---->|  XML   |
|        |      |        |      |        |
----------      ----------      ----------



USE SOURCE CODE
===============

All projects can be loaded from LTKNet.sln solution file.

	- For end application programmer, follow the source code of LLRPTest project. (package)
	- For LLRP driver developer, follow LLRP and LLRPNetCodeGenerator.


TODO LIST
=========
