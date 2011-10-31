<?xml version="1.0" encoding="UTF-8"?>
<!--=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
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
<xsl:call-template name='VendorDescriptors'/>
<xsl:call-template name='NamespaceDescriptors'/>
<xsl:call-template name='EnumerationStringTablesFields'/>
<xsl:call-template name='ClassDefinitionsMessages'/>
<xsl:call-template name='ClassDefinitionsParameters'/>
<xsl:call-template name='ClassDefinitionsChoices'/>
<xsl:call-template name='GenerateEnrollIntoTypeRegistryFunction'/>
</xsl:template>


<!--=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 -
 - @brief FileHeader template
 -
 - Invoked by top level template.
 -
 - Current node
 -      n/a - does not matter for this template
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
 * that is included by a platform specific .cpp source file.
 * That .cpp file takes care of prerequisites needed by this file.
 */


</xsl:template>


<!--=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 -
 - @brief VendorDescriptors template
 -
 - Invoked by top level template.
 -
 - Current node
 -      <llrpdef>
 -
 - Generates the vendor descriptor for each <vendorDefinition>.
 - Such are at the top level and referenced by custom messages
 - and parameters. The primary purpose of the <vendorDefinition>
 - is to establish a short, programming name and the PEN (private
 - enterprise number).
 -
 -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 -->

<xsl:template name='VendorDescriptors'>
/*
 * Vendor descriptors
 */

  <xsl:for-each select='LL:vendorDefinition'>
const CVendorDescriptor
g_vdesc<xsl:value-of select='$RegistryName'/> =
{
  "<xsl:value-of select='@name'/>",	// m_pName
  <xsl:value-of select='@vendorID'/>,	// m_VendorID
};

</xsl:for-each>
</xsl:template>


<!--=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 -
 - @brief NamespaceDescriptors template
 -
 - Invoked by top level template.
 -
 - Current node
 -      <llrpdef>
 -
 - Generates the namespace descriptor for each <namespaceDefinition>.
 - Such are at the top level and referenced by custom messages
 - and parameters. The primary purpose of the <namespaceDefinition>
 - is to establish the XML namespace used for custom elements.
 - Some LTK implementations use the namespace in a way consistent
 - with their methods. This C++ implementation does not.
 -
 -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 -->

<xsl:template name='NamespaceDescriptors'>
/*
 * Namespace descriptors
 */

  <xsl:for-each select='LL:namespaceDefinition'>
const CNamespaceDescriptor
g_nsdesc<xsl:value-of select='@prefix'/> =
{
  "<xsl:value-of select='@prefix'/>",	// m_pPrefix
  "<xsl:value-of select='@URI'/>",	// m_pURI
  "<xsl:value-of select='@schemaLocation'/>",	// m_pSchemaLocation
};

</xsl:for-each>
</xsl:template>


<!--=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 -
 - @brief EnumerationStringTablesFields template
 -
 - Invoked by top level template.
 -
 - Current node
 -      <llrpdef>
 -
 - Generates the enumeration string tables for LLRP fields.
 - These tables are referenced by field descriptors. They are
 - used to facilitate pretty-printing of enumerated LLRP fields,
 - and for converting from textual representations like XML.
 -
 -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 -->

<xsl:template name='EnumerationStringTablesFields'>
/*
 * Enumeration string tables
 */

  <xsl:for-each select='LL:enumerationDefinition|LL:customEnumerationDefinition'>
    <xsl:variable name='enumBaseName' select='@name'/>
const SEnumTableEntry
g_est<xsl:value-of select='$enumBaseName'/>[] =
{
  <xsl:for-each select='LL:entry'>
    {
        "<xsl:value-of select='@name'/>",
        <xsl:value-of select='$enumBaseName'/>_<xsl:value-of select='@name'/>,
    },</xsl:for-each>
    { 0, 0 }
};

</xsl:for-each>
</xsl:template>


<!--=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 -
 - @brief ClassDefinitionsMessages template
 -
 - Invoked by top level template.
 -
 - Current node
 -      <llrpdef>
 -
 - Generates definitions of the message classes: constructors,
 - destructors, accessors, static functions, and static data
 - including type descriptor and field descriptors.
 -
 - This loops through the message definitions, selects
 - important values, and invokes the ClassDefinitionCommon
 - template with the right <xsl:with-param>s.
 -
 -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 -->

<xsl:template name='ClassDefinitionsMessages'>

<xsl:for-each select='LL:messageDefinition'>


/*
*****************************************************************
**
** Message <xsl:value-of select='@name'/>
**
*****************************************************************
*/
  <xsl:call-template name='ClassDefinitionCommon'>
    <xsl:with-param name='ClassBase'>CMessage</xsl:with-param>
    <xsl:with-param name='ClassName'>C<xsl:value-of select='@name'/></xsl:with-param>
    <xsl:with-param name='LLRPName'><xsl:value-of select='@name'/></xsl:with-param>
    <xsl:with-param name='IsMessage'>TRUE</xsl:with-param>
    <xsl:with-param name='TypeNum'><xsl:value-of select='@typeNum'/></xsl:with-param>
    <xsl:with-param name='pVendorDescriptor'>NULL</xsl:with-param>
    <xsl:with-param name='pNamespaceDescriptor'>&amp;g_nsdescllrp</xsl:with-param>
    <xsl:with-param name='pResponseType'>
      <xsl:choose>
        <xsl:when test='@responseType'>&amp;C<xsl:value-of select='@responseType'/>::s_typeDescriptor</xsl:when>
        <xsl:otherwise>NULL</xsl:otherwise>
      </xsl:choose>
    </xsl:with-param>
    <xsl:with-param name='IsCustomParameter'>false</xsl:with-param>
  </xsl:call-template>
</xsl:for-each>

<xsl:for-each select='LL:customMessageDefinition'>


/*
*****************************************************************
**
** Custom message <xsl:value-of select='@name'/>
**
*****************************************************************
*/
  <xsl:call-template name='ClassDefinitionCommon'>
    <xsl:with-param name='ClassBase'>CMessage</xsl:with-param>
    <xsl:with-param name='ClassName'>C<xsl:value-of select='@name'/></xsl:with-param>
    <xsl:with-param name='LLRPName'><xsl:value-of select='@name'/></xsl:with-param>
    <xsl:with-param name='IsMessage'>TRUE</xsl:with-param>
    <xsl:with-param name='TypeNum'><xsl:value-of select='@subtype'/></xsl:with-param>
    <xsl:with-param name='pVendorDescriptor'>&amp;g_vdesc<xsl:value-of select='$RegistryName'/></xsl:with-param>
    <xsl:with-param name='pNamespaceDescriptor'>&amp;g_nsdesc<xsl:value-of select='@namespace'/></xsl:with-param>
    <xsl:with-param name='pResponseType'>
      <xsl:choose>
        <xsl:when test='@responseType'>&amp;C<xsl:value-of select='@responseType'/>::s_typeDescriptor</xsl:when>
        <xsl:otherwise>NULL</xsl:otherwise>
      </xsl:choose>
    </xsl:with-param>
    <xsl:with-param name='IsCustomParameter'>false</xsl:with-param>
  </xsl:call-template>
</xsl:for-each>

</xsl:template>


<!--=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 -
 - @brief ClassDefinitionsParameters template
 -
 - Invoked by top level template.
 - Current node
 -      <llrpdef>
 -
 - Generates definitions of the parameter classes: constructors,
 - destructors, accessors, static functions, and static data
 - including type descriptor and field descriptors.
 -
 - This loops through the parameter definitions, selects
 - important values, and invokes the ClassDefinitionCommon
 - template with the right <xsl:with-param>s.
 -
 -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 -->

<xsl:template name='ClassDefinitionsParameters'>

<xsl:for-each select='LL:parameterDefinition'>


/*
*****************************************************************
**
** Parameter <xsl:value-of select='@name'/>
**
*****************************************************************
*/
  <xsl:call-template name='ClassDefinitionCommon'>
    <xsl:with-param name='ClassBase'>CParameter</xsl:with-param>
    <xsl:with-param name='ClassName'>C<xsl:value-of select='@name'/></xsl:with-param>
    <xsl:with-param name='LLRPName'><xsl:value-of select='@name'/></xsl:with-param>
    <xsl:with-param name='IsMessage'>FALSE</xsl:with-param>
    <xsl:with-param name='TypeNum'><xsl:value-of select='@typeNum'/></xsl:with-param>
    <xsl:with-param name='pVendorDescriptor'>NULL</xsl:with-param>
    <xsl:with-param name='pNamespaceDescriptor'>&amp;g_nsdescllrp</xsl:with-param>
    <xsl:with-param name='pResponseType'>NULL</xsl:with-param>
    <xsl:with-param name='IsCustomParameter'>false</xsl:with-param>
  </xsl:call-template>
