//   Copyright (c) 2007 by the Board of Trustees of the University of Arkansas.
//
//   Licensed under the Apache License, Version 2.0 (the "License");
//   you may not use this file except in compliance with the License.
//   You may obtain a copy of the License at
//
//       http://www.apache.org/licenses/LICENSE-2.0
//
//   Unless required by applicable law or agreed to in writing, software
//   distributed under the License is distributed on an "AS IS" BASIS,
//   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied, 
//   including, without limitation, any warranties or conditions of TITLE, 
//   NON-INFRINGEMENT, MERCHANTABILITY, or FITNESS FOR A PARTICULAR PURPOSE.
//   See the License for the specific language governing permissions and
//   limitations under the License.


/*
 * codegen.java
 *
 * This code parses the llrp.xml file and auto-generates java class files.
 * The design doc is in docs/CodeGeneratorDesign.doc.
 */
import java.io.*;
import java.util.ArrayList;

// For XMLInterpreter
import org.xml.sax.*;
import org.xml.sax.helpers.*;
import javax.xml.parsers.*;
import java.sql.*;
import java.util.*;

class XMLInterpreter extends DefaultHandler {
    private Stack m_stack;
    private boolean m_isStackReadyForText;
    private Locator m_locator;
    private String m_savedFileName;
    
    public XMLInterpreter() {
        m_stack = new Stack();
        m_isStackReadyForText = false;
    }
    
    public void parse(String filename)
    {
	File file = null;
	InputSource src = null;
	try
	{
	    file = new File(filename); 
            src = new InputSource(new FileInputStream(file));
	}
	catch(Exception e)
	{
	    System.out.println("File Open Exception: " + e);
	    System.exit(-1);
	}

        try
        {
            //System.out.println("Parsing file \"" + filename + "\"");
            //System.out.println("user.dir = " + System.getProperty("user.dir"));
            //File file = new File(filename);
            //InputSource src = new InputSource(new FileInputStream(file));
            //SaxDBUnmarshaller dbUM = new SaxDBUnmarshaller();
            SAXParserFactory factory = SAXParserFactory.newInstance();
            SAXParser saxParser = factory.newSAXParser();
            XMLReader parser = saxParser.getXMLReader();
            parser.setContentHandler(this);
            parser.parse(src);
        }
        catch(Exception e)
        {
            System.err.println("Exception parsing \"" + filename + "\":" + e);
            e.printStackTrace();
            System.err.flush();
        }
    }
    
    public void setDocumentLocator(Locator rhs) {m_locator = rhs;}
    
