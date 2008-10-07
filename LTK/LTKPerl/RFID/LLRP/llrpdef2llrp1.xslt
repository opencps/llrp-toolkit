<?xml version="1.0"?>
<!--
 - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 -  Copyright 2008 Impinj, Inc.                                              -
 -                                                                           -
 -  Licensed under the Apache License, Version 2.0 (the "License");          -
 -  you may not use this file except in compliance with the License.         -
 -  You may obtain a copy of the License at                                  -
 -                                                                           -
 -      http://www.apache.org/licenses/LICENSE-2.0                           -
 -                                                                           -
 -  Unless required by applicable law or agreed to in writing, software      -
 -  distributed under the License is distributed on an "AS IS" BASIS,        -
 -  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. -
 -  See the License for the specific language governing permissions and      -
 -  limitations under the License.                                           - 
 -                                                                           -
 - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 - DESCRIPTION                                                               -
 -                                                                           -
 - This script converts the standard llrpdef.xml to the legacy llrp1.desc    -
 - format so that Schema.pm uses the LTK standard binary schema.             -
 -                                                                           -
 - Note: because of a design error in llrpdef.xsd in which it does not       -
 -       implement the namespace concept properly, this script has to assume -
 -       that all identifiers of extension names and parameters are unique   -
 -       among all vendors.                                                  -
 -                                                                           -
 - Note: custom unions are not supported since there aren't any yet.         -
 -                                                                           -
 - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 - AUTHOR                                                                    -
 -                                                                           -
 - John R. Hogerhuis                                                         -
 -                                                                           -
 - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 - TODO                                                                      -
 -                                                                           -
 - Rewrite Schema.pm to use llrpdef.xml directly                             -
 -                                                                           -
 - Figure out how to remove the 'discard unmatched text' workaround          -
 -                                                                           -
 - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
-->

<xsl:stylesheet version="1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns:xs="http://www.w3.org/2001/XMLSchema"
  xmlns:llb="http://www.llrp.org/ltk/schema/core/encoding/binary/1.0"
>

<!--
	XSLT script that converts the llrpdef.xml LLRP binary schema into
	llrp1.desc format
-->

<xsl:output method="text" indent="no" encoding="utf-8" omit-xml-declaration="yes"/>

<xsl:template match="/">

  <xsl:call-template name="FORCE_CORE_NS"/>
  <xsl:apply-templates mode="EXT_NS_DEF"/>

  <xsl:apply-templates mode="EXT_VENDOR_DEF"/>

  <xsl:apply-templates mode="MSG_DEF"/>
  <xsl:apply-templates mode="PARAM_DEF"/>
  <xsl:apply-templates mode="CHOICE_DEF"/>
  <xsl:apply-templates mode="ENUM_DEF"/>
</xsl:template>

<xsl:template name="FORCE_CORE_NS">

  <xsl:variable name="core-ns" select="//llb:namespaceDefinition[@prefix='llrp']"/>

  <xsl:choose>

    <!-- put whatever URI user requests, if they ask -->
    <xsl:when test="$core-ns">
      <xsl:apply-templates mode="CORE_NS_DEF"/>
    </xsl:when>

    <!-- attempt to derive core namespace from llb:llrpdef node if not explict -->
    <xsl:otherwise>
      <xsl:text>core-namespace "</xsl:text>
      <xsl:variable name="binary-encode-ns" select="namespace-uri(//llb:llrpdef)"/>
      <xsl:variable name="derived-xml-encode-ns">
        <xsl:call-template name="replace-string">
            <xsl:with-param name="text" select="$binary-encode-ns"/>
            <xsl:with-param name="replace" select="'encoding/binary'"/>
            <xsl:with-param name="with" select="'encoding/xml'"/>
        </xsl:call-template>
      </xsl:variable>
      <xsl:value-of select="$derived-xml-encode-ns"/>
      <xsl:text>"&#10;&#10;</xsl:text>
    </xsl:otherwise>

  </xsl:choose>

</xsl:template>

<xsl:template match="llb:namespaceDefinition[@prefix='llrp']" mode="CORE_NS_DEF">
  <xsl:text>core-namespace "</xsl:text>
  <xsl:value-of select="@URI"/>
  <xsl:text>"&#10;&#10;</xsl:text>