</xsl:for-each>

<xsl:for-each select='LL:customParameterDefinition'>


/*
*****************************************************************
**
** Custom parameter <xsl:value-of select='@name'/>
**
*****************************************************************
*/
  <xsl:call-template name='ClassDefinitionCommon'>
    <xsl:with-param name='ClassBase'>CParameter</xsl:with-param>
    <xsl:with-param name='ClassName'>C<xsl:value-of select='@name'/></xsl:with-param>
    <xsl:with-param name='LLRPName'><xsl:value-of select='@name'/></xsl:with-param>
    <xsl:with-param name='IsMessage'>FALSE</xsl:with-param>
    <xsl:with-param name='TypeNum'><xsl:value-of select='@subtype'/></xsl:with-param>
    <xsl:with-param name='pVendorDescriptor'>&amp;g_vdesc<xsl:value-of select='$RegistryName'/></xsl:with-param>
    <xsl:with-param name='pNamespaceDescriptor'>&amp;g_nsdesc<xsl:value-of select='@namespace'/></xsl:with-param>
    <xsl:with-param name='pResponseType'>NULL</xsl:with-param>
    <xsl:with-param name='IsCustomParameter'>true</xsl:with-param>
  </xsl:call-template>

llrp_bool_t
C<xsl:value-of select='@name'/>::isAllowedIn (
  const CTypeDescriptor *       pEnclosingElementType) const
{
  <xsl:for-each select='LL:allowedIn'>
    if(pEnclosingElementType == &amp;C<xsl:value-of select='@type'/>::s_typeDescriptor)
    {
        return TRUE;
    }
  </xsl:for-each>
    return FALSE;
}

</xsl:for-each>

</xsl:template>


<!--=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 -
 - @brief ClassDefinitionCommon template
 -
 - Invoked by templates
 -      ClassDefinitionsMessages
 -      ClassDefinitionsParameters
 -
 - Current node
 -      <llrpdef><messageDefinition>
 -      <llrpdef><parameterDefinition>
 -
 - @param   ClassBase       Name of the base class, CMessage
 -                          or CParameter usually
 - @param   ClassName       Name of generated class. This already has
 -                          "C" prefixed to the LLRP name.
 - @param   LLRPName        The original, LLRP name for the class
 - @param   IsMessage       Either TRUE or FALSE
 - @param   TypeNum         Prefix to BaseName to pair with the
 -                          generated enum of message/param types
 - @param   pVendorDescriptor The pointer expression for this parameters
 -                          vendor descriptor and, thereby, PEN for
 -                          custom elements (messages/parameters).
 -                          Usually NULL for standard LLRP elements.
 - @param   pNamespaceDescriptor The pointer expression for this parameters
 -                          namespace descriptor. Usually LLRP_nsdescllrp
 -                          for standard parameters and messages.
 - @param   pResponseType   The pointer expression for this messages
 -                          response counterpart.
 - @param   IsCustomParameter Either "true" or "false".
 -
 -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 -->

<xsl:template name='ClassDefinitionCommon'>
  <xsl:param name='ClassBase'/>
  <xsl:param name='ClassName'/>
  <xsl:param name='LLRPName'/>
  <xsl:param name='IsMessage'/>
  <xsl:param name='TypeNum'/>
  <xsl:param name='pVendorDescriptor'/>
  <xsl:param name='pNamespaceDescriptor'/>
  <xsl:param name='pResponseType'/>
  <xsl:param name='IsCustomParameter'/>

  <xsl:call-template name='ClassDefnTypeDescriptor'>
    <xsl:with-param name='ClassBase'><xsl:value-of select='$ClassBase'/></xsl:with-param>
    <xsl:with-param name='ClassName'><xsl:value-of select='$ClassName'/></xsl:with-param>
    <xsl:with-param name='LLRPName'><xsl:value-of select='$LLRPName'/></xsl:with-param>
    <xsl:with-param name='IsMessage'><xsl:value-of select='$IsMessage'/></xsl:with-param>
    <xsl:with-param name='TypeNum'><xsl:value-of select='$TypeNum'/></xsl:with-param>
    <xsl:with-param name='pVendorDescriptor'><xsl:value-of select='$pVendorDescriptor'/></xsl:with-param>
    <xsl:with-param name='pNamespaceDescriptor'><xsl:value-of select='$pNamespaceDescriptor'/></xsl:with-param>
    <xsl:with-param name='pResponseType'><xsl:value-of select='$pResponseType'/></xsl:with-param>
    <xsl:with-param name='IsCustomParameter'><xsl:value-of select='$IsCustomParameter'/></xsl:with-param>
  </xsl:call-template>

  <xsl:call-template name='ClassDefnFieldDescriptorTable'>
    <xsl:with-param name='ClassName'><xsl:value-of select='$ClassName'/></xsl:with-param>
  </xsl:call-template>

  <xsl:call-template name='ClassDefnFieldDescriptors'>
    <xsl:with-param name='ClassName'><xsl:value-of select='$ClassName'/></xsl:with-param>
  </xsl:call-template>

  <xsl:call-template name='DefaultConstructor'>
    <xsl:with-param name='ClassName'><xsl:value-of select='$ClassName'/></xsl:with-param>
  </xsl:call-template>

  <xsl:call-template name='DefaultDestructor'>
    <xsl:with-param name='ClassName'><xsl:value-of select='$ClassName'/></xsl:with-param>
  </xsl:call-template>

  <xsl:call-template name='StaticConstructFunction'>
    <xsl:with-param name='ClassName'><xsl:value-of select='$ClassName'/></xsl:with-param>
  </xsl:call-template>

  <xsl:call-template name='StaticDecodeFieldsFunction'>
    <xsl:with-param name='ClassName'><xsl:value-of select='$ClassName'/></xsl:with-param>
  </xsl:call-template>

  <xsl:call-template name='MemberDecodeFieldsFunction'>
    <xsl:with-param name='ClassName'><xsl:value-of select='$ClassName'/></xsl:with-param>
  </xsl:call-template>

  <xsl:call-template name='AssimilateSubParametersFunction'>
    <xsl:with-param name='ClassName'><xsl:value-of select='$ClassName'/></xsl:with-param>
  </xsl:call-template>

  <xsl:call-template name='SubParameterAccessorFunctions'>
    <xsl:with-param name='ClassName'><xsl:value-of select='$ClassName'/></xsl:with-param>
  </xsl:call-template>

  <xsl:call-template name='EncodeFunction'>
    <xsl:with-param name='ClassName'><xsl:value-of select='$ClassName'/></xsl:with-param>
  </xsl:call-template>

</xsl:template>


<!--=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 -
 - @brief ClassDefinitionsChoices template
 -
 - Invoked by top level template.
 - Current node
 -      <llrpdef>
 -
 - Generates definitions of the choice elements: isMember,
 - type descriptor, etc.
 -
 - This loops through the choice definitions, selects
 - important values, and invokes the ClassDefinitionCommon
 - template with the right <xsl:with-param>s.
 -
 -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 -->

<xsl:template name='ClassDefinitionsChoices'>

<xsl:for-each select='LL:choiceDefinition|LL:customChoiceDefinition'>


/*
*****************************************************************
**
** Choice <xsl:value-of select='@name'/>
**
*****************************************************************
*/
  <xsl:call-template name='ClassDefinitionOneChoice'>
    <xsl:with-param name='LLRPName'><xsl:value-of select='@name'/></xsl:with-param>
    <xsl:with-param name='pVendorDescriptor'>NULL</xsl:with-param>
  </xsl:call-template>
</xsl:for-each>

</xsl:template>


<!--=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 -
 - @brief ClassDefinitionOneChoice template
 -
 - Invoked by templates
 -      ClassDefinitionsChoices
 -
 - Current node
 -      <llrpdef><choiceDefinition>
 -
 - @param   LLRPName        The original, LLRP name for the element
 - @param   pVendorDescriptor The pointer expression for this parameters
 -                          vendor descriptor and, thereby, PEN for
 -                          custom elements (messages/parameters).
 -                          Usually NULL for stanrd LLRP elements.
 -
 -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 -->

