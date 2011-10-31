
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

CFrameDecoder::CFrameDecoder (
  const CTypeRegistry *         pTypeRegistry,
  unsigned char *               pBuffer,
  unsigned int                  nBuffer)
 : CDecoder(pTypeRegistry)
{
    m_pBuffer        = pBuffer;
    m_nBuffer        = nBuffer;

    m_iNext          = 0;
    m_BitFieldBuffer = 0;
    m_nBitFieldResid = 0;
}

CFrameDecoder::~CFrameDecoder (void)
{
}

CMessage *
CFrameDecoder::decodeMessage (void)
{
    CFrameDecoderStream     DecoderStream(this);
    CMessage *              pMessage;

    pMessage = DecoderStream.getMessage();

    return pMessage;
}

llrp_u8_t
CFrameDecoder::next_u8 (void)
{
    llrp_u8_t                   Value;

    assert(m_iNext + 1u <= m_nBuffer);

    Value = m_pBuffer[m_iNext++];

    return Value;
}

llrp_u16_t
CFrameDecoder::next_u16 (void)
{
    llrp_u16_t                  Value;

    assert(m_iNext + 2u <= m_nBuffer);

    Value = m_pBuffer[m_iNext++];
    Value <<= 8u;
    Value |= m_pBuffer[m_iNext++];

    return Value;
}

llrp_u32_t
CFrameDecoder::next_u32 (void)
{
    llrp_u32_t                  Value;

    assert(m_iNext + 4u <= m_nBuffer);

    Value = m_pBuffer[m_iNext++];
    Value <<= 8u;
    Value |= m_pBuffer[m_iNext++];
    Value <<= 8u;
    Value |= m_pBuffer[m_iNext++];
    Value <<= 8u;
    Value |= m_pBuffer[m_iNext++];

    return Value;
}

llrp_u64_t
CFrameDecoder::next_u64 (void)
{
    llrp_u64_t                  Value;

    assert(m_iNext + 8u <= m_nBuffer);

    Value = m_pBuffer[m_iNext++];
    Value <<= 8u;
    Value |= m_pBuffer[m_iNext++];
    Value <<= 8u;
    Value |= m_pBuffer[m_iNext++];
    Value <<= 8u;
    Value |= m_pBuffer[m_iNext++];
    Value <<= 8u;
    Value |= m_pBuffer[m_iNext++];
    Value <<= 8u;
    Value |= m_pBuffer[m_iNext++];
    Value <<= 8u;
    Value |= m_pBuffer[m_iNext++];
    Value <<= 8u;
    Value |= m_pBuffer[m_iNext++];

    return Value;
}

llrp_u8_t
CFrameDecoderStream::get_u8 (
  const CFieldDescriptor *      pFieldDescriptor)
{
    llrp_u8_t                   Value;

    if(checkAvailable(1u, pFieldDescriptor))
    {
        Value = m_pDecoder->next_u8();
    }
    else
    {
        Value = 0;
    }

    return Value;
}

llrp_s8_t
CFrameDecoderStream::get_s8 (
  const CFieldDescriptor *      pFieldDescriptor)
{
    llrp_s8_t                   Value;

    if(checkAvailable(1u, pFieldDescriptor))
    {
        Value = m_pDecoder->next_u8();
    }
    else
    {
        Value = 0;
    }

    return Value;
}

llrp_u8v_t
CFrameDecoderStream::get_u8v (
  const CFieldDescriptor *      pFieldDescriptor)
{
    llrp_u16_t                  nValue;
    llrp_u8v_t                  Value;

    nValue = getVarlenCount(pFieldDescriptor);

    if(0 < nValue)
    {
        if(checkAvailable(1u * nValue, pFieldDescriptor))
        {
            Value = llrp_u8v_t(nValue);
            if(verifyVectorAllocation(Value.m_pValue, pFieldDescriptor))
            {
                for(unsigned int Ix = 0; Ix < nValue; Ix++)
                {
                    Value.m_pValue[Ix] = m_pDecoder->next_u8();
                }
            }
        }
    }

    return Value;
}

