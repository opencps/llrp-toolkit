
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
 ** @brief Classes to encode LTK-XML 
 **
 ** These classes implement the basic Encoder and Decoder classes
 ** to convert LTKCPP objects to  LTK_XML syntax
 **
 *****************************************************************************/


namespace LLRP
{
class CXMLTextEncoder;
class CXMLTextEncoderStream;

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

};