</xsl:template>

<xsl:template match="llb:namespaceDefinition[@prefix!='llrp']" mode="EXT_NS_DEF">
  <xsl:text>namespace&#32;</xsl:text>
  <xsl:value-of select="@prefix"/><xsl:text>&#32;"</xsl:text>
  <xsl:value-of select="@URI"/>
  <xsl:text>"&#10;&#10;</xsl:text>
</xsl:template>

<xsl:template match="llb:vendorDefinition" mode="EXT_VENDOR_DEF">
  <xsl:text>vendor&#32;</xsl:text>
  <xsl:value-of select="@name"/><xsl:text>&#32;</xsl:text>
  <xsl:value-of select="@vendorID"/>
  <xsl:text>&#10;&#10;</xsl:text>
</xsl:template>

<xsl:template match="llb:enumerationDefinition" mode="ENUM_DEF">
  <xsl:text>enumeration&#32;</xsl:text>
  <xsl:value-of select="@name"/>
  <xsl:text>&#10;</xsl:text>
  <xsl:apply-templates mode="ENUM_DEF"/>
  <xsl:text>end&#10;&#10;</xsl:text>
</xsl:template>

<xsl:template match="llb:customEnumerationDefinition" mode="ENUM_DEF">
  <xsl:text>custom-enumeration&#32;</xsl:text>
  <xsl:value-of select="@namespace"/>
  <xsl:text>&#32;</xsl:text>
  <xsl:value-of select="@name"/>
  <xsl:text>&#10;</xsl:text>
  <xsl:apply-templates mode="ENUM_DEF"/>
  <xsl:text>end&#10;&#10;</xsl:text>
</xsl:template>

<xsl:template match="llb:entry" mode="ENUM_DEF">
  <xsl:text>&#32;&#32;enum&#32;</xsl:text>
  <xsl:value-of select="@value"/>
  <xsl:text>&#32;</xsl:text>
  <xsl:value-of select="@name"/>
  <xsl:text>&#10;</xsl:text>
</xsl:template>

<xsl:template match="llb:messageDefinition" mode="MSG_DEF">
  <xsl:text>message&#32;</xsl:text>
  <xsl:choose>
    <xsl:when test="@responseType">
      <xsl:text>cmd&#32;</xsl:text>
    </xsl:when>
    <xsl:when test="@name='CUSTOM_MESSAGE'">
      <xsl:text>cmd&#32;</xsl:text>
    </xsl:when>
    <xsl:when test="@name='ERROR_MESSAGE'">
      <xsl:text>rsp&#32;</xsl:text>
    </xsl:when>
    <xsl:when test="//llb:messageDefinition[@responseType = current()/@name]">
      <xsl:text>rsp&#32;</xsl:text>
    </xsl:when>
    <xsl:otherwise>
      <xsl:text>ntf&#32;</xsl:text>
    </xsl:otherwise>
  </xsl:choose>
  <xsl:value-of select="@typeNum"/>
  <xsl:text>&#32;</xsl:text>
  <xsl:value-of select="@name"/>
  <xsl:text>&#10;</xsl:text>
  <xsl:apply-templates mode="FIELDPARMS_BODY"/>
  <xsl:text>end&#10;&#10;</xsl:text>
</xsl:template>

<xsl:template match="llb:customMessageDefinition" mode="MSG_DEF">
  <xsl:text>custom-message&#32;</xsl:text>
  <xsl:value-of select="@vendor"/><xsl:text>&#32;</xsl:text>
  <xsl:value-of select="@subtype"/><xsl:text>&#32;</xsl:text>
  <xsl:value-of select="@namespace"/><xsl:text>&#32;</xsl:text>
  <xsl:value-of select="@name"/><xsl:text>&#10;</xsl:text>
  <xsl:apply-templates mode="FIELDPARMS_BODY"/>
  <xsl:text>end&#10;&#10;</xsl:text>
</xsl:template>