llrp_s8v_t
CFrameDecoderStream::get_s8v (
  const CFieldDescriptor *      pFieldDescriptor)
{
    llrp_u16_t                  nValue;
    llrp_s8v_t                  Value;

    nValue = getVarlenCount(pFieldDescriptor);

    if(0 < nValue)
    {
        if(checkAvailable(1u * nValue, pFieldDescriptor))
        {
            Value = llrp_s8v_t(nValue);
            if(verifyVectorAllocation(Value.m_pValue, pFieldDescriptor))
            {
                for(unsigned int Ix = 0; Ix < nValue; Ix++)
                {
                    Value.m_pValue[Ix] = m_pDecoder->next_u8();
                }
            }
        }
    }

    return Value;
}

llrp_u16_t
CFrameDecoderStream::get_u16 (
  const CFieldDescriptor *      pFieldDescriptor)
{
    llrp_u16_t                  Value;

    if(checkAvailable(2u, pFieldDescriptor))
    {
        Value = m_pDecoder->next_u16();
    }
    else
    {
        Value = 0;
    }

    return Value;
}

llrp_s16_t
CFrameDecoderStream::get_s16 (
  const CFieldDescriptor *      pFieldDescriptor)
{
    llrp_u16_t                  Value;

    if(checkAvailable(2u, pFieldDescriptor))
    {
        Value = m_pDecoder->next_u16();
    }
    else
    {
        Value = 0;
    }

    return Value;
}

llrp_u16v_t
CFrameDecoderStream::get_u16v (
  const CFieldDescriptor *      pFieldDescriptor)
{
    llrp_u16_t                  nValue;
    llrp_u16v_t                 Value;

    nValue = getVarlenCount(pFieldDescriptor);

    if(0 < nValue)
    {
        if(checkAvailable(2u * nValue, pFieldDescriptor))
        {
            Value = llrp_u16v_t(nValue);
            if(verifyVectorAllocation(Value.m_pValue, pFieldDescriptor))
            {
                for(unsigned int Ix = 0; Ix < nValue; Ix++)
                {
                    Value.m_pValue[Ix] = m_pDecoder->next_u16();
                }
            }
        }
    }

    return Value;
}

llrp_s16v_t
CFrameDecoderStream::get_s16v (
  const CFieldDescriptor *      pFieldDescriptor)
{
    llrp_u16_t                  nValue;
    llrp_s16v_t                 Value;

    nValue = getVarlenCount(pFieldDescriptor);

    if(0 < nValue)
    {
        if(checkAvailable(2u * nValue, pFieldDescriptor))
        {
            Value = llrp_s16v_t(nValue);
            if(verifyVectorAllocation(Value.m_pValue, pFieldDescriptor))
            {
                for(unsigned int Ix = 0; Ix < nValue; Ix++)
                {
                    Value.m_pValue[Ix] = m_pDecoder->next_u16();
                }
            }
        }
    }

    return Value;
}



llrp_u32_t
CFrameDecoderStream::get_u32 (
  const CFieldDescriptor *      pFieldDescriptor)
{
    llrp_u32_t                  Value;

    if(checkAvailable(4u, pFieldDescriptor))
    {
        Value = m_pDecoder->next_u32();
    }
    else
    {
        Value = 0;
    }

    return Value;
}

llrp_s32_t
CFrameDecoderStream::get_s32 (
  const CFieldDescriptor *      pFieldDescriptor)
{
    llrp_s32_t                  Value;

    if(checkAvailable(4u, pFieldDescriptor))
    {
        Value = m_pDecoder->next_u32();
    }
    else
    {
        Value = 0;
    }

    return Value;
}

llrp_u32v_t
CFrameDecoderStream::get_u32v (
  const CFieldDescriptor *      pFieldDescriptor)
{
    llrp_u16_t                  nValue;
    llrp_u32v_t                 Value;

    nValue = getVarlenCount(pFieldDescriptor);

    if(0 < nValue)
    {
        if(checkAvailable(4u * nValue, pFieldDescriptor))
        {
            Value = llrp_u32v_t(nValue);
            if(verifyVectorAllocation(Value.m_pValue, pFieldDescriptor))
            {
                for(unsigned int Ix = 0; Ix < nValue; Ix++)
                {
                    Value.m_pValue[Ix] = m_pDecoder->next_u32();
                }
            }
        }
    }

    return Value;
}

