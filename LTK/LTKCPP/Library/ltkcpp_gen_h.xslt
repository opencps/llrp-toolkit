<?xml version="1.0" encoding="UTF-8"?>
<!--
 -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 -  Copyright 2007,2008 Impinj, Inc.
 -
 -  Licensed under the Apache License, Version 2.0 (the "License");
 -  you may not use this file except in compliance with the License.
 -  You may obtain a copy of the License at
 -
 -      http://www.apache.org/licenses/LICENSE-2.0
 -
 -  Unless required by applicable law or agreed to in writing, software
 -  distributed under the License is distributed on an "AS IS" BASIS,
 -  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 -  See the License for the specific language governing permissions and
 -  limitations under the License.
 -
 -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 -->

<xsl:stylesheet
        version='1.0'
        xmlns:LL="http://www.llrp.org/ltk/schema/core/encoding/binary/1.0"
        xmlns:h="http://www.w3.org/1999/xhtml"
        xmlns:xsl='http://www.w3.org/1999/XSL/Transform'>
<xsl:output omit-xml-declaration='yes' method='text' encoding='iso-8859-1'/>

<!--=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 -
 - @brief top level template
 -
 - Matches the <llrpdef> root, thereby traversing it, and invokes
 - the various templates that generate the sections.
 -
 -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 -->

<xsl:template match='/LL:llrpdef'>
<xsl:call-template name='FileHeader'/>
<xsl:call-template name='ForwardDeclClassMessages'/>
<xsl:call-template name='ForwardDeclClassParameters'/>
<xsl:call-template name='VendorDescriptorDeclarations'/>
<xsl:call-template name='NamespaceDescriptorDeclarations'/>
<xsl:call-template name='EnumerationDefinitionsFields'/>
<xsl:call-template name='ClassDeclarationsMessages'/>
<xsl:call-template name='ClassDeclarationsParameters'/>
<xsl:call-template name='ClassDeclarationsChoices'/>

/** @brief Enrolls the types for <xsl:value-of select='$RegistryName'/> into the LTKCPP registry
 ** 
 ** LTKCPP needs an internal registry for storing all the type information.  This function
 ** is required to enroll the types for the <xsl:value-of select='$RegistryName'/> into
 ** the operating registry.  
 ** 
 ** For example -- in order to decode and encode packets from the core LLRP specification
 ** The user must EnrollCoreTypesIntoRegistry.
 **
 ** @ingroup LTKCoreElement
 */
extern void
enroll<xsl:value-of select='$RegistryName'/>TypesIntoRegistry (
  CTypeRegistry *               pTypeRegistry);
</xsl:template>


<!--=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 -
 - @brief FileHeader template
 -
 - Invoked by top level template.
 -
 - Generates the source file header that warns that the file is generated.
 - @todo    It would be nice if we could get a date and time stamp and
 -          maybe some details about what input descriptions were used.
 -
 -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 -->

<xsl:template name='FileHeader' xml:space='preserve'>
/*
 * Generated file - DO NOT EDIT
 *
 * This is the header file for the LLRP Tool Kit (LTK)
 * C++ (aka cpp) implementation. It is generated into a .inc file
 * that is included by a platform specific .h header file.
 * That .h file takes care of prerequisites needed by this file.
 */
</xsl:template>


<!--=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 -
 - @brief Documentation template generates comment for doxygen
 -
 - Invoked by templates
 -
 -
 -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 -->
<xsl:template name='Documentation' xml:space='default'>
  <xsl:param name='Brief'/>
/**
 ** @brief  <xsl:value-of select='$Brief'/>
 **
 <!-- applies the templates below to ensure that we translate the XHTML to html -->
<xsl:apply-templates/>
 **/
</xsl:template>

<!--=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 -
 - @brief For all templates that match the documentation field
 -
 - Prints out a list of the references into the code as href html tags to be
 - interpreted by doxygen etc.
 -
 -

 -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 -->
