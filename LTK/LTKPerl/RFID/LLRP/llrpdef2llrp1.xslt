<?xml version="1.0"?>
<!--
 - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 -  Copyright 2007 Impinj, Inc.                                              -
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
 - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 - AUTHOR                                                                    -
 -                                                                           -
 - John R. Hogerhuis                                                         -
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
  xmlns:llb="http://www.llrp.org/ltk/schema/core/encoding/binary/0.8"
>

<!--
	XSLT script that converts the llrpdef.xml LLRP binary schema into
	llrp1.desc format
-->

<xsl:output method="text" indent="no" encoding="utf-8" omit-xml-declaration="yes"/>

<xsl:template match="/">
  <xsl:apply-templates mode="MSG_DEF"/>
  <xsl:apply-templates mode="PARAM_DEF"/>
  <xsl:apply-templates mode="CHOICE_DEF"/>
  <xsl:apply-templates mode="ENUM_DEF"/>
</xsl:template>

<xsl:template match="llb:enumerationDefinition" mode="ENUM_DEF">
  <xsl:text>enumeration +&#32;</xsl:text>
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
  <xsl:text>message cmd&#32;</xsl:text>
  <xsl:value-of select="@typeNum"/>
  <xsl:text>&#32;</xsl:text>
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
    <xsl:text>&#32;</xsl:text>
    <xsl:text>enum=</xsl:text>
    <xsl:value-of select="@enumeration"/>
  </xsl:if>
  <xsl:text>&#10;</xsl:text>
</xsl:template>

<xsl:template name="formatParam">
  <xsl:text>&#32;&#32;param </xsl:text>
  <xsl:value-of select="@repeat"/>
  <xsl:text>&#32;</xsl:text>
  <xsl:value-of select="@type"/>
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
  <xsl:text>parameter union +&#32;</xsl:text>
  <xsl:value-of select="@name"/>
  <xsl:text>&#10;</xsl:text>
  <xsl:apply-templates mode="CHOICE_DEF"/>
  <xsl:text>end&#10;&#10;</xsl:text>
</xsl:template>

<xsl:template match="llb:choiceDefinition/llb:parameter" mode="CHOICE_DEF">
  <xsl:text>&#32;&#32;param 1&#32;</xsl:text>
  <xsl:value-of select="@type"/>
  <xsl:text>&#10;</xsl:text>
</xsl:template>

<xsl:template match="//text()" mode="MSG_DEF"></xsl:template>
<xsl:template match="//text()" mode="PARAM_DEF"></xsl:template>
<xsl:template match="//text()" mode="FIELDPARMS_BODY"></xsl:template>
<xsl:template match="//text()" mode="CHOICE_DEF"></xsl:template>
<xsl:template match="//text()" mode="ENUM_DEF"></xsl:template>

</xsl:stylesheet>