llrp_s32v_t
CFrameDecoderStream::get_s32v (
  const CFieldDescriptor *      pFieldDescriptor)
{
    llrp_u16_t                  nValue;
    llrp_s32v_t                 Value;

    nValue = getVarlenCount(pFieldDescriptor);

    if(0 < nValue)
    {
        if(checkAvailable(4u * nValue, pFieldDescriptor))
        {
            Value = llrp_s32v_t(nValue);
            if(verifyVectorAllocation(Value.m_pValue, pFieldDescriptor))
            {
                for(unsigned int Ix = 0; Ix < nValue; Ix++)
                {
                    Value.m_pValue[Ix] = m_pDecoder->next_u32();
                }
            }
        }
    }

    return Value;
}


llrp_u64_t
CFrameDecoderStream::get_u64 (
  const CFieldDescriptor *      pFieldDescriptor)
{
    llrp_u64_t                  Value;

    if(checkAvailable(8u, pFieldDescriptor))
    {
        Value = m_pDecoder->next_u64();
    }
    else
    {
        Value = 0;
    }

    return Value;
}

llrp_s64_t
CFrameDecoderStream::get_s64 (
  const CFieldDescriptor *      pFieldDescriptor)
{
    llrp_s64_t                  Value;

    if(checkAvailable(8u, pFieldDescriptor))
    {
        Value = m_pDecoder->next_u64();
    }
    else
    {
        Value = 0;
    }

    return Value;
}

llrp_u64v_t
CFrameDecoderStream::get_u64v (
  const CFieldDescriptor *      pFieldDescriptor)
{
    llrp_u16_t                  nValue;
    llrp_u64v_t                 Value;

    nValue = getVarlenCount(pFieldDescriptor);

    if(0 < nValue)
    {
        if(checkAvailable(8u * nValue, pFieldDescriptor))
        {
            Value = llrp_u64v_t(nValue);
            if(verifyVectorAllocation(Value.m_pValue, pFieldDescriptor))
            {
                for(unsigned int Ix = 0; Ix < nValue; Ix++)
                {
                    Value.m_pValue[Ix] = m_pDecoder->next_u64();
                }
            }
        }
    }

    return Value;
}

llrp_s64v_t
CFrameDecoderStream::get_s64v (
  const CFieldDescriptor *      pFieldDescriptor)
{
    llrp_u16_t                  nValue;
    llrp_s64v_t                 Value;

    nValue = getVarlenCount(pFieldDescriptor);

    if(0 < nValue)
    {
        if(checkAvailable(8u * nValue, pFieldDescriptor))
        {
            Value = llrp_s64v_t(nValue);
            if(verifyVectorAllocation(Value.m_pValue, pFieldDescriptor))
            {
                for(unsigned int Ix = 0; Ix < nValue; Ix++)
                {
                    Value.m_pValue[Ix] = m_pDecoder->next_u64();
                }
            }
        }
    }

    return Value;
}

llrp_u1_t
CFrameDecoderStream::get_u1 (
  const CFieldDescriptor *      pFieldDescriptor)
{
    llrp_u1_t                   Value;

    Value = getBitField(1, pFieldDescriptor);

    return Value;
}

llrp_u1v_t
CFrameDecoderStream::get_u1v (
  const CFieldDescriptor *      pFieldDescriptor)
{
    llrp_u16_t                  nBit;
    llrp_u1v_t                  Value;

    nBit = getVarlenCount(pFieldDescriptor);

    if(0 < nBit)
    {
        unsigned int                nByte = (nBit + 7u) / 8u;

        if(checkAvailable(nByte, pFieldDescriptor))
        {
            Value = llrp_u1v_t(nBit);
            if(verifyVectorAllocation(Value.m_pValue, pFieldDescriptor))
            {
                for(unsigned int Ix = 0; Ix < nByte; Ix++)
                {
                    Value.m_pValue[Ix] = m_pDecoder->next_u8();
                }
            }
        }
    }

    return Value;
}

llrp_u2_t
CFrameDecoderStream::get_u2 (
  const CFieldDescriptor *      pFieldDescriptor)
{
    llrp_u2_t                   Value;

    Value = getBitField(2, pFieldDescriptor);

    return Value;
}