<xsl:template match="llb:customParameterDefinition" mode="PARAM_DEF">
  <xsl:text>custom-parameter&#32;</xsl:text>
  <xsl:value-of select="@vendor"/><xsl:text>&#32;</xsl:text>
  <xsl:value-of select="@subtype"/><xsl:text>&#32;</xsl:text>
  <xsl:value-of select="@namespace"/><xsl:text>&#32;</xsl:text>
  <xsl:value-of select="@name"/>
  <xsl:text>&#10;</xsl:text>
  <xsl:apply-templates mode="FIELDPARMS_BODY"/>
  <xsl:text>end&#10;&#10;</xsl:text>
</xsl:template>

<xsl:template match="llb:parameterDefinition" mode="PARAM_DEF">
  <xsl:text>parameter&#32;</xsl:text>
  <xsl:choose>
    <xsl:when test="@typeNum &lt; 128">
      <xsl:text>tv&#32;</xsl:text>
    </xsl:when>
    <xsl:when test="@typeNum &lt; 2048">
      <xsl:text>tlv&#32;</xsl:text>
    </xsl:when>
  </xsl:choose>
  <xsl:value-of select="@typeNum"/>
  <xsl:text>&#32;</xsl:text>
  <xsl:value-of select="@name"/>
  <xsl:text>&#10;</xsl:text>
  <xsl:apply-templates mode="FIELDPARMS_BODY"/>
  <xsl:text>end&#10;&#10;</xsl:text>
</xsl:template>

<xsl:template match="llb:field" mode="FIELDPARMS_BODY">
  <xsl:text>&#32;&#32;field&#32;</xsl:text>
  <xsl:choose>
    <xsl:when test="@type = 'bytesToEnd'">
      <xsl:text>data_u8v</xsl:text>
    </xsl:when>
    <xsl:otherwise>
      <xsl:value-of select="@type"/>
    </xsl:otherwise>
  </xsl:choose> 
  <xsl:text>&#32;</xsl:text>
  <xsl:value-of select="@name"/>
  <xsl:if test="@format">
    <xsl:text>&#32;</xsl:text>
    <xsl:text>fmt=</xsl:text>
    <xsl:value-of select="@format"/>
  </xsl:if>
  <xsl:if test="@enumeration">
    <xsl:variable name="ns" select="//llb:customEnumerationDefinition[@name = current()/@enumeration]/@namespace"/>
    <xsl:if test="$ns">
      <xsl:text>&#32;ns=</xsl:text>
      <xsl:value-of select="$ns"/>
    </xsl:if>
    <xsl:text>&#32;</xsl:text>
    <xsl:text>enum=</xsl:text>
    <xsl:value-of select="@enumeration"/>
  </xsl:if>
  <xsl:text>&#10;</xsl:text>
</xsl:template>

<xsl:template name="formatParam">
  <xsl:choose>
    <xsl:when test="@type='Custom'">
       <xsl:text>extension-point&#10;</xsl:text>
       <xsl:apply-templates select="//llb:allowedIn[@type = current()/../@name]" mode="EXTENSION_POINT"/>
    </xsl:when>
    <xsl:otherwise>
      <xsl:text>&#32;&#32;param </xsl:text>
      <xsl:value-of select="translate(@repeat, 'N', 'n')"/>
      <xsl:text>&#32;</xsl:text>
      <xsl:value-of select="@type"/>
      <xsl:variable name="ns" select="//llb:customParameterDefinition[@name = current()/@type]/@namespace"/>
      <xsl:if test="$ns">
        <xsl:text>&#32;</xsl:text>
        <xsl:value-of select="$ns"/>
      </xsl:if>
      <xsl:variable name="nsc" select="//llb:customChoiceDefinition[@name = current()/@type]/@namespace"/>
      <xsl:if test="$nsc">
        <xsl:text>&#32;</xsl:text>
        <xsl:value-of select="$nsc"/>
      </xsl:if>
      <xsl:if test="@name">
        <xsl:text>&#32;name=</xsl:text>
        <xsl:value-of select="@name"/>
      </xsl:if>
      <xsl:text>&#10;</xsl:text>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template name="formatAllow" match="llb:allowedIn" mode="EXTENSION_POINT">
  <xsl:text>&#32;&#32;allow </xsl:text>
  <xsl:value-of select="translate(@repeat, 'N', 'n')"/>
  <xsl:text>&#32;</xsl:text>
  <xsl:value-of select="../@name"/>
  <xsl:variable name="ns" select="//llb:customParameterDefinition[@name = current()/../@name]/@namespace"/>
  <xsl:if test="$ns">
    <xsl:text>&#32;</xsl:text>
    <xsl:value-of select="$ns"/>
  </xsl:if>
  <xsl:if test="@name">
    <xsl:text>&#32;name=</xsl:text>
    <xsl:value-of select="@name"/>
  </xsl:if>
   <xsl:text>&#10;</xsl:text>
