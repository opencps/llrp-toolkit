<?xml version="1.0" encoding="UTF-8"?>
<!--
 -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 -  Copyright 2007 Impinj, Inc.
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
        xmlns:LL="http://www.llrp.org/ltk/schema/core/encoding/binary/0.8"
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
<xsl:call-template name='IsMemberTestFunctionDeclarations'/>
<xsl:call-template name='EnumerationDefinitionsMessages'/>
<xsl:call-template name='EnumerationDefinitionsParameters'/>
<xsl:call-template name='EnumerationDefinitionsVendors'/>
<xsl:call-template name='EnumerationDefinitionsFields'/>
<xsl:call-template name='ClassDeclarationsMessages'/>
<xsl:call-template name='ClassDeclarationsParameters'/>

extern CTypeRegistry *
getTheTypeRegistry(void);
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
 - @brief IsMemberTestFunctionDeclarations template
 -
 - Invoked by top level template.
 -
 - Generates declarations of the functions that test whether
 - an LLRP parameter is the member of a choice (union)
 -
 -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 -->

<xsl:template name='IsMemberTestFunctionDeclarations'>

/*
 * Choice (union) membership test functions.
 */
<xsl:for-each select='LL:choiceDefinition'>
extern llrp_bool_t
isMemberOf<xsl:value-of select='@name'/> (
  CParameter *                  pParam);
</xsl:for-each>

</xsl:template>


<!--=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 -
 - @brief EnumerationDefinitionsMessages template
 -
 - Invoked by top level template.
 -
 - Generates definitions of the enumeration for standard message types.
 - This does not include custom message types. Names are prefixed MT_.
 -
 -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 -->

<xsl:template name='EnumerationDefinitionsMessages'>

/*
 * Standard message TypeNum enumeration definition.
 */

enum EMessageType
{
<xsl:for-each select='LL:messageDefinition'>
    MT_<xsl:value-of select='@name'/> = <xsl:value-of select='@typeNum'/>,</xsl:for-each>
};

</xsl:template>



<!--=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 -
 - @brief EnumerationDefinitionsParameters template
 -
 - Invoked by top level template.
 -
 - Generates definitions of the enumeration for standard parameter types.
 - This does not include custom message types. Names are prefixed PT_.
 -
 -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 -->

<xsl:template name='EnumerationDefinitionsParameters'>

/*
 * Standard parameter TypeNum enumeration definition.
 */

enum EParameterType
{
<xsl:for-each select='LL:parameterDefinition'>
    PT_<xsl:value-of select='@name'/> = <xsl:value-of select='@typeNum'/>,</xsl:for-each>
};

</xsl:template>



<!--=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 -
 - @brief EnumerationDefinitionsVendors template
 -
 - Invoked by top level template.
 -
 - Generates definitions of the enumeration for vendor PEN's (IDs)
 - used in CUSTOM_MESSAGE and Custom parameter.
 -
 -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 -->

<xsl:template name='EnumerationDefinitionsVendors'>

/*
 * Vendor ID (PEN, Private Enterprise Number) definition.
 */

enum EVendorID
{
<xsl:for-each select='LL:vendorDefinition' xml:space='preserve'>
    <xsl:value-of select='@name'/> = <xsl:value-of select='@vendorID'/>,</xsl:for-each>
};

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

<xsl:for-each select='LL:enumerationDefinition'>
  <xsl:variable name='enumBaseName' select='@name'/>
enum E<xsl:value-of select='@name'/>
{
<xsl:for-each select='LL:entry' xml:space='preserve'>
    <xsl:value-of select='$enumBaseName'/>_<xsl:value-of select='@name'/> = <xsl:value-of select='@value'/>,</xsl:for-each>
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

<xsl:for-each select='LL:messageDefinition'>
  <xsl:call-template name='ClassDeclarationCommon'>
    <xsl:with-param name='ClassBase'>CMessage</xsl:with-param>
    <xsl:with-param name='ClassName'>C<xsl:value-of select='@name'/></xsl:with-param>
  </xsl:call-template>
</xsl:for-each>

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

<xsl:for-each select='LL:parameterDefinition'>
  <xsl:call-template name='ClassDeclarationCommon'>
    <xsl:with-param name='ClassBase'>CParameter</xsl:with-param>
    <xsl:with-param name='ClassName'>C<xsl:value-of select='@name'/></xsl:with-param>
  </xsl:call-template>
</xsl:for-each>

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
 -
 -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 -->

<xsl:template name='ClassDeclarationCommon' xml:space='preserve'>
  <xsl:param name='ClassBase'/>
  <xsl:param name='ClassName'/>
class <xsl:value-of select='$ClassName'/> : public <xsl:value-of select='$ClassBase'/>
{
  public:
    <xsl:value-of select='$ClassName'/> (void);
    ~<xsl:value-of select='$ClassName'/> (void);

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

    static CElement *
    s_construct (void);

    static void
    s_decodeFields (
      CDecoderStream *          pDecoderStream,
      CElement *                pElement);

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
          <xsl:when test='@enumeration'>E<xsl:value-of select='@enumeration'/></xsl:when>
          <xsl:otherwise>llrp_<xsl:value-of select='@type'/>_t</xsl:otherwise>
        </xsl:choose>
      </xsl:with-param>
      <xsl:with-param name='MemberName'>
        <xsl:choose>
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

  public:
    static const CFieldDescriptor
    s_fd<xsl:value-of select='$BaseName'/>;

    inline <xsl:value-of select='$FieldType'/>
    get<xsl:value-of select='$BaseName'/> (void)
    {
        return <xsl:value-of select='$MemberName'/>;
    }

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
    inline <xsl:value-of select='$NativeType'/> *
    get<xsl:value-of select='$Name'/> (void)
    {
        return m_p<xsl:value-of select='$Name'/>;
    }

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
    inline std::list&lt;<xsl:value-of select='$NativeType'/> *&gt;::iterator
    begin<xsl:value-of select='$Name'/> (void)
    {
        return m_list<xsl:value-of select='$Name'/>.begin();
    }

    inline std::list&lt;<xsl:value-of select='$NativeType'/> *&gt;::iterator
    end<xsl:value-of select='$Name'/> (void)
    {
        return m_list<xsl:value-of select='$Name'/>.end();
    }

    inline void
    clear<xsl:value-of select='$Name'/> (void)
    {
        clearSubParameterList ((tListOfParameters *) &amp;m_list<xsl:value-of select='$Name'/>);
    }

    EResultCode
    add<xsl:value-of select='$Name'/> (
      <xsl:value-of select='$NativeType'/> * pValue);

</xsl:template>


</xsl:stylesheet>