    //localName wasn't working!!
    public void startElement(String uri, String localName, String qName,
            Attributes attribs)
    {
        m_isStackReadyForText = false;
        
        //System.err.println("element=" + qName);
        //System.err.flush();
        
      try
      {
        // Complex elements first
	if(qName.equals("classes"))
	{
	    // push something onto the stack to avoid stack underflow
	    m_stack.push("something");
	}
	else if(qName.equals("message"))
        {
            String name = resolveAttrib(uri, "name", attribs, "unknown");
	    String type = resolveAttrib(uri, "type", attribs, "unknown");
	    message m = new message(name, Integer.parseInt(type));
            m_stack.push(m);
        }
	else if(qName.equals("param"))
        {
            String name = resolveAttrib(uri, "name", attribs, "unknown");
	    String type = resolveAttrib(uri, "type", attribs, "unknown");
	    String supertype = resolveAttrib(uri,"superclass",attribs,"TLVParameter");
	    param p = new param(name, Integer.parseInt(type), supertype);
            m_stack.push(p);
        }
	else if(qName.equals("groupclass"))
	{
            String name = resolveAttrib(uri, "name", attribs, "unknown");
	    String supertype = resolveAttrib(uri,"superclass",attribs,"TLVParameter");
	    groupclass gc = new groupclass(name, supertype);
	    m_stack.push(gc);
	}
	else if(qName.equals("bytefield"))
        {
            String name = resolveAttrib(uri, "name", attribs, "unknown");
	    bytefield f = new bytefield(name);
            m_stack.push(f);
        }
	else if(qName.equals("shortfield"))
        {
            String name = resolveAttrib(uri, "name", attribs, "unknown");
	    shortfield f = new shortfield(name);
            m_stack.push(f);
        }
	else if(qName.equals("intfield"))
        {
            String name = resolveAttrib(uri, "name", attribs, "unknown");
	    intfield f = new intfield(name);
            m_stack.push(f);
        }
	else if(qName.equals("longfield"))
        {
            String name = resolveAttrib(uri, "name", attribs, "unknown");
	    longfield f = new longfield(name);
            m_stack.push(f);
        }
	else if(qName.equals("stringfield"))
        {
            String name = resolveAttrib(uri, "name", attribs, "unknown");
	    stringfield f = new stringfield(name);
            m_stack.push(f);
        }
	else if(qName.equals("bitfield"))
        {
            String bits = resolveAttrib(uri, "bits", attribs, "unknown");
	    bitfield f = new bitfield(Integer.parseInt(bits));
            m_stack.push(f);
        }
	else if(qName.equals("boolean"))
        {
            String name = resolveAttrib(uri, "name", attribs, "unknown");
            String mask = resolveAttrib(uri, "mask", attribs, "unknown");
	    bitfield f = (bitfield) m_stack.peek();
	    f.addBoolean(name,mask);
        }
	else if(qName.equals("numeric"))
        {
            String name = resolveAttrib(uri, "name", attribs, "unknown");
            String mask = resolveAttrib(uri, "mask", attribs, "unknown");
            String shift = resolveAttrib(uri, "shift", attribs, "unknown");
	    bitfield f = (bitfield) m_stack.peek();
	    f.addNumeric(name,mask,shift);
        }
        else if(qName.equals("pfield"))
        {
            String name = resolveAttrib(uri, "name", attribs, "unknown");
            String type = resolveAttrib(uri, "type", attribs, "unknown");
	    int itype = 0;
	    if(type.equalsIgnoreCase("list")) itype = 2;
	    else if(type.equalsIgnoreCase("required")) itype = 1;
	    else if(type.equalsIgnoreCase("optional")) itype = 0;
	    else
	    {
		System.out.println("Ran into unknown type for pfield.");
		System.exit(-1);
	    }
	    pfield f = new pfield(name, itype);
            m_stack.push(f);
        }
	else if(qName.equals("arrayfield"))
	{
            String name = resolveAttrib(uri, "name", attribs, "unknown");
            String type = resolveAttrib(uri, "type", attribs, "unknown");
	    arrayfield af = new arrayfield(name,type);
	    m_stack.push(af);
	}
      }
      catch(Exception ex)
      {
          System.err.println("Parsing Exception: " + ex + "\nAborting.");
          System.err.println("File: " + m_savedFileName + ", Line:" + m_locator.getLineNumber());
          System.exit(-1);
      }
    }
    
    public void endElement(String uri, String localName, String qName)
    {
        m_isStackReadyForText = false;
        
        //System.err.println("end element=" + qName);
        //System.err.flush();

        Object tmp = m_stack.pop();
      try 
      {          
        if(qName.equals("classes"))
        {
            System.err.println("finished parsing classes");
        }
        else if(qName.equals("message"))
        {
            message m = (message) tmp;
	    m.genCode();
        }
        else if(qName.equals("param"))
        {
            param p = (param) tmp;
	    p.genCode();
        }
	else if(qName.equals("groupclass"))
	{
	    groupclass gc = (groupclass) tmp;
	    gc.genCode();
	}
        else if( qName.equals("bytefield") ||
		 qName.equals("shortfield") ||
		 qName.equals("intfield") ||
		 qName.equals("longfield") ||
		 qName.equals("stringfield") ||
		 qName.equals("bitfield") ||
		 qName.equals("arrayfield") ||
		 qName.equals("pfield"))
        {
	    classGenerator c = (classGenerator) m_stack.peek();
	    field f = (field) tmp;
	    c.addField(f);
        }
	else if(qName.equals("boolean") || qName.equals("numeric") )
	{
	    m_stack.push(tmp); // shouldn't have been popped
	}
      }
      catch(Exception ex)
      {
          System.err.println("" + ex + "\nAborting.");
          System.err.println("File: " + m_savedFileName + ", Line:" + m_locator.getLineNumber());
          System.exit(-1);
      }
    }
    
    public void characters(char[] data, int start, int length)
    {
        if(m_isStackReadyForText)
        {
            ((StringBuffer)m_stack.peek()).append(data, start, length);
        }
        else
        {
            // Read data that is part of unrecognized element
        }
    }
    
    private String resolveAttrib(String uri, String localName, 
            Attributes attribs, String defaultValue)
    {
        String tmp = attribs.getValue(uri, localName);
        return (tmp!=null)?(tmp):(defaultValue);
    }
}

abstract class field
{
  protected String name = "Unknown";
  protected boolean varLen = false;
  protected boolean paramField = false;