<xsl:template name='ClassDefinitionOneChoice'>
  <xsl:param name='LLRPName'/>
  <xsl:param name='pVendorDescriptor'/>

const CTypeDescriptor
C<xsl:value-of select='$LLRPName'/>::s_typeDescriptor =
{
    FALSE,		// m_bIsMessage
    "<xsl:value-of select='$LLRPName'/>",	// m_pName
    <xsl:value-of select='$pVendorDescriptor'/>,	// m_pVendorDescriptor
    NULL,	// m_pNamespaceDescriptor
    (llrp_u32_t)(-1),	// m_TypeNum
    NULL,   // m_ppFieldDescriptorTable
    NULL,                // m_pfConstruct
    NULL,             // m_pfDecodeFields
};

llrp_bool_t
C<xsl:value-of select='$LLRPName'/>::isMember (
  CParameter *                  pParameter)
{
    const CTypeDescriptor *     pType;

    pType = pParameter-&gt;m_pType;

  <xsl:for-each select='LL:parameter'>
    <xsl:choose>
      <xsl:when test='@type = "Custom"'>
    if(pParameter->isAllowedExtension(&amp;s_typeDescriptor))
    {
        return TRUE;
    }
      </xsl:when>
      <xsl:otherwise>
    if(&amp;C<xsl:value-of select='@type'/>::s_typeDescriptor == pType)
    {
        return TRUE;
    }
      </xsl:otherwise>
    </xsl:choose>
</xsl:for-each>

    return FALSE;
}

</xsl:template>


<!--=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 -
 - @brief ClassDefnTypeDescriptor template
 -
 - Invoked by templates
 -      ClassDefinitionCommon
 -
 - Current node
 -      <llrpdef><messageDefinition>
 -      <llrpdef><parameterDefinition>
 -
 - @param   ClassBase       Name of the base class, CMessage
 -                          or CParameter usually
 - @param   ClassName       Name of generated class. This already has
 -                          "C" prefixed to the LLRP name.
 - @param   LLRPName        The original, LLRP name for the class
 - @param   IsMessage       Either TRUE or FALSE
 - @param   TypeNum         Prefix to BaseName to pair with the
 -                          generated enum of message/param types
 - @param   pVendorDescriptor The pointer expression for this parameters
 -                          vendor descriptor and, thereby, PEN for
 -                          custom elements (messages/parameters).
 -                          Usually NULL for standard LLRP elements.
 - @param   pNamespaceDescriptor The pointer expression for this parameters
 -                          namespace descriptor. Usually g_nsdescllrp
 -                          for standard parameters and messages.
 - @param   pResponseType   The pointer expression for this messages
 -                          response counterpart.
 - @param   IsCustomParameter Either "true" or "false".
 -
 -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 -->

<xsl:template name='ClassDefnTypeDescriptor'>
  <xsl:param name='ClassBase'/>
  <xsl:param name='ClassName'/>
  <xsl:param name='LLRPName'/>
  <xsl:param name='IsMessage'/>
  <xsl:param name='TypeNum'/>
  <xsl:param name='pVendorDescriptor'/>
  <xsl:param name='pNamespaceDescriptor'/>
  <xsl:param name='pResponseType'/>
  <xsl:param name='IsCustomParameter'/>

const CTypeDescriptor
<xsl:value-of select='$ClassName'/>::s_typeDescriptor =
{
    <xsl:value-of select='$IsMessage'/>,		// m_bIsMessage
    "<xsl:value-of select='$LLRPName'/>",	// m_pName
    <xsl:value-of select='$pVendorDescriptor'/>,	// m_pVendorDescriptor
    <xsl:value-of select='$pNamespaceDescriptor'/>,	// m_pNamespaceDescriptor
    <xsl:value-of select='$TypeNum'/>,	// m_TypeNum
    <xsl:value-of select='$pResponseType'/>,    // m_pResponseType

    s_apFieldDescriptorTable,   // m_ppFieldDescriptorTable
    s_construct,                // m_pfConstruct
    s_decodeFields,             // m_pfDecodeFields
};

</xsl:template>


<!--=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 -
 - @brief ClassDefnFieldDescriptorTable template
 -
 - Invoked by templates
 -      ClassDefinitionCommon
 -
 - Current node
 -      <llrpdef><messageDefinition>
 -      <llrpdef><parameterDefinition>
 -
 - @param   ClassName       Name of generated class. This already has
 -                          "C" prefixed to the LLRP name.
 -
 -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 -->

<xsl:template name='ClassDefnFieldDescriptorTable'>
  <xsl:param name='ClassName'/>

const CFieldDescriptor * const
<xsl:value-of select='$ClassName'/>::s_apFieldDescriptorTable[] =
{
  <xsl:for-each select='LL:field'>
    &amp;s_fd<xsl:value-of select='@name'/>,</xsl:for-each>
    NULL
};

</xsl:template>


<!--=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 -
 - @brief ClassDefnFieldDescriptors template
 -
 - Invoked by templates
 -      ClassDefinitionCommon
 -
 - Current node
 -      <llrpdef><messageDefinition>
 -      <llrpdef><parameterDefinition>
 -
 - @param   ClassName       Name of generated class. This already has
 -                          "C" prefixed to the LLRP name.
 -
 -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 -->

<xsl:template name='ClassDefnFieldDescriptors'>
  <xsl:param name='ClassName'/>
  <xsl:for-each select='LL:field'>
    <xsl:choose>
      <xsl:when test='@enumeration'>
        <xsl:call-template name='ClassDefnFieldDescEnum'>
          <xsl:with-param name='ClassName'><xsl:value-of select='$ClassName'/></xsl:with-param>
          <xsl:with-param name='FieldBaseName'><xsl:value-of select='@name'/></xsl:with-param>
          <xsl:with-param name='FieldBaseType'><xsl:value-of select='@type'/></xsl:with-param>
          <xsl:with-param name='Enumeration'><xsl:value-of select='@enumeration'/></xsl:with-param>
        </xsl:call-template>
      </xsl:when>
      <xsl:otherwise>
        <xsl:call-template name='ClassDefnFieldDescPlain'>
          <xsl:with-param name='ClassName'><xsl:value-of select='$ClassName'/></xsl:with-param>
          <xsl:with-param name='FieldBaseName'><xsl:value-of select='@name'/></xsl:with-param>
          <xsl:with-param name='FieldBaseType'><xsl:value-of select='@type'/></xsl:with-param>
          <xsl:with-param name='FieldFormat'>
            <xsl:choose>
              <xsl:when test='@format'>
                <xsl:value-of select='@format'/>
              </xsl:when>
              <xsl:otherwise>Normal</xsl:otherwise>
            </xsl:choose>
          </xsl:with-param>
        </xsl:call-template>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:for-each>
</xsl:template>


<!--=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 -
 - @brief ClassDefnFieldDescEnum template
 -
 - Invoked by templates
 -      ClassDefnFieldDescriptor
 -
 - Current node
 -      <llrpdef><messageDefinition><field enumeration=...>
 -      <llrpdef><parameterDefinition><field enumeration=...>
 -
 - This determines the field type (FT_Exx) then invokes template
 - ClassDefnFieldDescCommon with appropriate <xsl:with-param>'s
 -
 - In all cases:
 -      The derived MemberName is "m_e$FieldBaseName", note the "e" for enum
 -      The derived EnumStrTable is "g_est$Enumeration"
 -      $ClassName and $FieldBaseName are passed unchanged
 -
 - Each case determines the FieldEnumType that is mapped
 - u1=>FT_E1, u2=>FT_E2, u8=>FT_E8, u16=>FT_E16, u32=>FT_E32
 -
 - @param   ClassName       Name of generated class. This already has
 -                          "C" prefixed to the LLRP name.
 - @param   FieldBaseName   The original, LLRP name for the field
 - @param   FieldBaseType   The type, for enums it is one of
 -                          u1, u2, u8, u16, or u32
 - @param   Enumeration     The enumeration base name
 -
 -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 -->