llrp_u96_t
CFrameDecoderStream::get_u96 (
  const CFieldDescriptor *      pFieldDescriptor)
{
    llrp_u96_t                  Value;

    if(checkAvailable(12u, pFieldDescriptor))
    {
        for(unsigned int Ix = 0; Ix < 12u; Ix++)
        {
            Value.m_aValue[Ix] = m_pDecoder->next_u8();
        }
    }

    return Value;
}

llrp_utf8v_t
CFrameDecoderStream::get_utf8v (
  const CFieldDescriptor *      pFieldDescriptor)
{
    llrp_u16_t                  nValue;
    llrp_utf8v_t                Value;

    nValue = getVarlenCount(pFieldDescriptor);

    if(0 < nValue)
    {
        if(checkAvailable(1u * nValue, pFieldDescriptor))
        {
            Value = llrp_utf8v_t(nValue);
            if(verifyVectorAllocation(Value.m_pValue, pFieldDescriptor))
            {
                for(unsigned int Ix = 0; Ix < nValue; Ix++)
                {
                    Value.m_pValue[Ix] = m_pDecoder->next_u8();
                }
            }
        }
    }

    return Value;
}

llrp_bytesToEnd_t
CFrameDecoderStream::get_bytesToEnd (
  const CFieldDescriptor *      pFieldDescriptor)
{
    llrp_u16_t                  nValue;
    llrp_bytesToEnd_t           Value;

    nValue = getRemainingByteCount();

    if(0 < nValue)
    {
        if(checkAvailable(1u * nValue, pFieldDescriptor))
        {
            Value = llrp_bytesToEnd_t(nValue);
            if(verifyVectorAllocation(Value.m_pValue, pFieldDescriptor))
            {
                for(unsigned int Ix = 0; Ix < nValue; Ix++)
                {
                    Value.m_pValue[Ix] = m_pDecoder->next_u8();
                }
            }
        }
    }

    return Value;
}

int
CFrameDecoderStream::get_e1 (
  const CFieldDescriptor *      pFieldDescriptor)
{
    int                         eValue;

    eValue = (int)get_u1(pFieldDescriptor);

    return eValue;
}

int
CFrameDecoderStream::get_e2 (
  const CFieldDescriptor *      pFieldDescriptor)
{
    int                         eValue;

    eValue = (int)get_u2(pFieldDescriptor);

    return eValue;
}

int
CFrameDecoderStream::get_e8 (
  const CFieldDescriptor *      pFieldDescriptor)
{
    int                         eValue;

    eValue = (int)get_u8(pFieldDescriptor);

    return eValue;
}

int
CFrameDecoderStream::get_e16 (
  const CFieldDescriptor *      pFieldDescriptor)
{
    int                         eValue;

    eValue = (int)get_u16(pFieldDescriptor);

    return eValue;
}

int
CFrameDecoderStream::get_e32 (
  const CFieldDescriptor *      pFieldDescriptor)
{
    int                         eValue;

    eValue = (int)get_u32(pFieldDescriptor);

    return eValue;
}

llrp_u8v_t
CFrameDecoderStream::get_e8v (
  const CFieldDescriptor *      pFieldDescriptor)
{
    return get_u8v(pFieldDescriptor);
}

void
CFrameDecoderStream::get_reserved (
  unsigned int                  nBit)
{
    CErrorDetails *             pError = &m_pDecoder->m_ErrorDetails;

    if(RC_OK != pError->m_eResultCode)
    {
        return;
    }

    while(0 < nBit)
    {
        unsigned int            Step = 7u & nBit;

        if(0 != m_pDecoder->m_nBitFieldResid)
        {
            if(Step != m_pDecoder->m_nBitFieldResid)
            {
                pError->m_eResultCode = RC_UnalignedReservedBits;
                pError->m_pWhatStr    = "unaligned reserved bits";
                pError->m_pRefType    = m_pRefType;
                pError->m_pRefField   = NULL;
                pError->m_OtherDetail = m_pDecoder->m_iNext;
                return;
            }

            nBit -= Step;
            m_pDecoder->m_nBitFieldResid = 0;
        }
        else
        {
            if(0 != Step)
            {
                pError->m_eResultCode = RC_UnalignedReservedBits;
                pError->m_pWhatStr    = "unaligned reserved bits";
                pError->m_pRefType    = m_pRefType;
                pError->m_pRefField   = NULL;
                pError->m_OtherDetail = m_pDecoder->m_iNext;
                return;
            }

            if(m_pDecoder->m_iNext >= m_iLimit)
            {
                pError->m_eResultCode = RC_ReservedBitsUnderrun;
                pError->m_pWhatStr    = "underrun at reserved bits";
                pError->m_pRefType    = m_pRefType;
                pError->m_pRefField   = NULL;
                pError->m_OtherDetail = m_pDecoder->m_iNext;
                return;
            }

            m_pDecoder->next_u8();
            nBit -= 8;
        }
    }
}