  public boolean isVarLen() {return varLen;}
  public boolean isParamField() {return paramField;}

  public abstract void genDecl();
  public abstract void genGetSet();
  public abstract void genSerialize();
  public abstract void genDeserialize();
  public abstract void genXML();

  public void genDeserializeFixup(boolean first) {}
  public void genCtorInfo() {} // by default, no constructor information generated
  //public void genShow()
  //{
  //  System.out.println("    System.out.println(\""+name+": \" + " + "_" + name+");");
  //}
}

class arrayfield extends field
{
  private String dt_to = "";
  private String dt_small = "";
  private String dt_big = "";
  private String dt_obj = "";
  private String dt_value = "";
  private String varName = "";


  public arrayfield(String _name, String _type)
  {
    name = _name;
    varName = name+"Elements";
    if(_type.equalsIgnoreCase("byte"))
    {
	dt_small = "byte";
	dt_big = "Byte";
	dt_value = "byteValue()";
	dt_obj = "Byte";
    }
    else if(_type.equalsIgnoreCase("short"))
    {
	dt_small = "short";
	dt_big = "Short";
	dt_value = "shortValue()";
	dt_obj = "Short";
    }
    else if(_type.equalsIgnoreCase("int"))
    {
	dt_small = "int";
	dt_big = "Int";
	dt_value = "intValue()";
	dt_obj = "Integer";
    }
    else if(_type.equalsIgnoreCase("long"))
    {
	dt_small = "long";
	dt_big = "Long";
	dt_value = "longValue()";
	dt_obj = "Long";
    }
    else
    {
        System.out.println("arrayField ctor: unknown type " + _type);
	System.exit(-1);
    }

  }

  public void genDecl()
  {
    System.out.println("  private ArrayList<"+dt_obj+"> " + varName + " = null;");
  }
  public void genXML()
  {
    //System.out.println("    sb.append(indent+\"  <array name=\\\""+name+"\\\">\");");
    System.out.println("    {");
    System.out.println("      int i;");
    System.out.println("      for(i=0;i<"+varName+".size();i++)");
    System.out.println("        sb.append(indent+\"  <"+name+"Element value=\\\"\"+"+varName+".get(i)+\"\\\"/>\\n\");");
    System.out.println("    }");
  }

  public void genCtorInfo() 
  {
    System.out.println("    "+varName+" = new ArrayList<"+dt_obj+">();");
  }

  public void genGetSet()
  {
    System.out.println("  public int get"+name+"Count() {return "+varName+".size();}");
    System.out.println("  public void add"+name+"Element("+dt_small+" x) {"+varName+".add(x);}");
    System.out.println("  public void add"+name+"Element(String x) {"+varName+".add(("+dt_small+")Integer.parseInt(x));}");
    System.out.println("  public "+dt_small+" get"+name+"Element(int index) {return "+varName+".get(index)."+dt_value+";}");
  }

  public void genSerialize()
  {
    // First, serialize the count
    System.out.println("    out.writeShort((short)"+varName+".size());");
    // Now, serialize the array
    System.out.println("    {");
    System.out.println("      int i;");
    System.out.println("      for(i=0;i<"+varName+".size();i++)");
    System.out.println("        out.write"+dt_big+"("+varName+".get(i)."+dt_value+");");
    System.out.println("    }");
  }

  public void genDeserialize()
  {
    // read in the count then each element of the array
    System.out.println("    {");
    System.out.println("      int cnt = in.readShort();");
    System.out.println("      int i;");
    System.out.println("      for(i=0;i<cnt;i++)");
    System.out.println("        add"+name+"Element(in.read"+dt_big+"());");
    System.out.println("    }");
  }

  //public void genShow()
  //{
  //  System.out.println("    System.out.print(\""+name+" array: [\");");
  //  System.out.println("    {");
  //  System.out.println("      int i;");
  //  System.out.println("      for(i=0;i<"+varName+".size();i++)");
  //  System.out.println("        System.out.print("+varName+".get(i)."+dt_value+" + \" \");");
  //  System.out.println("    }");
  //  System.out.println("    System.out.println(\"]\");");
  //}
}



  
class pfield extends field
{
  public static final int TYPE_OPTIONAL = 0;
  public static final int TYPE_REQUIRED = 1;
  public static final int TYPE_LIST = 2;
  public static final int TYPE_UNKNOWN = 3;

  private int pftype = TYPE_UNKNOWN;
  private String pname = "";