<xsl:template name='ClassDefnFieldDescEnum'>
  <xsl:param name='ClassName'/>
  <xsl:param name='FieldBaseName'/>
  <xsl:param name='FieldBaseType'/>
  <xsl:param name='Enumeration'/>
  <xsl:variable name='FieldEnumType'>
    <xsl:choose>
      <xsl:when test='$FieldBaseType="u1"'  >FT_E1</xsl:when>
      <xsl:when test='$FieldBaseType="u2"'  >FT_E2</xsl:when>
      <xsl:when test='$FieldBaseType="u8"'  >FT_E8</xsl:when>
      <xsl:when test='$FieldBaseType="u16"' >FT_E16</xsl:when>
      <xsl:when test='$FieldBaseType="u32"' >FT_E32</xsl:when>
      <xsl:when test='$FieldBaseType="u8v"' >FT_E8V</xsl:when>
      <xsl:otherwise>FT_EBOGOSITY</xsl:otherwise>
    </xsl:choose>
  </xsl:variable>
  <xsl:call-template name='ClassDefnFieldDescCommon'>
    <xsl:with-param name='ClassName'><xsl:value-of select='$ClassName'/></xsl:with-param>
    <xsl:with-param name='MemberName'>m_e<xsl:value-of select='$FieldBaseName'/></xsl:with-param>
    <xsl:with-param name='FieldBaseName'><xsl:value-of select='$FieldBaseName'/></xsl:with-param>
    <xsl:with-param name='FieldEnumType'><xsl:value-of select='$FieldEnumType'/></xsl:with-param>
    <xsl:with-param name='FieldEnumFormat'>FMT_NORMAL</xsl:with-param>
    <xsl:with-param name='EnumStrTable'>g_est<xsl:value-of select='$Enumeration'/></xsl:with-param>
  </xsl:call-template>
</xsl:template>


<!--=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 -
 - @brief ClassDefnFieldDescPlain template
 -
 - Invoked by templates
 -      ClassDefnFieldDescriptor
 -
 - Current node
 -      <llrpdef><messageDefinition><field>
 -      <llrpdef><parameterDefinition><field>
 -
 - This determines the field type (FT_xx) then invokes template
 - ClassDefnFieldDescCommon with appropriate <xsl:with-param>'s
 -
 - The derived MemberName is "m_$FieldBaseName"
 - The EnumStrTable is NULL
 - $ClassName and $FieldBaseName are passed unchanged
 - The FieldEnumType is derived by mapping $FieldBaseType (@type)
 -
 - @param   ClassName       Name of generated class. This already has
 -                          "C" prefixed to the LLRP name.
 - @param   FieldBaseName   The original, LLRP name for the field
 - @param   FieldBaseType   The type, something like {u,s}{8,16,32,64}[v]
 - @param   FieldFormat     The format, something like Normal, Hex, Datetime
 -
 -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 -->

<xsl:template name='ClassDefnFieldDescPlain'>
  <xsl:param name='ClassName'/>
  <xsl:param name='FieldBaseName'/>
  <xsl:param name='FieldBaseType'/>
  <xsl:param name='FieldFormat'/>
  <xsl:variable name='FieldEnumType'>
    <xsl:choose>
      <xsl:when test='$FieldBaseType="u8"'  >FT_U8</xsl:when>
      <xsl:when test='$FieldBaseType="s8"'  >FT_S8</xsl:when>
      <xsl:when test='$FieldBaseType="u8v"' >FT_U8V</xsl:when>
      <xsl:when test='$FieldBaseType="s8v"' >FT_S8V</xsl:when>
      <xsl:when test='$FieldBaseType="u16"' >FT_U16</xsl:when>
      <xsl:when test='$FieldBaseType="s16"' >FT_S16</xsl:when>
      <xsl:when test='$FieldBaseType="u16v"'>FT_U16V</xsl:when>
      <xsl:when test='$FieldBaseType="s16v"'>FT_S16V</xsl:when>
      <xsl:when test='$FieldBaseType="u32"' >FT_U32</xsl:when>
      <xsl:when test='$FieldBaseType="s32"' >FT_S32</xsl:when>
      <xsl:when test='$FieldBaseType="u32v"'>FT_U32V</xsl:when>
      <xsl:when test='$FieldBaseType="s32v"'>FT_S32V</xsl:when>
      <xsl:when test='$FieldBaseType="u64"' >FT_U64</xsl:when>
      <xsl:when test='$FieldBaseType="s64"' >FT_S64</xsl:when>
      <xsl:when test='$FieldBaseType="u64v"'>FT_U64V</xsl:when>
      <xsl:when test='$FieldBaseType="s64v"'>FT_S64V</xsl:when>
      <xsl:when test='$FieldBaseType="u1"'  >FT_U1</xsl:when>
      <xsl:when test='$FieldBaseType="u2"'  >FT_U2</xsl:when>
      <xsl:when test='$FieldBaseType="u1v"' >FT_U1V</xsl:when>
      <xsl:when test='$FieldBaseType="u96"' >FT_U96</xsl:when>
      <xsl:when test='$FieldBaseType="utf8v"' >FT_UTF8V</xsl:when>
      <xsl:when test='$FieldBaseType="bytesToEnd"' >FT_BYTESTOEND</xsl:when>
      <xsl:otherwise>FT_BOGOSITY</xsl:otherwise>
    </xsl:choose>
  </xsl:variable>
  <xsl:variable name='FieldEnumFormat'>
    <xsl:choose>
      <xsl:when test='$FieldFormat="Normal"'  >FMT_NORMAL</xsl:when>
      <xsl:when test='$FieldFormat="Dec"'     >FMT_DEC</xsl:when>
      <xsl:when test='$FieldFormat="Hex"'     >FMT_HEX</xsl:when>
      <xsl:when test='$FieldFormat="Datetime"'>FMT_DATETIME</xsl:when>
      <xsl:when test='$FieldFormat="UTF8"'    >FMT_UTF8</xsl:when>
      <xsl:otherwise>FMT_BOGOSITY</xsl:otherwise>
    </xsl:choose>
  </xsl:variable>
  <xsl:call-template name='ClassDefnFieldDescCommon'>
    <xsl:with-param name='ClassName'><xsl:value-of select='$ClassName'/></xsl:with-param>
    <xsl:with-param name='MemberName'>m_<xsl:value-of select='$FieldBaseName'/></xsl:with-param>
    <xsl:with-param name='FieldBaseName'><xsl:value-of select='$FieldBaseName'/></xsl:with-param>
    <xsl:with-param name='FieldEnumType'><xsl:value-of select='$FieldEnumType'/></xsl:with-param>
    <xsl:with-param name='FieldEnumFormat'><xsl:value-of select='$FieldEnumFormat'/></xsl:with-param>
    <xsl:with-param name='EnumStrTable'>NULL</xsl:with-param>
  </xsl:call-template>
</xsl:template>


<!--=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 -
 - @brief ClassDefnFieldDescCommon template
 -
 - Invoked by templates
 -      ClassDefnFieldDescEnum
 -      ClassDefnFieldDescPlain
 -
 - Current node
 -      <llrpdef><messageDefinition><field>
 -      <llrpdef><parameterDefinition><field>
 -
 - @param   ClassName       Name of generated class. This already has
 -                          "C" prefixed to the LLRP name.
 - @param   MemberName      The name of the class variable,
 -                          m_Something or m_eSomething
 - @param   FieldBaseName   The original, LLRP name for the field
 - @param   FieldEnumType   The enum symbol from
 -                          CFieldDescriptor::FieldType
 - @param   EnumStrTable    The initializer for the enum string table
 -                          base pointer
 -
 -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 -->

<xsl:template name='ClassDefnFieldDescCommon'>
  <xsl:param name='ClassName'/>
  <xsl:param name='MemberName'/>
  <xsl:param name='FieldBaseName'/>
  <xsl:param name='FieldEnumType'/>
  <xsl:param name='FieldEnumFormat'/>
  <xsl:param name='EnumStrTable'/>

const CFieldDescriptor
<xsl:value-of select='$ClassName'/>::s_fd<xsl:value-of select='$FieldBaseName'/> =
{
    CFieldDescriptor::<xsl:value-of select='$FieldEnumType'/>,	// m_eFieldType
    CFieldDescriptor::<xsl:value-of select='$FieldEnumFormat'/>,	// m_eFieldFormat
    "<xsl:value-of select='$FieldBaseName'/>",	// m_pName
    <xsl:value-of select='$EnumStrTable'/>	// m_pEnumTable
};
</xsl:template>


