
/*
 ***************************************************************************
 *  Copyright 2007,2008 Impinj, Inc.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 ***************************************************************************
 */


/**
 *****************************************************************************
 **
 ** @file  ltkcpp_xmltext.h
 **
 ** @brief Classes to encode and decode LTK-XML 
 **
 ** These classes implement the basic Encoder and Decoder classes
 ** to convert LTKCPP objects to and from LTK_XML syntax
 **
 *****************************************************************************/

/* forward declare these so we don't need to include the
** whole XML header files here. Make sure they are out
** of the namespace and extern C since they need to 
** match the C types later */
extern "C"
{
    struct _xmlNode;
    struct _xmlDoc;
}

namespace LLRP
{
class CXMLTextEncoder;
class CXMLTextEncoderStream;
class CXMLTextDecoder;
class CXMLTextDecoderStream;

class CXMLTextEncoder : public CEncoder
{
    friend class CXMLTextEncoderStream;

  private:
    char *                      m_pBuffer;
    int                         m_nBuffer;
    int                         m_iNext;

  public:
    int                         m_bOverflow;

  public:
    CXMLTextEncoder (
      char *                    pBuffer,
      int                       nBuffer);

    ~CXMLTextEncoder (void);

    void
    encodeElement (
      const CElement *          pElement);
};

class CXMLTextDecoder : public CDecoder
{
    friend class CXMLTextDecoderStream;

  private:
    struct _xmlDoc *     	    m_pDoc;
    struct _xmlNode *           m_pxmlNodeTree;

  public:
    CXMLTextDecoder (
  const CTypeRegistry *         pTypeRegistry,
      char *                    pBuffer,
      int                       nBuffer);

    CXMLTextDecoder (
  const CTypeRegistry *         pTypeRegistry,
      struct _xmlNode *         pNodeTree);


    CXMLTextDecoder (
  const CTypeRegistry *         pTypeRegistry,
      char *                    fname);

    ~CXMLTextDecoder (void);

    CMessage *
    decodeMessage (void);

};

class CXMLTextEncoderStream : public CEncoderStream
{
    friend class CXMLTextEncoder;

  public:
    void
    putRequiredSubParameter (
      const CParameter *        pParameter,
      const CTypeDescriptor *   pRefType);

    void
    putOptionalSubParameter (
      const CParameter *        pParameter,
      const CTypeDescriptor *   pRefType);

    void
    putRequiredSubParameterList (
      const tListOfParameters * pParameterList,
      const CTypeDescriptor *   pRefType);

    void
    putOptionalSubParameterList (
      const tListOfParameters * pParameterList,
      const CTypeDescriptor *   pRefType);

    /*
     * 8-bit types
     */

    void
    put_u8 (
      llrp_u8_t                 Value,
      const CFieldDescriptor *  pFieldDescriptor);

    void
    put_s8 (
      llrp_s8_t                 Value,
      const CFieldDescriptor *  pFieldDescriptor);

    void
    put_u8v (
      llrp_u8v_t                Value,
      const CFieldDescriptor *  pFieldDescriptor);

    void
    put_s8v (
      llrp_s8v_t                Value,
      const CFieldDescriptor *  pFieldDescriptor);

    /*
     * 16-bit types
     */

    void
    put_u16 (
      llrp_u16_t                Value,
      const CFieldDescriptor *  pFieldDescriptor);

    void
    put_s16 (
      llrp_s16_t                Value,
      const CFieldDescriptor *  pFieldDescriptor);

    void
    put_u16v (
      llrp_u16v_t               Value,
      const CFieldDescriptor *  pFieldDescriptor);

    void
    put_s16v (
      llrp_s16v_t               Value,
      const CFieldDescriptor *  pFieldDescriptor);

    /*
     * 32-bit types
     */

    void
    put_u32 (
      llrp_u32_t                Value,
      const CFieldDescriptor *  pFieldDescriptor);

    void
    put_s32 (
      llrp_s32_t                Value,
      const CFieldDescriptor *  pFieldDescriptor);

    void
    put_u32v (
      llrp_u32v_t               Value,
      const CFieldDescriptor *  pFieldDescriptor);

    void
    put_s32v (
      llrp_s32v_t               Value,
      const CFieldDescriptor *  pFieldDescriptor);

    /*
     * 64-bit types
     */

    void
    put_u64 (
      llrp_u64_t                Value,
      const CFieldDescriptor *  pFieldDescriptor);