CFrameDecoderStream::CFrameDecoderStream (
  CFrameDecoder *               pDecoder)
{
    m_pDecoder                  = pDecoder;
    m_pEnclosingDecoderStream   = NULL;
    m_iBegin                    = pDecoder->m_iNext;
    m_iLimit                    = pDecoder->m_nBuffer;
    m_pRefType                  = NULL;
}

CFrameDecoderStream::CFrameDecoderStream (
  CFrameDecoderStream *         pEnclosingDecoderStream)
{
    m_pDecoder                  = pEnclosingDecoderStream->m_pDecoder;
    m_pEnclosingDecoderStream   = pEnclosingDecoderStream;
    m_iBegin                    = m_pDecoder->m_iNext;
    m_iLimit                    = pEnclosingDecoderStream->m_iLimit;
    m_pRefType                  = NULL;
}

CMessage *
CFrameDecoderStream::getMessage (void)
{
    CErrorDetails *             pError = &m_pDecoder->m_ErrorDetails;
    const CTypeRegistry *       pRegistry = m_pDecoder->m_pRegistry;
    const CTypeDescriptor *     pTypeDescriptor = NULL;
    llrp_u16_t                  Type;
    llrp_u16_t                  Vers;
    llrp_u32_t                  nLength;
    unsigned int                iLimit;
    llrp_u32_t                  MessageID;

    if(RC_OK != pError->m_eResultCode)
    {
        return NULL;
    }

    Type = get_u16(&g_fdMessageHeader_Type);
    Vers = (Type >> 10) & 3;
    Type &= 0x3FF;

    if(RC_OK != pError->m_eResultCode)
    {
        return NULL;
    }

    if(1u != Vers)
    {
        pError->m_eResultCode = RC_BadVersion;
        pError->m_pWhatStr    = "unsupported version";
        pError->m_pRefType    = NULL;
        pError->m_pRefField   = &g_fdMessageHeader_Type;
        pError->m_OtherDetail = m_pDecoder->m_iNext;
        return NULL;
    }

    nLength = get_u32(&g_fdMessageHeader_Length);

    if(RC_OK != pError->m_eResultCode)
    {
        return NULL;
    }

    if(10u > nLength)
    {
        pError->m_eResultCode = RC_InvalidLength;
        pError->m_pWhatStr    = "message length too small";
        pError->m_pRefType    = NULL;
        pError->m_pRefField   = &g_fdMessageHeader_Length;
        pError->m_OtherDetail = m_pDecoder->m_iNext;
        return NULL;
    }

    iLimit = m_iBegin + nLength;

    if(iLimit > m_iLimit)
    {
        pError->m_eResultCode = RC_ExcessiveLength;
        pError->m_pWhatStr    = "message length exceeds enclosing length";
        pError->m_pRefType    = NULL;
        pError->m_pRefField   = &g_fdMessageHeader_Length;
        pError->m_OtherDetail = m_pDecoder->m_iNext;
        return NULL;
    }

    m_iLimit = iLimit;

    MessageID = get_u32(&g_fdMessageHeader_MessageID);

    if(RC_OK != pError->m_eResultCode)
    {
        return NULL;
    }

    /* Custom? */
    if(1023u == Type)
    {
        llrp_u32_t              VendorPEN;
        llrp_u8_t               Subtype;

        VendorPEN = get_u32(&g_fdMessageHeader_VendorPEN);
        Subtype   = get_u8(&g_fdMessageHeader_Subtype);

        if(RC_OK != pError->m_eResultCode)
        {
            return NULL;
        }

        pTypeDescriptor = pRegistry->lookupCustomMessage(VendorPEN, Subtype);
        if(NULL == pTypeDescriptor)
        {
            /*
             * If we don't have a definition for a particular
             * CUSTOM message, just use the generic one.
             */
            m_pDecoder->m_iNext -= 5;   /* back up to VendorPEN and SubType */
            pTypeDescriptor = pRegistry->lookupMessage(1023u);
        }
    }
    else
    {
        pTypeDescriptor = pRegistry->lookupMessage(Type);
    }

    if(NULL == pTypeDescriptor)
    {
        pError->m_eResultCode = RC_UnknownMessageType;
        pError->m_pWhatStr    = "unknown message type";
        pError->m_pRefType    = NULL;
        pError->m_pRefField   = &g_fdMessageHeader_Type;
        pError->m_OtherDetail = 0;
        return NULL;
    }

    m_pRefType = pTypeDescriptor;

    CMessage *                  pMessage;

    pMessage = (CMessage *) pTypeDescriptor->constructElement();

    if(NULL == pMessage)
    {
        pError->m_eResultCode = RC_MessageAllocationFailed;
        pError->m_pWhatStr    = "message allocation failed";
        pError->m_pRefType    = pTypeDescriptor;
        pError->m_pRefField   = NULL;
        pError->m_OtherDetail = m_pDecoder->m_iNext;
        return NULL;
    }

    pMessage->setMessageID(MessageID);

    pMessage->decodeFields(this);

    if(RC_OK != pError->m_eResultCode)
    {
        delete pMessage;
        return NULL;
    }

    /*
     * Subparameters
     */
    while(0 < getRemainingByteCount() &&
          RC_OK == pError->m_eResultCode)
    {
        CFrameDecoderStream         NestStream(this);
        CParameter *                pParameter;

        pParameter = NestStream.getParameter();

        if(NULL == pParameter)
        {
            if(RC_OK == pError->m_eResultCode)
            {
                pError->m_eResultCode = RC_Botch;
                pError->m_pWhatStr    = "botch -- no param and no error";
                pError->m_pRefType    = pTypeDescriptor;
                pError->m_pRefField   = NULL;
                pError->m_OtherDetail = m_pDecoder->m_iNext;
            }
            break;
        }

        pParameter->m_pParent = pMessage;
        pMessage->addSubParameterToAllList(pParameter);

    }

    if(RC_OK == pError->m_eResultCode)
    {
        if(m_pDecoder->m_iNext != m_iLimit)
        {
            pError->m_eResultCode = RC_ExtraBytes;
            pError->m_pWhatStr    = "extra bytes at end of message";
            pError->m_pRefType    = pTypeDescriptor;
            pError->m_pRefField   = NULL;
            pError->m_OtherDetail = m_pDecoder->m_iNext;
        }
    }

    if(RC_OK != pError->m_eResultCode)
    {
        delete pMessage;
        return NULL;
    }

    pMessage->assimilateSubParameters(pError);

    if(RC_OK != pError->m_eResultCode)
    {
        delete pMessage;
        return NULL;
    }

    return pMessage;
}