<!--=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 -
 - @brief DefaultConstructor template
 -
 - Invoked by templates
 -      ClassDefinitionCommon
 -
 - Current node
 -      <llrpdef><messageDefinition>
 -      <llrpdef><parameterDefinition>
 -
 - Assign 0 to:
 -      simple (non-array) fields, including enumerated values
 -      parameter and choice pointers for repeats "1" and "0-1"
 -
 - Array-type fields and lists are initialized by their default
 - constructors.
 -
 - @param   ClassName       Name of generated class. This already has
 -                          "C" prefixed to the LLRP name.
 -
 -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 -->

<xsl:template name='DefaultConstructor'>
  <xsl:param name='ClassName'/>
<xsl:value-of select='$ClassName'/>::<xsl:value-of select='$ClassName'/> (void)
{
    m_pType = &amp;s_typeDescriptor;
  <xsl:call-template name='DefaultConsFields'/>
  <xsl:call-template name='DefaultConsParamPtrs'/>
}

</xsl:template>


<!--=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 -
 - @brief DefaultConsFields template
 -
 - Invoked by templates
 -      DefaultConstructor
 -
 - Current node
 -      <llrpdef><messageDefinition>
 -      <llrpdef><parameterDefinition>
 -
 - Assign 0 to simple (non-array) fields, including enumerated values.
 - The only tricky bit is recognizing and properly casting enumerations.
 - Array-type fields are initialized by their default constructors.
 -
 -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 -->

<xsl:template name='DefaultConsFields'>
  <xsl:for-each select='LL:field'>
    <xsl:choose>
      <xsl:when test='@enumeration and @type != "u8v"'>
    m_e<xsl:value-of select='@name'/> = (E<xsl:value-of select='@enumeration'/>) 0;</xsl:when>
      <xsl:when test='@type="u1" or @type="u2"
            or @type="u8" or @type="u16" or @type="u32" or @type="u64"
            or @type="s8" or @type="s16" or @type="s32" or @type="s64"'>
    m_<xsl:value-of select='@name'/> = 0;</xsl:when>
    </xsl:choose>
  </xsl:for-each>
</xsl:template>


<!--=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 -
 - @brief DefaultConsParamPtrs template
 -
 - Invoked by templates
 -      DefaultConstructor
 -
 - Current node
 -      <llrpdef><messageDefinition>
 -      <llrpdef><parameterDefinition>
 -
 - Assign NULL to parameter and choice pointers. Such pointers
 - are used for parameter/choice references with repeats "1" and "0-1".
 - For repeats "0-N" and "1-N" std::list's are used and the default
 - constructor for those takes care of initialization. The only
 - tricky part is checking for name= is the parameter/choice
 - reference.
 -
 -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 -->

<xsl:template name='DefaultConsParamPtrs'>
  <xsl:for-each select='LL:parameter|LL:choice'>
    <xsl:choose>
      <xsl:when test='@repeat="1" or @repeat="0-1"'>
        <xsl:choose>
          <xsl:when test='@name'>
    m_p<xsl:value-of select='@name'/> = NULL;</xsl:when>
          <xsl:otherwise>
    m_p<xsl:value-of select='@type'/> = NULL;</xsl:otherwise>
        </xsl:choose>
      </xsl:when>
    </xsl:choose>
  </xsl:for-each>
</xsl:template>


<!--=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 -
 - @brief DefaultDestructor template
 -
 - Invoked by templates
 -      ClassDefinitionCommon
 -
 - Current node
 -      <llrpdef><messageDefinition>
 -      <llrpdef><parameterDefinition>
 -
 - Nothing to do, really. All lists, array-type fields, etc, are
 - handled by the compiler generated destructor. All we have to do
 - is make an empty function.
 -
 - @param   ClassName       Name of generated class. This already has
 -                          "C" prefixed to the LLRP name.
 -
 -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 -->

<xsl:template name='DefaultDestructor'>
  <xsl:param name='ClassName'/>
<xsl:value-of select='$ClassName'/>::~<xsl:value-of select='$ClassName'/> (void)
{
}

</xsl:template>


<!--=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 -
 - @brief StaticConstructFunction template
 -
 - Invoked by templates
 -      ClassDefinitionCommon
 -
 - Current node
 -      <llrpdef><messageDefinition>
 -      <llrpdef><parameterDefinition>
 -
 - Generate the s_construct() function that can be called through
 - the type registry.
 -
 - @param   ClassName       Name of generated class. This already has
 -                          "C" prefixed to the LLRP name.
 -
 -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 -->

<xsl:template name='StaticConstructFunction'>
  <xsl:param name='ClassName'/>
CElement *
<xsl:value-of select='$ClassName'/>::s_construct (void)
{
    return (CElement *) new <xsl:value-of select='$ClassName'/>();
}

</xsl:template>


<!--=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 -
 - @brief StaticDecodeFieldsFunction template
 -
 - Invoked by templates
 -      ClassDefinitionCommon
 -
 - Current node
 -      <llrpdef><messageDefinition>
 -      <llrpdef><parameterDefinition>
 -
 - Generate the s_decodeFields() function that can be called through
 - the type registry. The pTargetElement can be NULL for when we want
 - to just use a decoder as a pretty-printer of an external (eg binary
 - frame) representation without creating any object instances.
 -
 - @param   ClassName       Name of generated class. This already has
 -                          "C" prefixed to the LLRP name.
 -
 -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 -->

<xsl:template name='StaticDecodeFieldsFunction'>
  <xsl:param name='ClassName'/>
void
<xsl:value-of select='$ClassName'/>::s_decodeFields (
  CDecoderStream *              pDecoderStream,
  CElement *                    pTargetElement)
{
    <xsl:value-of select='$ClassName'/> * pTarget = (<xsl:value-of select='$ClassName'/> *) pTargetElement;

  <xsl:for-each select='LL:field|LL:reserved'>
    <xsl:choose>
      <xsl:when test='self::LL:field'>
        <xsl:call-template name='DecodeOneField'/>
      </xsl:when>
      <xsl:when test='self::LL:reserved'>
        <xsl:call-template name='DecodeOneReserved'/>
      </xsl:when>
    </xsl:choose>
  </xsl:for-each>
}

</xsl:template>


<!--=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 -
 - @brief DecodeOneField template
 -
 - Invoked by templates
 -      StaticDecodeFieldsFunction
 -
 - Current node
 -      <llrpdef><messageDefinition><field>
 -      <llrpdef><parameterDefinition><field>
 -
 -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 -->

<xsl:template name='DecodeOneField'>
  <xsl:choose>
    <xsl:when test='@enumeration and @type = "u8v"'>
    if(NULL != pTarget)
    {
        pTarget->m_<xsl:value-of select='@name'/> = pDecoderStream->get_e8v(&amp;s_fd<xsl:value-of select='@name'/>);
    }
    else
    {
        pDecoderStream->get_e8v(&amp;s_fd<xsl:value-of select='@name'/>);
    }
    </xsl:when>
    <xsl:when test='@enumeration'>
      <xsl:call-template name='DecodeOneFieldEnum'/>
    </xsl:when>
    <xsl:otherwise>
    if(NULL != pTarget)
    {
        pTarget->m_<xsl:value-of select='@name'/> = pDecoderStream->get_<xsl:value-of select='@type'/>(&amp;s_fd<xsl:value-of select='@name'/>);
    }
    else
    {
        pDecoderStream->get_<xsl:value-of select='@type'/>(&amp;s_fd<xsl:value-of select='@name'/>);
    }
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>


<!--=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 -
 - @brief DecodeOneFieldEnum template
 -
 - Invoked by templates
 -      DecodeOneField
 -
 - Current node
 -      <llrpdef><messageDefinition><field enumeration=...>
 -      <llrpdef><parameterDefinition><field enumeration=...>
 -
 -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 -->

<xsl:template name='DecodeOneFieldEnum'>
  <xsl:variable name='eType'>
    <xsl:choose>
      <xsl:when test='@type="u1"'>e1</xsl:when>
      <xsl:when test='@type="u2"'>e2</xsl:when>
      <xsl:when test='@type="u8"'>e8</xsl:when>
      <xsl:when test='@type="u16"'>e16</xsl:when>
      <xsl:when test='@type="u32"'>e32</xsl:when>
      <xsl:when test='@type="u8v"'>e8v</xsl:when>
      <xsl:otherwise>bogus</xsl:otherwise>
    </xsl:choose>
  </xsl:variable>
    if(NULL != pTarget)
    {
        pTarget->m_e<xsl:value-of select='@name'/> = (E<xsl:value-of select='@enumeration'/>) pDecoderStream->get_<xsl:value-of select='$eType'/>(&amp;s_fd<xsl:value-of select='@name'/>);
    }
    else
    {
        pDecoderStream->get_<xsl:value-of select='$eType'/>(&amp;s_fd<xsl:value-of select='@name'/>);
    }