  public pfield(String _pname, int _type)
  {
    pname = _pname;
    pftype = _type;
    name = "_" + pname + "Param";
    if(pftype == TYPE_LIST) name += "s";
    if(pftype != TYPE_REQUIRED) varLen = true;
    paramField = true;
  }

  public void genDecl()
  {
    if(pftype == TYPE_LIST)
      System.out.println("  private ArrayList<" + pname + "> " + name + " = null;");
    else
      System.out.println("  private " + pname + " " + name + " = null;");
  }

  public void genXML()
  {
    if(pftype == TYPE_LIST)
    {
      System.out.println("    {");
      System.out.println("      int i;");
      System.out.println("      for(i=0;i<"+name+".size();i++)");
      System.out.println("        "+name+".get(i).toXML(sb, indent+\"  \");");
      System.out.println("    }");
    }
    else
      System.out.println("    if("+name+"!=null) "+name+".toXML(sb, indent+\"  \");");
  }

  public void genGetSet()
  {
    if(pftype == TYPE_LIST)
    {
      String fname1 = "getNum" + pname + "Params";
      String fname2 = "add" + pname + "Param";
      String fname3 = "get" + pname + "Param";
      System.out.println("  public int " + fname1 + "() { return " + name + ".size();}");
      System.out.println("  public void " + fname2 + "(" + pname + " p) {" + name + ".add(p);}");
      System.out.println("  public " + pname + " " + fname3 + "(int idx) {return " + name + ".get(idx);}");
    }
    else
    {
      String fname1 = "set" + pname + "Param";
      String fname2 = "get" + pname + "Param";
      System.out.println("  public void " + fname1 + "(" + pname + " p) {" + name + " = p;}");
      System.out.println("  public " + pname + " " + fname2 + "() {return " + name + ";}");
    }
  }

  public void genCtorInfo()
  {
    if(pftype == TYPE_LIST)
    {
      System.out.println("    " + name + " = new ArrayList<" + pname + ">();");
    }
    else if(pftype == TYPE_REQUIRED)
    {
      System.out.println("    " + name + " = new " + pname + "();");
    }
  }


  public void genSerialize()
  {
    if(pftype == TYPE_LIST)
    {
      System.out.println("    {");
      System.out.println("      int i;");
      System.out.println("      for(i=0; i<" + name + ".size(); i++)");
      System.out.println("        " + name + ".get(i).serialize(out);");
      System.out.println("    }");
    }
    else
    {
      System.out.println("    if(" + name + "!=null) " + name + ".serialize(out);");
    }
  }

  public void genDeserialize()
  {
    if(pftype == TYPE_REQUIRED)
      System.out.println("    " + name + " = ("+pname+") Parameter.deserialize(in);");
  }

  public void genDeserializeFixup(boolean first)
  {
    String verb = "add";
    if(pftype != TYPE_LIST) verb = "set";
    System.out.println("      " + (first?"":"else ") + "if(" + pname + ".class.isInstance(p))");
    System.out.println("        "+verb+pname+"Param(("+pname+")p);");
  }

  //public void genShow()
  //{
  //  if(pftype == TYPE_LIST)
  //  {
  //    System.out.println("    {");
  //    System.out.println("      int i;");
  //    System.out.println("      for(i=0; i<" + name + ".size(); i++)");
  //    System.out.println("        " + name + ".get(i).show();");
  //    System.out.println("    }");
  //  }
  //  else
  //  {
  //    System.out.println("    if(" + name + "!=null) " + name + ".show();");
  //  }
  //}
}


class subfield_info
{
  public String name = "";
  public String mask = "";
  public String shift = "";
  public boolean isBoolean = false;

  public subfield_info(String _name, String _mask, String _shift, boolean _isBoolean)
  {
    name = _name;
    mask = _mask;
    shift = _shift;
    isBoolean = _isBoolean;
  }
}

class bitfield extends field
{
  //private ArrayList<String> fieldNames = new ArrayList<String>();
  //private ArrayList<String> fieldMasks = new ArrayList<String>();
  private int numBits = 0;
  private ArrayList<subfield_info> subfields = new ArrayList<subfield_info>();

  public bitfield(int bits)
  {
    numBits = bits;
  }

  public void addBoolean(String fname, String fmask)
  {
    subfield_info sfi = new subfield_info(fname, fmask, "0", true);
    subfields.add(sfi);
  }

  public void addNumeric(String fname, String fmask, String fshift)
  {
    subfield_info sfi = new subfield_info(fname, fmask, fshift, false);
    subfields.add(sfi);
  }