</xsl:template>

<xsl:template match="llb:parameter" mode="FIELDPARMS_BODY">
  <xsl:call-template name="formatParam"/>
</xsl:template>

<xsl:template match="llb:reserved" mode="FIELDPARMS_BODY">
  <xsl:text>&#32;&#32;reserved&#32;</xsl:text>
  <xsl:value-of select="@bitCount"/>
  <xsl:text>&#10;</xsl:text>
</xsl:template>

<xsl:template match="llb:choice" mode="FIELDPARMS_BODY">
  <xsl:call-template name="formatParam"/>
</xsl:template>

<xsl:template match="llb:choiceDefinition" mode="CHOICE_DEF">
  <xsl:text>parameter union&#32;</xsl:text>
  <xsl:value-of select="@name"/>
  <xsl:text>&#10;</xsl:text>
  <xsl:apply-templates mode="CHOICE_DEF"/>
  <xsl:text>end&#10;&#10;</xsl:text>
</xsl:template>

<xsl:template match="llb:customChoiceDefinition" mode="CHOICE_DEF">
  <xsl:text>custom-union&#32;</xsl:text>
  <xsl:value-of select="@namespace"/><xsl:text>&#32;</xsl:text>
  <xsl:value-of select="@name"/>
  <xsl:text>&#10;</xsl:text>
  <xsl:apply-templates mode="CHOICE_DEF"/>
  <xsl:text>end&#10;&#10;</xsl:text>
</xsl:template>

<xsl:template match="llb:choiceDefinition/llb:parameter" mode="CHOICE_DEF">
  <xsl:choose>
    <xsl:when test="@type='Custom'">
      <xsl:text>extension-point&#10;</xsl:text>
      <xsl:apply-templates select="//llb:allowedIn[@type = current()/../@name]" mode="EXTENSION_POINT"/>
    </xsl:when>
    <xsl:otherwise>
      <xsl:text>&#32;&#32;param 1&#32;</xsl:text>
      <xsl:value-of select="@type"/>
      <xsl:text>&#10;</xsl:text>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template match="llb:customChoiceDefinition/llb:parameter" mode="CHOICE_DEF">
  <xsl:text>&#32;&#32;param 1&#32;</xsl:text>
  <xsl:value-of select="@type"/>
  <xsl:text>&#32;</xsl:text>
  <xsl:value-of select="../@namespace"/>
  <xsl:text>&#10;</xsl:text>
</xsl:template>

<!-- utility functions -->
<xsl:template name="replace-string">
  <xsl:param name="text"/>
  <xsl:param name="replace"/>
  <xsl:param name="with"/>
  <xsl:choose>
    <xsl:when test="contains($text,$replace)">
      <xsl:value-of select="substring-before($text,$replace)"/>
      <xsl:value-of select="$with"/>
      <xsl:call-template name="replace-string">
        <xsl:with-param name="text" select="substring-after($text,$replace)"/>
        <xsl:with-param name="replace" select="$replace"/>
        <xsl:with-param name="with" select="$with"/>
      </xsl:call-template>
    </xsl:when>
    <xsl:otherwise>
      <xsl:value-of select="$text"/>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template match="//text()" mode="CORE_NS_DEF"/>
<xsl:template match="//text()" mode="EXT_NS_DEF"/>

<xsl:template match="//text()" mode="EXT_VENDOR_DEF"/>

<xsl:template match="//text()" mode="MSG_DEF"></xsl:template>
<xsl:template match="//text()" mode="PARAM_DEF"></xsl:template>
<xsl:template match="//text()" mode="FIELDPARMS_BODY"></xsl:template>
<xsl:template match="//text()" mode="CHOICE_DEF"></xsl:template>
<xsl:template match="//text()" mode="ENUM_DEF"></xsl:template>

<xsl:template match="//text()" mode="EXTENSION_POINT"></xsl:template>

</xsl:stylesheet>
