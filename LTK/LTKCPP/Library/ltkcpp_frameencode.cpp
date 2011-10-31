
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



#include <assert.h>

#include "ltkcpp_platform.h"
#include "ltkcpp_base.h"
#include "ltkcpp_frame.h"


namespace LLRP
{

CFrameEncoder::CFrameEncoder (
  unsigned char *               pBuffer,
  unsigned int                  nBuffer)
 : CEncoder()
{
    m_pBuffer        = pBuffer;
    m_nBuffer        = nBuffer;

    m_iNext          = 0;
    m_BitFieldBuffer = 0;
    m_nBitFieldResid = 0;
}

CFrameEncoder::~CFrameEncoder (void)
{
}

void
CFrameEncoder::encodeElement (
  const CElement *              pElement)
{
    CFrameEncoderStream         MyEncoderStream(this);

    MyEncoderStream.putElement(pElement);
}

unsigned int
CFrameEncoder::getLength(void)
{
    return m_iNext;
}

void
CFrameEncoderStream::putRequiredSubParameter (
  const CParameter *            pParameter,
  const CTypeDescriptor *       pRefType)
{
    if(NULL == pParameter)
    {
        CErrorDetails *         pError = &m_pEncoder->m_ErrorDetails;

        pError->missingParameter(pRefType);

        return;
    }

    CFrameEncoderStream         NestEncoderStream(this);

    NestEncoderStream.putElement(pParameter);
}

void
CFrameEncoderStream::putOptionalSubParameter (
  const CParameter *            pParameter,
  const CTypeDescriptor *       pRefType)
{
    if(NULL == pParameter)
    {
        return;
    }

    CFrameEncoderStream         NestEncoderStream(this);

    NestEncoderStream.putElement(pParameter);
}

void
CFrameEncoderStream::putRequiredSubParameterList (
  const tListOfParameters *     pParameterList,
  const CTypeDescriptor *       pRefType)
{
    if(pParameterList->empty())
    {
        CErrorDetails *         pError = &m_pEncoder->m_ErrorDetails;

        pError->missingParameter(pRefType);

        return;
    }

    for(
        tListOfParameters::const_iterator Cur = pParameterList->begin();
        Cur != pParameterList->end();
        Cur++)
    {
        putRequiredSubParameter(*Cur, pRefType);
    }
}

void
CFrameEncoderStream::putOptionalSubParameterList (
  const tListOfParameters *     pParameterList,
  const CTypeDescriptor *       pRefType)
{
    for(
        tListOfParameters::const_iterator Cur = pParameterList->begin();
        Cur != pParameterList->end();
        Cur++)
    {
        putRequiredSubParameter(*Cur, pRefType);
    }
}


void
CFrameEncoder::next_u8 (
  llrp_u8_t                     Value)
{
    assert(m_iNext + 1u <= m_nBuffer);

    m_pBuffer[m_iNext++] = Value;
}

void
CFrameEncoder::next_u16 (
  llrp_u16_t                    Value)
{
    assert(m_iNext + 2u <= m_nBuffer);

    m_pBuffer[m_iNext++] = Value >> 8u;
    m_pBuffer[m_iNext++] = Value >> 0u;
}

void
CFrameEncoder::next_u32 (
  llrp_u32_t                    Value)
{
    assert(m_iNext + 4u <= m_nBuffer);

    m_pBuffer[m_iNext++] = Value >> 24u;
    m_pBuffer[m_iNext++] = Value >> 16u;
    m_pBuffer[m_iNext++] = Value >> 8u;
    m_pBuffer[m_iNext++] = Value >> 0u;
}

void
CFrameEncoder::next_u64 (
  llrp_u64_t                    Value)
{
    assert(m_iNext + 8u <= m_nBuffer);

    m_pBuffer[m_iNext++] = (llrp_byte_t)(Value >> 56u);
    m_pBuffer[m_iNext++] = (llrp_byte_t)(Value >> 48u);
    m_pBuffer[m_iNext++] = (llrp_byte_t)(Value >> 40u);
    m_pBuffer[m_iNext++] = (llrp_byte_t)(Value >> 32u);
    m_pBuffer[m_iNext++] = (llrp_byte_t)(Value >> 24u);
    m_pBuffer[m_iNext++] = (llrp_byte_t)(Value >> 16u);
    m_pBuffer[m_iNext++] = (llrp_byte_t)(Value >> 8u);
    m_pBuffer[m_iNext++] = (llrp_byte_t)(Value >> 0u);
}

/*
 * 8-bit types
 */

void
CFrameEncoderStream::put_u8 (
  llrp_u8_t                     Value,
  const CFieldDescriptor *      pFieldDescriptor)
{
    if(checkAvailable(1u, pFieldDescriptor))
    {
        m_pEncoder->next_u8(Value);
    }
}

void
CFrameEncoderStream::put_s8 (
  llrp_s8_t                     Value,
  const CFieldDescriptor *      pFieldDescriptor)
{
    if(checkAvailable(1u, pFieldDescriptor))
    {
        m_pEncoder->next_u8(Value);
    }
}

void
CFrameEncoderStream::put_u8v (
  llrp_u8v_t                    Value,
  const CFieldDescriptor *      pFieldDescriptor)
{
    unsigned int                nByte;

    nByte = 2u + Value.m_nValue * 1u;

    if(checkAvailable(nByte, pFieldDescriptor))
    {
        unsigned int            i;

        m_pEncoder->next_u16(Value.m_nValue);
        for(i = 0; i < Value.m_nValue; i++)
        {
            m_pEncoder->next_u8(Value.m_pValue[i]);
        }
    }
}

void
CFrameEncoderStream::put_s8v (
  llrp_s8v_t                    Value,
  const CFieldDescriptor *      pFieldDescriptor)
{
    unsigned int                nByte;

    nByte = 2u + Value.m_nValue * 1u;

    if(checkAvailable(nByte, pFieldDescriptor))
    {
        unsigned int            i;

        m_pEncoder->next_u16(Value.m_nValue);
        for(i = 0; i < Value.m_nValue; i++)
        {
            m_pEncoder->next_u8(Value.m_pValue[i]);
        }
    }
}

/*
 * 16-bit types
 */

void
CFrameEncoderStream::put_u16 (
  llrp_u16_t                    Value,
  const CFieldDescriptor *      pFieldDescriptor)
{
    if(checkAvailable(2u, pFieldDescriptor))
    {
        m_pEncoder->next_u16(Value);
    }
}

void
CFrameEncoderStream::put_s16 (
  llrp_s16_t                    Value,
  const CFieldDescriptor *      pFieldDescriptor)
{
    if(checkAvailable(2u, pFieldDescriptor))
    {
        m_pEncoder->next_u16(Value);
    }
}

void
CFrameEncoderStream::put_u16v (
  llrp_u16v_t                   Value,
  const CFieldDescriptor *      pFieldDescriptor)
{
    unsigned int                nByte;

    nByte = 2u + Value.m_nValue * 2u;

    if(checkAvailable(nByte, pFieldDescriptor))
    {
        unsigned int            i;

        m_pEncoder->next_u16(Value.m_nValue);
        for(i = 0; i < Value.m_nValue; i++)
        {
            m_pEncoder->next_u16(Value.m_pValue[i]);
        }
    }
}

void
CFrameEncoderStream::put_s16v (
  llrp_s16v_t                   Value,
  const CFieldDescriptor *      pFieldDescriptor)
{
    unsigned int                nByte;

    nByte = 2u + Value.m_nValue * 2u;

    if(checkAvailable(nByte, pFieldDescriptor))
    {
        unsigned int            i;

        m_pEncoder->next_u16(Value.m_nValue);
        for(i = 0; i < Value.m_nValue; i++)
        {
            m_pEncoder->next_u16(Value.m_pValue[i]);
        }
    }
}

/*
 * 32-bit types
 */

void
CFrameEncoderStream::put_u32 (
  llrp_u32_t                    Value,
  const CFieldDescriptor *      pFieldDescriptor)
{
    if(checkAvailable(4u, pFieldDescriptor))
    {
        m_pEncoder->next_u32(Value);
    }
}

void
CFrameEncoderStream::put_s32 (
  llrp_s32_t                    Value,
  const CFieldDescriptor *      pFieldDescriptor)
{
    if(checkAvailable(4u, pFieldDescriptor))
    {
        m_pEncoder->next_u32(Value);
    }
}

void
CFrameEncoderStream::put_u32v (
  llrp_u32v_t                   Value,
  const CFieldDescriptor *      pFieldDescriptor)
{
    unsigned int                nByte;

    nByte = 2u + Value.m_nValue * 4u;

    if(checkAvailable(nByte, pFieldDescriptor))
    {
        unsigned int            i;

        m_pEncoder->next_u16(Value.m_nValue);
        for(i = 0; i < Value.m_nValue; i++)
        {
            m_pEncoder->next_u32(Value.m_pValue[i]);
        }
    }
}

void
CFrameEncoderStream::put_s32v (
  llrp_s32v_t                   Value,
  const CFieldDescriptor *      pFieldDescriptor)
{
    unsigned int                nByte;

    nByte = 2u + Value.m_nValue * 4u;

    if(checkAvailable(nByte, pFieldDescriptor))
    {
        unsigned int            i;

        m_pEncoder->next_u16(Value.m_nValue);
        for(i = 0; i < Value.m_nValue; i++)
        {
            m_pEncoder->next_u32(Value.m_pValue[i]);
        }
    }
}

/*
 * 64-bit types
 */

void
CFrameEncoderStream::put_u64 (
  llrp_u64_t                    Value,
  const CFieldDescriptor *      pFieldDescriptor)
{
    if(checkAvailable(8u, pFieldDescriptor))
    {
        m_pEncoder->next_u64(Value);
    }
}

void
CFrameEncoderStream::put_s64 (
  llrp_s64_t                    Value,
  const CFieldDescriptor *      pFieldDescriptor)
{
    if(checkAvailable(8u, pFieldDescriptor))
    {
        m_pEncoder->next_u64(Value);
    }
}

void
CFrameEncoderStream::put_u64v (
  llrp_u64v_t                   Value,
  const CFieldDescriptor *      pFieldDescriptor)
{
    unsigned int                nByte;

    nByte = 2u + Value.m_nValue * 8u;

    if(checkAvailable(nByte, pFieldDescriptor))
    {
        unsigned int            i;

        m_pEncoder->next_u16(Value.m_nValue);
        for(i = 0; i < Value.m_nValue; i++)
        {
            m_pEncoder->next_u64(Value.m_pValue[i]);
        }
    }
}

void
CFrameEncoderStream::put_s64v (
  llrp_s64v_t                   Value,
  const CFieldDescriptor *      pFieldDescriptor)
{
    unsigned int                nByte;

    nByte = 2u + Value.m_nValue * 8u;

    if(checkAvailable(nByte, pFieldDescriptor))
    {
        unsigned int            i;

        m_pEncoder->next_u16(Value.m_nValue);
        for(i = 0; i < Value.m_nValue; i++)
        {
            m_pEncoder->next_u64(Value.m_pValue[i]);
        }
    }
}

/*
 * Special types
 */

void
CFrameEncoderStream::put_u1 (
  llrp_u1_t                     Value,
  const CFieldDescriptor *      pFieldDescriptor)
{
    putBitField(1u, Value, pFieldDescriptor);
}

void
CFrameEncoderStream::put_u1v (
  llrp_u1v_t                    Value,
  const CFieldDescriptor *      pFieldDescriptor)
{
    unsigned int                nByte;

    nByte = (Value.m_nBit + 7u) / 8u;

    if(checkAvailable(2u + nByte, pFieldDescriptor))
    {
        unsigned int            i;

        m_pEncoder->next_u16(Value.m_nBit);
        for(i = 0; i < nByte; i++)
        {
            m_pEncoder->next_u8(Value.m_pValue[i]);
        }
    }
}

void
CFrameEncoderStream::put_u2 (
  llrp_u2_t                     Value,
  const CFieldDescriptor *      pFieldDescriptor)
{
    putBitField(2u, Value, pFieldDescriptor);
}

void
CFrameEncoderStream::put_u96 (
  llrp_u96_t                    Value,
  const CFieldDescriptor *      pFieldDescriptor)
{
    if(checkAvailable(12u, pFieldDescriptor))
    {
        unsigned int            i;

        for(i = 0; i < 12u; i++)
        {
            m_pEncoder->next_u8(Value.m_aValue[i]);
        }
    }
}

void
CFrameEncoderStream::put_utf8v (
  llrp_utf8v_t                  Value,
  const CFieldDescriptor *      pFieldDescriptor)
{
    unsigned int                nByte;

    nByte = 2u + Value.m_nValue * 1u;

    if(checkAvailable(nByte, pFieldDescriptor))
    {
        unsigned int            i;

        m_pEncoder->next_u16(Value.m_nValue);
        for(i = 0; i < Value.m_nValue; i++)
        {
            m_pEncoder->next_u8(Value.m_pValue[i]);
        }
    }
}

void
CFrameEncoderStream::put_bytesToEnd (
  llrp_bytesToEnd_t             Value,
  const CFieldDescriptor *      pFieldDescriptor)
{
    unsigned int                nByte;

    nByte = Value.m_nValue * 1u;

    if(checkAvailable(nByte, pFieldDescriptor))
    {
        unsigned int            i;

        for(i = 0; i < Value.m_nValue; i++)
        {
            m_pEncoder->next_u8(Value.m_pValue[i]);
        }
    }
}

/*
 * Enumerated types of various sizes
 */

void
CFrameEncoderStream::put_e1 (
  int                           eValue,
  const CFieldDescriptor *      pFieldDescriptor)
{
    put_u1((llrp_u1_t)eValue, pFieldDescriptor);
}

void
CFrameEncoderStream::put_e2 (
  int                           eValue,
  const CFieldDescriptor *      pFieldDescriptor)
{
    put_u2((llrp_u2_t)eValue, pFieldDescriptor);
}

void
CFrameEncoderStream::put_e8 (
  int                           eValue,
  const CFieldDescriptor *      pFieldDescriptor)
{
    put_u8((llrp_u8_t)eValue, pFieldDescriptor);
}

void
CFrameEncoderStream::put_e16 (
  int                           eValue,
  const CFieldDescriptor *      pFieldDescriptor)
{
    put_u16((llrp_u16_t)eValue, pFieldDescriptor);
}

void
CFrameEncoderStream::put_e32 (
  int                           eValue,
  const CFieldDescriptor *      pFieldDescriptor)
{
    put_u32((llrp_u32_t)eValue, pFieldDescriptor);
}

void
CFrameEncoderStream::put_e8v (
  llrp_u8v_t                    Value,
  const CFieldDescriptor *      pFieldDescriptor)
{
    put_u8v(Value, pFieldDescriptor);
}

/*
 * Reserved types are some number of bits
 */

void
CFrameEncoderStream::put_reserved (
  unsigned int                  nBits)
{
    CErrorDetails *             pError = &m_pEncoder->m_ErrorDetails;

    if(RC_OK != pError->m_eResultCode)
    {
        return;
    }

    while(nBits > 0)
    {
        unsigned int            Step = 7u & nBits;

        if(0 != m_pEncoder->m_nBitFieldResid)
        {
            if(Step != m_pEncoder->m_nBitFieldResid)
            {
                pError->m_eResultCode = RC_UnalignedReservedBits;
                pError->m_pWhatStr    = "unaligned reserved bits";
                pError->m_pRefType    = m_pRefType;
                pError->m_pRefField   = NULL;
                pError->m_OtherDetail = m_pEncoder->m_iNext;
                return;
            }

            m_pEncoder->next_u8(m_pEncoder->m_BitFieldBuffer);
            nBits -= Step;
            m_pEncoder->m_BitFieldBuffer = 0;
            m_pEncoder->m_nBitFieldResid = 0;
        }
        else
        {
            if(0 != Step)
            {
                pError->m_eResultCode = RC_UnalignedReservedBits;
                pError->m_pWhatStr    = "unaligned reserved bits";
                pError->m_pRefType    = m_pRefType;
                pError->m_pRefField   = NULL;
                pError->m_OtherDetail = m_pEncoder->m_iNext;
                return;
            }

            if(m_pEncoder->m_iNext >= m_pEncoder->m_nBuffer)
            {
                pError->m_eResultCode = RC_ReservedBitsOverrun;
                pError->m_pWhatStr    = "overrun at reserved bits";
                pError->m_pRefType    = m_pRefType;
                pError->m_pRefField   = NULL;
                pError->m_OtherDetail = m_pEncoder->m_iNext;
                return;
            }

            m_pEncoder->next_u8(0);
            nBits -= 8;
        }
    }
}

CFrameEncoderStream::CFrameEncoderStream (
  CFrameEncoder *               pEncoder)
{
    m_pEncoder                  = pEncoder;
    m_pEnclosingEncoderStream   = NULL;
    m_iBegin                    = m_pEncoder->m_iNext;
    m_pRefType                  = NULL;
}

CFrameEncoderStream::CFrameEncoderStream (
  CFrameEncoderStream *         pEnclosingEncoderStream)
{
    m_pEncoder                  = pEnclosingEncoderStream->m_pEncoder;
    m_pEnclosingEncoderStream   = pEnclosingEncoderStream;
    m_iBegin                    = m_pEncoder->m_iNext;
    m_pRefType                  = NULL;
}

void
CFrameEncoderStream::putElement (
  const CElement *              pElement)
{
    CErrorDetails *             pError = &m_pEncoder->m_ErrorDetails;
    enum { MSG, TLV, TV, CUST_MSG, CUST_TLV } eFormat;

    if(RC_OK != pError->m_eResultCode)
    {
        return;
    }

    m_pRefType = pElement->m_pType;

    if(m_pRefType->m_bIsMessage)
    {
        eFormat = (NULL == m_pRefType->m_pVendorDescriptor) ? MSG : CUST_MSG;
    }
    else if(NULL == m_pRefType->m_pVendorDescriptor &&
            128 > m_pRefType->m_TypeNum)
    {
        /* TV parameter, never custom, no length */
        eFormat = TV;
    }
    else
    {
        /* TLV parameter */
        eFormat = (NULL == m_pRefType->m_pVendorDescriptor) ? TLV : CUST_TLV;
    }

    /*
     * Format the element header. The length part, if one,
     * is a place holder and back-patched later.
     */
    switch(eFormat)
    {
    default:
        assert(0);
        break;

    case MSG:
        {
            llrp_u16_t          VersType;

            VersType = (1u << 10u) | m_pRefType->m_TypeNum;
            put_u16(VersType, &g_fdMessageHeader_Type);
            put_u32(0, &g_fdMessageHeader_Length);
            put_u32(((const CMessage *)pElement)->getMessageID(),
                &g_fdMessageHeader_MessageID);
        }
        break;

    case CUST_MSG:
        {
            llrp_u16_t          VersType;

            /* Custom message */
            VersType = (1u << 10u) | 1023u;
            put_u16(VersType,
                &g_fdMessageHeader_Type);
            /* length is a placeholder */
            put_u32(0, &g_fdMessageHeader_Length);
            put_u32(((const CMessage *)pElement)->getMessageID(),
                &g_fdMessageHeader_MessageID);
            put_u32(
                m_pRefType->m_pVendorDescriptor->m_VendorID,
                &g_fdMessageHeader_VendorPEN);
            put_u8(m_pRefType->m_TypeNum, &g_fdMessageHeader_Subtype);
        }
        break;

    case TV:
        put_u8(m_pRefType->m_TypeNum | 0x80u, &g_fdParameterHeader_TVType);
        break;

    case TLV:
        /* Standard parameter */
        put_u16(m_pRefType->m_TypeNum, &g_fdParameterHeader_TLVType);
        put_u16(0, &g_fdParameterHeader_TLVLength);
        break;

    case CUST_TLV:
        /* Custom parameter */
        put_u16(1023u, &g_fdParameterHeader_TLVType);
        put_u16(0, &g_fdParameterHeader_TLVLength);
        put_u32(
            m_pRefType->m_pVendorDescriptor->m_VendorID,
            &g_fdParameterHeader_VendorPEN);
        put_u32(m_pRefType->m_TypeNum, &g_fdParameterHeader_Subtype);
        break;
    }

    if(RC_OK != pError->m_eResultCode)
    {
        return;
    }

    pElement->encode(this);

    unsigned int        nLength;
    unsigned char *     pLen;

    nLength = m_pEncoder->m_iNext - m_iBegin;
    pLen = &m_pEncoder->m_pBuffer[m_iBegin];

    switch(eFormat)
    {
    default:
        assert(0);
        break;

    case MSG:
    case CUST_MSG:
        assert(nLength >= 10);
        pLen += 2;
        pLen[0] = nLength >> 24u;
        pLen[1] = nLength >> 16u;
        pLen[2] = nLength >> 8u;
        pLen[3] = nLength >> 0u;
        break;

    case TV:
        break;

    case TLV:
    case CUST_TLV:
        assert(nLength >= 4);
        pLen += 2;
        pLen[0] = nLength >> 8u;
        pLen[1] = nLength >> 0u;
        break;
    }
}

llrp_bool_t
CFrameEncoderStream::checkAvailable (
  unsigned int                  nByte,
  const CFieldDescriptor *      pFieldDescriptor)
{
    CErrorDetails *             pError = &m_pEncoder->m_ErrorDetails;

    if(RC_OK != pError->m_eResultCode)
    {
        return FALSE;
    }

    if(m_pEncoder->m_iNext + nByte > m_pEncoder->m_nBuffer)
    {
        pError->m_eResultCode = RC_FieldOverrun;
        pError->m_pRefField   = pFieldDescriptor;
        pError->m_pRefType    = m_pRefType;
        pError->m_pWhatStr    = "overrun at field";
        pError->m_OtherDetail = m_pEncoder->m_iNext;

        return FALSE;
    }

    if(0 != m_pEncoder->m_nBitFieldResid)
    {
        pError->m_eResultCode = RC_UnalignedBitField;
        pError->m_pRefField   = pFieldDescriptor;
        pError->m_pRefType    = m_pRefType;
        pError->m_pWhatStr    = "unalign/incomplete bit field";
        pError->m_OtherDetail = m_pEncoder->m_iNext;

        return FALSE;
    }

    return TRUE;
}

llrp_bool_t
CFrameEncoderStream::putBitField (
  unsigned int                  nBit,
  unsigned int                  Value,
  const CFieldDescriptor *      pFieldDescriptor)
{
    CErrorDetails *             pError = &m_pEncoder->m_ErrorDetails;

    if(0 == m_pEncoder->m_nBitFieldResid)
    {
        if(!checkAvailable(1u, pFieldDescriptor))
        {
            return FALSE;
        }
        m_pEncoder->m_BitFieldBuffer = 0;
        m_pEncoder->m_nBitFieldResid = 8u;
    }

    if(m_pEncoder->m_nBitFieldResid < nBit)
    {
        pError->m_eResultCode = RC_UnalignedBitField;
        pError->m_pWhatStr    = "unalign/incomplete bit field";
        pError->m_pRefType    = m_pRefType;
        pError->m_pRefField   = pFieldDescriptor;
        pError->m_OtherDetail = m_pEncoder->m_iNext;
        return FALSE;
    }

    m_pEncoder->m_nBitFieldResid -= nBit;

    Value &= (1u << nBit) - 1u;

    m_pEncoder->m_BitFieldBuffer |= Value << m_pEncoder->m_nBitFieldResid;


    if(0 == m_pEncoder->m_nBitFieldResid)
    {
        m_pEncoder->next_u8(m_pEncoder->m_BitFieldBuffer);
        m_pEncoder->m_BitFieldBuffer = 0;
        m_pEncoder->m_nBitFieldResid = 0;
    }

    return TRUE;
}

};
