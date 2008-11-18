
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
 ** @file  ltkcpp_frame.h
 **
 ** @brief Classes to encode and decod LLRP binary frames
 **
 ** These classes implement the basic Encoder and Decoder classes
 ** to convert LTKCPP objects to and from LLRP binary syntax.
 **
 *****************************************************************************/



namespace LLRP
{
class CFrameExtract;
class CFrameDecoder;
class CFrameDecoderStream;
class CFrameEncoder;
class CFrameEncoderStream;

class CFrameExtract
{
  public:
    enum {
        UNKNOWN,
        READY,
        FRAME_ERROR,
        NEED_MORE
    }                           m_eStatus;

    llrp_u32_t                  m_MessageLength;
    llrp_u16_t                  m_MessageType;
    llrp_u8_t                   m_ProtocolVersion;
    llrp_u32_t                  m_MessageID;

    unsigned int                m_nBytesNeeded;

    CFrameExtract (void);

    CFrameExtract (
      const unsigned char *     pBuffer,
      unsigned int              nBuffer);
};

class CFrameDecoder : public CDecoder
{
    friend class CFrameDecoderStream;

  public:
    CFrameDecoder (
      const CTypeRegistry *     pTypeRegistry,
      unsigned char *           pBuffer,
      unsigned int              nBuffer);

    ~CFrameDecoder (void);

    CMessage *
    decodeMessage (void);

  private:
    unsigned char *             m_pBuffer;
    unsigned int                m_nBuffer;

    unsigned int                m_iNext;
    unsigned int                m_BitFieldBuffer;
    unsigned int                m_nBitFieldResid;

    llrp_u8_t
    next_u8(void);

    llrp_u16_t
    next_u16(void);

    llrp_u32_t
    next_u32(void);

    llrp_u64_t
    next_u64(void);
};

class CFrameDecoderStream : public CDecoderStream
{
    friend class CFrameDecoder;

  public:
    /*
     * 8-bit types
     */

    llrp_u8_t
    get_u8 (
      const CFieldDescriptor *  pFieldDescriptor);

    llrp_s8_t
    get_s8 (
      const CFieldDescriptor *  pFieldDescriptor);

    llrp_u8v_t
    get_u8v (
      const CFieldDescriptor *  pFieldDescriptor);

    llrp_s8v_t
    get_s8v (
      const CFieldDescriptor *  pFieldDescriptor);

    /*
     * 16-bit types
     */

    llrp_u16_t
    get_u16 (
      const CFieldDescriptor *  pFieldDescriptor);

    llrp_s16_t
    get_s16 (
      const CFieldDescriptor *  pFieldDescriptor);

    llrp_u16v_t
    get_u16v (
      const CFieldDescriptor *  pFieldDescriptor);

    llrp_s16v_t
    get_s16v (
      const CFieldDescriptor *  pFieldDescriptor);

    /*
     * 32-bit types
     */

    llrp_u32_t
    get_u32 (
      const CFieldDescriptor *  pFieldDescriptor);

    llrp_s32_t
    get_s32 (
      const CFieldDescriptor *  pFieldDescriptor);

    llrp_u32v_t
    get_u32v (
      const CFieldDescriptor *  pFieldDescriptor);

    llrp_s32v_t
    get_s32v (
      const CFieldDescriptor *  pFieldDescriptor);

    /*
     * 64-bit types
     */

    llrp_u64_t
    get_u64 (
      const CFieldDescriptor *  pFieldDescriptor);

    llrp_s64_t
    get_s64 (
      const CFieldDescriptor *  pFieldDescriptor);

    llrp_u64v_t
    get_u64v (
      const CFieldDescriptor *  pFieldDescriptor);

    llrp_s64v_t
    get_s64v (
      const CFieldDescriptor *  pFieldDescriptor);

    /*
     * Special types
     */

    llrp_u1_t
    get_u1 (
      const CFieldDescriptor *  pFieldDescriptor);

    llrp_u1v_t
    get_u1v (
      const CFieldDescriptor *  pFieldDescriptor);