<xsl:template match='LL:annotation/LL:documentation'>
&lt;ul&gt;
  <xsl:for-each select='h:a'>  
    &lt;li&gt;&lt;b&gt;Document Reference&lt;/b&gt;  <xsl:apply-templates select="."/> &lt;/li&gt;
  </xsl:for-each>
&lt;/ul&gt;  
</xsl:template>

<!--=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 -
 - @brief For all templates that match the description field
 -
 - Prints out each detailed description and also the relevant copyright information
 -
 - Separate each description section by a line in case there is different copyrights
 - for different descriptions
 -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 -->
<xsl:template match='LL:annotation/LL:description'>
  <xsl:apply-templates/>
  <xsl:if test="@copyright">&lt;SMALL&gt;&lt;i&gt;<xsl:value-of select="normalize-space(@copyright)"/>&lt;/i&gt;&lt;/SMALL&gt;</xsl:if> 
  &lt;HR&gt;
</xsl:template>

<!--=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 -
 - @brief These next few templates translate the XHTML to the proper doxygen format
 -
 - Invoked by templates\
 - If i wanted to do a thorough job, I'd have to translsate all the tags
 - found in http://www.stack.nl/~dimitri/doxygen/htmlcmds.html. 
 -
 -
 -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 -->
 <!-- Copy the html a (link) elements -->
 <xsl:template match="h:a">
   &lt;a href=<xsl:value-of select="@href"/>&gt;<xsl:apply-templates/>&lt;/a&gt;
 </xsl:template>
 
 <!-- Copy the html boldface element-->
 <xsl:template match="h:b">
   &lt;b&gt;<xsl:apply-templates/>&lt;/b&gt;
 </xsl:template>

 <!-- Copy the html un-numbered list element -->
 <xsl:template match="h:ul">
    &lt;ul&gt;<xsl:apply-templates/>&lt;/ul&gt; 
 </xsl:template>

 <!-- Copy the html list item element -->
 <xsl:template match="h:li">
    &lt;li&gt;<xsl:apply-templates/>&lt;/li&gt;
 </xsl:template>

 <!-- Copy the html paragraph element-->
 <xsl:template match="h:p">
    &lt;p&gt;<xsl:apply-templates/>&lt;/p&gt; 
 </xsl:template>

<!--=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 -
 - @brief ForwardDeclClassMessages template
 -
 - Invoked by top level template.
 -
 - Generates the section containing forward declarations
 - of the classes for LLRP messages.
 -
 -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 -->

<xsl:template name='ForwardDeclClassMessages'>

/*
 * Message classes - forward decls
 */
<xsl:for-each select='LL:messageDefinition'>
class C<xsl:value-of select='@name'/>;</xsl:for-each>

/* Custom messages */
<xsl:for-each select='LL:customMessageDefinition'>
class C<xsl:value-of select='@name'/>;</xsl:for-each>

</xsl:template>


<!--=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 -
 - @brief ForwardDeclClassParameters template
 -
 - Invoked by top level template.
 -
 - Generates the section containing forward declarations
 - of the classes for LLRP parameters.
 -
 -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 -->

<xsl:template name='ForwardDeclClassParameters'>

/*
 * Parameter classes - forward decls
 */
<xsl:for-each select='LL:parameterDefinition'>
class C<xsl:value-of select='@name'/>;</xsl:for-each>

/* Custom parameters */
<xsl:for-each select='LL:customParameterDefinition'>
class C<xsl:value-of select='@name'/>;</xsl:for-each>

</xsl:template>


<!--=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 -
 - @brief VendorDescriptorDeclarations template
 -
 - Invoked by top level template.
 -
 - Generates declarations of the vendor descriptors.
 -
 -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 -->

<xsl:template name='VendorDescriptorDeclarations'>

/*
 * Vendor descriptor declarations.
 */
<xsl:for-each select='LL:vendorDefinition'>
extern const CVendorDescriptor
g_vdesc<xsl:value-of select='$RegistryName'/>;
</xsl:for-each>

</xsl:template>