CParameter *
CFrameDecoderStream::getParameter (void)
{
    CErrorDetails *             pError = &m_pDecoder->m_ErrorDetails;
    const CTypeRegistry *       pRegistry = m_pDecoder->m_pRegistry;
    const CTypeDescriptor *     pTypeDescriptor = NULL;
    llrp_u16_t                  Type;
    bool                        bIsTV;

    if(RC_OK != pError->m_eResultCode)
    {
        return NULL;
    }

    Type = get_u8(&g_fdParameterHeader_TVType);

    if(RC_OK != pError->m_eResultCode)
    {
        return NULL;
    }

    if(0 != (Type&0x80))
    {
        /*
         * Type-Value (TV).
         * All we can do is inherit the length from
         * the enclosing element.
         */
        Type &= 0x7F;
        bIsTV = TRUE;
    }
    else
    {
        /*
         * Type-Length-Value (TLV).
         * Back up and get the real type number,
         * then get the length.
         */
        m_pDecoder->m_iNext--;
        Type = get_u16(&g_fdParameterHeader_TLVType);
        Type &= 0x3FF;

        if(RC_OK != pError->m_eResultCode)
        {
            return NULL;
        }

        llrp_u16_t              nLength;

        nLength = get_u16(&g_fdParameterHeader_TLVLength);

        if(RC_OK != pError->m_eResultCode)
        {
            return NULL;
        }

        if(4u > nLength)
        {
            pError->m_eResultCode = RC_InvalidLength;
            pError->m_pWhatStr    = "TLV parameter length too small";
            pError->m_pRefType    = NULL;
            pError->m_pRefField   = &g_fdParameterHeader_TLVLength;
            pError->m_OtherDetail = m_pDecoder->m_iNext;
            return NULL;
        }

        unsigned int            iLimit;

        iLimit = m_iBegin + nLength;

        if(iLimit > m_iLimit)
        {
            pError->m_eResultCode = RC_ExcessiveLength;
            pError->m_pWhatStr    =
                            "TLV parameter length exceeds enclosing length";
            pError->m_pRefType    = NULL;
            pError->m_pRefField   = &g_fdParameterHeader_TLVLength;
            pError->m_OtherDetail = m_pDecoder->m_iNext;
            return NULL;
        }

        m_iLimit = iLimit;

        bIsTV = FALSE;
    }

    /* Custom? */
    if(1023u == Type)
    {
        llrp_u32_t              VendorPEN;
        llrp_u32_t               Subtype;

        VendorPEN = get_u32(&g_fdParameterHeader_VendorPEN);
        Subtype   = get_u32(&g_fdParameterHeader_Subtype);

        if(RC_OK != pError->m_eResultCode)
        {
            return NULL;
        }

        pTypeDescriptor = pRegistry->lookupCustomParameter(VendorPEN, Subtype);
        if(NULL == pTypeDescriptor)
        {
            /*
             * If we don't have a definition for a particular
             * CUSTOM message, just use the generic one.
             */
            m_pDecoder->m_iNext -= 8;   /* back up to VendorPEN and SubType */
            pTypeDescriptor = pRegistry->lookupParameter(1023u);
        }
    }
    else
    {
        pTypeDescriptor = pRegistry->lookupParameter(Type);
    }

    if(NULL == pTypeDescriptor)
    {
        pError->m_eResultCode = RC_UnknownParameterType;
        pError->m_pWhatStr    = "unknown parameter type";
        pError->m_pRefType    = NULL;
        if(bIsTV)
        {
            pError->m_pRefField = &g_fdParameterHeader_TVType;
        }
        else
        {
            pError->m_pRefField = &g_fdParameterHeader_TLVType;
        }
        pError->m_OtherDetail = m_pDecoder->m_iNext;
        return NULL;
    }

    m_pRefType = pTypeDescriptor;

    CParameter *                pParameter;

    pParameter = (CParameter *) pTypeDescriptor->constructElement();

    if(NULL == pParameter)
    {
        pError->m_eResultCode = RC_ParameterAllocationFailed;
        pError->m_pWhatStr    = "parameter allocation failed";
        pError->m_pRefType    = pTypeDescriptor;
        pError->m_pRefField   = NULL;
        pError->m_OtherDetail = m_pDecoder->m_iNext;
        return NULL;
    }

    pParameter->decodeFields(this);

    if(RC_OK != pError->m_eResultCode)
    {
        delete pParameter;
        return NULL;
    }

    if(!bIsTV)
    {
        /*
         * Subparameters
         */
        while(0 < getRemainingByteCount() &&
          RC_OK == pError->m_eResultCode)
        {
            CFrameDecoderStream         NestStream(this);
            CParameter *                pSubParameter;

            pSubParameter = NestStream.getParameter();

            if(NULL == pSubParameter)
            {
                if(RC_OK == pError->m_eResultCode)
                {
                    pError->m_eResultCode = RC_Botch;
                    pError->m_pWhatStr    = "botch -- no param and no error";
                    pError->m_pRefType    = pTypeDescriptor;
                    pError->m_pRefField   = NULL;
                    pError->m_OtherDetail = m_pDecoder->m_iNext;
                }
                break;
            }

            pSubParameter->m_pParent = pParameter;
            pParameter->addSubParameterToAllList(pSubParameter);
        }

        if(RC_OK == pError->m_eResultCode)
        {
            if(m_pDecoder->m_iNext != m_iLimit)
            {
                pError->m_eResultCode = RC_ExtraBytes;
                pError->m_pWhatStr    = "extra bytes at end of TLV parameter";
                pError->m_pRefType    = pTypeDescriptor;
                pError->m_pRefField   = NULL;
                pError->m_OtherDetail = m_pDecoder->m_iNext;
            }
        }

        if(RC_OK != pError->m_eResultCode)
        {
            delete pParameter;
            return NULL;
        }

        pParameter->assimilateSubParameters(pError);

        if(RC_OK != pError->m_eResultCode)
        {
            delete pParameter;
            return NULL;
        }
    }

    return pParameter;
}

