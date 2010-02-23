<?xml version="1.0" encoding="UTF-8" ?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns:llrp="http://www.llrp.org/ltk/schema/core/encoding/binary/1.0"
  xmlns:h="http://www.w3.org/1999/xhtml">

  <xsl:output omit-xml-declaration='yes' method='text' indent='yes'/>

  <xsl:template name="DefineDataType" match="field">
    <xsl:choose>
      <xsl:when test="@enumeration and @type!='u8v'">ENUM_<xsl:value-of select="@enumeration"/>
      </xsl:when>
      <xsl:otherwise>
        <xsl:choose>
          <xsl:when test="@type = 'u1'">bool</xsl:when>
          <xsl:when test="@type = 'u1v'">LLRPBitArray</xsl:when>
          <xsl:when test="@type = 'u2'">TwoBits</xsl:when>
          <xsl:when test="@type = 'u8'">byte</xsl:when>
          <xsl:when test="@type = 'u8v'">ByteArray</xsl:when>
          <xsl:when test="@type = 's8'">sbyte</xsl:when>
          <xsl:when test="@type = 's8v'">ByteArray</xsl:when>
          <xsl:when test="@type = 'u16'">UInt16</xsl:when>
          <xsl:when test="@type = 'u16v'">UInt16Array</xsl:when>
          <xsl:when test="@type = 's16'">Int16</xsl:when>
          <xsl:when test="@type = 's16v'">Int16Array</xsl:when>
          <xsl:when test="@type = 'u32'">UInt32</xsl:when>
          <xsl:when test="@type = 'u32v'">UInt32Array</xsl:when>
          <xsl:when test="@type = 's32'">Int32</xsl:when>
          <xsl:when test="@type = 's32v'">Int32Array</xsl:when>
          <xsl:when test="@type = 'u64'">UInt64</xsl:when>
          <xsl:when test="@type = 'utf8v'">string</xsl:when>
          <xsl:when test="@type = 'u96'">LLRPBitArray</xsl:when>
          <xsl:when test="@type = 'bytesToEnd'">ByteArray</xsl:when>
          <xsl:otherwise>Unkown_Type</xsl:otherwise>
        </xsl:choose>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>
  <xsl:template name="DefineDefaultValue" match="field">
    <xsl:choose>
      <xsl:when test="@enumeration and @type!='u8v'">;</xsl:when>
      <xsl:otherwise>
        <xsl:choose>
          <xsl:when test="@type = 'u1'">=false;</xsl:when>
          <xsl:when test="@type = 'u1v'">=new LLRPBitArray();</xsl:when>
          <xsl:when test="@type = 'u2'">=new TwoBits(0);</xsl:when>
          <xsl:when test="@type = 'u8'">=0;</xsl:when>
          <xsl:when test="@type = 'u8v'">=new ByteArray();</xsl:when>
          <xsl:when test="@type = 's8'">=0;</xsl:when>
          <xsl:when test="@type = 's8v'">=new SignedByteArray();</xsl:when>
          <xsl:when test="@type = 'u16'">=0;</xsl:when>
          <xsl:when test="@type = 'u16v'">=new UInt16Array();</xsl:when>
          <xsl:when test="@type = 's16'">=0;</xsl:when>
          <xsl:when test="@type = 's16v'">=new Int16Array();</xsl:when>          
          <xsl:when test="@type = 'u32'">=0;</xsl:when>
          <xsl:when test="@type = 'u32v'">=new UInt32Array();</xsl:when>
          <xsl:when test="@type = 's32'">=0;</xsl:when>
          <xsl:when test="@type = 's32v'">=new Int32Array();</xsl:when>          
          <xsl:when test="@type = 'u64'">=0;</xsl:when>
          <xsl:when test="@type = 'utf8v'">=string.Empty;</xsl:when>
          <xsl:when test="@type = 'u96'">=new LLRPBitArray();</xsl:when>
          <xsl:when test="@type = 'bytesToEnd'">=new ByteArray();</xsl:when>
          <xsl:otherwise>Unknown_Type</xsl:otherwise>
        </xsl:choose>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>
  <xsl:template name="DefineDataLength" match="field">
    <xsl:choose>
      <xsl:when test="@enumeration and @type!='u8v'">
        private Int16 <xsl:value-of select="@name"/>_len = <xsl:choose>
          <xsl:when test="@type = 'u1'">1;</xsl:when>
          <xsl:when test="@type = 'u2'">2;</xsl:when>
          <xsl:when test="@type = 'u8'">8;</xsl:when>
          <xsl:when test="@type = 's8'">8;</xsl:when>          
          <xsl:when test="@type = 'u16'">16;</xsl:when>
          <xsl:when test="@type = 's16'">16;</xsl:when>          
          <xsl:when test="@type = 'u32'">32;</xsl:when>
          <xsl:when test="@type = 's32'">32;</xsl:when>
          <xsl:when test="@type = 'u64'">64;</xsl:when>
          <xsl:otherwise>Unknown_Length;</xsl:otherwise>
        </xsl:choose>
      </xsl:when>
      <xsl:when test="@format='Hex'">
        private Int16 <xsl:value-of select="@name"/>_len;
      </xsl:when>
      <xsl:when test="@format='UTF8'">
        private Int16 <xsl:value-of select="@name"/>_len;
      </xsl:when>
      <xsl:otherwise>
        private Int16 <xsl:value-of select="@name"/>_len=0;
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>
  <xsl:template name="DefineParameterName" match="field">
    <xsl:choose>
      <xsl:when test="@name">
        <xsl:value-of select="@name"/>
      </xsl:when>
      <xsl:otherwise>
        <xsl:value-of select="@type"/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>
  <xsl:template name="DefineParameterType" match="parameter">
    <xsl:choose>
      <xsl:when test="@repeat = '1'"></xsl:when>
      <xsl:when test="@repeat = '0-1'"></xsl:when>
      <xsl:when test="@repeat = '1-N'">[]</xsl:when>
      <xsl:when test="@repeat = '0-N'">[]</xsl:when>
      <xsl:otherwise>Unknown_Type</xsl:otherwise>
    </xsl:choose>
  </xsl:template>

  <xsl:template name="PARAMToString">
        /// <xsl:text disable-output-escaping="yes">&lt;</xsl:text>summary<xsl:text disable-output-escaping="yes">&gt;</xsl:text>
        /// Serialize a Xml string
        /// <xsl:text disable-output-escaping="yes">&lt;</xsl:text>/summary<xsl:text disable-output-escaping="yes">&gt;</xsl:text>
        /// <xsl:text disable-output-escaping="yes">&lt;</xsl:text>returns<xsl:text disable-output-escaping="yes">&gt;</xsl:text>Xml string<xsl:text disable-output-escaping="yes">&lt;</xsl:text>/returns<xsl:text disable-output-escaping="yes">&gt;</xsl:text>
        public override string ToString()
        {
            int len;

            string xml_str = "<xsl:text disable-output-escaping="yes">&lt;</xsl:text><xsl:value-of select="@name"/><xsl:text disable-output-escaping="yes">&gt;</xsl:text>";
            xml_str += "\r\n";

    <xsl:for-each select="*">
      <xsl:choose>
        <xsl:when test="name()='field'">
            if (<xsl:value-of select="@name"/> != null)
            {
                try
                {
          <xsl:choose>
            <xsl:when test="(@type='u1' or @type='u8' or @type='s8' or @type='u16' or @type='s16' or @type='u32' or @type='s32' or @type='u64') and not(@enumeration)">
                    xml_str += "  <xsl:text disable-output-escaping="yes">&lt;</xsl:text><xsl:value-of select="@name"/><xsl:text disable-output-escaping="yes">&gt;</xsl:text>" + Util.ConvertValueTypeToString(<xsl:value-of select="@name"/>, "<xsl:value-of select="@type"/>", "<xsl:value-of select="@format"/>") + "<xsl:text disable-output-escaping="yes">&lt;/</xsl:text><xsl:value-of select="@name"/><xsl:text disable-output-escaping="yes">&gt;</xsl:text>";
            </xsl:when>
            <xsl:when test="(@type='u8v' or @type='s8v' or @type='u16v' or @type='s16v'or @type='u32v' or @type='s32v' or @type='utf8v' or @type='u96' or @type='bytesToEnd') and not(@enumeration)">
                    xml_str += "  <xsl:text disable-output-escaping="yes">&lt;</xsl:text><xsl:value-of select="@name"/><xsl:text disable-output-escaping="yes">&gt;</xsl:text>" + Util.ConvertArrayTypeToString(<xsl:value-of select="@name"/>, "<xsl:value-of select="@type"/>", "<xsl:value-of select="@format"/>") + "<xsl:text disable-output-escaping="yes">&lt;/</xsl:text><xsl:value-of select="@name"/><xsl:text disable-output-escaping="yes">&gt;</xsl:text>";
            </xsl:when>
            <xsl:when test="@type='u1v'">
                    xml_str += "  <xsl:text disable-output-escaping="yes">&lt;</xsl:text><xsl:value-of select="@name"/> Count=\"" + <xsl:value-of select="@name"/>.Count + "\"<xsl:text disable-output-escaping="yes">&gt;</xsl:text>" + Util.ConvertArrayTypeToString(<xsl:value-of select="@name"/>, "<xsl:value-of select="@type"/>", "<xsl:value-of select="@format"/>") + "<xsl:text disable-output-escaping="yes">&lt;/</xsl:text><xsl:value-of select="@name"/><xsl:text disable-output-escaping="yes">&gt;</xsl:text>";
            </xsl:when>
            <xsl:when test="(@type='u8v' and @enumeration)">
                    xml_str += "  <xsl:text disable-output-escaping="yes">&lt;</xsl:text><xsl:value-of select="@name"/><xsl:text disable-output-escaping="yes">&gt;</xsl:text>" + <xsl:value-of select="@name"/>.ToString(typeof(ENUM_<xsl:value-of select="@enumeration"/>)) + "<xsl:text disable-output-escaping="yes">&lt;/</xsl:text><xsl:value-of select="@name"/><xsl:text disable-output-escaping="yes">&gt;</xsl:text>";
            </xsl:when>
            <xsl:otherwise>
                    xml_str += "  <xsl:text disable-output-escaping="yes">&lt;</xsl:text><xsl:value-of select="@name"/><xsl:text disable-output-escaping="yes">&gt;</xsl:text>" + <xsl:value-of select="@name"/>.ToString() + "<xsl:text disable-output-escaping="yes">&lt;/</xsl:text><xsl:value-of select="@name"/><xsl:text disable-output-escaping="yes">&gt;</xsl:text>";
            </xsl:otherwise>
          </xsl:choose>
	                xml_str += "\r\n";
                }
                catch { }
            }
        </xsl:when>
        <xsl:when test="name()='parameter'">
            if (<xsl:call-template name='DefineParameterName'/> != null)
            {
          <xsl:choose>
            <xsl:when test="@type='Custom'">
                len = <xsl:call-template name='DefineParameterName'/>.Length;
                for (int i=0; i<xsl:text disable-output-escaping="yes">&lt;</xsl:text>len; i++)
                {
                    xml_str += Util.Indent(<xsl:call-template name='DefineParameterName'/>[i].ToString());
                }
            </xsl:when>
            <xsl:otherwise>
              <xsl:choose>
                <xsl:when test="@repeat = '0-N' or @repeat = '1-N'">
                len = <xsl:call-template name='DefineParameterName'/>.Length;
                for (int i=0; i<xsl:text disable-output-escaping="yes">&lt;</xsl:text>len; i++)
                {
                    xml_str += Util.Indent(<xsl:call-template name='DefineParameterName'/>[i].ToString());
                }
                </xsl:when>
                <xsl:otherwise>
                xml_str += Util.Indent(<xsl:call-template name='DefineParameterName'/>.ToString());
                </xsl:otherwise>
              </xsl:choose>
            </xsl:otherwise>
          </xsl:choose>
            }
        </xsl:when>
        <xsl:when test="name()='choice'">
          <xsl:variable name="choiceParameterName">
            <xsl:call-template name='DefineParameterName'/>
          </xsl:variable>
            if (<xsl:call-template name='DefineParameterName'/> != null)
            {
                len = <xsl:copy-of select='$choiceParameterName'/>.Count;
                for (int i=0; i<xsl:text disable-output-escaping="yes">&lt;</xsl:text>len; i++)
                {
                    xml_str += Util.Indent(<xsl:copy-of select='$choiceParameterName'/>[i].ToString());
                }
            }
        </xsl:when>
        <xsl:otherwise>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:for-each>
            xml_str += "<xsl:text disable-output-escaping="yes">&lt;/</xsl:text><xsl:value-of select="@name"/><xsl:text disable-output-escaping="yes">&gt;</xsl:text>";
            xml_str += "\r\n";

            return xml_str;
        }
  </xsl:template>
  <xsl:template name="PARAMFromXmlNode">
        /// <xsl:text disable-output-escaping="yes">&lt;</xsl:text>summary<xsl:text disable-output-escaping="yes">&gt;</xsl:text>
        /// Deserialize a XML node to parameter
        /// <xsl:text disable-output-escaping="yes">&lt;</xsl:text>/summary<xsl:text disable-output-escaping="yes">&gt;</xsl:text>
        /// <xsl:text disable-output-escaping="yes">&lt;</xsl:text>param name="node"<xsl:text disable-output-escaping="yes">&gt;</xsl:text>Xml node to be deserialized<xsl:text disable-output-escaping="yes">&lt;</xsl:text>/param<xsl:text disable-output-escaping="yes">&gt;</xsl:text>
        /// <xsl:text disable-output-escaping="yes">&lt;</xsl:text>returns<xsl:text disable-output-escaping="yes">&gt;</xsl:text>PARAM_<xsl:value-of select="@name"/><xsl:text disable-output-escaping="yes">&lt;</xsl:text>/returns<xsl:text disable-output-escaping="yes">&gt;</xsl:text>
        public static PARAM_<xsl:value-of select="@name"/> FromXmlNode(XmlNode node)
        {
            ArrayList customArr = new ArrayList();
            string count;
            string val;

            XmlNamespaceManager nsmgr = new XmlNamespaceManager(node.OwnerDocument.NameTable);
            nsmgr.AddNamespace("", LLRPConstants.NAMESPACE_URI);
            nsmgr.AddNamespace(LLRPConstants.NAMESPACE_PREFIX, LLRPConstants.NAMESPACE_URI);

            PARAM_<xsl:value-of select="@name"/> param = new PARAM_<xsl:value-of select="@name"/>();

    <xsl:for-each select="*">
      <xsl:choose>
        <xsl:when test="name()='field'">
            val = XmlUtil.GetNodeValue(node, "<xsl:value-of select="@name"/>");
          <xsl:choose>
            <xsl:when test="@enumeration and @type!='u8v'">
            param.<xsl:value-of select="@name"/> = (<xsl:call-template name='DefineDataType'/>)Enum.Parse(typeof(<xsl:call-template name='DefineDataType'/>), val);
            </xsl:when>
            <xsl:when test="(@type='u1' or @type='u8' or @type='s8' or @type='u16' or @type='s16' or @type='u32' or @type='s32' or @type='u64') and not(@enumeration)">
            param.<xsl:value-of select="@name"/> = (<xsl:call-template name='DefineDataType'/>)(Util.ParseValueTypeFromString(val, "<xsl:value-of select="@type"/>", "<xsl:value-of select="@format"/>"));
            </xsl:when>
            <xsl:when test="(@type='u8v' or @type='s8v' or @type='u16v' or @type='s16v' or @type='u32v' or @type='s32v' or @type='utf8v' or @type='u96' or @type='bytesToEnd') and not(@enumeration)">
            param.<xsl:value-of select="@name"/> = (<xsl:call-template name='DefineDataType'/>)(Util.ParseArrayTypeFromString(val, "<xsl:value-of select="@type"/>", "<xsl:value-of select="@format"/>"));
            </xsl:when>
            <xsl:when test="@type='u1v'">
            param.<xsl:value-of select="@name"/> = (<xsl:call-template name='DefineDataType'/>)(Util.ParseArrayTypeFromString(val, "<xsl:value-of select="@type"/>", "<xsl:value-of select="@format"/>"));
            count = XmlUtil.GetNodeAttribute(node, "<xsl:value-of select="@name"/>", "Count");
            if (count != string.Empty)
            {
                param.<xsl:value-of select="@name"/>.Count = Convert.ToInt32(count);
            }
            </xsl:when>
            <xsl:when test="(@type='u8v' and @enumeration)">
            param.<xsl:value-of select="@name"/> = <xsl:call-template name='DefineDataType'/>.FromString(val, typeof(ENUM_<xsl:value-of select="@enumeration"/>));
            </xsl:when>
            <xsl:otherwise>
            param.<xsl:value-of select="@name"/> = <xsl:call-template name='DefineDataType'/>.FromString(val);
            </xsl:otherwise>
          </xsl:choose>
        </xsl:when>
        <xsl:when test="name()='parameter'">
            try
            {
          <xsl:choose>
            <xsl:when test="@type='Custom'">
                ArrayList xnl = XmlUtil.GetXmlNodeCustomChildren(node, nsmgr);
                if (xnl != null)
                {
                    for (int i=0; i<xsl:text disable-output-escaping="yes">&lt;</xsl:text>xnl.Count; i++)
                    {
                        if (!customArr.Contains(xnl[i]))
                        {
                            ICustom_Parameter custom =
                                CustomParamDecodeFactory.DecodeXmlNodeToCustomParameter((XmlNode)xnl[i]);
                            if (custom != null)
                            {
                                if (param.AddCustomParameter(custom))
                                {
                                    customArr.Add(xnl[i]);
                                }
                            }
                        }
                    }
                }
            </xsl:when>
            <xsl:otherwise>
              <xsl:choose>
                <xsl:when test="@repeat = '0-N' or @repeat = '1-N'">
                XmlNodeList xnl = XmlUtil.GetXmlNodes(node, "<xsl:call-template name='DefineParameterName'/>", nsmgr);
                if (xnl != null)
                {
                    if (xnl.Count != 0)
                    {
                        param.<xsl:call-template name='DefineParameterName'/> = new PARAM_<xsl:value-of select="@type"/>[xnl.Count];
                        for (int i=0; i<xsl:text disable-output-escaping="yes">&lt;</xsl:text>xnl.Count; i++)
                        {
                            param.<xsl:call-template name='DefineParameterName'/>[i] = PARAM_<xsl:value-of select="@type"/>.FromXmlNode(xnl[i]);
                        }
                    }
                }
                </xsl:when>
                <xsl:otherwise>
                XmlNodeList xnl = XmlUtil.GetXmlNodes(node, "<xsl:call-template name='DefineParameterName'/>", nsmgr);
                if (xnl != null)
                {
                    if (xnl.Count != 0)
                    {
                        param.<xsl:call-template name='DefineParameterName'/> = PARAM_<xsl:value-of select="@type"/>.FromXmlNode(xnl[0]);
                    }
                }
                </xsl:otherwise>
              </xsl:choose>
            </xsl:otherwise>
          </xsl:choose>
            } catch { }
        </xsl:when>
        <xsl:when test="name()='choice'">
            {
          <xsl:variable name="choiceParameterName">
            <xsl:call-template name='DefineParameterName'/>
          </xsl:variable>
            param.<xsl:copy-of select='$choiceParameterName'/> = new UNION_<xsl:value-of select="@type"/>();
          <xsl:for-each select='../../llrp:choiceDefinition'>
            <xsl:if test='@name=$choiceParameterName'>
            try
            {
                foreach (XmlNode ccnode in node.ChildNodes)
                {
                    switch (ccnode.Name)
                    {
              <xsl:for-each select='*'>
                    case "<xsl:call-template name='DefineParameterName'/>":
                <xsl:choose>
                  <xsl:when test="@type='Custom'">
                    default:
                        if (!customArr.Contains(ccnode))
                        {
                            ICustom_Parameter custom =
                                CustomParamDecodeFactory.DecodeXmlNodeToCustomParameter(ccnode);
                            if (custom != null)
                            {
                                if (param.<xsl:copy-of select='$choiceParameterName'/>.AddCustomParameter(custom))
                                {
                                    customArr.Add(ccnode);
                                }
                            }
                        }
                        break;
                  </xsl:when>
                  <xsl:otherwise>
                        param.<xsl:copy-of select='$choiceParameterName'/>.Add(PARAM_<xsl:value-of select='@type'/>.FromXmlNode(ccnode));
                        break;
                  </xsl:otherwise>
                </xsl:choose>
              </xsl:for-each>
                    }
                }
            } catch { }
            </xsl:if>
          </xsl:for-each>
            }
        </xsl:when>
      </xsl:choose>
    </xsl:for-each>
            return param;
        }
  </xsl:template>
  <xsl:template name="PARAMEncodeToBitArray">
        /// <xsl:text disable-output-escaping="yes">&lt;</xsl:text>summary<xsl:text disable-output-escaping="yes">&gt;</xsl:text>
        /// Encode this parameter into existing bit array
        /// <xsl:text disable-output-escaping="yes">&lt;</xsl:text>/summary<xsl:text disable-output-escaping="yes">&gt;</xsl:text>
        /// <xsl:text disable-output-escaping="yes">&lt;</xsl:text>param name="bit_array"<xsl:text disable-output-escaping="yes">&gt;</xsl:text>Existing bit array to be appended to<xsl:text disable-output-escaping="yes">&lt;</xsl:text>/param<xsl:text disable-output-escaping="yes">&gt;</xsl:text>
        /// <xsl:text disable-output-escaping="yes">&lt;</xsl:text>param name="cursor"<xsl:text disable-output-escaping="yes">&gt;</xsl:text>Current cursor in existing bit array<xsl:text disable-output-escaping="yes">&lt;</xsl:text>/param<xsl:text disable-output-escaping="yes">&gt;</xsl:text>
        public override void ToBitArray(ref bool[] bit_array, ref int cursor)
        {
            int len;
            int cursor_old = cursor;
            BitArray bArr;

            if (tvCoding)
            {
                bit_array[cursor] = true;
                cursor++;

                bArr = Util.ConvertIntToBitArray(typeID, 7);
                bArr.CopyTo(bit_array, cursor);

                cursor += 7;
            }
            else
            {
                cursor += 6;
                bArr = Util.ConvertIntToBitArray(typeID, 10);
                bArr.CopyTo(bit_array, cursor);

                cursor += 10;
                cursor += 16;   // Omit the parameter length, will be added at the end.
            }

    <xsl:for-each select="*">
      <xsl:if test="name()='field'">
            if (<xsl:value-of select="@name"/> != null)
            {
        <xsl:choose>
          <xsl:when test="@type='u1v' or @type='u8v' or @type='s8v' or @type='u16v' or @type='s16v' or @type='u32v' or @type='s32v' or @type='utf8v'">
                try
                {
                    int temp_cursor = cursor;
            <xsl:choose>
              <xsl:when test="@type='utf8v'">
                    BitArray tempBitArr = Util.ConvertIntToBitArray((UInt32)(<xsl:value-of select="@name"/>.Length), 16);
              </xsl:when>
              <xsl:otherwise>
                    BitArray tempBitArr = Util.ConvertIntToBitArray((UInt32)(<xsl:value-of select="@name"/>.Count), 16);
              </xsl:otherwise>
            </xsl:choose>
                    tempBitArr.CopyTo(bit_array, cursor);
                    cursor += 16;

                    tempBitArr = Util.ConvertObjToBitArray(<xsl:value-of select="@name"/>, <xsl:value-of select="@name"/>_len);
                    tempBitArr.CopyTo(bit_array, cursor);
                    cursor += tempBitArr.Length;
                }
                catch { }
          </xsl:when>
          <xsl:otherwise>
                try
                {
                    BitArray tempBitArr = Util.ConvertObjToBitArray(<xsl:value-of select="@name"/>, <xsl:value-of select="@name"/>_len);
                    tempBitArr.CopyTo(bit_array, cursor);
                    cursor += tempBitArr.Length;
                }
                catch { }
          </xsl:otherwise>
        </xsl:choose>
            }
      </xsl:if>
      <xsl:if test="name()='reserved'">
            cursor += param_reserved_len<xsl:copy-of select="position()"/>;
      </xsl:if>
      <xsl:if test="name()='parameter'">
            if (<xsl:call-template name='DefineParameterName'/> != null)
            {
        <xsl:choose>
          <xsl:when test="@repeat = '0-N' or @repeat = '1-N'">
                len = <xsl:call-template name='DefineParameterName'/>.Length;
                for (int i=0; i<xsl:text disable-output-escaping="yes">&lt;</xsl:text>len; i++)
                {
                    <xsl:call-template name='DefineParameterName'/>[i].ToBitArray(ref bit_array, ref cursor);
                }
          </xsl:when>
          <xsl:otherwise>
                <xsl:call-template name='DefineParameterName'/>.ToBitArray(ref bit_array, ref cursor);
          </xsl:otherwise>
        </xsl:choose>
            }
      </xsl:if>
      <xsl:if test="name()='choice'">
        <xsl:variable name="choiceParameterName">
          <xsl:call-template name='DefineParameterName'/>
        </xsl:variable>
            len = <xsl:copy-of select='$choiceParameterName'/>.Count;
            for (int i=0; i<xsl:text disable-output-escaping="yes">&lt;</xsl:text>len; i++)
            {
                <xsl:copy-of select='$choiceParameterName'/>[i].ToBitArray(ref bit_array, ref cursor);
            }
      </xsl:if>
    </xsl:for-each>

            if (!tvCoding)
            {
                UInt32 param_len = (UInt32)(cursor - cursor_old) / 8;
                bArr = Util.ConvertIntToBitArray(param_len, 16);
                bArr.CopyTo(bit_array, cursor_old + 16);
            }
        }
  </xsl:template>
  <xsl:template name="PARAMDecodeFromBitArray">
        /// <xsl:text disable-output-escaping="yes">&lt;</xsl:text>summary<xsl:text disable-output-escaping="yes">&gt;</xsl:text>
        /// Decode from existing bit array.
        /// <xsl:text disable-output-escaping="yes">&lt;</xsl:text>/summary<xsl:text disable-output-escaping="yes">&gt;</xsl:text>
        /// <xsl:text disable-output-escaping="yes">&lt;</xsl:text>param name="bit_array"<xsl:text disable-output-escaping="yes">&gt;</xsl:text>Existing bit array to be decoded<xsl:text disable-output-escaping="yes">&lt;</xsl:text>/param<xsl:text disable-output-escaping="yes">&gt;</xsl:text>
        /// <xsl:text disable-output-escaping="yes">&lt;</xsl:text>param name="cursor"<xsl:text disable-output-escaping="yes">&gt;</xsl:text>Current cursor in existing bit array<xsl:text disable-output-escaping="yes">&lt;</xsl:text>/param<xsl:text disable-output-escaping="yes">&gt;</xsl:text>
        /// <xsl:text disable-output-escaping="yes">&lt;</xsl:text>param name="length"<xsl:text disable-output-escaping="yes">&gt;</xsl:text>Total bits to be decoded<xsl:text disable-output-escaping="yes">&lt;</xsl:text>/param<xsl:text disable-output-escaping="yes">&gt;</xsl:text>
        /// <xsl:text disable-output-escaping="yes">&lt;</xsl:text>returns<xsl:text disable-output-escaping="yes">&gt;</xsl:text>PARAM_<xsl:value-of select="@name"/><xsl:text disable-output-escaping="yes">&lt;</xsl:text>/returns<xsl:text disable-output-escaping="yes">&gt;</xsl:text>
        public new static PARAM_<xsl:value-of select="@name"/> FromBitArray(ref BitArray bit_array, ref int cursor, int length)
        {
            if (cursor <xsl:text disable-output-escaping="yes">&gt;=</xsl:text> length) return null;

            UInt16 loop_control_counter = 1;    // Used for control choice element parsing loop

            int field_len = 0;
            int orig_cursor = cursor;
            int max_cursor = length;
            object obj_val;
            int parameter_len = 0;
            ArrayList param_list = new ArrayList();

            PARAM_<xsl:value-of select="@name"/> param = new PARAM_<xsl:value-of select="@name"/>();

            int param_type = 0;

            param.tvCoding = bit_array[cursor];
            if (param.tvCoding)
            {
                cursor++;
                param_type = (int)(UInt64)Util.CalculateVal(ref bit_array, ref cursor, 7);
            }
            else
            {
                cursor += 6;
                param_type = (int)(UInt64)Util.CalculateVal(ref bit_array, ref cursor, 10);
                param.length = (UInt16)(int)Util.DetermineFieldLength(ref bit_array, ref cursor);
                max_cursor = orig_cursor + param.length * 8;
            }

            if (param_type != param.TypeID)
            {
                cursor = orig_cursor;
                return null;
            }

    <xsl:for-each select="*">
      <xsl:if test="name()='field'">
            if (cursor <xsl:text disable-output-escaping="yes">&gt;</xsl:text> length || cursor <xsl:text disable-output-escaping="yes">&gt;</xsl:text> max_cursor)
            {
                throw new Exception("Input data is not a complete LLRP message");
            }
        <xsl:if test="@type='u1v' or @type='u8v' or @type='s8v' or @type='u16v' or @type='s16v' or @type='u32v' or @type='s32v' or @type='utf8v'">
            field_len = Util.DetermineFieldLength(ref bit_array, ref cursor);
        </xsl:if>
        <xsl:if test="@type='bytesToEnd'">
            field_len = (bit_array.Length - cursor)/8;
        </xsl:if>
        <xsl:if test="@type='u96'">
            field_len = 96;
        </xsl:if>
        <xsl:if test="@type='u2'">
            field_len = 2;
        </xsl:if>
        <xsl:if test="@type='u1'">
            field_len = 1;
        </xsl:if>
        <xsl:if test="@type='u8' or @type='s8'">
            field_len = 8;
        </xsl:if>
        <xsl:if test="@type='u16' or @type='s16'">
            field_len = 16;
        </xsl:if>
        <xsl:if test="@type='u32' or @type='s32'">
            field_len = 32;
        </xsl:if>
        <xsl:if test="@type='u64'">
            field_len = 64;
        </xsl:if>
        <xsl:choose>
          <xsl:when test="@enumeration and @type!='u8v'">
            Util.ConvertBitArrayToObj(ref bit_array, ref cursor, out obj_val, typeof(UInt32), field_len);
            param.<xsl:value-of select="@name"/> = (<xsl:call-template name='DefineDataType'/>)(UInt32)obj_val;
          </xsl:when>
          <xsl:otherwise>
            Util.ConvertBitArrayToObj(ref bit_array, ref cursor, out obj_val, typeof(<xsl:call-template name='DefineDataType'/>), field_len);
            param.<xsl:value-of select="@name"/> = (<xsl:call-template name='DefineDataType'/>)obj_val;
          </xsl:otherwise>
        </xsl:choose>
      </xsl:if>
      <xsl:if test="name()='reserved'">
            cursor += param_reserved_len<xsl:copy-of select="position()"/>;
      </xsl:if>
      <xsl:if test="name()='parameter'">
        <xsl:choose>
          <xsl:when test="@type='Custom'">
            while (true)
            {
                int temp_cursor = cursor;
                bool success = false;
                ICustom_Parameter custom =
                    CustomParamDecodeFactory.DecodeCustomParameter(ref bit_array, ref cursor, length);
                if (custom != null)
                {
                    if (cursor <xsl:text disable-output-escaping="yes">&lt;=</xsl:text> max_cursor)
                    {
                        if (param.AddCustomParameter(custom))
                        {
                            success = true;
                        }
                    }
                }

                if (!success)
                {
                    cursor = temp_cursor;
                    break;
                }
            }
          </xsl:when>
          <xsl:otherwise>
            <xsl:if test="@repeat = '1-N' or @repeat = '0-N'">
            param_list = new ArrayList();
            PARAM_<xsl:value-of select="@type"/> _param_<xsl:value-of select="@type"/>;
            while ((_param_<xsl:value-of select="@type"/> = PARAM_<xsl:value-of select="@type"/>.FromBitArray(ref bit_array, ref cursor, length)) != null)
            {
                param_list.Add(_param_<xsl:value-of select="@type"/>);
            }

            if (param_list.Count <xsl:text disable-output-escaping="yes">&gt;</xsl:text> 0)
            {
                param.<xsl:call-template name='DefineParameterName'/> = new PARAM_<xsl:value-of select="@type"/>[param_list.Count];
                for (int i=0; i<xsl:text disable-output-escaping="yes">&lt;</xsl:text>param_list.Count; i++)
                {
                    param.<xsl:call-template name='DefineParameterName'/>[i] = (PARAM_<xsl:value-of select="@type"/>)param_list[i];
                }
            }
            </xsl:if>
            <xsl:if test="@repeat = '1' or @repeat='0-1'">
            param.<xsl:call-template name='DefineParameterName'/> = PARAM_<xsl:value-of select="@type"/>.FromBitArray(ref bit_array, ref cursor, length);
            </xsl:if>
          </xsl:otherwise>
        </xsl:choose>
      </xsl:if>
      <xsl:if test="name()='choice'">
        <xsl:variable name="choiceParameterName">
          <xsl:call-template name='DefineParameterName'/>
        </xsl:variable>
            loop_control_counter = 1;
            while (loop_control_counter != 0)
            {
                loop_control_counter = 0;
        <xsl:for-each select='../../llrp:choiceDefinition'>
          <xsl:if test='@name=$choiceParameterName'>
            <xsl:for-each select='*'>
              <xsl:choose>
                <xsl:when test="@type='Custom'">
                int temp_cursor = cursor;
                ICustom_Parameter _param_Custom =
                    CustomParamDecodeFactory.DecodeCustomParameter(ref bit_array, ref cursor, length);
                if (_param_Custom != null)
                {
                    if (param.<xsl:copy-of select='$choiceParameterName'/>.AddCustomParameter(_param_Custom))
                    {
                        loop_control_counter++;
                    }
                    else
                    {
                        cursor = temp_cursor;
                    }
                }
                </xsl:when>
                <xsl:otherwise>
                PARAM_<xsl:value-of select='@type'/> _param_<xsl:value-of select='@type'/> =
                    PARAM_<xsl:value-of select='@type'/>.FromBitArray(ref bit_array, ref cursor, length);
                if (_param_<xsl:value-of select='@type'/> != null)
                {
                    loop_control_counter++;
                    param.<xsl:copy-of select='$choiceParameterName'/>.Add(_param_<xsl:value-of select='@type'/>);
                }
                </xsl:otherwise>
              </xsl:choose>
            </xsl:for-each>
          </xsl:if>
        </xsl:for-each>
            }
      </xsl:if>
    </xsl:for-each>
            return param;
        }
  </xsl:template>

  <xsl:template name ="MSGDecodeFromBitArray">
        /// <xsl:text disable-output-escaping="yes">&lt;</xsl:text>summary<xsl:text disable-output-escaping="yes">&gt;</xsl:text>
        /// Decode bit array to a MSG_<xsl:value-of select="@name"/>
        /// <xsl:text disable-output-escaping="yes">&lt;</xsl:text>/summary<xsl:text disable-output-escaping="yes">&gt;</xsl:text>
        /// <xsl:text disable-output-escaping="yes">&lt;</xsl:text>param name="bit_array"<xsl:text disable-output-escaping="yes">&gt;</xsl:text>input bit array<xsl:text disable-output-escaping="yes">&lt;</xsl:text>/param<xsl:text disable-output-escaping="yes">&gt;</xsl:text>
        /// <xsl:text disable-output-escaping="yes">&lt;</xsl:text>param name="cursor"<xsl:text disable-output-escaping="yes">&gt;</xsl:text>pointer to current position<xsl:text disable-output-escaping="yes">&lt;</xsl:text>/param<xsl:text disable-output-escaping="yes">&gt;</xsl:text>
        /// <xsl:text disable-output-escaping="yes">&lt;</xsl:text>param name="length"<xsl:text disable-output-escaping="yes">&gt;</xsl:text>data length<xsl:text disable-output-escaping="yes">&lt;</xsl:text>/param<xsl:text disable-output-escaping="yes">&gt;</xsl:text>
        /// <xsl:text disable-output-escaping="yes">&lt;</xsl:text>returns<xsl:text disable-output-escaping="yes">&gt;</xsl:text>MSG_<xsl:value-of select="@name"/><xsl:text disable-output-escaping="yes">&lt;</xsl:text>/returns<xsl:text disable-output-escaping="yes">&gt;</xsl:text>
        public new static MSG_<xsl:value-of select="@name"/> FromBitArray(ref BitArray bit_array, ref int cursor, int length)
        {
            if (cursor <xsl:text disable-output-escaping="yes">&gt;</xsl:text> length) return null;

            UInt16 loop_control_counter = 1;    // Used for control choice element parsing loop

            int field_len = 0;
            object obj_val;
            ArrayList param_list = new ArrayList();

            MSG_<xsl:value-of select="@name"/> obj = new MSG_<xsl:value-of select="@name"/>();

            int msg_type = 0;
            cursor += 6;
            msg_type = (int)(UInt64)Util.CalculateVal(ref bit_array, ref cursor, 10);

            if (msg_type != obj.msgType)
            {
                cursor -=16;
                return null;
            }

            obj.msgLen = (UInt32)(UInt64)Util.CalculateVal(ref bit_array, ref cursor, 32);
            obj.msgID = (UInt32)(UInt64)Util.CalculateVal(ref bit_array, ref cursor, 32);

    <xsl:for-each select="*">
      <xsl:if test="name()='field'">
            if (cursor <xsl:text disable-output-escaping="yes">&gt;</xsl:text> length)
                throw new Exception("Input data is not a complete LLRP message");
        <xsl:if test="@type='u1v' or @type='u8v' or @type='s8v' or @type='u16v' or @type='s16v' or @type='u32v' or @type='s32v' or @type='utf8v'">
            field_len = Util.DetermineFieldLength(ref bit_array, ref cursor);
        </xsl:if>
        <xsl:if test="@type='bytesToEnd'">
            field_len = (bit_array.Length - cursor) / 8;
        </xsl:if>
        <xsl:if test="@type='u96'">
            field_len = 96;
        </xsl:if>
        <xsl:if test="@type='u2'">
            field_len = 2;
        </xsl:if>
        <xsl:if test="@type='u1'">
            field_len = 1;
        </xsl:if>
        <xsl:if test="@type='u8' or @type='s8'">
            field_len = 8;
        </xsl:if>
        <xsl:if test="@type='u16' or @type='s16'">
            field_len = 16;
        </xsl:if>
        <xsl:if test="@type='u32' or @type='s32'">
            field_len = 32;
        </xsl:if>
        <xsl:if test="@type='u64'">
            field_len = 64;
        </xsl:if>
        <xsl:choose>
          <xsl:when test="@enumeration and @type!='u8v'">
            Util.ConvertBitArrayToObj(ref bit_array, ref cursor, out obj_val, typeof(UInt32), field_len);
            obj.<xsl:value-of select="@name"/> = (<xsl:call-template name='DefineDataType'/>)(UInt32)obj_val;
          </xsl:when>
          <xsl:otherwise>
            Util.ConvertBitArrayToObj(ref bit_array, ref cursor, out obj_val, typeof(<xsl:call-template name='DefineDataType'/>), field_len);
            obj.<xsl:value-of select="@name"/> = (<xsl:call-template name='DefineDataType'/>)obj_val;
          </xsl:otherwise>
        </xsl:choose>
      </xsl:if>
      <xsl:if test="name()='reserved'">
            cursor += param_reserved_len<xsl:copy-of select="position()"/>;
      </xsl:if>
      <xsl:if test="name()='parameter'">
        <xsl:choose>
          <xsl:when test="@type='Custom'">
            while (true)
            {
                int temp_cursor = cursor;
                bool success = false;
                ICustom_Parameter custom =
                    CustomParamDecodeFactory.DecodeCustomParameter(ref bit_array, ref cursor, length);
                if (custom != null)
                {
                    if (obj.AddCustomParameter(custom))
                    {
                        success = true;
                    }
                }

                if (!success)
                {
                    cursor = temp_cursor;
                    break;
                }
            }
          </xsl:when>
          <xsl:otherwise>
            <xsl:if test="@repeat = '1-N' or @repeat = '0-N'">
            param_list = new ArrayList();
            PARAM_<xsl:value-of select="@type"/> _param_<xsl:value-of select="@type"/>;
            while ((_param_<xsl:value-of select="@type"/> = PARAM_<xsl:value-of select="@type"/>.FromBitArray(ref bit_array, ref cursor, length)) != null)
            {
                param_list.Add(_param_<xsl:value-of select="@type"/>);
            }
            if (param_list.Count <xsl:text disable-output-escaping="yes">&gt;</xsl:text> 0)
            {
                obj.<xsl:call-template name='DefineParameterName'/> = new PARAM_<xsl:value-of select="@type"/>[param_list.Count];
                for (int i=0; i<xsl:text disable-output-escaping="yes">&lt;</xsl:text>param_list.Count; i++)
                {
                    obj.<xsl:call-template name='DefineParameterName'/>[i] = (PARAM_<xsl:value-of select="@type"/>)param_list[i];
                }
            }
            </xsl:if>
            <xsl:if test="@repeat = '1' or @repeat='0-1'">
            obj.<xsl:call-template name='DefineParameterName'/> = PARAM_<xsl:value-of select="@type"/>.FromBitArray(ref bit_array, ref cursor, length);
            </xsl:if>
          </xsl:otherwise>
        </xsl:choose>
      </xsl:if>
      <xsl:if test="name()='choice'">
        <xsl:variable name="choiceParameterName">
          <xsl:call-template name='DefineParameterName'/>
        </xsl:variable>
            loop_control_counter = 1;
            while (loop_control_counter != 0)
            {
                loop_control_counter = 0;
        <xsl:for-each select='../../llrp:choiceDefinition'>
          <xsl:if test='@name=$choiceParameterName'>
            <xsl:for-each select='*'>
                PARAM_<xsl:value-of select='@type'/> _param_<xsl:value-of select='@type'/> = PARAM_<xsl:value-of select='@type'/>.FromBitArray(ref bit_array, ref cursor, length);
                if (_param_<xsl:value-of select='@type'/> != null)
                {
                    loop_control_counter++;
                    obj.<xsl:copy-of select='$choiceParameterName'/>.Add(_param_<xsl:value-of select='@type'/>);
                }
                while ((_param_<xsl:value-of select='@type'/> = PARAM_<xsl:value-of select='@type'/>.FromBitArray(ref bit_array, ref cursor, length)) != null)
                {
                    obj.<xsl:copy-of select='$choiceParameterName'/>.Add(_param_<xsl:value-of select='@type'/>);
                }
            </xsl:for-each>
            }
          </xsl:if>
        </xsl:for-each>
      </xsl:if>
    </xsl:for-each>
            return obj;
        }
  </xsl:template>
  <xsl:template name ="MSGEncodeToBitArray">
        /// <xsl:text disable-output-escaping="yes">&lt;</xsl:text>summary<xsl:text disable-output-escaping="yes">&gt;</xsl:text>
        /// Encode message to boolean (bit) array
        /// <xsl:text disable-output-escaping="yes">&lt;</xsl:text>/summary<xsl:text disable-output-escaping="yes">&gt;</xsl:text>
        /// <xsl:text disable-output-escaping="yes">&lt;</xsl:text>returns<xsl:text disable-output-escaping="yes">&gt;</xsl:text>boolean array<xsl:text disable-output-escaping="yes">&lt;</xsl:text>/returns<xsl:text disable-output-escaping="yes">&gt;</xsl:text>
        public override bool[] ToBitArray()
        {
            int len = 0;
            int cursor = 0;
            bool[] bit_array = new bool[4*1024*1024*8]; // Handle messages up to 4 MB

            BitArray bArr = Util.ConvertIntToBitArray(version, 3);
            cursor += 3;
            bArr.CopyTo(bit_array, cursor);

            cursor += 3;
            bArr = Util.ConvertIntToBitArray(msgType, 10);
            bArr.CopyTo(bit_array, cursor);

            cursor += 10;
            bArr = Util.ConvertIntToBitArray(msgLen ,32);
            bArr.CopyTo(bit_array, cursor);

            cursor += 32;
            bArr = Util.ConvertIntToBitArray(msgID,32);
            bArr.CopyTo(bit_array, cursor);

            cursor += 32;
    <xsl:for-each select="*">
      <xsl:if test="name()='field'">
            if (<xsl:value-of select="@name"/> != null)
            {
        <xsl:choose>
          <xsl:when test="@type='u1v' or @type='u8v' or @type='s8v' or @type='u16v' or @type='s16v' or @type='u32v' or type='s32v' or @type='utf8v'">
                try
                {
                    int temp_cursor = cursor;
            <xsl:choose>
              <xsl:when test="@type='utf8v'">
                    BitArray tempBitArr = Util.ConvertIntToBitArray((UInt32)(<xsl:value-of select="@name"/>.Length), 16);
              </xsl:when>
              <xsl:otherwise>
                    BitArray tempBitArr = Util.ConvertIntToBitArray((UInt32)(<xsl:value-of select="@name"/>.Count), 16);
              </xsl:otherwise>
            </xsl:choose>
                    tempBitArr.CopyTo(bit_array, cursor);
                    cursor += 16;

                    tempBitArr = Util.ConvertObjToBitArray(<xsl:value-of select="@name"/>, <xsl:value-of select="@name"/>_len);
                    tempBitArr.CopyTo(bit_array, cursor);
                    cursor += tempBitArr.Length;
                }
                catch { }
          </xsl:when>
          <xsl:otherwise>
                try
                {
                    BitArray tempBitArr = Util.ConvertObjToBitArray(<xsl:value-of select="@name"/>, <xsl:value-of select="@name"/>_len);
                    tempBitArr.CopyTo(bit_array, cursor);
                    cursor += tempBitArr.Length;
                }
                catch { }
          </xsl:otherwise>
        </xsl:choose>
            }
      </xsl:if>
      <xsl:if test="name()='reserved'">
            cursor += param_reserved_len<xsl:copy-of select="position()"/>;
      </xsl:if>
      <xsl:if test="name()='parameter'">
            if (<xsl:call-template name='DefineParameterName'/> != null)
            {
        <xsl:choose>
          <xsl:when test="@repeat = '0-N' or @repeat = '1-N'">
                len = <xsl:call-template name='DefineParameterName'/>.Length;
                for (int i=0; i<xsl:text disable-output-escaping="yes">&lt;</xsl:text>len; i++)
                {
                    <xsl:call-template name='DefineParameterName'/>[i].ToBitArray(ref bit_array, ref cursor);
                }
          </xsl:when>
          <xsl:otherwise>
                <xsl:call-template name='DefineParameterName'/>.ToBitArray(ref bit_array, ref cursor);
          </xsl:otherwise>
        </xsl:choose>
            }
      </xsl:if>
      <xsl:if test="name()='choice'">
        <xsl:variable name="choiceParameterName">
          <xsl:call-template name='DefineParameterName'/>
        </xsl:variable>
            len = <xsl:copy-of select='$choiceParameterName'/>.Count;
            for (int i=0; i<xsl:text disable-output-escaping="yes">&lt;</xsl:text>len; i++)
            {
                <xsl:copy-of select='$choiceParameterName'/>[i].ToBitArray(ref bit_array, ref cursor);
            }
      </xsl:if>
    </xsl:for-each>

            UInt32 msg_len = (UInt32)cursor / 8;
            bArr = Util.ConvertIntToBitArray(msg_len ,32);
            bArr.CopyTo(bit_array, 16);

            bool[] boolArr = new bool[cursor];
            Array.Copy(bit_array, 0, boolArr, 0, cursor);

            return boolArr;
        }
  </xsl:template>
  <xsl:template name ="MSGToString">
        /// <xsl:text disable-output-escaping="yes">&lt;</xsl:text>summary<xsl:text disable-output-escaping="yes">&gt;</xsl:text>
        /// Serialize native message to xml string
        /// <xsl:text disable-output-escaping="yes">&lt;</xsl:text>/summary<xsl:text disable-output-escaping="yes">&gt;</xsl:text>
        /// <xsl:text disable-output-escaping="yes">&lt;</xsl:text>returns<xsl:text disable-output-escaping="yes">&gt;</xsl:text>Xml string<xsl:text disable-output-escaping="yes">&lt;</xsl:text>/returns<xsl:text disable-output-escaping="yes">&gt;</xsl:text>
        public override string ToString()
        {
            int len;

            string xml_str = "<xsl:text disable-output-escaping="yes">&lt;</xsl:text><xsl:value-of select="@name"/>";
            xml_str += string.Format(" xmlns=\"{0}\"\n", LLRPConstants.NAMESPACE_URI);
            xml_str += string.Format(" xmlns:llrp=\"{0}\"\n", LLRPConstants.NAMESPACE_URI);
            xml_str += " xmlns:xsi= \"http://www.w3.org/2001/XMLSchema-instance\"\n";
            xml_str += string.Format(" xsi:schemaLocation=\"{0} {1}\"\n", LLRPConstants.NAMESPACE_URI, LLRPConstants.NAMESPACE_SCHEMALOCATION);
            xml_str += " Version=\"" + version.ToString() + "\" MessageID=\"" + MSG_ID.ToString() + "\"" + "<xsl:text disable-output-escaping="yes">&gt;</xsl:text>" + "\r\n";
    <xsl:for-each select="*">
      <xsl:choose>
        <xsl:when test="name()='field'">
            if (<xsl:value-of select="@name"/> != null)
            {
                try
                {
          <xsl:choose>
            <xsl:when test="(@type='u1' or @type='u8' or @type='s8' or @type='u16' or @type='s16' or @type='u32' or @type='s32' or @type='u64') and not(@enumeration)">
                    xml_str += "  <xsl:text disable-output-escaping="yes">&lt;</xsl:text><xsl:value-of select="@name"/><xsl:text disable-output-escaping="yes">&gt;</xsl:text>" + Util.ConvertValueTypeToString(<xsl:value-of select="@name"/>, "<xsl:value-of select="@type"/>", "<xsl:value-of select="@format"/>") + "<xsl:text disable-output-escaping="yes">&lt;/</xsl:text><xsl:value-of select="@name"/><xsl:text disable-output-escaping="yes">&gt;</xsl:text>";
            </xsl:when>
            <xsl:when test="(@type='u8v' or @type='s8v' or @type='u16v' or @type='s16v' or @type='u32v' or @type='s32v' or @type='utf8v' or @type='u96' or @type='bytesToEnd') and not(@enumeration)">
                    xml_str += "  <xsl:text disable-output-escaping="yes">&lt;</xsl:text><xsl:value-of select="@name"/><xsl:text disable-output-escaping="yes">&gt;</xsl:text>" + Util.ConvertArrayTypeToString(<xsl:value-of select="@name"/>, "<xsl:value-of select="@type"/>", "<xsl:value-of select="@format"/>") + "<xsl:text disable-output-escaping="yes">&lt;/</xsl:text><xsl:value-of select="@name"/><xsl:text disable-output-escaping="yes">&gt;</xsl:text>";
            </xsl:when>
            <xsl:when test="@type='u1v'">
                    xml_str += "  <xsl:text disable-output-escaping="yes">&lt;</xsl:text><xsl:value-of select="@name"/> Count=\"" + <xsl:value-of select="@name"/>.Count + "\"<xsl:text disable-output-escaping="yes">&gt;</xsl:text>" + Util.ConvertArrayTypeToString(<xsl:value-of select="@name"/>, "<xsl:value-of select="@type"/>", "<xsl:value-of select="@format"/>") + "<xsl:text disable-output-escaping="yes">&lt;/</xsl:text><xsl:value-of select="@name"/><xsl:text disable-output-escaping="yes">&gt;</xsl:text>";
            </xsl:when>
            <xsl:when test="(@type='u8v' and @enumeration)">
                    xml_str += "  <xsl:text disable-output-escaping="yes">&lt;</xsl:text><xsl:value-of select="@name"/><xsl:text disable-output-escaping="yes">&gt;</xsl:text>" + <xsl:value-of select="@name"/>.ToString(typeof(ENUM_<xsl:value-of select="@enumeration"/>)) + "<xsl:text disable-output-escaping="yes">&lt;/</xsl:text><xsl:value-of select="@name"/><xsl:text disable-output-escaping="yes">&gt;</xsl:text>";
            </xsl:when>
            <xsl:otherwise>
                    xml_str += "  <xsl:text disable-output-escaping="yes">&lt;</xsl:text><xsl:value-of select="@name"/><xsl:text disable-output-escaping="yes">&gt;</xsl:text>" + <xsl:value-of select="@name"/>.ToString() + "<xsl:text disable-output-escaping="yes">&lt;/</xsl:text><xsl:value-of select="@name"/><xsl:text disable-output-escaping="yes">&gt;</xsl:text>";
            </xsl:otherwise>
          </xsl:choose>
                    xml_str += "\r\n";
                }
                catch { }
            }
        </xsl:when>
        <xsl:when test="name()='parameter'">
            if (<xsl:call-template name='DefineParameterName'/> != null)
            {
          <xsl:choose>
            <xsl:when test="@type='Custom'">
                len = <xsl:call-template name='DefineParameterName'/>.Length;
                for (int i=0; i<xsl:text disable-output-escaping="yes">&lt;</xsl:text>len; i++)
                {
		            xml_str += Util.Indent(<xsl:call-template name='DefineParameterName'/>[i].ToString());
                }
            </xsl:when>
            <xsl:otherwise>
              <xsl:choose>
                <xsl:when test="@repeat = '0-N' or @repeat = '1-N'">
                len = <xsl:call-template name='DefineParameterName'/>.Length;
                for (int i=0; i<xsl:text disable-output-escaping="yes">&lt;</xsl:text>len; i++)
                {
		            xml_str += Util.Indent(<xsl:call-template name='DefineParameterName'/>[i].ToString());
                }
                </xsl:when>
                <xsl:otherwise>
                xml_str += Util.Indent(<xsl:call-template name='DefineParameterName'/>.ToString());
                </xsl:otherwise>
              </xsl:choose>
            </xsl:otherwise>
          </xsl:choose>
            }
        </xsl:when>
        <xsl:when test="name()='choice'">
          <xsl:variable name="choiceParameterName">
            <xsl:call-template name='DefineParameterName'/>
          </xsl:variable>
            if (<xsl:call-template name='DefineParameterName'/> != null)
            {
                len = <xsl:copy-of select='$choiceParameterName'/>.Count;
                for (int i=0; i<xsl:text disable-output-escaping="yes">&lt;</xsl:text>len; i++)
                {
                    xml_str += Util.Indent(<xsl:copy-of select='$choiceParameterName'/>[i].ToString());
                }
            }
        </xsl:when>
      </xsl:choose>
    </xsl:for-each>
            xml_str += "<xsl:text disable-output-escaping="yes">&lt;/</xsl:text><xsl:value-of select="@name"/><xsl:text disable-output-escaping="yes">&gt;</xsl:text>";
            return xml_str;
        }
  </xsl:template>
  <xsl:template name ="MSGFromString">
        /// <xsl:text disable-output-escaping="yes">&lt;</xsl:text>summary<xsl:text disable-output-escaping="yes">&gt;</xsl:text>
        /// Deserialize a xml string to a MSG_<xsl:value-of select="@name"/>
        /// <xsl:text disable-output-escaping="yes">&lt;</xsl:text>/summary<xsl:text disable-output-escaping="yes">&gt;</xsl:text>
        /// <xsl:text disable-output-escaping="yes">&lt;</xsl:text>param name="str"<xsl:text disable-output-escaping="yes">&gt;</xsl:text>Input Xml string<xsl:text disable-output-escaping="yes">&lt;</xsl:text>/param<xsl:text disable-output-escaping="yes">&gt;</xsl:text>
        /// <xsl:text disable-output-escaping="yes">&lt;</xsl:text>returns<xsl:text disable-output-escaping="yes">&gt;</xsl:text>MSG_<xsl:value-of select="@name"/><xsl:text disable-output-escaping="yes">&lt;</xsl:text>/returns<xsl:text disable-output-escaping="yes">&gt;</xsl:text>
        public new static MSG_<xsl:value-of select="@name"/>  FromString(string str)
        {
            string count;
            string val;

            XmlDocument xdoc = new XmlDocument();
            xdoc.LoadXml(str);
            XmlNode node = (XmlNode)xdoc.DocumentElement;

            XmlNamespaceManager nsmgr = new XmlNamespaceManager(node.OwnerDocument.NameTable);
            nsmgr.AddNamespace("", LLRPConstants.NAMESPACE_URI);
            nsmgr.AddNamespace(LLRPConstants.NAMESPACE_PREFIX, LLRPConstants.NAMESPACE_URI);

            MSG_<xsl:value-of select="@name"/> msg = new MSG_<xsl:value-of select="@name"/>();
            try { msg.MSG_ID = Convert.ToUInt32(XmlUtil.GetNodeAttrValue(node, "MessageID")); }catch { }

    <xsl:for-each select="*">
      <xsl:choose>
        <xsl:when test="name()='field'">
            val = XmlUtil.GetNodeValue(node, "<xsl:value-of select="@name"/>");
          <xsl:choose>
            <xsl:when test="@enumeration and @type!='u8v'">
            msg.<xsl:value-of select="@name"/> = (<xsl:call-template name='DefineDataType'/>)Enum.Parse(typeof(<xsl:call-template name='DefineDataType'/>), val);
            </xsl:when>
            <xsl:when test="(@type='u1' or @type='u8' or @type='s8' or @type='u16' or @type='s16' or @type='u32' or @type='s32' or @type='u64') and not(@enumeration)">
            msg.<xsl:value-of select="@name"/> = (<xsl:call-template name='DefineDataType'/>)(Util.ParseValueTypeFromString(val, "<xsl:value-of select="@type"/>", "<xsl:value-of select="@format"/>"));
            </xsl:when>
            <xsl:when test="(@type='u8v' or @type='s8v' or @type='u16v' or @type='s16v' or @type='u32v' or @type='s32v' or @type='utf8v' or @type='u96' or @type='bytesToEnd') and not(@enumeration)">
            msg.<xsl:value-of select="@name"/> = (<xsl:call-template name='DefineDataType'/>)(Util.ParseArrayTypeFromString(val, "<xsl:value-of select="@type"/>", "<xsl:value-of select="@format"/>"));
            </xsl:when>
            <xsl:when test="@type='u1v'">
            msg.<xsl:value-of select="@name"/> = (<xsl:call-template name='DefineDataType'/>)(Util.ParseArrayTypeFromString(val, "<xsl:value-of select="@type"/>", "<xsl:value-of select="@format"/>"));
            count = XmlUtil.GetNodeAttribute(node, "<xsl:value-of select="@name"/>", "Count");
            if (count != string.Empty)
            {
                msg.<xsl:value-of select="@name"/>.Count = Convert.ToInt32(count);
            }
            </xsl:when>
            <xsl:when test="(@type='u8v' and @enumeration)">
            msg.<xsl:value-of select="@name"/> = <xsl:call-template name='DefineDataType'/>.FromString(val, typeof(ENUM_<xsl:value-of select="@enumeration"/>));
            </xsl:when>
            <xsl:otherwise>
            msg.<xsl:value-of select="@name"/> = <xsl:call-template name='DefineDataType'/>.FromString(val);
            </xsl:otherwise>
          </xsl:choose>
        </xsl:when>
        <xsl:when test="name()='parameter'">
            try
            {
          <xsl:choose>
            <xsl:when test="@type='Custom'">
                ArrayList xnl = XmlUtil.GetXmlNodeCustomChildren(node, nsmgr);
                if (xnl != null)
                {
                    for(int i=0; i<xsl:text disable-output-escaping="yes">&lt;</xsl:text>xnl.Count; i++)
                    {
                        ICustom_Parameter custom =
                            CustomParamDecodeFactory.DecodeXmlNodeToCustomParameter((XmlNode)xnl[i]);
                        if (custom != null)
                        {
                            msg.AddCustomParameter(custom);
                        }
                    }
                }
            </xsl:when>
            <xsl:otherwise>
              <xsl:choose>
                <xsl:when test="@repeat = '0-N' or @repeat = '1-N'">
                XmlNodeList xnl = XmlUtil.GetXmlNodes(node, "<xsl:call-template name='DefineParameterName'/>", nsmgr);
                if (xnl != null)
                {
                    if (xnl.Count != 0)
                    {
                        msg.<xsl:call-template name='DefineParameterName'/> = new PARAM_<xsl:value-of select="@type"/>[xnl.Count];
                        for (int i=0; i<xsl:text disable-output-escaping="yes">&lt;</xsl:text>xnl.Count; i++)
                        {
                            msg.<xsl:call-template name='DefineParameterName'/>[i] = PARAM_<xsl:value-of select="@type"/>.FromXmlNode(xnl[i]);
                        }
                    }
                }
                </xsl:when>
                <xsl:otherwise>
                XmlNodeList xnl = XmlUtil.GetXmlNodes(node, "<xsl:call-template name='DefineParameterName'/>", nsmgr);
                if (xnl != null)
                {
                    if (xnl.Count != 0)
                    {
                        msg.<xsl:call-template name='DefineParameterName'/> = PARAM_<xsl:value-of select="@type"/>.FromXmlNode(xnl[0]);
                    }
                }
                </xsl:otherwise>
              </xsl:choose>
            </xsl:otherwise>
          </xsl:choose>
            } catch { }
        </xsl:when>
        <xsl:when test="name()='choice'">
            {
          <xsl:variable name="choiceParameterName">
            <xsl:call-template name='DefineParameterName'/>
          </xsl:variable>
            msg.<xsl:copy-of select='$choiceParameterName'/> = new UNION_<xsl:value-of select="@type"/>();
          <xsl:for-each select='../../llrp:choiceDefinition'>
            <xsl:if test='@name=$choiceParameterName'>
            try
            {
                foreach (XmlNode ccnode in node.ChildNodes)
                {
                    switch (ccnode.Name)
                    {
              <xsl:for-each select='*'>
                    case "<xsl:call-template name='DefineParameterName'/>":
                <xsl:choose>
                  <xsl:when test="@type='Custom'">
                    default:
                        ICustom_Parameter custom =
                            CustomParamDecodeFactory.DecodeXmlNodeToCustomParameter(ccnode);
                        if (custom != null)
                        {
                            msg.AddCustomParameter(custom);
                        }
                        break;
                  </xsl:when>
                  <xsl:otherwise>
                        msg.<xsl:copy-of select='$choiceParameterName'/>.Add(PARAM_<xsl:value-of select='@type'/>.FromXmlNode(ccnode));
                        break;
                  </xsl:otherwise>
                </xsl:choose>
              </xsl:for-each>
                    }
                }
            } catch { }
            </xsl:if>
          </xsl:for-each>
            }
        </xsl:when>
      </xsl:choose>
    </xsl:for-each>
            return msg;
        }
  </xsl:template>

  <xsl:template name ="Comments">
    ///<xsl:text disable-output-escaping="yes">&lt;</xsl:text>summary<xsl:text disable-output-escaping="yes">&gt;</xsl:text>
    ///<xsl:for-each select ="llrp:annotation/llrp:description/h:p">
    ///<xsl:value-of select="."/></xsl:for-each>
    ///
    ///For more information, please refer to:
    ///<xsl:for-each select ="llrp:annotation/llrp:documentation/h:a">
    ///<xsl:text disable-output-escaping="yes">&lt;</xsl:text>see cref="<xsl:value-of select="@href"/>"<xsl:text disable-output-escaping="yes">&gt;</xsl:text><xsl:value-of select="."/>,<xsl:text disable-output-escaping="yes">&lt;</xsl:text>/see<xsl:text disable-output-escaping="yes">&gt;</xsl:text>
    </xsl:for-each>
    ///<xsl:text disable-output-escaping="yes">&lt;</xsl:text>/summary<xsl:text disable-output-escaping="yes">&gt;</xsl:text>
  </xsl:template>

</xsl:stylesheet>