</xsl:template>


<!--=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 -
 - @brief DecodeOneReserved template
 -
 - Invoked by templates
 -      StaticDecodeFieldsFunction
 -
 - Current node
 -      <llrpdef><messageDefinition><reserved>
 -      <llrpdef><parameterDefinition><reserved>
 -
 -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 -->

<xsl:template name='DecodeOneReserved'>
    pDecoderStream->get_reserved(<xsl:value-of select='@bitCount'/>);
</xsl:template>


<!--=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 -
 - @brief MemberDecodeFieldsFunction template
 -
 - Invoked by templates
 -      ClassDefinitionCommon
 -
 - Current node
 -      <llrpdef><messageDefinition>
 -      <llrpdef><parameterDefinition>
 -
 - Generate the decodeFields() member function. All it does is
 - call s_decodeFields passing the 'this' pointer.
 -
 - @param   ClassName       Name of generated class. This already has
 -                          "C" prefixed to the LLRP name.
 -
 -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 -->

<xsl:template name='MemberDecodeFieldsFunction'>
  <xsl:param name='ClassName'/>
void
<xsl:value-of select='$ClassName'/>::decodeFields (
  CDecoderStream *              pDecoderStream)
{
    s_decodeFields(pDecoderStream, (CElement *)this);
}

</xsl:template>


<!--=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 -
 - @brief AssimilateSubParametersFunction template
 -
 - Invoked by templates
 -      ClassDefinitionCommon
 -
 - Current node
 -      <llrpdef><messageDefinition>
 -      <llrpdef><parameterDefinition>
 -
 - @param   ClassName       Name of generated class. This already has
 -                          "C" prefixed to the LLRP name.
 -
 -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 -->

<xsl:template name='AssimilateSubParametersFunction'>
  <xsl:param name='ClassName'/>
void
<xsl:value-of select='$ClassName'/>::assimilateSubParameters (
  CErrorDetails *               pError)
{
    tListOfParameters::iterator Cur = m_listAllSubParameters.begin();
    tListOfParameters::iterator End = m_listAllSubParameters.end();
    const CTypeDescriptor *     pType;

  <xsl:for-each select='LL:parameter|LL:choice'>
    <xsl:choose>
      <xsl:when test='self::LL:parameter and @type = "Custom"'>
        <xsl:call-template name='AssimSubParamExtension'/>
      </xsl:when>
      <xsl:when test='self::LL:parameter'>
        <xsl:call-template name='AssimSubParamParameter'/>
      </xsl:when>
      <xsl:when test='self::LL:choice'>
        <xsl:call-template name='AssimSubParamChoice'/>
      </xsl:when>
      <xsl:otherwise>
    HELP -- parameter/choice <xsl:value-of select='@type'/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:for-each>

    if(Cur != End)
    {
        pError->unexpectedParameter(*Cur);
    }

    return;

  missing:
    pError->missingParameter(pType);
}

</xsl:template>


<!--=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 -
 - @brief AssimSubParamParameter template
 -
 - Invoked by templates
 -      AssimilateSubParametersFunction
 -
 - Current node
 -      <llrpdef><messageDefinition><parameter>
 -      <llrpdef><parameterDefinition><parameter>
 -
 -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 -->

<xsl:template name='AssimSubParamParameter'>
  <xsl:variable name='MemberBaseName'>
    <xsl:choose>
      <xsl:when test='@name'><xsl:value-of select='@name'/></xsl:when>
      <xsl:otherwise><xsl:value-of select='@type'/></xsl:otherwise>
    </xsl:choose>
  </xsl:variable>
  <xsl:variable name='ParamType'>C<xsl:value-of select='@type'/></xsl:variable>

    // <xsl:value-of select='@repeat'/> of <xsl:value-of select='$MemberBaseName'/>
    pType = &amp;<xsl:value-of select='$ParamType'/>::s_typeDescriptor;
  <xsl:choose>
    <xsl:when test='@repeat="1"'>
    if(Cur == End || (*Cur)->m_pType != pType)
    {
        goto missing;
    }
    m_p<xsl:value-of select='$MemberBaseName'/> = (<xsl:value-of select='$ParamType'/> *)*Cur++;
    </xsl:when>
    <xsl:when test='@repeat="0-1"'>
    if(Cur != End &amp;&amp; (*Cur)->m_pType == pType)
    {
        m_p<xsl:value-of select='$MemberBaseName'/> = (<xsl:value-of select='$ParamType'/> *)*Cur++;
    }
    </xsl:when>
    <xsl:when test='@repeat="0-N"'>
    while(Cur != End &amp;&amp; (*Cur)->m_pType == pType)
    {
        m_list<xsl:value-of select='$MemberBaseName'/>.push_back((<xsl:value-of select='$ParamType'/> *)*Cur);
        Cur++;
    }
    </xsl:when>
    <xsl:when test='@repeat="1-N"'>
    if(Cur == End || (*Cur)->m_pType != pType)
    {
        goto missing;
    }
    while(Cur != End &amp;&amp; (*Cur)->m_pType == pType)
    {
        m_list<xsl:value-of select='$MemberBaseName'/>.push_back((<xsl:value-of select='$ParamType'/> *)*Cur);
        Cur++;
    }
    </xsl:when>
    <xsl:otherwise>
    HELP -- parameter/choice <xsl:value-of select='@type'/> <xsl:value-of select='@repeat'/>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>


<!--=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 -
 - @brief AssimSubParamChoice template
 -
 - Invoked by templates
 -      AssimilateSubParametersFunction
 -
 - Current node
 -      <llrpdef><messageDefinition><choice>
 -      <llrpdef><parameterDefinition><choice>
 -
 -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 -->

<xsl:template name='AssimSubParamChoice'>
  <xsl:variable name='MemberBaseName'>
    <xsl:choose>
      <xsl:when test='@name'><xsl:value-of select='@name'/></xsl:when>
      <xsl:otherwise><xsl:value-of select='@type'/></xsl:otherwise>
    </xsl:choose>
  </xsl:variable>
  <xsl:variable name='isMember'>C<xsl:value-of select='@type'/>::isMember(*Cur)</xsl:variable>

    // <xsl:value-of select='@repeat'/> of choice <xsl:value-of select='$MemberBaseName'/>
    pType = NULL;
  <xsl:choose>
    <xsl:when test='@repeat="1"'>
    if(Cur == End || !<xsl:value-of select='$isMember'/>)
    {
        goto missing;
    }
    m_p<xsl:value-of select='$MemberBaseName'/> = *Cur++;
    </xsl:when>
    <xsl:when test='@repeat="0-1"'>
    if(Cur != End &amp;&amp; <xsl:value-of select='$isMember'/>)
    {
        m_p<xsl:value-of select='$MemberBaseName'/> = *Cur++;
    }
    </xsl:when>
    <xsl:when test='@repeat="0-N"'>
    while(Cur != End &amp;&amp; <xsl:value-of select='$isMember'/>)
    {
        m_list<xsl:value-of select='$MemberBaseName'/>.push_back(*Cur);
        Cur++;
    }
    </xsl:when>
    <xsl:when test='@repeat="1-N"'>
    if(Cur == End || !<xsl:value-of select='$isMember'/>)
    {
        goto missing;
    }
    while(Cur != End &amp;&amp; <xsl:value-of select='$isMember'/>)
    {
        m_list<xsl:value-of select='$MemberBaseName'/>.push_back(*Cur);
        Cur++;
    }
    </xsl:when>
    <xsl:otherwise>
    HELP -- parameter/choice <xsl:value-of select='@type'/> <xsl:value-of select='@repeat'/>
    </xsl:otherwise>
  </xsl:choose>

</xsl:template>


<!--=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 -
 - @brief AssimSubParamExtension template
 -
 - Invoked by templates
 -      AssimilateSubParametersFunction
 -
 - Current node
 -      <llrpdef><messageDefinition><parameter type='Custom'>
 -      <llrpdef><parameterDefinition><parameter type='Custom'>
 -
 -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 -->