    llrp_u2_t
    get_u2 (
      const CFieldDescriptor *  pFieldDescriptor);

    llrp_u96_t
    get_u96 (
      const CFieldDescriptor *  pFieldDescriptor);

    llrp_utf8v_t
    get_utf8v (
      const CFieldDescriptor *  pFieldDescriptor);

    llrp_bytesToEnd_t
    get_bytesToEnd (
      const CFieldDescriptor *  pFieldDescriptor);

    /*
     * Enumerated types of various sizes
     */

    int
    get_e1 (
      const CFieldDescriptor *  pFieldDescriptor);

    int
    get_e2 (
      const CFieldDescriptor *  pFieldDescriptor);

    int
    get_e8 (
      const CFieldDescriptor *  pFieldDescriptor);

    int
    get_e16 (
      const CFieldDescriptor *  pFieldDescriptor);

    int
    get_e32 (
      const CFieldDescriptor *  pFieldDescriptor);

    llrp_u8v_t
    get_e8v (
      const CFieldDescriptor *  pFieldDescriptor);

    /*
     * Reserved types are some number of bits
     */

    void
    get_reserved (
      unsigned int              nBit);

  private:
    CFrameDecoder *             m_pDecoder;
    CFrameDecoderStream *       m_pEnclosingDecoderStream;
    const CTypeDescriptor *     m_pRefType;
    unsigned int                m_iBegin;
    unsigned int                m_iLimit;

    CFrameDecoderStream (
      CFrameDecoder *           pDecoder);

    CFrameDecoderStream (
      CFrameDecoderStream *     pEnclosingDecoderStream);

    /* no destructor necessary */

    CMessage *
    getMessage (void);

    CParameter *
    getParameter (void);

    unsigned int
    getRemainingByteCount (void);

    llrp_bool_t
    checkAvailable (
      unsigned int              nByte,
      const CFieldDescriptor *  pFieldDescriptor);

    unsigned int
    getBitField (
      unsigned int              nBit,
      const CFieldDescriptor *  pFieldDescriptor);

    llrp_u16_t
    getVarlenCount (
      const CFieldDescriptor *  pFieldDescriptor);

    llrp_bool_t
    verifyVectorAllocation (
      const void *              pValue,
      const CFieldDescriptor *  pFieldDescriptor);
};

class CFrameEncoder : public CEncoder
{
  friend class CFrameEncoderStream;

  public:
    CFrameEncoder (
      unsigned char *           pBuffer,
      unsigned int              nBuffer);

    ~CFrameEncoder (void);

    void
    encodeElement (
      const CElement *          pElement);

    unsigned int
    getLength(void);

  private:
    unsigned char *             m_pBuffer;
    unsigned int                m_nBuffer;

    unsigned int                m_iNext;
    unsigned int                m_BitFieldBuffer;
    unsigned int                m_nBitFieldResid;

    void
    next_u8 (
      llrp_u8_t                 Value);

    void
    next_u16 (
      llrp_u16_t                Value);

    void
    next_u32 (
      llrp_u32_t                Value);

    void
    next_u64 (
      llrp_u64_t                Value);
};

class CFrameEncoderStream : public CEncoderStream
{
  friend class CFrameEncoder;

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
      const CFieldDescriptor    *pFieldDescriptor);

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
    CFrameEncoder *             m_pEncoder;
    CFrameEncoderStream *       m_pEnclosingEncoderStream;
    const CTypeDescriptor *     m_pRefType;
    unsigned int                m_iBegin;

    CFrameEncoderStream (
      CFrameEncoder *           pEncoder);

    CFrameEncoderStream (
      CFrameEncoderStream *     pEnclosingEncoderStream);

    /* no destructor needed */

    void
    putElement (
      const CElement *          pElement);

    llrp_bool_t
    checkAvailable (
      unsigned int              nByte,
      const CFieldDescriptor *  pFieldDescriptor);

    llrp_bool_t
    putBitField (
      unsigned int              nBit,
      unsigned int              Value,
      const CFieldDescriptor *  pFieldDescriptor);
};


};