<!--=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 -
 - @brief NamespaceDescriptorDeclarations template
 -
 - Invoked by top level template.
 -
 - Generates declarations of the namespace descriptors.
 -
 -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 -->

<xsl:template name='NamespaceDescriptorDeclarations'>

/*
 * Namespace descriptor declarations.
 */
<xsl:for-each select='LL:namespaceDefinition'>
extern const CNamespaceDescriptor
g_nsdesc<xsl:value-of select='@prefix'/>;
</xsl:for-each>

</xsl:template>


<!--=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 -
 - @brief EnumerationDefinitionsFields template
 -
 - Invoked by top level template.
 -
 - Generates definitions of the enumerations and declares the
 - enumeration string tables.
 -
 - The enumeration entry names must be unique in the LLRP namespace.
 - The values must be per the spec.
 -
 - The enumeration string tables are referenced by field descriptors.
 -
 - NB: The LLRP specification does not define enumeration names.
 -     The names in the input description were derived from text
 -     of the LLRP specification.
 -
 -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 -->

<xsl:template name='EnumerationDefinitionsFields'>

/*
 * Enumeration definitions and declarations of
 * enumeration string tables.
 */

<xsl:for-each select='LL:enumerationDefinition|LL:customEnumerationDefinition'>
  <xsl:variable name='enumBaseName' select='@name'/>
  <xsl:call-template name='Documentation'>
    <xsl:with-param name='Brief'>Global enumeration E<xsl:value-of select='@name'/> for LLRP enumerated field <xsl:value-of select='$enumBaseName'/></xsl:with-param>
  </xsl:call-template>
enum E<xsl:value-of select='@name'/>
{
<xsl:for-each select='LL:entry' xml:space='preserve'>
    <xsl:value-of select='$enumBaseName'/>_<xsl:value-of select='@name'/> = <xsl:value-of select='@value'/>, /**&lt; <xsl:value-of select='@name'/> */ </xsl:for-each> 
};

extern const SEnumTableEntry
g_est<xsl:value-of select='$enumBaseName'/>[];

</xsl:for-each>

</xsl:template>


<!--=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 -
 - @brief ClassDeclarationsMessages template
 -
 - Invoked by top level template.
 -
 - Generates declaration of the message classes.
 - A message class is derived from the CMessage base class
 - which is derived from the CElement base class.
 - This loops through the message definitions, selects
 - important values, and invokes the ClassDeclarationCommon
 - template with the right <xsl:with-param>s.
 -
 -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 -->

<xsl:template name='ClassDeclarationsMessages'>
/** 
 * \defgroup <xsl:value-of select='$RegistryName'/>Message  <xsl:value-of select='$RegistryName'/> Message Classes
 * Classes to manipulate the messages defined by the <xsl:value-of select='$RegistryName'/> LLRP protocol
 */
/*@{*/ 
<xsl:for-each select='LL:messageDefinition|LL:customMessageDefinition'>
  <xsl:call-template name='Documentation'>
    <xsl:with-param name='Brief'>Class Definition C<xsl:value-of select='@name'/> for LLRP message <xsl:value-of select='@name'/></xsl:with-param>
  </xsl:call-template>
  <xsl:call-template name='ClassDeclarationCommon'>
    <xsl:with-param name='ClassBase'>CMessage</xsl:with-param>
    <xsl:with-param name='ClassName'>C<xsl:value-of select='@name'/></xsl:with-param>
    <xsl:with-param name='IsCustomParameter'>false</xsl:with-param>
  </xsl:call-template>
</xsl:for-each>
/*@}*/
</xsl:template>


<!--=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 -
 - @brief ClassDeclarationsParameters template
 -
 - Invoked by top level template.
 -
 - Generates declaration of the parameter classes.
 - A parameter class is derived from the CParameter base class
 - which is derived from the CElement base class.
 - This loops through the parameter definitions, selects
 - important values, and invokes the ClassDeclarationCommon
 - template with the right <xsl:with-param>s.
 -
 -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 -->