  public void genDecl()
  {
    int i;
    for(i=0; i<subfields.size(); i++)
    {
        subfield_info sfi = subfields.get(i);
	if(sfi.isBoolean)
	  System.out.println("  private boolean _" + sfi.name + " = false;");
	else
	  System.out.println("  private int _" + sfi.name + " = 0;");
    }
  }

  public void genXML()
  {
    int i;
    for(i=0; i<subfields.size(); i++)
    {
        subfield_info sfi = subfields.get(i);
        System.out.println("    sb.append(indent+\"  <"+sfi.name+" value=\\\"\"+_"+sfi.name+"+\"\\\"/>\\n\");");
    }
  }

  public void genGetSet()
  {
    int i;
    for(i=0; i<subfields.size(); i++)
    {
        subfield_info sfi = subfields.get(i);
	if(sfi.isBoolean)
	{
	  System.out.println("  public void set" + sfi.name + "(boolean b) {_" + sfi.name + " = b;}");
	  System.out.println("  public void set" + sfi.name + "(String s) {_" + sfi.name + " = Boolean.parseBoolean(s);}");
	  System.out.println("  public boolean get" + sfi.name + "() {return _" + sfi.name + ";}");
	}
	else
	{
	  System.out.println("  public void set" + sfi.name + "(int i) {_" + sfi.name + " = i;}");
	  System.out.println("  public void set" + sfi.name + "(String s) {_" + sfi.name + " = Integer.parseInt(s);}");
	  System.out.println("  public int get" + sfi.name + "() {return _" + sfi.name + ";}");
	}
    }
  }

  public void genSerialize()
  {
    String type = "";
    String wType = "";
    switch(numBits)
    {
      case 8: type = "byte"; wType = "Byte"; break;
      case 16: type = "short"; wType = "Short"; break;
      case 32: type = "int"; wType = "Int"; break;
      default:
	System.out.println("Help!  Encountered non 8/16/32 bit field.");
	System.exit(-1);
    }

    System.out.println("    " + type + " x = 0;");
    int i;
    for(i=0; i<subfields.size(); i++)
    {
      subfield_info sfi = subfields.get(i);
      if(sfi.isBoolean)
        System.out.println("    if (_" + sfi.name + ") x = (" + type + ") (x | " + sfi.mask + ");");
      else
        System.out.println("    x = (" + type + ") (x | ((_" + sfi.name + " & " + sfi.mask + ") << " + sfi.shift + "));");
    }

    System.out.println("    out.write" + wType + "(x);");
  }
    
  public void genDeserialize()
  {
    String type = "";
    String rType = "";
    switch(numBits)
    {
      case 8: type = "byte"; rType = "Byte"; break;
      case 16: type = "short"; rType = "Short"; break;
      case 32: type = "int"; rType = "Int"; break;
      default:
	System.out.println("Help!  Encountered non 8/16/32 bit field.");
	System.exit(-1);
    }

    System.out.println("    " + type + " x = in.read" + rType + "();");
    int i;
    for(i=0; i<subfields.size(); i++)
    {
      subfield_info sfi = subfields.get(i);
      if(sfi.isBoolean)
        System.out.println("    _" + sfi.name + " = ( (x & " + sfi.mask + ") != 0);");
      else
        System.out.println("    _" + sfi.name + " = ( (x >> " + sfi.shift + ") & " + sfi.mask + ");");
    }
  }

  //public void genShow()
  //{
  //  int i;
  //  for(i=0; i<subfields.size(); i++)
  //  {
  //    subfield_info sfi = subfields.get(i);
  //    System.out.println("    System.out.println(\""+sfi.name+": \" + " + "_" + sfi.name +");");
  //  }
  //}
}

class bytefield extends field
{
  public bytefield(String _name) {name=_name; varLen=false;}

  public void genDecl()
  {
    System.out.println("  private byte _" + name + " = 0;");
  }
  public void genXML()
  {
    System.out.println("    sb.append(indent+\"  <"+name+" value=\\\"\"+_"+name+"+\"\\\"/>\\n\");");
  }
  public void genGetSet()
  {
    System.out.println("  public void set" + name + "(byte b) { _" + name + " = b;}");
    System.out.println("  public void set" + name + "(String s) { _" + name + " = Byte.parseByte(s);}");
    System.out.println("  public byte get" + name + "() {return _" + name + ";}");
  }
  public void genSerialize()
  {
    System.out.println("    out.writeByte(_" + name + ");");
  }
  public void genDeserialize()
  {
    System.out.println("    _" + name + " = in.readByte();");
  }
} 