unsigned int
CFrameDecoderStream::getRemainingByteCount (void)
{
    if(m_pDecoder->m_iNext < m_iLimit)
    {
        return m_iLimit - m_pDecoder->m_iNext;
    }
    else
    {
        return 0;
    }
}

llrp_bool_t
CFrameDecoderStream::checkAvailable (
  unsigned int                  nByte,
  const CFieldDescriptor *      pFieldDescriptor)
{
    CErrorDetails *             pError = &m_pDecoder->m_ErrorDetails;

    if(RC_OK != pError->m_eResultCode)
    {
        return FALSE;
    }

    if(m_pDecoder->m_iNext + nByte > m_iLimit)
    {
        pError->m_eResultCode = RC_FieldUnderrun;
        pError->m_pWhatStr    = "underrun at field";
        pError->m_pRefType    = m_pRefType;
        pError->m_pRefField   = pFieldDescriptor;
        pError->m_OtherDetail = m_pDecoder->m_iNext;
        return FALSE;
    }

    if(0 != m_pDecoder->m_nBitFieldResid)
    {
        pError->m_eResultCode = RC_UnalignedBitField;
        pError->m_pWhatStr    = "unaligned/incomplete bit field";
        pError->m_pRefType    = m_pRefType;
        pError->m_pRefField   = pFieldDescriptor;
        pError->m_OtherDetail = m_pDecoder->m_iNext;
        return FALSE;
    }

    return TRUE;
}