<xsl:template name='ClassDeclarationsParameters'>
/** 
 * \defgroup <xsl:value-of select='$RegistryName'/>Parameter <xsl:value-of select='$RegistryName'/> Parameter Classes
 * Classes to manipulate the parameters defined by the <xsl:value-of select='$RegistryName'/> LLRP protocol
 */
/*@{*/ 
<xsl:for-each select='LL:parameterDefinition|LL:customParameterDefinition'>
  <xsl:call-template name='Documentation'>
    <xsl:with-param name='Brief'>Class Definition C<xsl:value-of select='@name'/> for LLRP parameter <xsl:value-of select='@name'/></xsl:with-param>
  </xsl:call-template>
  <xsl:call-template name='ClassDeclarationCommon'>
    <xsl:with-param name='ClassBase'>CParameter</xsl:with-param>
    <xsl:with-param name='ClassName'>C<xsl:value-of select='@name'/></xsl:with-param>
    <xsl:with-param name='IsCustomParameter'>
      <xsl:choose>
        <xsl:when test='self::LL:customParameterDefinition'>true</xsl:when>
        <xsl:otherwise>false</xsl:otherwise>
      </xsl:choose>
    </xsl:with-param>
  </xsl:call-template>
</xsl:for-each>
/*@}*/ 
</xsl:template>


<!--=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 -
 - @brief ClassDeclarationCommon template
 -
 - Invoked by templates
 -      ClassDeclarationsMessages
 -      ClassDeclarationsParameters
 -
 - @param   ClassBase       Name of the base class, CMessage
 -                          or CParameter usually
 - @param   ClassName       Name of generated class. This already has
 -                          "C" prefixed to the LLRP name.
 - @param   IsCustomParameter Either "true" or "false".
 -
 -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 -->

<xsl:template name='ClassDeclarationCommon' xml:space='preserve'>
  <xsl:param name='ClassBase'/>
  <xsl:param name='ClassName'/>
  <xsl:param name='IsCustomParameter'/>
class <xsl:value-of select='$ClassName'/> : public <xsl:value-of select='$ClassBase'/>
{
  public:
    <xsl:value-of select='$ClassName'/> (void);
    ~<xsl:value-of select='$ClassName'/> (void);

/** @name Internal Framework Functions */
//@{

    static const CFieldDescriptor * const
    s_apFieldDescriptorTable[];

    static const CTypeDescriptor
    s_typeDescriptor;

    void
    decodeFields (
      CDecoderStream *          pDecoderStream);

    void
    assimilateSubParameters (
      CErrorDetails *           pError);

    void
    encode (
      CEncoderStream *          pEncoderStream) const;

  <xsl:if test='$IsCustomParameter = "true"'>
    llrp_bool_t
    isAllowedIn (
      const CTypeDescriptor *   pEnclosingElementType) const;
  </xsl:if>

    static CElement *
    s_construct (void);

    static void
    s_decodeFields (
      CDecoderStream *          pDecoderStream,
      CElement *                pElement);
//@}

  <xsl:call-template name='ClassDeclFields'/>
  <xsl:call-template name='ClassDeclSubParameters'/>
};

</xsl:template>


<!--=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 -
 - @brief ClassDeclFields template
 -
 - Invoked by template
 -      ClassDeclarationCommon
 -
 - Current node
 -      <llrpdef><messageDefinition>
 -      <llrpdef><parameterDefinition>
 -
 - Generate for each field the member variable and accessor functions.
 -
 -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 -->