<xsl:template name='AssimSubParamExtension'>
  <xsl:variable name='MemberBaseName'>
    <xsl:choose>
      <xsl:when test='@name'><xsl:value-of select='@name'/></xsl:when>
      <xsl:otherwise><xsl:value-of select='@type'/></xsl:otherwise>
    </xsl:choose>
  </xsl:variable>
  <xsl:variable name='isAllowed'>(*Cur)-&gt;isAllowedExtension(&amp;s_typeDescriptor)</xsl:variable>

    // <xsl:value-of select='@repeat'/> of choice <xsl:value-of select='$MemberBaseName'/>
    pType = NULL;
  <xsl:choose>
    <xsl:when test='@repeat="1"'>
    if(Cur == End || !<xsl:value-of select='$isAllowed'/>)
    {
        goto missing;
    }
    m_p<xsl:value-of select='$MemberBaseName'/> = *Cur++;
    </xsl:when>
    <xsl:when test='@repeat="0-1"'>
    if(Cur != End &amp;&amp; <xsl:value-of select='$isAllowed'/>)
    {
        m_p<xsl:value-of select='$MemberBaseName'/> = *Cur++;
    }
    </xsl:when>
    <xsl:when test='@repeat="0-N"'>
    while(Cur != End &amp;&amp; <xsl:value-of select='$isAllowed'/>)
    {
        m_list<xsl:value-of select='$MemberBaseName'/>.push_back(*Cur);
        Cur++;
    }
    </xsl:when>
    <xsl:when test='@repeat="1-N"'>
    if(Cur == End || !<xsl:value-of select='$isAllowed'/>)
    {
        goto missing;
    }
    while(Cur != End &amp;&amp; <xsl:value-of select='$isAllowed'/>)
    {
        m_list<xsl:value-of select='$MemberBaseName'/>.push_back(*Cur);
        Cur++;
    }
    </xsl:when>
    <xsl:otherwise>
    HELP -- parameter/choice <xsl:value-of select='@type'/> <xsl:value-of select='@repeat'/>
    </xsl:otherwise>
  </xsl:choose>

</xsl:template>


<!--=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 -
 - @brief EncodeFunction template
 -
 - Invoked by templates
 -      ClassDefinitionCommon
 -
 - Current node
 -      <llrpdef><messageDefinition>
 -      <llrpdef><parameterDefinition>
 -
 - @param   ClassName       Name of generated class. This already has
 -                          "C" prefixed to the LLRP name.
 -
 -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 -->

<xsl:template name='EncodeFunction'>
  <xsl:param name='ClassName'/>
void
<xsl:value-of select='$ClassName'/>::encode (
  CEncoderStream *              pEncoderStream) const
{
    const CTypeDescriptor *     pType;

  <xsl:call-template name='EncodeAllFields'/>
  <xsl:call-template name='EncodeAllSubParameters'/>
}

</xsl:template>


<!--=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 -
 - @brief EncodeAllFields template
 -
 - Invoked by templates
 -      EncodeFunction
 -
 - Current node
 -      <llrpdef><messageDefinition>
 -      <llrpdef><parameterDefinition>
 -
 -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 -->

<xsl:template name='EncodeAllFields'>
  <xsl:for-each select='LL:field|LL:reserved'>
    <xsl:choose>
      <xsl:when test='self::LL:field'>
        <xsl:call-template name='EncodeOneField'/>
      </xsl:when>
      <xsl:when test='self::LL:reserved'>
        <xsl:call-template name='EncodeOneReserved'/>
      </xsl:when>
      <xsl:otherwise>
    HELP -- EncodeAllFields
      </xsl:otherwise>
    </xsl:choose>
  </xsl:for-each>
</xsl:template>


<!--=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 -
 - @brief EncodeOneField template
 -
 - Invoked by templates
 -      EncodeAllFields
 -
 - Current node
 -      <llrpdef><messageDefinition><field>
 -      <llrpdef><parameterDefinition><field>
 -
 -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 -->

<xsl:template name='EncodeOneField'>
  <xsl:choose>
    <xsl:when test='@enumeration and @type = "u8v"'>
    pEncoderStream->put_e8v(m_<xsl:value-of select='@name'/>, &amp;s_fd<xsl:value-of select='@name'/>);
    </xsl:when>
    <xsl:when test='@enumeration'>
      <xsl:call-template name='EncodeOneFieldEnum'/>
    </xsl:when>
    <xsl:otherwise>
      <xsl:call-template name='EncodeOneFieldPlain'/>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>


<!--=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 -
 - @brief EncodeOneFieldPlain template
 -
 - Invoked by templates
 -      EncodeOneField
 -
 - Current node
 -      <llrpdef><messageDefinition><field>
 -      <llrpdef><parameterDefinition><field>
 -
 -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 -->

<xsl:template name='EncodeOneFieldPlain'>
    pEncoderStream->put_<xsl:value-of select='@type'/>(m_<xsl:value-of select='@name'/>, &amp;s_fd<xsl:value-of select='@name'/>);
</xsl:template>


<!--=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 -
 - @brief EncodeOneFieldEnum template
 -
 - Invoked by templates
 -      EncodeOneField
 -
 - Current node
 -      <llrpdef><messageDefinition><field enumeration=...>
 -      <llrpdef><parameterDefinition><field enumeration=...>
 -
 -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 -->

<xsl:template name='EncodeOneFieldEnum'>
  <xsl:variable name='eType'>
    <xsl:choose>
      <xsl:when test='@type="u1"' >e1</xsl:when>
      <xsl:when test='@type="u2"' >e2</xsl:when>
      <xsl:when test='@type="u8"' >e8</xsl:when>
      <xsl:when test='@type="u16"'>e16</xsl:when>
      <xsl:when test='@type="u32"'>e32</xsl:when>
      <xsl:otherwise>ebogus</xsl:otherwise>
    </xsl:choose>
  </xsl:variable>
    pEncoderStream->put_<xsl:value-of select='$eType'/>((int)m_e<xsl:value-of select='@name'/>, &amp;s_fd<xsl:value-of select='@name'/>);
</xsl:template>


<!--=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 -
 - @brief EncodeOneReserved template
 -
 - Invoked by templates
 -      EncodeAllFields
 -
 - Current node
 -      <llrpdef><messageDefinition><reserved>
 -      <llrpdef><parameterDefinition><reserved>
 -
 -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 -->

<xsl:template name='EncodeOneReserved'>
    pEncoderStream->put_reserved(<xsl:value-of select='@bitCount'/>);
</xsl:template>


<!--=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 -
 - @brief EncodeAllSubParameters template
 -
 - Invoked by templates
 -      EncodeFunction
 -
 - Current node
 -      <llrpdef><messageDefinition>
 -      <llrpdef><parameterDefinition>
 -
 -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 -->

<xsl:template name='EncodeAllSubParameters'>
  <xsl:for-each select='LL:parameter|LL:choice'>
    <xsl:variable name='MemberBaseName'>
      <xsl:choose>
        <xsl:when test='@name'><xsl:value-of select='@name'/></xsl:when>
        <xsl:otherwise><xsl:value-of select='@type'/></xsl:otherwise>
      </xsl:choose>
    </xsl:variable>
    <xsl:choose>
      <xsl:when test='self::LL:parameter'>
    pType = &amp;C<xsl:value-of select='@type'/>::s_typeDescriptor;
      </xsl:when>
      <xsl:otherwise>
    pType = NULL;
      </xsl:otherwise>
    </xsl:choose>
    <xsl:choose>
      <xsl:when test='@repeat="1"'>
    pEncoderStream->putRequiredSubParameter(m_p<xsl:value-of select='$MemberBaseName'/>, pType);
      </xsl:when>
      <xsl:when test='@repeat="0-1"'>
    pEncoderStream->putOptionalSubParameter(m_p<xsl:value-of select='$MemberBaseName'/>, pType);
      </xsl:when>
      <xsl:when test='@repeat="0-N"'>
    pEncoderStream->putOptionalSubParameterList((tListOfParameters *) &amp;m_list<xsl:value-of select='$MemberBaseName'/>, pType);
      </xsl:when>
      <xsl:when test='@repeat="1-N"'>
    pEncoderStream->putRequiredSubParameterList((tListOfParameters *) &amp;m_list<xsl:value-of select='$MemberBaseName'/>, pType);
      </xsl:when>
      <xsl:otherwise>
    HELP -- EncodeAllSubParameters <xsl:value-of select='@type'/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:for-each>
</xsl:template>