class shortfield extends field
{
  public shortfield(String _name) {name=_name; varLen=false;}

  public void genDecl()
  {
    System.out.println("  private short _" + name + " = 0;");
  }
  public void genXML()
  {
    System.out.println("    sb.append(indent+\"  <"+name+" value=\\\"\"+_"+name+"+\"\\\"/>\\n\");");
  }
  public void genGetSet()
  {
    System.out.println("  public void set" + name + "(short b) { _" + name + " = b;}");
    System.out.println("  public void set" + name + "(String s) { _" + name + " = Short.parseShort(s);}");
    System.out.println("  public short get" + name + "() {return _" + name + ";}");
  }
  public void genSerialize()
  {
    System.out.println("    out.writeShort(_" + name + ");");
  }
  public void genDeserialize()
  {
    System.out.println("    _" + name + " = in.readShort();");
  }
} 

class intfield extends field
{
  public intfield(String _name) {name=_name; varLen=false;}

  public void genDecl()
  {
    System.out.println("  private int _" + name + " = 0;");
  }
  public void genXML()
  {
    System.out.println("    sb.append(indent+\"  <"+name+" value=\\\"\"+_"+name+"+\"\\\"/>\\n\");");
  }
  public void genGetSet()
  {
    System.out.println("  public void set" + name + "(int b) { _" + name + " = b;}");
    System.out.println("  public void set" + name + "(String s) { _" + name + " = Integer.parseInt(s);}");
    System.out.println("  public int get" + name + "() {return _" + name + ";}");
  }
  public void genSerialize()
  {
    System.out.println("    out.writeInt(_" + name + ");");
  }
  public void genDeserialize()
  {
    System.out.println("    _" + name + " = in.readInt();");
  }
} 

class longfield extends field
{
  public longfield(String _name) {name=_name; varLen=false;}

  public void genDecl()
  {
    System.out.println("  private long _" + name + " = 0;");
  }
  public void genXML()
  {
    System.out.println("    sb.append(indent+\"  <"+name+" value=\\\"\"+_"+name+"+\"\\\"/>\\n\");");
  }
  public void genGetSet()
  {
    System.out.println("  public void set" + name + "(long b) { _" + name + " = b;}");
    System.out.println("  public void set" + name + "(String s) { _" + name + " = Long.parseLong(s);}");
    System.out.println("  public long get" + name + "() {return _" + name + ";}");
  }
  public void genSerialize()
  {
    System.out.println("    out.writeLong(_" + name + ");");
  }
  public void genDeserialize()
  {
    System.out.println("    _" + name + " = in.readLong();");
  }
} 

class stringfield extends field
{
  public stringfield(String _name) {name=_name; varLen=false;}

  public void genDecl()
  {
    System.out.println("  private String _" + name + " = \"\";");
  }
  public void genXML()
  {
    System.out.println("    sb.append(indent+\"  <"+name+" value=\\\"\"+_"+name+"+\"\\\"/>\\n\");");
  }
  public void genGetSet()
  {
    System.out.println("  public void set" + name + "(String s) { _" + name + " = s;}");
    System.out.println("  public String get" + name + "() {return _" + name + ";}");
  }
  public void genSerialize()
  {
    System.out.println("    out.writeUTF(_" + name + ");");
  }
  public void genDeserialize()
  {
    System.out.println("    _" + name + " = in.readUTF();");
  }
} 

class groupclass
{
  private String name;
  private String supername;

  public groupclass(String _name, String _supername)
  {
    name = _name;
    supername = _supername;
  }

  public void genCode()
  {
    java.util.Date now = new java.util.Date();
    FileOutputStream fout = null;
    try
    {
      fout = new FileOutputStream(name+".java",false);
    }
    catch(Exception e)
    {
      System.err.println("FAILED to open " + name + ".java.  Exception: " + e);
      System.exit(-1);
    }
    PrintStream pout = new PrintStream(fout);
    System.setOut(pout);

    System.out.println("/*");
    System.out.println(" * File: " + name + ".java");
    System.out.println(" * auto-generated " + now);
    System.out.println(" */");
    System.out.println("package edu.uark.csce.llrp;");
    System.out.println("public abstract class " + name + " extends " + supername + "\n{\n}");
    pout.flush();
  }
}
  
class classGenerator 
{
  protected ArrayList<field> fields = new ArrayList<field>();
  protected String name = "";
  protected int typenum = 0;
  protected String extensionOf = "TLVParameter";