<xsl:template name='ClassDeclFields'>
  <xsl:for-each select='LL:field'>
    <xsl:call-template name='ClassDeclOneField'>
      <xsl:with-param name='FieldType'>
        <xsl:choose>
          <xsl:when test='@enumeration and @type = "u8v"'>llrp_u8v_t</xsl:when>
          <xsl:when test='@enumeration'>E<xsl:value-of select='@enumeration'/></xsl:when>
          <xsl:otherwise>llrp_<xsl:value-of select='@type'/>_t</xsl:otherwise>
        </xsl:choose>
      </xsl:with-param>
      <xsl:with-param name='MemberName'>
        <xsl:choose>
          <xsl:when test='@enumeration and @type = "u8v"'>m_<xsl:value-of select='@name'/></xsl:when>
          <xsl:when test='@enumeration'>m_e<xsl:value-of select='@name'/></xsl:when>
          <xsl:otherwise>m_<xsl:value-of select='@name'/></xsl:otherwise>
       </xsl:choose>
      </xsl:with-param>
      <xsl:with-param name='BaseName'><xsl:value-of select='@name'/></xsl:with-param>
    </xsl:call-template>
  </xsl:for-each>
</xsl:template>


<!--=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 -
 - @brief ClassDeclOneField template
 -
 - Invoked by templates
 -      ClassDeclFields
 -
 - Current node
 -      <llrpdef><messageDefinition><field>
 -      <llrpdef><parameterDefinition><field>
 -
 - Generate the field member and accessor (get/set) functions
 - for a field. The accessors are inline. The tricky part
 - is determining whether the field is enumerated or not
 - and adjust the native type and member name accordingly.
 -
 -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 -->

<xsl:template name='ClassDeclOneField'
              xml:space='preserve'>
  <xsl:param name='FieldType'/>
  <xsl:param name='MemberName'/>
  <xsl:param name='BaseName'/>
  protected:
    <xsl:value-of select='$FieldType'/> <xsl:value-of select='$MemberName'/>;

/** @name Internal Framework Functions */
//@{
  public:
    static const CFieldDescriptor
    s_fd<xsl:value-of select='$BaseName'/>;
//@}

    /** @brief Get accessor functions for the LLRP <xsl:value-of select='$BaseName'/> field */
    inline <xsl:value-of select='$FieldType'/>
    get<xsl:value-of select='$BaseName'/> (void)
    {
        return <xsl:value-of select='$MemberName'/>;
    }

    /** @brief Set accessor functions for the LLRP <xsl:value-of select='$BaseName'/> field */
    inline void
    set<xsl:value-of select='$BaseName'/> (
      <xsl:value-of select='$FieldType'/> value)
    {
        <xsl:value-of select='$MemberName'/> = value;
    }

</xsl:template>


<!--=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 -
 - @brief ClassDeclSubParameters template
 -
 - Invoked by template
 -      ClassDeclarationCommon
 -
 - Generate for each parameter (or choice) the member variable
 - and declare accessor functions. This gets tricky:
 -      * Determine if it is a parameter or choice
 -      * Determine the name, either explicit name= or derived from type=
 -      * Determine the kind of repeat (1, 0-1, 0-N, 1-N)
 -      * Make the right kind of member variable (* or std::list)
 -
 -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 -->

<xsl:template name='ClassDeclSubParameters'>
  <xsl:for-each select='LL:parameter|LL:choice'>
    <xsl:choose>
      <xsl:when test='self::LL:parameter and @type = "Custom"'>
        <xsl:call-template name='ClassDeclSubExtensionPoint'/>
      </xsl:when>
      <xsl:when test='self::LL:parameter'>
        <xsl:call-template name='ClassDeclSubParam'/>
      </xsl:when>
      <xsl:when test='self::LL:choice'>
        <xsl:call-template name='ClassDeclSubChoice'/>
      </xsl:when>
      <xsl:otherwise>
        HELP: ClassDeclSubParameters <xsl:value-of select='@type'/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:for-each>
</xsl:template>


<!--=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 -
 - @brief ClassDeclSubParam template
 -
 - Invoked by template
 -      ClassDeclSubParameters
 -
 -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 -->