    void
    put_s64 (
      llrp_s64_t                Value,
      const CFieldDescriptor *  pFieldDescriptor);

    void
    put_u64v (
      llrp_u64v_t               Value,
      const CFieldDescriptor *  pFieldDescriptor);

    void
    put_s64v (
      llrp_s64v_t               Value,
      const CFieldDescriptor *  pFieldDescriptor);

    /*
     * Special types
     */

    void
    put_u1 (
      llrp_u1_t                 Value,
      const CFieldDescriptor *  pFieldDescriptor);

    void
    put_u1v (
      llrp_u1v_t                Value,
      const CFieldDescriptor *  pFieldDescriptor);

    void
    put_u2 (
      llrp_u2_t                 Value,
      const CFieldDescriptor *  pFieldDescriptor);

    void
    put_u96 (
      llrp_u96_t                Value,
      const CFieldDescriptor *  pFieldDescriptor);

    void
    put_utf8v (
      llrp_utf8v_t              Value,
      const CFieldDescriptor *  pFieldDescriptor);

    void
    put_bytesToEnd (
      llrp_bytesToEnd_t         Value,
      const CFieldDescriptor *  pFieldDescriptor);

    /*
     * Enumerated types of various sizes
     */

    void
    put_e1 (
      int                       eValue,
      const CFieldDescriptor *  pFieldDescriptor);

    void
    put_e2 (
      int                       eValue,
      const CFieldDescriptor *  pFieldDescriptor);

    void
    put_e8 (
      int                       eValue,
      const CFieldDescriptor *  pFieldDescriptor);

    void
    put_e16 (
      int                       eValue,
      const CFieldDescriptor *  pFieldDescriptor);

    void
    put_e32 (
      int                       eValue,
      const CFieldDescriptor *  pFieldDescriptor);

    void
    put_e8v (
      llrp_u8v_t                Value,
      const CFieldDescriptor *  pFieldDescriptor);

    /*
     * Reserved types are some number of bits
     */

    void
    put_reserved (
      unsigned int              nBits);

  private:
    CXMLTextEncoderStream (
      CXMLTextEncoder *         pEncoder);

    CXMLTextEncoderStream (
      CXMLTextEncoderStream *   pEnclosingEncoderStream);

    CXMLTextEncoder *           m_pEncoder;
    CXMLTextEncoderStream *     m_pEnclosingEncoderStream;
    const CTypeDescriptor *     m_pRefType;
    unsigned int                m_nDepth;

    void
    putElement (
      const CElement *          pElement);

    void
    put_enum (
      int                       eValue,
      const CFieldDescriptor *  pFieldDescriptor);

    void
    indent(int delta = 0);

    void
    appendOpenTag (
      const char *              pName);

    void
    appendCloseTag (
      const char *              pName);

    void
    appendPrefixedTagName (
      const char *              pName);

    void
    appendFormat (
      char *                    pFmtStr,
                                ...);

};

class CXMLTextDecoderStream : public CDecoderStream
{
  friend class CXMLTextDecoder;

  private:
    CXMLTextDecoderStream (
      CXMLTextDecoder *         pDecoder);

    CXMLTextDecoderStream (
      CXMLTextDecoderStream *   pEnclosingDecoderStream);

  public:    

    CElement *
    decodeElement (
      int                           bAllowMessage,
      int                           bSkipBlanks);      
      /*
     * 8-bit types
     */

    /** @brief Decodes an llrp_u8_t (unsigned 8-bit number) into the specified field descriptor */
    llrp_u8_t
    get_u8 (
      const CFieldDescriptor *  pFieldDesc);

    /** @brief Decodes a llrp_s8_t (signed 8-bit number) into the specified field descriptor */
    llrp_s8_t
    get_s8 (
      const CFieldDescriptor *  pFieldDesc);

    /** @brief Decodes an llrp_u8v_t into the specified field descriptor */
    llrp_u8v_t
    get_u8v (
      const CFieldDescriptor *  pFieldDesc);

    /** @brief Decodes an llrp_s8v_t into the specified field descriptor */
    llrp_s8v_t
    get_s8v (
      const CFieldDescriptor *  pFieldDesc);

    /*
     * 16-bit types
     */

    /** @brief Decodes an llrp_u16_t (unsigned 16-bit number) into the specified field descriptor */
    llrp_u16_t
    get_u16 (
      const CFieldDescriptor *  pFieldDesc);