  public classGenerator(String _name, int _typenum, String eo) 
  {
    name=_name; 
    typenum=_typenum;
    extensionOf = eo;
  }

  public void addField(field f) {fields.add(f);}

  private void genDecls()
  {
    System.out.println("  // member variables");
    int i;
    for(i=0; i<fields.size(); i++)
      fields.get(i).genDecl();
    System.out.println("");
  }

  private void genCtor()
  {
    System.out.println("  public " + name + "()");
    System.out.println("  {");
    int i;
    for(i=0; i<fields.size(); i++)
      fields.get(i).genCtorInfo();
    System.out.println("  }");
    System.out.println();
  }


  private void genGetSets()
  {
    System.out.println("  // accessors and mutators");
    int i;
    for(i=0; i<fields.size(); i++)
      fields.get(i).genGetSet();
    System.out.println("");
  }

  protected void genGetType()
  {
  }

  private void genSerialize()
  {
    System.out.println("  public void serializeBody(DataOutputStream out) throws IOException");
    System.out.println("  {");
    int i;
    for(i=0; i<fields.size(); i++)
      fields.get(i).genSerialize();
    System.out.println("  }");
    System.out.println();
  }

  protected void genDeserialize()
  {
  }

  //private void genShow()
  //{
  //  System.out.println("  public void showBody()\n  {");
  //  System.out.println("    System.out.println(\"Type: " + name +"\");");
  //  int i;
  //  for(i=0; i<fields.size(); i++)
  //    fields.get(i).genShow();
  //  System.out.println("  }\n");
  //}

  protected void genXML()
  {
    //System.out.println("  public void toXML(StringBuffer sb, String indent)");
    //System.out.println("  {");
    //System.out.println("    sb.append(indent+\"<"+name+">\\n\");");
    //int i;
    //for(i=0; i<fields.size(); i++)
    //  fields.get(i).genXML();
    //System.out.println("    sb.append(indent+\"</"+name+">\\n\");");
    //System.out.println("  }");
    //System.out.println();
  }

  public void genCode()
  {
    java.util.Date now = new java.util.Date();
    FileOutputStream fout = null;
    try
    {
      fout = new FileOutputStream(name+".java",false);
    }
    catch(Exception e)
    {
      System.err.println("FAILED to open " + name + ".java.  Exception: " + e);
      System.exit(-1);
    }
    PrintStream pout = new PrintStream(fout);
    System.setOut(pout);

    System.out.println("//   Copyright (c) 2007 by the Board of Trustees of the University of Arkansas.");
    System.out.println("//");
    System.out.println("// Licensed under the Apache License, Version 2.0 (the \"License\");");
    System.out.println("// you may not use this file except in compliance with the License.");
    System.out.println("// You may obtain a copy of the License at");
    System.out.println("//");
    System.out.println("//     http://www.apache.org/licenses/LICENSE-2.0");
    System.out.println("//");
    System.out.println("// Unless required by applicable law or agreed to in writing, software");
    System.out.println("// distributed under the License is distributed on an \"AS IS\" BASIS,");
    System.out.println("// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied,"); 
    System.out.println("// including, without limitation, any warranties or conditions of TITLE,"); 
    System.out.println("// NON-INFRINGEMENT, MERCHANTABILITY, or FITNESS FOR A PARTICULAR PURPOSE.");
    System.out.println("// See the License for the specific language governing permissions and");
    System.out.println("// limitations under the License.");
    System.out.println();
    System.out.println();

    System.out.println("/*");
    System.out.println(" * File: " + name + ".java");
    System.out.println(" * auto-generated " + now);
    System.out.println(" */");
    System.out.println("package edu.uark.csce.llrp;");
    System.out.println("import java.io.*;");
    System.out.println("import java.util.ArrayList;");
    System.out.println("public class " + name + " extends " + extensionOf);
    System.out.println("{");
    genDecls();
    genCtor();
    genGetSets();

    System.out.println("  //Methods mandated by " + extensionOf + " class");
    genGetType();
    genSerialize();
    genDeserialize();
    //genShow();
    genXML();

    System.out.println("}");

    pout.flush();
  }
}

class param extends classGenerator
{
  public param(String _name, int _type, String supertype)
  {
    super(_name, _type, supertype);
  }

  protected void genGetType()
  {
    System.out.println("  public int getParamType() {return "+typenum+";}");
  }