unsigned int
CFrameDecoderStream::getBitField (
  unsigned int                  nBit,
  const CFieldDescriptor *      pFieldDescriptor)
{
    CErrorDetails *             pError = &m_pDecoder->m_ErrorDetails;
    unsigned int                Value;

    if(0 == m_pDecoder->m_nBitFieldResid)
    {
        if(checkAvailable(1u, pFieldDescriptor))
        {
            m_pDecoder->m_BitFieldBuffer = m_pDecoder->next_u8();
            m_pDecoder->m_nBitFieldResid = 8u;
        }
        else
        {
            return 0;
        }
    }

    if(m_pDecoder->m_nBitFieldResid < nBit)
    {
        pError->m_eResultCode = RC_UnalignedBitField;
        pError->m_pWhatStr    = "unaligned/incomplete bit field";
        pError->m_pRefType    = m_pRefType;
        pError->m_pRefField   = pFieldDescriptor;
        pError->m_OtherDetail = m_pDecoder->m_iNext;
        return 0;
    }

    m_pDecoder->m_nBitFieldResid -= nBit;

    Value = m_pDecoder->m_BitFieldBuffer >> m_pDecoder->m_nBitFieldResid;
    Value &= (1u << nBit) - 1u;

    return Value;
}

llrp_u16_t
CFrameDecoderStream::getVarlenCount (
  const CFieldDescriptor *      pFieldDescriptor)
{
    llrp_u16_t                  nValue;

    if(checkAvailable(2u, pFieldDescriptor))
    {
        nValue = m_pDecoder->next_u16();
    }
    else
    {
        nValue = 0;
    }

    return nValue;
}

llrp_bool_t
CFrameDecoderStream::verifyVectorAllocation (
  const void *              pValue,
  const CFieldDescriptor *  pFieldDescriptor)
{
    if(NULL == pValue)
    {
        CErrorDetails *     pError = &m_pDecoder->m_ErrorDetails;

        pError->m_eResultCode = RC_FieldAllocationFailed;
        pError->m_pWhatStr    = "field allocation failed";
        pError->m_pRefType    = m_pRefType;
        pError->m_pRefField   = pFieldDescriptor;
        pError->m_OtherDetail = m_pDecoder->m_iNext;

        return FALSE;
    }
    else
    {
        return TRUE;
    }
}

};  /* namespace LLRP */