<!--=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 -
 - @brief SubParameterAccessorFunctions template
 -
 - Invoked by templates
 -      ClassDefinitionCommon
 -
 - Current node
 -      <llrpdef><messageDefinition>
 -      <llrpdef><parameterDefinition>
 -
 - @param   ClassName       Name of generated class. This already has
 -                          "C" prefixed to the LLRP name.
 -
 -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 -->

<xsl:template name='SubParameterAccessorFunctions'>
  <xsl:param name='ClassName'/>
  <xsl:for-each select='LL:parameter|LL:choice'>
    <xsl:choose>
      <xsl:when test='self::LL:parameter and @type = "Custom"'>
        <xsl:call-template name='SubParamAccessorsExtensionPoint'>
          <xsl:with-param name='ClassName'><xsl:value-of select='$ClassName'/></xsl:with-param>
        </xsl:call-template>
      </xsl:when>
      <xsl:when test='self::LL:parameter'>
        <xsl:call-template name='SubParamAccessorsParam'>
          <xsl:with-param name='ClassName'><xsl:value-of select='$ClassName'/></xsl:with-param>
        </xsl:call-template>
      </xsl:when>
      <xsl:when test='self::LL:choice'>
        <xsl:call-template name='SubParamAccessorsChoice'>
          <xsl:with-param name='ClassName'><xsl:value-of select='$ClassName'/></xsl:with-param>
        </xsl:call-template>
      </xsl:when>
      <xsl:otherwise>
    HELP -- SubParameterAccessorFunctions <xsl:value-of select='@type'/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:for-each>
</xsl:template>


<!--=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 -
 - @brief SubParamAccessorsParam template
 -
 - Invoked by templates
 -      ClassDefinitionCommon
 -
 - Current node
 -      <llrpdef><messageDefinition><parameter>
 -      <llrpdef><parameterDefinition><parameter>
 -
 - @param   ClassName       Name of generated class. This already has
 -                          "C" prefixed to the LLRP name.
 -
 -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 -->

<xsl:template name='SubParamAccessorsParam'>
  <xsl:param name='ClassName'/>
  <xsl:variable name='MemberBaseName'>
    <xsl:choose>
      <xsl:when test='@name'><xsl:value-of select='@name'/></xsl:when>
      <xsl:otherwise><xsl:value-of select='@type'/></xsl:otherwise>
    </xsl:choose>
  </xsl:variable>
  <xsl:variable name='ParamType'>C<xsl:value-of select='@type'/></xsl:variable>
  <xsl:choose>
    <xsl:when test='@repeat="1" or @repeat="0-1"'>
EResultCode
<xsl:value-of select='$ClassName'/>::set<xsl:value-of select='$MemberBaseName'/> (
  <xsl:value-of select='$ParamType'/> *     pValue)
{
    removeSubParameterFromAllList(m_p<xsl:value-of select='$MemberBaseName'/>);
    delete m_p<xsl:value-of select='$MemberBaseName'/>;
    m_p<xsl:value-of select='$MemberBaseName'/> = pValue;
    addSubParameterToAllList(pValue);
    return RC_OK;
}
</xsl:when>
    <xsl:when test='@repeat="0-N" or @repeat="1-N"'>
EResultCode
<xsl:value-of select='$ClassName'/>::add<xsl:value-of select='$MemberBaseName'/> (
  <xsl:value-of select='$ParamType'/> *     pValue)
{
    m_list<xsl:value-of select='$MemberBaseName'/>.push_back(pValue);
    addSubParameterToAllList(pValue);
    return RC_OK;
}
</xsl:when>
    <xsl:otherwise>
    HELP -- SubParamAccessorsParam <xsl:value-of select='$MemberBaseName'/>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>


<!--=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 -
 - @brief SubParamAccessorsChoice template
 -
 - Invoked by templates
 -      ClassDefinitionCommon
 -
 - Current node
 -      <llrpdef><messageDefinition><choice>
 -      <llrpdef><parameterDefinition><choice>
 -
 - @param   ClassName       Name of generated class. This already has
 -                          "C" prefixed to the LLRP name.
 -
 -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 -->

<xsl:template name='SubParamAccessorsChoice'>
  <xsl:param name='ClassName'/>
  <xsl:variable name='MemberBaseName'>
    <xsl:choose>
      <xsl:when test='@name'><xsl:value-of select='@name'/></xsl:when>
      <xsl:otherwise><xsl:value-of select='@type'/></xsl:otherwise>
    </xsl:choose>
  </xsl:variable>
  <xsl:variable name='isMember'>C<xsl:value-of select='@type'/>::isMember(pValue)</xsl:variable>
  <xsl:choose>
    <xsl:when test='@repeat="1" or @repeat="0-1"'>
EResultCode
<xsl:value-of select='$ClassName'/>::set<xsl:value-of select='$MemberBaseName'/> (
  CParameter *                  pValue)
{
    if(!<xsl:value-of select='$isMember'/>)
    {
        return RC_InvalidChoiceMember;
    }

    removeSubParameterFromAllList(m_p<xsl:value-of select='$MemberBaseName'/>);
    delete m_p<xsl:value-of select='$MemberBaseName'/>;
    m_p<xsl:value-of select='$MemberBaseName'/> = pValue;
    addSubParameterToAllList(pValue);
    return RC_OK;
}
</xsl:when>
    <xsl:when test='@repeat="0-N" or @repeat="1-N"'>
EResultCode
<xsl:value-of select='$ClassName'/>::add<xsl:value-of select='$MemberBaseName'/> (
  CParameter *                  pValue)
{
    if(!<xsl:value-of select='$isMember'/>)
    {
        return RC_InvalidChoiceMember;
    }

    m_list<xsl:value-of select='$MemberBaseName'/>.push_back(pValue);
    addSubParameterToAllList(pValue);
    return RC_OK;
}
</xsl:when>
    <xsl:otherwise>
    HELP -- SubParamAccessorsChoice <xsl:value-of select='$MemberBaseName'/>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>


<!--=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 -
 - @brief SubParamAccessorsExtensionPoint template
 -
 - Invoked by templates
 -      ClassDefinitionCommon
 -
 - Current node
 -      <llrpdef><messageDefinition><parameter type='Custom'>
 -      <llrpdef><parameterDefinition><parameter type='Custom'>
 -
 - @param   ClassName       Name of generated class. This already has
 -                          "C" prefixed to the LLRP name.
 -
 -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 -->

<xsl:template name='SubParamAccessorsExtensionPoint'>
  <xsl:param name='ClassName'/>
  <xsl:variable name='MemberBaseName'>
    <xsl:choose>
      <xsl:when test='@name'><xsl:value-of select='@name'/></xsl:when>
      <xsl:otherwise><xsl:value-of select='@type'/></xsl:otherwise>
    </xsl:choose>
  </xsl:variable>
  <xsl:choose>
    <xsl:when test='@repeat="0-N" or @repeat="1-N"'>
EResultCode
<xsl:value-of select='$ClassName'/>::add<xsl:value-of select='$MemberBaseName'/> (
  CParameter *                  pValue)
{
    if(!pValue->isAllowedIn(&amp;s_typeDescriptor))
    {
        return RC_NotAllowedAtExtensionPoint;
    }

    m_list<xsl:value-of select='$MemberBaseName'/>.push_back(pValue);
    addSubParameterToAllList(pValue);
    return RC_OK;
}
</xsl:when>
    <xsl:otherwise>
    HELP -- SubParamAccessorsExtensionPoint <xsl:value-of select='$MemberBaseName'/>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>


<!--=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 -
 - @brief GenerateEnrollIntoTypeRegistryFunction template
 -
 - Invoked by top level template.
 -
 - Current node
 -      <llrpdef>
 -
 - Generates a function that constructs a CTypeRegistry
 - with all the known parameter and message types.
 -
 -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 -->

<xsl:template name='GenerateEnrollIntoTypeRegistryFunction'>
void
enroll<xsl:value-of select='$RegistryName'/>TypesIntoRegistry (
  CTypeRegistry *               pTypeRegistry)
{
  <xsl:for-each select='LL:parameterDefinition|LL:messageDefinition|LL:customParameterDefinition|LL:customMessageDefinition'>
    pTypeRegistry->enroll(&amp;C<xsl:value-of select='@name'/>::s_typeDescriptor);
  </xsl:for-each>
}
</xsl:template>

</xsl:stylesheet>