  protected void genDeserialize()
  {
    System.out.println("  public void deserializeBody(DataInputStream in, int bytes) throws IOException");
    System.out.println("  {");
    
    // First, see whether we need to worry about variable length fields
    boolean varLen = false;
    int i;
    for(i=0; i<fields.size(); i++)
    {
      if(fields.get(i).isVarLen())
      {
	varLen = true;
	break;
      }
    }
    if(varLen)
      System.out.println("    int startAvail = in.available();");

    for(i=0; i<fields.size(); i++)
    {
      // Skip parameter fields on first pass if variable length fields present
      if(varLen && fields.get(i).isParamField()) continue;

      fields.get(i).genDeserialize();
    }

    // If variable length fields are present, then add a loop to handle them
    if(varLen)
    {
      System.out.println("    while( (startAvail - in.available()) < bytes)");
      System.out.println("    {");
      System.out.println("      Parameter p = Parameter.deserialize(in);");
      boolean first = true;
      for(i=0; i<fields.size(); i++)
      {
	if(fields.get(i).isParamField() == false) continue;
	fields.get(i).genDeserializeFixup(first);
	first = false;
      }
      System.out.println("      else throw new IOException(\""+name+".deserializeBody(): Unhandled parameter class \"+p.getClass().getName());"); 
      System.out.println("    }");
    }

    System.out.println("  }");
    System.out.println();
  }

  protected void genXML()
  {
    System.out.println("  public void toXML(StringBuffer sb, String indent)");
    System.out.println("  {");
    System.out.println("    sb.append(indent+\"<"+name+">\\n\");");
    int i;
    for(i=0; i<fields.size(); i++)
      fields.get(i).genXML();
    System.out.println("    sb.append(indent+\"</"+name+">\\n\");");
    System.out.println("  }");
    System.out.println();
  }

}

class message extends classGenerator
{
  public message(String _name, int _type)
  {
    super(_name, _type, "Message");
  }

  protected void genGetType()
  {
    System.out.println("  public int getMessageType() {return "+typenum+";}");
  }

  protected void genDeserialize()
  {
    System.out.println("  public void deserializeBody(DataInputStream in) throws IOException");
    System.out.println("  {");
    
    // First, see whether we need to worry about variable length fields
    boolean varLen = false;
    int i;
    for(i=0; i<fields.size(); i++)
    {
      if(fields.get(i).isVarLen())
      {
	varLen = true;
	break;
      }
    }

    for(i=0; i<fields.size(); i++)
    {
      // Skip parameter fields on first pass if variable length fields present
      if(varLen && fields.get(i).isParamField()) continue;

      fields.get(i).genDeserialize();
    }

    // If variable length fields are present, then add a loop to handle them
    if(varLen)
    {
      System.out.println("    while( in.available() > 0)");
      System.out.println("    {");
      System.out.println("      Parameter p = Parameter.deserialize(in);");
      boolean first = true;
      for(i=0; i<fields.size(); i++)
      {
	if(fields.get(i).isParamField() == false) continue;
	fields.get(i).genDeserializeFixup(first);
	first = false;
      }
      System.out.println("      else throw new IOException(\""+name+".deserializeBody(): Unhandled parameter class \"+p.getClass().getName());"); 
      System.out.println("    }");
    }

    System.out.println("  }");
    System.out.println();
  }

  protected void genXML()
  {
    System.out.println("  public void toXML(StringBuffer sb, String indent)");
    System.out.println("  {");
    System.out.println("    sb.append(indent+\"<Message type=\\\""+name+"\\\">\\n\");");
    int i;
    for(i=0; i<fields.size(); i++)
      fields.get(i).genXML();
    System.out.println("    sb.append(indent+\"</Message>\\n\");");
    System.out.println("  }");
    System.out.println();
  }

}





    


public class codegen
{
  public static void main(String[] args) 
  {
    XMLInterpreter x = new XMLInterpreter();
    String filename = args[0];
    x.parse(filename);

    /*
    param p1 = new param("UTCTimestamp", 128);

    longfield f1 = new longfield("Microseconds");
    p1.addField(f1);

    bitfield f2 = new bitfield(16);
    f2.addSubField("CanSetAntennaProperties", "0x8000");
    f2.addSubField("HasUTCClockCapability", "0x4000");
    p1.addField(f2);

    pfield f3 = new pfield("ReceiveSensitivityTableEntry", pfield.TYPE_LIST);
    p1.addField(f3);

    pfield f4 = new pfield("GPIOCapabilities", pfield.TYPE_REQUIRED);
    p1.addField(f4);

    pfield f5 = new pfield("Custom", pfield.TYPE_OPTIONAL);
    p1.addField(f5);

    p1.genCode();
    */
  }
}