<xsl:template name='ClassDeclSubParam'>
  <xsl:choose>
    <xsl:when test='@name'>
      <xsl:call-template name='ClassDeclSubXXXWithNameAndType'>
        <xsl:with-param name='Name'><xsl:value-of select='@name'/></xsl:with-param>
        <xsl:with-param name='NativeType'>C<xsl:value-of select='@type'/></xsl:with-param>
        <xsl:with-param name='Repeat'><xsl:value-of select='@repeat'/></xsl:with-param>
      </xsl:call-template>
    </xsl:when>
    <xsl:otherwise>
      <xsl:call-template name='ClassDeclSubXXXWithNameAndType'>
        <xsl:with-param name='Name'><xsl:value-of select='@type'/></xsl:with-param>
        <xsl:with-param name='NativeType'>C<xsl:value-of select='@type'/></xsl:with-param>
        <xsl:with-param name='Repeat'><xsl:value-of select='@repeat'/></xsl:with-param>
      </xsl:call-template>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>


<!--=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 -
 - @brief ClassDeclSubChoice template
 -
 - Invoked by template
 -      ClassDeclSubParameters
 -
 -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 -->

<xsl:template name='ClassDeclSubChoice'>
  <xsl:choose>
    <xsl:when test='@name'>
      <xsl:call-template name='ClassDeclSubXXXWithNameAndType'>
        <xsl:with-param name='Name'><xsl:value-of select='@name'/></xsl:with-param>
        <xsl:with-param name='NativeType'>CParameter</xsl:with-param>
        <xsl:with-param name='Repeat'><xsl:value-of select='@repeat'/></xsl:with-param>
      </xsl:call-template>
    </xsl:when>
    <xsl:otherwise>
      <xsl:call-template name='ClassDeclSubXXXWithNameAndType'>
        <xsl:with-param name='Name'><xsl:value-of select='@type'/></xsl:with-param>
        <xsl:with-param name='NativeType'>CParameter</xsl:with-param>
        <xsl:with-param name='Repeat'><xsl:value-of select='@repeat'/></xsl:with-param>
      </xsl:call-template>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>


<!--=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 -
 - @brief ClassDeclSubExtensionPoint template
 -
 - Invoked by template
 -      ClassDeclSubParameters
 -
 -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 -->

<xsl:template name='ClassDeclSubExtensionPoint'>
  <xsl:call-template name='ClassDeclSubXXXWithNameAndType'>
    <xsl:with-param name='Name'><xsl:value-of select='@type'/></xsl:with-param>
    <xsl:with-param name='NativeType'>CParameter</xsl:with-param>
    <xsl:with-param name='Repeat'><xsl:value-of select='@repeat'/></xsl:with-param>
  </xsl:call-template>
</xsl:template>


<!--=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 -
 - @brief ClassDeclSubXXXWithNameAndType template
 -
 - Invoked by template
 -      ClassDeclSubParam
 -      ClassDeclSubChoice
 -
 - Common for parameters and choices.
 -
 - @param   Name
 - @param   NativeType
 - @param   Repeat
 -
 -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 -->

<xsl:template name='ClassDeclSubXXXWithNameAndType'>
  <xsl:param name='Name'/>
  <xsl:param name='NativeType'/>
  <xsl:param name='Repeat'/>
  <xsl:choose>
    <xsl:when test='$Repeat="1" or $Repeat="0-1"'>
      <xsl:call-template name='ClassDeclSubXXXWithNameAndType_Ptr'>
        <xsl:with-param name='Name'><xsl:value-of select='$Name'/></xsl:with-param>
        <xsl:with-param name='NativeType'><xsl:value-of select='$NativeType'/></xsl:with-param>
      </xsl:call-template>
    </xsl:when>
    <xsl:when test='$Repeat="0-N" or $Repeat="1-N"'>
      <xsl:call-template name='ClassDeclSubXXXWithNameAndType_List'>
        <xsl:with-param name='Name'><xsl:value-of select='$Name'/></xsl:with-param>
        <xsl:with-param name='NativeType'><xsl:value-of select='$NativeType'/></xsl:with-param>
      </xsl:call-template>
    </xsl:when>
    <xsl:otherwise xml:space='preserve'>
    HELP: ClassDeclSubXXXWithNameAndType <xsl:value-of select='$Name'/>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>


