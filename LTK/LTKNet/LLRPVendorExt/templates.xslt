<?xml version="1.0" encoding="UTF-8" ?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" 
	xmlns:llrp="http://www.llrp.org/ltk/schema/core/encoding/binary/1.0"
  xmlns:h="http://www.w3.org/1999/xhtml">
  <xsl:output omit-xml-declaration='yes' method='text' indent='yes'/>

  <xsl:template name="DefineDataType" match="field">
    <xsl:choose>
      <xsl:when test="@enumeration and @type!='u8v'">
        ENUM_<xsl:value-of select="@enumeration"/>
      </xsl:when>
      <xsl:otherwise>
        <xsl:choose>
          <xsl:when test="@type = 'u1'">bool</xsl:when>
          <xsl:when test="@type = 'u1v'">LLRPBitArray</xsl:when>
          <xsl:when test="@type = 'u2'">TwoBits</xsl:when>
          <xsl:when test="@type = 'u8'">byte</xsl:when>
          <xsl:when test="@type = 's8'">sbyte</xsl:when>
          <xsl:when test="@type = 'u8v'">ByteArray</xsl:when>
          <xsl:when test="@type = 'u16'">UInt16</xsl:when>
          <xsl:when test="@type = 'u16v'">UInt16Array</xsl:when>
          <xsl:when test="@type = 's16'">Int16</xsl:when>
          <xsl:when test="@type = 'u32'">UInt32</xsl:when>
          <xsl:when test="@type = 'u32v'">UInt32Array</xsl:when>
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
      <xsl:when test="@enumeration">;</xsl:when>
      <xsl:otherwise>
        <xsl:choose>
          <xsl:when test="@type = 'u1'">=false;</xsl:when>
          <xsl:when test="@type = 'u1v'">=new LLRPBitArray();</xsl:when>
          <xsl:when test="@type = 'u2'">=new TwoBits(0);</xsl:when>
          <xsl:when test="@type = 'u8'">=0;</xsl:when>
          <xsl:when test="@type = 's8'">=0;</xsl:when>
          <xsl:when test="@type = 'u8v'">=new ByteArray();</xsl:when>
          <xsl:when test="@type = 'u16'">=0;</xsl:when>
          <xsl:when test="@type = 'u16v'">=new UInt16Array();</xsl:when>
          <xsl:when test="@type = 's16'">=0;</xsl:when>
          <xsl:when test="@type = 'u32'">=0;</xsl:when>
          <xsl:when test="@type = 'u32v'">=new UInt32Array();</xsl:when>
          <xsl:when test="@type = 'u64'">=0;</xsl:when>
          <xsl:when test="@type = 'utf8v'">=string.Empty;</xsl:when>
          <xsl:when test="@type = 'u96'">=new LLRPBitArray();</xsl:when>
          <xsl:when test="@type = 'bytesToEnd'">=new ByteArray();</xsl:when>
          <xsl:otherwise>Unkown_Type</xsl:otherwise>
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
          <xsl:when test="@type = 'u16'">16;</xsl:when>
          <xsl:when test="@type = 'u32'">32;</xsl:when>
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

  <xsl:template name="VendorPARAMDecodeFromBitArray">
    /// <xsl:text disable-output-escaping="yes">&lt;</xsl:text>summary<xsl:text disable-output-escaping="yes">&gt;</xsl:text>
    /// Decode from existing bit array.
    /// <xsl:text disable-output-escaping="yes">&lt;</xsl:text>/summary<xsl:text disable-output-escaping="yes">&gt;</xsl:text>
    /// <xsl:text disable-output-escaping="yes">&lt;</xsl:text>param name="bit_array"<xsl:text disable-output-escaping="yes">&gt;</xsl:text>Existing bit array to be decoded<xsl:text disable-output-escaping="yes">&lt;</xsl:text>/param<xsl:text disable-output-escaping="yes">&gt;</xsl:text>
    /// <xsl:text disable-output-escaping="yes">&lt;</xsl:text>param name="cursor"<xsl:text disable-output-escaping="yes">&gt;</xsl:text>Current cursor in existing bit array<xsl:text disable-output-escaping="yes">&lt;</xsl:text>/param<xsl:text disable-output-escaping="yes">&gt;</xsl:text>
    /// <xsl:text disable-output-escaping="yes">&lt;</xsl:text>param name="length"<xsl:text disable-output-escaping="yes">&gt;</xsl:text>Total bits to be decoded<xsl:text disable-output-escaping="yes">&lt;</xsl:text>/param<xsl:text disable-output-escaping="yes">&gt;</xsl:text>
    /// <xsl:text disable-output-escaping="yes">&lt;</xsl:text>returns<xsl:text disable-output-escaping="yes">&gt;</xsl:text>PARAM_<xsl:value-of select="@name"/><xsl:text disable-output-escaping="yes">&lt;</xsl:text>/returns<xsl:text disable-output-escaping="yes">&gt;</xsl:text>
    public new static PARAM_<xsl:value-of select="@name"/> FromBitArray(ref BitArray bit_array, ref int cursor, int length)
    {
    if(cursor<xsl:text disable-output-escaping="yes">&gt;=</xsl:text>length)return null;

    UInt16 loop_control_counter = 1;    //used for control choice element parsing loop

    int field_len = 0;
    object obj_val;
    int parameter_len = 0;
    ArrayList param_list = new ArrayList();

    PARAM_<xsl:value-of select="@name"/> obj = new PARAM_<xsl:value-of select="@name"/>();

    int param_type = 0;

    if(bit_array[cursor])obj.tvCoding = true;
    if(obj.tvCoding)
    {
    cursor ++;
    param_type = (int)(UInt64)Util.CalculateVal(ref bit_array, ref cursor, 7);

    if(param_type!= obj.TypeID)
    {
    cursor -=8;
    return null;
    }
    }
    else
    {
    cursor += 6;
    param_type = (int)(UInt64)Util.CalculateVal(ref bit_array, ref cursor, 10);

    if(param_type!=obj.TypeID)
    {
    cursor -=16;
    return null;
    }
    obj.length = (UInt16)(int)Util.DetermineFieldLength(ref bit_array, ref cursor);
    }

    obj.VendorIdentifier = (UInt32)(UInt64)Util.CalculateVal(ref bit_array, ref cursor, 32);
    obj.ParameterSubtype = (UInt32)(UInt64)Util.CalculateVal(ref bit_array, ref cursor, 32);

    <xsl:for-each select="*">
      <xsl:if test="name()='field'">
        if(cursor<xsl:text disable-output-escaping="yes">&gt;</xsl:text>length)throw new Exception("Input data is not a complete LLRP message");
        <xsl:if test="@type='u1v' or @type='u8v' or @type='u16v' or @type='u32v' or @type='utf8v' or @type='bytesToEnd'">
          field_len = Util.DetermineFieldLength(ref bit_array, ref cursor);
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
        <xsl:if test="@type='u32'">
          field_len = 32;
        </xsl:if>
        <xsl:if test="@type='u64'">
          field_len = 64;
        </xsl:if>
        <xsl:choose>
          <xsl:when test="@enumeration">
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
            <xsl:if test="@repeat = '1-N' or @repeat = '0-N'">
              param_list = new ArrayList();
              PARAM_<xsl:copy-of select="$vendor_name"/>_Custom _param_<xsl:value-of select="@type"/> =  PARAM_<xsl:copy-of select="$vendor_name"/>_Custom.FromBitArray(ref bit_array, ref cursor, length);
              if(_param_<xsl:value-of select="@type"/>!=null)
              {param_list.Add(_param_<xsl:value-of select="@type"/>);
              while((_param_<xsl:value-of select="@type"/>=PARAM_<xsl:copy-of select="$vendor_name"/>_Custom.FromBitArray(ref bit_array, ref cursor, length))!=null)param_list.Add(_param_<xsl:value-of select="@type"/>);
              if(param_list.Count<xsl:text disable-output-escaping="yes">&gt;</xsl:text>0)
              {
              obj.<xsl:call-template name='DefineParameterName'/> = new PARAM_<xsl:copy-of select="$vendor_name"/>_Custom[param_list.Count];
              for(int i=0;i<xsl:text disable-output-escaping="yes">&lt;</xsl:text>param_list.Count;i++)
              obj.<xsl:call-template name='DefineParameterName'/>[i] = (PARAM_<xsl:copy-of select="$vendor_name"/>_Custom)param_list[i];
              }
              }
            </xsl:if>
            <xsl:if test="@repeat = '1' or @repeat='0-1'">
              obj.<xsl:call-template name='DefineParameterName'/> = PARAM_<xsl:copy-of select="$vendor_name"/>_Custom.FromBitArray(ref bit_array, ref cursor, length);
            </xsl:if>
          </xsl:when>
          <xsl:otherwise>
            <xsl:if test="@repeat = '1-N' or @repeat = '0-N'">
              param_list = new ArrayList();
              PARAM_<xsl:value-of select="@type"/> _param_<xsl:value-of select="@type"/> =  PARAM_<xsl:value-of select="@type"/>.FromBitArray(ref bit_array, ref cursor, length);
              if(_param_<xsl:value-of select="@type"/>!=null)
              {param_list.Add(_param_<xsl:value-of select="@type"/>);
              while((_param_<xsl:value-of select="@type"/>=PARAM_<xsl:value-of select="@type"/>.FromBitArray(ref bit_array, ref cursor, length))!=null)param_list.Add(_param_<xsl:value-of select="@type"/>);
              if(param_list.Count<xsl:text disable-output-escaping="yes">&gt;</xsl:text>0)
              {
              obj.<xsl:call-template name='DefineParameterName'/> = new PARAM_<xsl:value-of select="@type"/>[param_list.Count];
              for(int i=0;i<xsl:text disable-output-escaping="yes">&lt;</xsl:text>param_list.Count;i++)
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
        while(loop_control_counter!=0)
        {
        loop_control_counter = 0;
        <xsl:for-each select='../../llrp:choiceDefinition'>
          <xsl:if test='@name=$choiceParameterName'>
            <xsl:for-each select='*'>
              <xsl:choose>
                <xsl:when test="@type='Custom'">
                  ICustom_Parameter sub_custom = CustomParamDecodeFactory.DecodeCustomParameter(ref bit_array, ref cursor, length);
                  if(sub_custom!=null)
                  {
                  loop_control_counter++;
                  param.<xsl:copy-of select='$choiceParameterName'/>.Add(sub_custom);
                  while((sub_custom = CustomParamDecodeFactory.DecodeCustomParameter(ref bit_array, ref cursor, length))!=null)
                  param.<xsl:copy-of select='$choiceParameterName'/>.Add(sub_custom);
                  }
                </xsl:when>
                <xsl:otherwise>
                  PARAM_<xsl:value-of select='@type'/> _param_<xsl:value-of select='@type'/> = PARAM_<xsl:value-of select='@type'/>.FromBitArray(ref bit_array, ref cursor, length);
                  if(_param_<xsl:value-of select='@type'/>!=null)
                  {
                  loop_control_counter++;
                  param.<xsl:copy-of select='$choiceParameterName'/>.Add(_param_<xsl:value-of select='@type'/>);
                  while((_param_<xsl:value-of select='@type'/> = PARAM_<xsl:value-of select='@type'/>.FromBitArray(ref bit_array, ref cursor, length))!=null)
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
    return obj;
    }
  </xsl:template>
  <xsl:template name="VendorPARAMEncodeToBitArray">
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

    if(tvCoding)
    {
    bit_array[cursor] = true;
    cursor++;

    bArr = Util.ConvertIntToBitArray(typeID, 7);
    bArr.CopyTo(bit_array, cursor);

    cursor+=7;
    }
    else
    {
    cursor += 6;
    bArr = Util.ConvertIntToBitArray(typeID, 10);
    bArr.CopyTo(bit_array, cursor);

    cursor+=10;

    cursor+=16;
    }

    bArr = Util.ConvertIntToBitArray(VendorIdentifier, 32);
    bArr.CopyTo(bit_array, cursor);
    cursor +=32;

    bArr = Util.ConvertIntToBitArray(ParameterSubtype, 32);
    bArr.CopyTo(bit_array, cursor);
    cursor +=32;

    <xsl:for-each select="*">
      <xsl:if test="name()='field'">
        if(<xsl:value-of select="@name"/>!=null)
        {
        <xsl:choose>
          <xsl:when test="@type='u1v' or @type='u8v' or @type='u16v' or @type='u32v' or @type='utf8v' or @type='u96' or @type='bytesToEnd'">
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
            cursor+=16;

            tempBitArr = Util.ConvertObjToBitArray(<xsl:value-of select="@name"/>, <xsl:value-of select="@name"/>_len);
            tempBitArr.CopyTo(bit_array, cursor);
            cursor += tempBitArr.Length;
            }
            catch
            {
            }
          </xsl:when>
          <xsl:otherwise>
            try
            {
            BitArray tempBitArr = Util.ConvertObjToBitArray(<xsl:value-of select="@name"/>, <xsl:value-of select="@name"/>_len);
            tempBitArr.CopyTo(bit_array, cursor);
            cursor += tempBitArr.Length;
            }
            catch{}
          </xsl:otherwise>
        </xsl:choose>
        }
      </xsl:if>
      <xsl:if test="name()='reserved'">
        cursor += param_reserved_len<xsl:copy-of select="position()"/>;
      </xsl:if>
      <xsl:if test="name()='parameter'">
        if(<xsl:call-template name='DefineParameterName'/> != null)
        {
        <xsl:choose>
          <xsl:when test="@repeat = '0-N' or @repeat = '1-N'">
            len = <xsl:call-template name='DefineParameterName'/>.Length;
            for(int i=0; i<xsl:text disable-output-escaping="yes">&lt;</xsl:text>len;i++)
            <xsl:call-template name='DefineParameterName'/>[i].ToBitArray(ref bit_array, ref cursor);
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
        for(int i=0; i<xsl:text disable-output-escaping="yes">&lt;</xsl:text>len;i++)<xsl:copy-of select='$choiceParameterName'/>[i].ToBitArray(ref bit_array, ref cursor);
      </xsl:if>
    </xsl:for-each>

    if(!tvCoding)
    {
    UInt32 param_len = (UInt32)(cursor-cursor_old)/8;
    bArr = Util.ConvertIntToBitArray(param_len,16);
    bArr.CopyTo(bit_array, cursor_old+16);
    }
    }
  </xsl:template>

  <xsl:template name="VendorMSGToString">
    /// <xsl:text disable-output-escaping="yes">&lt;</xsl:text>summary<xsl:text disable-output-escaping="yes">&gt;</xsl:text>
    /// Serialize native message to xml string
    /// <xsl:text disable-output-escaping="yes">&lt;</xsl:text>/summary<xsl:text disable-output-escaping="yes">&gt;</xsl:text>
    /// <xsl:text disable-output-escaping="yes">&lt;</xsl:text>returns>Xml string<xsl:text disable-output-escaping="yes">&lt;</xsl:text>/returns>
    public override string ToString()
    {
    int len;
    string xml_str = "<xsl:text disable-output-escaping="yes">&lt;</xsl:text><xsl:value-of select="@name"/>"+ " Version=\"" + version.ToString() + "\" MessageID=\"" + MSG_ID.ToString() + "\"" + "<xsl:text disable-output-escaping="yes">&gt;</xsl:text>";
    <xsl:for-each select="*">
      <xsl:choose>
        <xsl:when test="name()='field'">
          if(<xsl:value-of select="@name"/>!=null)
          {
          try
          {
          <xsl:choose>
            <xsl:when test="(@type='u1' or @type='u8' or @type='s8' or @type='u16' or @type='s16' or @type='u32' or @type='s32' or @type='u64') and not(@enumeration)">
              xml_str +="<xsl:text disable-output-escaping="yes">&lt;</xsl:text><xsl:value-of select="@name"/><xsl:text disable-output-escaping="yes">&gt;</xsl:text>" + Util.ConvertValueTypeToString(<xsl:value-of select="@name"/>, "<xsl:value-of select="@type"/>", "<xsl:value-of select="@format"/>") + "<xsl:text disable-output-escaping="yes">&lt;/</xsl:text><xsl:value-of select="@name"/><xsl:text disable-output-escaping="yes">&gt;</xsl:text>";
            </xsl:when>
            <xsl:when test="(@type='u8v' or @type='u16v' or @type='u32v' or @type='utf8v' or @type='u1v' or @type='u96' or @type='bytesToEnd') and not(@enumeration)">
              xml_str +="<xsl:text disable-output-escaping="yes">&lt;</xsl:text><xsl:value-of select="@name"/><xsl:text disable-output-escaping="yes">&gt;</xsl:text>" + Util.ConvertArrayTypeToString(<xsl:value-of select="@name"/>, "<xsl:value-of select="@type"/>", "<xsl:value-of select="@format"/>") + "<xsl:text disable-output-escaping="yes">&lt;/</xsl:text><xsl:value-of select="@name"/><xsl:text disable-output-escaping="yes">&gt;</xsl:text>";
            </xsl:when>
            <xsl:otherwise>
              xml_str +="<xsl:text disable-output-escaping="yes">&lt;</xsl:text><xsl:value-of select="@name"/><xsl:text disable-output-escaping="yes">&gt;</xsl:text>" + <xsl:value-of select="@name"/>.ToString() + "<xsl:text disable-output-escaping="yes">&lt;/</xsl:text><xsl:value-of select="@name"/><xsl:text disable-output-escaping="yes">&gt;</xsl:text>";
            </xsl:otherwise>
          </xsl:choose>
          }
          catch{}
          }
        </xsl:when>
        <xsl:when test="name()='parameter'">
          if(<xsl:call-template name='DefineParameterName'/>!= null)
          {
          <xsl:choose>
            <xsl:when test="@repeat = '0-N' or @repeat = '1-N'">
              len = <xsl:call-template name='DefineParameterName'/>.Length;
              for(int i=0; i<xsl:text disable-output-escaping="yes">&lt;</xsl:text>len;i++)
              xml_str += <xsl:call-template name='DefineParameterName'/>[i].ToString();
            </xsl:when>
            <xsl:otherwise>
              xml_str += <xsl:call-template name='DefineParameterName'/>.ToString();
            </xsl:otherwise>
          </xsl:choose>
          }
        </xsl:when>
        <xsl:when test="name()='choice'">
          <xsl:variable name="choiceParameterName">
            <xsl:call-template name='DefineParameterName'/>
          </xsl:variable>
          if(<xsl:call-template name='DefineParameterName'/>!= null)
          {
          len = <xsl:copy-of select='$choiceParameterName'/>.Count;
          for(int i=0; i<xsl:text disable-output-escaping="yes">&lt;</xsl:text>len;i++)xml_str += <xsl:copy-of select='$choiceParameterName'/>[i].ToString();
          }
        </xsl:when>
        <xsl:otherwise>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:for-each>
    xml_str += "<xsl:text disable-output-escaping="yes">&lt;/</xsl:text><xsl:value-of select="@name"/><xsl:text disable-output-escaping="yes">&gt;</xsl:text>";
    return xml_str;
    }
  </xsl:template>
  <xsl:template name="VendorMSGFromString">
    /// <xsl:text disable-output-escaping="yes">&lt;</xsl:text>summary<xsl:text disable-output-escaping="yes">&gt;</xsl:text>
    /// Deserialize a xml string to a MSG_<xsl:value-of select="@name"/>
    /// <xsl:text disable-output-escaping="yes">&lt;</xsl:text>/summary<xsl:text disable-output-escaping="yes">&gt;</xsl:text>
    /// <xsl:text disable-output-escaping="yes">&lt;</xsl:text>param name="str"<xsl:text disable-output-escaping="yes">&gt;</xsl:text>Input Xml string<xsl:text disable-output-escaping="yes">&lt;</xsl:text>/param<xsl:text disable-output-escaping="yes">&gt;</xsl:text>
    /// <xsl:text disable-output-escaping="yes">&lt;</xsl:text>returns<xsl:text disable-output-escaping="yes">&gt;</xsl:text>MSG_<xsl:value-of select="@name"/><xsl:text disable-output-escaping="yes">&lt;</xsl:text>/returns<xsl:text disable-output-escaping="yes">&gt;</xsl:text>
    public new static MSG_<xsl:value-of select="@name"/>  FromString(string str)
    {
    string val;

    XmlDocument xdoc = new XmlDocument();
    xdoc.LoadXml(str);
    XmlNode node = (XmlNode)xdoc.DocumentElement;

    MSG_<xsl:value-of select="@name"/> msg = new MSG_<xsl:value-of select="@name"/>();
    try{msg.MSG_ID = Convert.ToUInt16(XmlUtil.GetNodeAttrValue(node, "MessageID"));}catch{}

    <xsl:for-each select="*">
      <xsl:choose>
        <xsl:when test="name()='field'">
          val = XmlUtil.GetNodeValue(node, "<xsl:value-of select="@name"/>");
          <xsl:choose>
            <xsl:when test="@enumeration">
              msg.<xsl:value-of select="@name"/> = (<xsl:call-template name='DefineDataType'/>)Enum.Parse(typeof(<xsl:call-template name='DefineDataType'/>), val);
            </xsl:when>
            <xsl:when test="(@type='u1' or @type='u8' or @type='s8' or @type='u16' or @type='s16' or @type='u32' or @type='s32' or @type='u64') and not(@enumeration)">
              msg.<xsl:value-of select="@name"/> = (<xsl:call-template name='DefineDataType'/>)(Util.ParseValueTypeFromString(val, "<xsl:value-of select="@type"/>", "<xsl:value-of select="@format"/>"));
            </xsl:when>
            <xsl:when test="(@type='u8v' or @type='u16v' or @type='u32v' or @type='utf8v' or @type='u1v' or @type='u96' or @type='bytesToEnd') and not(@enumeration)">
              msg.<xsl:value-of select="@name"/> = (<xsl:call-template name='DefineDataType'/>)(Util.ParseArrayTypeFromString(val, "<xsl:value-of select="@type"/>", "<xsl:value-of select="@format"/>"));
            </xsl:when>
            <xsl:otherwise>
              msg.<xsl:value-of select="@name"/> = <xsl:call-template name='DefineDataType'/>.FromString(val);
            </xsl:otherwise>
          </xsl:choose>
        </xsl:when>
        <xsl:when test="name()='parameter'">
          {
          <xsl:choose>
            <xsl:when test="@type='Custom'">
              <xsl:choose>
                <xsl:when test="@repeat = '0-N' or @repeat = '1-N'">
                  XmlNodeList xnl = XmlUtil.GetXmlNodes(node, "<xsl:call-template name='DefineParameterName'/>");
                  if(xnl.Count!=0)
                  {
                  msg.<xsl:call-template name='DefineParameterName'/> = new PARAM_<xsl:copy-of select="$vendor_name"/>_Custom[xnl.Count];
                  for(int i=0; i<xsl:text disable-output-escaping="yes">&lt;</xsl:text>xnl.Count; i++)
                  msg.<xsl:call-template name='DefineParameterName'/>[i] = PARAM_<xsl:copy-of select="$vendor_name"/>_Custom.FromXmlNode(xnl[i]);
                  }
                </xsl:when>
                <xsl:otherwise>
                  XmlNodeList xnl = XmlUtil.GetXmlNodes(node, "<xsl:call-template name='DefineParameterName'/>");
                  if(xnl.Count!=0)
                  msg.<xsl:call-template name='DefineParameterName'/> = PARAM_<xsl:copy-of select="$vendor_name"/>_Custom.FromXmlNode(xnl[0]);
                </xsl:otherwise>
              </xsl:choose>
            </xsl:when>
            <xsl:otherwise>
              <xsl:choose>
                <xsl:when test="@repeat = '0-N' or @repeat = '1-N'">
                  XmlNodeList xnl = XmlUtil.GetXmlNodes(node, "<xsl:call-template name='DefineParameterName'/>");
                  if(xnl.Count!=0)
                  {
                  msg.<xsl:call-template name='DefineParameterName'/> = new PARAM_<xsl:value-of select="@type"/>[xnl.Count];
                  for(int i=0; i<xsl:text disable-output-escaping="yes">&lt;</xsl:text>xnl.Count; i++)
                  msg.<xsl:call-template name='DefineParameterName'/>[i] = PARAM_<xsl:value-of select="@type"/>.FromXmlNode(xnl[i]);
                  }
                </xsl:when>
                <xsl:otherwise>
                  XmlNodeList xnl = XmlUtil.GetXmlNodes(node, "<xsl:call-template name='DefineParameterName'/>");
                  if(xnl.Count!=0)
                  msg.<xsl:call-template name='DefineParameterName'/> = PARAM_<xsl:value-of select="@type"/>.FromXmlNode(xnl[0]);
                </xsl:otherwise>
              </xsl:choose>
            </xsl:otherwise>
          </xsl:choose>
          }
        </xsl:when>
        <xsl:when test="name()='choice'">
          {
          <xsl:variable name="choiceParameterName">
            <xsl:call-template name='DefineParameterName'/>
          </xsl:variable>
          msg.<xsl:copy-of select='$choiceParameterName'/> = new UNION_<xsl:value-of select="@type"/>();
          <xsl:for-each select='../../llrp:customChoiceDefinition'>
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
                    ICustom_Parameter custom = CustomParamDecodeFactory.DecodeXmlNodeToCustomParameter(ccnode);
                    if(custom!=null)msg.<xsl:copy-of select='$choiceParameterName'/>.Add(custom);
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
              }catch{}
            </xsl:if>
          </xsl:for-each>
          }
        </xsl:when>
        <xsl:otherwise>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:for-each>
    return msg;
    }
  </xsl:template>

  <xsl:template name="VendorPARAMToString">
    /// <xsl:text disable-output-escaping="yes">&lt;</xsl:text>summary<xsl:text disable-output-escaping="yes">&gt;</xsl:text>
    /// Serialize a Xml string
    /// <xsl:text disable-output-escaping="yes">&lt;</xsl:text>/summary<xsl:text disable-output-escaping="yes">&gt;</xsl:text>
    /// <xsl:text disable-output-escaping="yes">&lt;</xsl:text>returns<xsl:text disable-output-escaping="yes">&gt;</xsl:text>Xml string<xsl:text disable-output-escaping="yes">&lt;</xsl:text>/returns<xsl:text disable-output-escaping="yes">&gt;</xsl:text>
    public override string ToString()
    {
    int len;
    string xml_str = "<xsl:text disable-output-escaping="yes">&lt;</xsl:text><xsl:copy-of select="$vendor_name"/>:<xsl:value-of select="@name"/>";
    xml_str +=" xmlns:<xsl:copy-of select="$vendor_name"/>=\"http://www.<xsl:copy-of select="$vendor_name"/>.com\"<xsl:text disable-output-escaping="yes">&gt;</xsl:text>";
    <xsl:for-each select="*">
      <xsl:choose>
        <xsl:when test="name()='field'">
          if(<xsl:value-of select="@name"/>!=null)
          {
          <xsl:choose>
            <xsl:when test="(@type='u1' or @type='u8' or @type='s8' or @type='u16' or @type='s16' or @type='u32' or @type='s32' or @type='u64') and not(@enumeration)">
              xml_str +="<xsl:text disable-output-escaping="yes">&lt;</xsl:text><xsl:value-of select="@name"/><xsl:text disable-output-escaping="yes">&gt;</xsl:text>" + Util.ConvertValueTypeToString(<xsl:value-of select="@name"/>, "<xsl:value-of select="@type"/>", "<xsl:value-of select="@format"/>") + "<xsl:text disable-output-escaping="yes">&lt;/</xsl:text><xsl:value-of select="@name"/><xsl:text disable-output-escaping="yes">&gt;</xsl:text>";
            </xsl:when>
            <xsl:when test="(@type='u8v' or @type='u16v' or @type='u32v' or @type='utf8v' or @type='u1v' or @type='u96' or @type='bytesToEnd') and not(@enumeration)">
              xml_str +="<xsl:text disable-output-escaping="yes">&lt;</xsl:text><xsl:value-of select="@name"/><xsl:text disable-output-escaping="yes">&gt;</xsl:text>" + Util.ConvertArrayTypeToString(<xsl:value-of select="@name"/>, "<xsl:value-of select="@type"/>", "<xsl:value-of select="@format"/>") + "<xsl:text disable-output-escaping="yes">&lt;/</xsl:text><xsl:value-of select="@name"/><xsl:text disable-output-escaping="yes">&gt;</xsl:text>";
            </xsl:when>
            <xsl:otherwise>
              xml_str +="<xsl:text disable-output-escaping="yes">&lt;</xsl:text><xsl:value-of select="@name"/><xsl:text disable-output-escaping="yes">&gt;</xsl:text>" + <xsl:value-of select="@name"/>.ToString() + "<xsl:text disable-output-escaping="yes">&lt;/</xsl:text><xsl:value-of select="@name"/><xsl:text disable-output-escaping="yes">&gt;</xsl:text>";
            </xsl:otherwise>
          </xsl:choose>
          }
        </xsl:when>
        <xsl:when test="name()='parameter'">
          if(<xsl:call-template name='DefineParameterName'/>!= null)
          {
          <xsl:choose>

            <xsl:when test="@repeat = '0-N' or @repeat = '1-N'">
              len = <xsl:call-template name='DefineParameterName'/>.Length;
              for(int i=0; i<xsl:text disable-output-escaping="yes">&lt;</xsl:text>len;i++)
              xml_str += <xsl:call-template name='DefineParameterName'/>[i].ToString();
            </xsl:when>
            <xsl:otherwise>
              xml_str += <xsl:call-template name='DefineParameterName'/>.ToString();
            </xsl:otherwise>
          </xsl:choose>
          }
        </xsl:when>
        <xsl:when test="name()='choice'">
          <xsl:variable name="choiceParameterName">
            <xsl:call-template name='DefineParameterName'/>
          </xsl:variable>
          if(<xsl:call-template name='DefineParameterName'/>!= null)
          {
          len = <xsl:copy-of select='$choiceParameterName'/>.Count;
          for(int i=0; i<xsl:text disable-output-escaping="yes">&lt;</xsl:text>len;i++)xml_str += <xsl:copy-of select='$choiceParameterName'/>[i].ToString();
          }
        </xsl:when>
        <xsl:otherwise>

        </xsl:otherwise>
      </xsl:choose>
    </xsl:for-each>
    xml_str += "<xsl:text disable-output-escaping="yes">&lt;/</xsl:text><xsl:copy-of select="$vendor_name"/>:<xsl:value-of select="@name"/><xsl:text disable-output-escaping="yes">&gt;</xsl:text>";

    return xml_str;
    }
  </xsl:template>
  <xsl:template name="VendorPARAMFromXmlNode">
    /// <xsl:text disable-output-escaping="yes">&lt;</xsl:text>summary<xsl:text disable-output-escaping="yes">&gt;</xsl:text>
    /// Deserialize a XML node to parameter
    /// <xsl:text disable-output-escaping="yes">&lt;</xsl:text>/summary<xsl:text disable-output-escaping="yes">&gt;</xsl:text>
    /// <xsl:text disable-output-escaping="yes">&lt;</xsl:text>param name="node"<xsl:text disable-output-escaping="yes">&gt;</xsl:text>Xml node to be deserialized<xsl:text disable-output-escaping="yes">&lt;</xsl:text>/param<xsl:text disable-output-escaping="yes">&gt;</xsl:text>
    /// <xsl:text disable-output-escaping="yes">&lt;</xsl:text>returns<xsl:text disable-output-escaping="yes">&gt;</xsl:text>PARAM_<xsl:value-of select="@name"/><xsl:text disable-output-escaping="yes">&lt;</xsl:text>/returns<xsl:text disable-output-escaping="yes">&gt;</xsl:text>
    public new static PARAM_<xsl:value-of select="@name"/>  FromXmlNode(XmlNode node)
    {
    string val;
    PARAM_<xsl:value-of select="@name"/> param = new PARAM_<xsl:value-of select="@name"/>();
    <xsl:for-each select="*">
      <xsl:if test="name()='field'">
        val = XmlUtil.GetNodeValue(node, "<xsl:value-of select="@name"/>");
        <xsl:choose>
          <xsl:when test="@enumeration">
            param.<xsl:value-of select="@name"/> = (<xsl:call-template name='DefineDataType'/>)Enum.Parse(typeof(<xsl:call-template name='DefineDataType'/>), val);
          </xsl:when>
          <xsl:when test="(@type='u1' or @type='u8' or @type='s8' or @type='u16' or @type='s16' or @type='u32' or @type='s32' or @type='u64') and not(@enumeration)">
            param.<xsl:value-of select="@name"/> = (<xsl:call-template name='DefineDataType'/>)(Util.ParseValueTypeFromString(val, "<xsl:value-of select="@type"/>", "<xsl:value-of select="@format"/>"));
          </xsl:when>
          <xsl:when test="(@type='u8v' or @type='u16v' or @type='u32v' or @type='utf8v' or @type='u1v' or @type='u96' or @type='bytesToEnd') and not(@enumeration)">
            param.<xsl:value-of select="@name"/> = (<xsl:call-template name='DefineDataType'/>)(Util.ParseArrayTypeFromString(val, "<xsl:value-of select="@type"/>", "<xsl:value-of select="@format"/>"));
          </xsl:when>
          <xsl:otherwise>
            param.<xsl:value-of select="@name"/> = <xsl:call-template name='DefineDataType'/>.FromString(val);
          </xsl:otherwise>
        </xsl:choose>
      </xsl:if>
      <xsl:if test="name()='parameter'">
        {
        <xsl:choose>
          <xsl:when test="@type='Custom'">
            <xsl:choose>
              <xsl:when test="@repeat = '0-N' or @repeat = '1-N'">
                XmlNodeList xnl = XmlUtil.GetXmlNodes(node, "<xsl:call-template name='DefineParameterName'/>");
                if(xnl.Count!=0)
                {
                param.<xsl:call-template name='DefineParameterName'/> = new PARAM_<xsl:copy-of select="$vendor_name"/>_Custom[xnl.Count];
                for(int i=0; i<xsl:text disable-output-escaping="yes">&lt;</xsl:text>xnl.Count; i++)
                param.<xsl:call-template name='DefineParameterName'/>[i] = PARAM_<xsl:copy-of select="$vendor_name"/>_Custom.FromXmlNode(xnl[i]);
                }
              </xsl:when>
              <xsl:otherwise>
                XmlNodeList xnl = XmlUtil.GetXmlNodes(node, "<xsl:call-template name='DefineParameterName'/>");
                if(xnl.Count!=0)
                param.<xsl:call-template name='DefineParameterName'/> = PARAM_<xsl:copy-of select="$vendor_name"/>_Custom.FromXmlNode(xnl[0]);
              </xsl:otherwise>
            </xsl:choose>
          </xsl:when>
          <xsl:otherwise>
            <xsl:choose>
              <xsl:when test="@repeat = '0-N' or @repeat = '1-N'">
                XmlNodeList xnl = XmlUtil.GetXmlNodes(node, "<xsl:call-template name='DefineParameterName'/>");
                if(xnl.Count!=0)
                {
                param.<xsl:call-template name='DefineParameterName'/> = new PARAM_<xsl:value-of select="@type"/>[xnl.Count];
                for(int i=0; i<xsl:text disable-output-escaping="yes">&lt;</xsl:text>xnl.Count; i++)
                param.<xsl:call-template name='DefineParameterName'/>[i] = PARAM_<xsl:value-of select="@type"/>.FromXmlNode(xnl[i]);
                }
              </xsl:when>
              <xsl:otherwise>
                XmlNodeList xnl = XmlUtil.GetXmlNodes(node, "<xsl:call-template name='DefineParameterName'/>");
                if(xnl.Count!=0)
                param.<xsl:call-template name='DefineParameterName'/> = PARAM_<xsl:value-of select="@type"/>.FromXmlNode(xnl[0]);
              </xsl:otherwise>
            </xsl:choose>
          </xsl:otherwise>
        </xsl:choose>

        }
      </xsl:if>
      <xsl:if test="name()='choice'">
        {
        <xsl:variable name="choiceParameterName">
          <xsl:call-template name='DefineParameterName'/>
        </xsl:variable>
        param.<xsl:copy-of select='$choiceParameterName'/> = new UNION_<xsl:value-of select="@type"/>();
        <xsl:for-each select='../../llrp:customChoiceDefinition'>
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
                  ICustom_Parameter custom = CustomParamDecodeFactory.DecodeXmlNodeToCustomParameter(ccnode);
                  if(custom!=null)param.<xsl:copy-of select='$choiceParameterName'/>.Add(custom);
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
            }catch{}
          </xsl:if>
        </xsl:for-each>
        }
      </xsl:if>
    </xsl:for-each>
    return param;
    }
  </xsl:template>
  
</xsl:stylesheet>