    /** @brief Decodes a llrp_s16_t (signed 16-bit number) into the specified field descriptor */
    llrp_s16_t
    get_s16 (
      const CFieldDescriptor *  pFieldDesc);

    /** @brief Decodes a llrp_u16v_t into the specified field descriptor */
    llrp_u16v_t
    get_u16v (
      const CFieldDescriptor *  pFieldDesc);

    /** @brief Decodes an llrp_u16v_t into the specified field descriptor */
    llrp_s16v_t
    get_s16v (
      const CFieldDescriptor *  pFieldDesc);

    /*
     * 32-bit types
     */

    /** @brief Decodes an llrp_u32_t (unsigned 32-bit number) into the specified field descriptor */
    llrp_u32_t
    get_u32 (
      const CFieldDescriptor *  pFieldDesc);

    /** @brief Decodes an llrp_s32_t (signed 32-bit number) into the specified field descriptor */
    llrp_s32_t
    get_s32 (
      const CFieldDescriptor *  pFieldDesc);

    /** @brief Decodes an llrp_u32v_t into the specified field descriptor */
    llrp_u32v_t
    get_u32v (
      const CFieldDescriptor *  pFieldDesc);

    /** @brief Decodes an llrp_s32v_t into the specified field descriptor */
    llrp_s32v_t
    get_s32v (
      const CFieldDescriptor *  pFieldDesc);

    /*
     * 64-bit types
     */

    /** @brief Decodes an llrp_u64_t (unsigned 64-bit number) into the specified field descriptor */
    llrp_u64_t
    get_u64 (
      const CFieldDescriptor *  pFieldDesc);

    /** @brief Decodes an llrp_s64_t (signed 64-bit number) into the specified field descriptor */
    llrp_s64_t
    get_s64 (
      const CFieldDescriptor *  pFieldDesc);

    /** @brief Decodes an llrp_u64v_t into the specified field descriptor */
    llrp_u64v_t
    get_u64v (
      const CFieldDescriptor *  pFieldDesc);

    /** @brief Decodes an llrp_s64v_t into the specified field descriptor */
    llrp_s64v_t
    get_s64v (
      const CFieldDescriptor *  pFieldDesc);

    /*
     * Special types
     */

    /** @brief Decodes an llrp_u1_t (unsigned 1 bit number) into the specified field descriptor */
    llrp_u1_t
    get_u1 (
      const CFieldDescriptor *  pFieldDesc);

    /** @brief Decodes an llrp_u1v_t into the specified field descriptor */
    llrp_u1v_t
    get_u1v (
      const CFieldDescriptor *  pFieldDesc);

    /** @brief Decodes an llrp_u2_t into the specified field descriptor */
    llrp_u2_t
    get_u2 (
      const CFieldDescriptor *  pFieldDesc);

    /** @brief Decodes an llrp_u96_t into the specified field descriptor */
    llrp_u96_t
    get_u96 (
      const CFieldDescriptor *  pFieldDesc);

    /** @brief Decodes an llrp_utf8v_t into the specified field descriptor */
    llrp_utf8v_t
    get_utf8v (
      const CFieldDescriptor *  pFieldDesc);

    /** @brief Decodes an llrp_bytesToEnd_t into the specified field descriptor */
    llrp_bytesToEnd_t
    get_bytesToEnd (
      const CFieldDescriptor *  pFieldDesc);

    /*
     * Enumerated types of various sizes
     */

    /** @brief Decodes a 1 bit enumerated field into the specified field descriptor */
    int
    get_e1 (
      const CFieldDescriptor *  pFieldDesc);

    /** @brief Decodes a 2 bit enumerated field into the specified field descriptor */
    int
    get_e2 (
      const CFieldDescriptor *  pFieldDesc);

    /** @brief Decodes a 8 bit enumerated field into the specified field descriptor */
    int
    get_e8 (
      const CFieldDescriptor *  pFieldDesc);

    /** @brief Decodes a 16 bit enumerated field into the specified field descriptor */
    int
    get_e16 (
      const CFieldDescriptor *  pFieldDesc);

    /** @brief Decodes a 32 bit enumerated field into the specified field descriptor */
    int
    get_e32 (
      const CFieldDescriptor *  pFieldDesc);

    /** @brief Decodes an enumerated u8v field into the specified field descriptor */
    llrp_u8v_t
    get_e8v (
      const CFieldDescriptor *  pFieldDesc);