<!--=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 -
 - @brief ClassDeclSubXXXWithNameAndType_Ptr template
 -
 - Invoked by template
 -      ClassDeclSubXXXWithNameAndType
 -
 - Common for parameters and choices.
 -
 - @param   Name
 - @param   NativeType
 -
 -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 -->

<xsl:template name='ClassDeclSubXXXWithNameAndType_Ptr'
              xml:space='preserve'>
  <xsl:param name='Name'/>
  <xsl:param name='NativeType'/>
  protected:
    <xsl:value-of select='$NativeType'/> * m_p<xsl:value-of select='$Name'/>;

  public:
    /** @brief Get accessor functions for the LLRP <xsl:value-of select='$Name'/> sub-parameter */  
    inline <xsl:value-of select='$NativeType'/> *
    get<xsl:value-of select='$Name'/> (void)
    {
        return m_p<xsl:value-of select='$Name'/>;
    }

    /** @brief Set accessor functions for the LLRP <xsl:value-of select='$Name'/> sub-parameter */  
    EResultCode
    set<xsl:value-of select='$Name'/> (
      <xsl:value-of select='$NativeType'/> * pValue);

</xsl:template>


<!--=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 -
 - @brief ClassDeclSubXXXWithNameAndType_List template
 -
 - Invoked by template
 -      ClassDeclSubXXXWithNameAndType
 -
 - Common for parameters and choices.
 -
 - @param   Name
 - @param   NativeType
 -
 -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 -->

<xsl:template name='ClassDeclSubXXXWithNameAndType_List'
              xml:space='preserve'>
  <xsl:param name='Name'/>
  <xsl:param name='NativeType'/>
  protected:
    std::list&lt;<xsl:value-of select='$NativeType'/> *&gt; m_list<xsl:value-of select='$Name'/>;

  public:
     /** @brief  Returns the first element of the <xsl:value-of select='$Name'/> sub-parameter list*/  
    inline std::list&lt;<xsl:value-of select='$NativeType'/> *&gt;::iterator
    begin<xsl:value-of select='$Name'/> (void)
    {
        return m_list<xsl:value-of select='$Name'/>.begin();
    }

     /** @brief  Returns the last element of the <xsl:value-of select='$Name'/> sub-parameter list*/  
    inline std::list&lt;<xsl:value-of select='$NativeType'/> *&gt;::iterator
    end<xsl:value-of select='$Name'/> (void)
    {
        return m_list<xsl:value-of select='$Name'/>.end();
    }

     /** @brief  Clears the LLRP <xsl:value-of select='$Name'/> sub-parameter list*/  
    inline void
    clear<xsl:value-of select='$Name'/> (void)
    {
        clearSubParameterList ((tListOfParameters *) &amp;m_list<xsl:value-of select='$Name'/>);
    }

     /** @brief  Count of the LLRP <xsl:value-of select='$Name'/> sub-parameter list*/  
    inline int
    count<xsl:value-of select='$Name'/> (void)
    {
        return (int) (m_list<xsl:value-of select='$Name'/>.size());
    }

    EResultCode
     /** @brief  Add a <xsl:value-of select='$Name'/> to the LLRP sub-parameter list*/  
    add<xsl:value-of select='$Name'/> (
      <xsl:value-of select='$NativeType'/> * pValue);

</xsl:template>


<!--=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 -
 - @brief ClassDeclarationsChoices template
 -
 - Invoked by top level template.
 -
 - Generates declaration of the choice classes.
 - A choice class is really a set of functions and type descriptors.
 -
 -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 -->

<xsl:template name='ClassDeclarationsChoices'>

<xsl:for-each select='LL:choiceDefinition|LL:customChoiceDefinition'>
class C<xsl:value-of select='@name'/>
{
/** @name Internal Framework Functions */
//@{
  public:
    static const CTypeDescriptor
    s_typeDescriptor;

    static llrp_bool_t
    isMember (
      CParameter *              pParameter);
//@}

};
</xsl:for-each>
</xsl:template>


</xsl:stylesheet>