    /*
     * Reserved means some number of bits
     */
    /* @brief skips a number of reserved bits and discards during the encode/decode process */
    void
    get_reserved (
      unsigned int          nBits);

  private:
    CXMLTextDecoder *           m_pDecoder;
    CXMLTextDecoderStream *     m_pEnclosingDecoderStream;
    const CTypeDescriptor *     m_pRefType;
    struct _xmlNode *           m_pTargetNode;
    struct _xmlNode *           m_pCurrentChildNode;
    struct _xmlNode *           m_pLastFieldNode;

    llrp_bool_t
    verifyVectorAllocation (
      const void *              pValue,
      const CFieldDescriptor *  pFieldDescriptor);

    static int
    isInsignificantNode (
      struct _xmlNode *         pnode);

/* some class member field helpers */
    int 
    getFieldStringPtr(
      const CFieldDescriptor *  pFieldDescriptor,
      const llrp_u8_t **        pbuf,
      const llrp_u8_t **        pend);

    llrp_s64v_t
    getSpacedVectorField (
      const CFieldDescriptor *  pFieldDescriptor,
      llrp_s64_t                minValue,
      llrp_s64_t                maxValue);

    llrp_s64v_t
    getFixedVectorField (
      const CFieldDescriptor *  pFieldDescriptor,
      unsigned int              vectorSize,
      llrp_s64_t                minValue,
      llrp_s64_t                maxValue);

    llrp_s64_t
    getIntegerField (
      const CFieldDescriptor *  pFieldDescriptor,
      llrp_s64_t                minValue,
      llrp_s64_t                maxValue);

    llrp_s64_t getInteger(
      const CFieldDescriptor *  pFieldDescriptor,
      const llrp_u8_t *         pbuf,
      const llrp_u8_t *         pend,
      CErrorDetails            *pError,
      llrp_s64_t                minValue,
      llrp_s64_t                maxValue);

/* static string helpers */

    static const llrp_u8_t *
    getSingleU1(
      const llrp_u8_t *         pbuf,
      const llrp_u8_t *         pend,
      llrp_s64_t *              pValue);

    static const llrp_u8_t *
    getSingleEnum(
      const CFieldDescriptor *  pFieldDescriptor,
      const llrp_u8_t *         pbuf,
      const llrp_u8_t *         pend,
      llrp_s64_t *              pValue);

    static const llrp_u8_t *
    getSingleDecimal(
      const llrp_u8_t *         pbuf,
      const llrp_u8_t *         pend,
      llrp_s64_t *              pValue);

    static const llrp_u8_t *
    getSingleHexidecimal(
      const llrp_u8_t *         pbuf,
      const llrp_u8_t *         pend,
      llrp_s64_t *              pValue);

    static int
    cleanString(
      const llrp_u8_t **        ppbuf,
      const llrp_u8_t **        ppend);

    static const llrp_u8_t *
    getSingleTimestamp(
      const llrp_u8_t *         pbuf,
      const llrp_u8_t *         pend,
      llrp_s64_t *              pValue);

    static llrp_u16_t 
    countElements(
      const char *              pval, 
      int                      nval);

    private:
      static const llrp_s64_t MAX_U8 =  ((1ull << 8u) - 1u);
      static const llrp_s64_t MIN_U8 =  0ull;
      static const llrp_s64_t MAX_S8 =  ((1ull << 7u) - 1u);
      static const llrp_s64_t MIN_S8 =  (-1ll - MAX_S8);

      static const llrp_s64_t MAX_U16 = ((1ull << 16u) - 1u);
      static const llrp_s64_t MIN_U16 = 0ull;
      static const llrp_s64_t MAX_S16 = ((1ull << 15u) - 1u);
      static const llrp_s64_t MIN_S16 = (-1ll - MAX_S16);

      static const llrp_s64_t MAX_U32 = ((1ull << 32u) - 1u);
      static const llrp_s64_t MIN_U32 = 0ull;
      static const llrp_s64_t MAX_S32 = ((1ull << 31u) - 1u);
      static const llrp_s64_t MIN_S32 = (-1ll - MAX_S32);

      static const llrp_s64_t MAX_U64 = ((1ull << 63u) - 1u);    // one bit lost
      static const llrp_s64_t MIN_U64 = 0ull;
      static const llrp_s64_t MAX_S64 = ((1ull << 63u) - 1u);
      static const llrp_s64_t MIN_S64 = (-1ll - MAX_S64);

};

};
