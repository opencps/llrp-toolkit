
/*
 ***************************************************************************
 *  Copyright 2007 Impinj, Inc.
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



#include <stdio.h>

#include "ltkcpp_platform.h"
#include "ltkcpp_base.h"
#include "ltkcpp_prxml.h"



namespace LLRP
{

CPrXMLEncoder::CPrXMLEncoder (void)
{
}

CPrXMLEncoder::~CPrXMLEncoder (void)
{
}

void
CPrXMLEncoder::encodeElement (
  const CElement *              pElement)
{
    CPrXMLEncoderStream         MyEncoderStream(this);

    MyEncoderStream.putElement(pElement);
}

void
CPrXMLEncoderStream::putRequiredSubParameter (
  const CParameter *            pParameter,
  const CTypeDescriptor *       pRefType)
{
    if(NULL == pParameter)
    {
        printf("warning: missing %s\n",
            (NULL == pRefType) ? "<something>" : pRefType->m_pName);
        return;
    }

    CPrXMLEncoderStream         NestEncoderStream(this);

    NestEncoderStream.putElement(pParameter);
}

void
CPrXMLEncoderStream::putOptionalSubParameter (
  const CParameter *            pParameter,
  const CTypeDescriptor *       pRefType)
{
    if(NULL == pParameter)
    {
        return;
    }

    CPrXMLEncoderStream         NestEncoderStream(this);

    NestEncoderStream.putElement(pParameter);
}

void
CPrXMLEncoderStream::putRequiredSubParameterList (
  const tListOfParameters *     pParameterList,
  const CTypeDescriptor *       pRefType)
{
    if(pParameterList->empty())
    {
        printf("warning: missing list of %s\n",
            (NULL == pRefType) ? "<something>" : pRefType->m_pName);
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
CPrXMLEncoderStream::putOptionalSubParameterList (
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


/*
 * 8-bit types
 */

void
CPrXMLEncoderStream::put_u8 (
  llrp_u8_t                     Value,
  const CFieldDescriptor *      pFieldDesc)
{
    indent();
    printf("<%s>%u</%s>\n",
        pFieldDesc->m_pName, Value, pFieldDesc->m_pName);
}

void
CPrXMLEncoderStream::put_s8 (
  llrp_s8_t                     Value,
  const CFieldDescriptor *      pFieldDesc)
{
    indent();
    printf("<%s>%d</%s>\n",
        pFieldDesc->m_pName, Value, pFieldDesc->m_pName);
}

void
CPrXMLEncoderStream::put_u8v (
  llrp_u8v_t                    Value,
  const CFieldDescriptor *      pFieldDesc)
{
    indent();
    printf("<%s>", pFieldDesc->m_pName);
    for(int i = 0; i < Value.m_nValue; i++)
    {
        if(0 < i)
        {
            printf(" ");
        }
        printf("%u", Value.m_pValue[i]);
    }
    printf("</%s>\n", pFieldDesc->m_pName);
}

void
CPrXMLEncoderStream::put_s8v (
  llrp_s8v_t                    Value,
  const CFieldDescriptor *      pFieldDesc)
{
    indent();
    printf("<%s>", pFieldDesc->m_pName);
    for(int i = 0; i < Value.m_nValue; i++)
    {
        if(0 < i)
        {
            printf(" ");
        }
        printf("%d", Value.m_pValue[i]);
    }
    printf("</%s>\n", pFieldDesc->m_pName);
}

/*
 * 16-bit types
 */

void
CPrXMLEncoderStream::put_u16 (
  llrp_u16_t                    Value,
  const CFieldDescriptor *      pFieldDesc)
{
    indent();
    printf("<%s>%u</%s>\n",
        pFieldDesc->m_pName, Value, pFieldDesc->m_pName);
}

void
CPrXMLEncoderStream::put_s16 (
  llrp_s16_t                    Value,
  const CFieldDescriptor *      pFieldDesc)
{
    indent();
    printf("<%s>%d</%s>\n",
        pFieldDesc->m_pName, Value, pFieldDesc->m_pName);
}

void
CPrXMLEncoderStream::put_u16v (
  llrp_u16v_t                   Value,
  const CFieldDescriptor *      pFieldDesc)
{
    indent();
    printf("<%s>", pFieldDesc->m_pName);
    for(int i = 0; i < Value.m_nValue; i++)
    {
        if(0 < i)
        {
            printf(" ");
        }
        printf("%u", Value.m_pValue[i]);
    }
    printf("</%s>\n", pFieldDesc->m_pName);
}

void
CPrXMLEncoderStream::put_s16v (
  llrp_s16v_t                   Value,
  const CFieldDescriptor *      pFieldDesc)
{
    indent();
    printf("<%s>", pFieldDesc->m_pName);
    for(int i = 0; i < Value.m_nValue; i++)
    {
        if(0 < i)
        {
            printf(" ");
        }
        printf("%d", Value.m_pValue[i]);
    }
    printf("</%s>\n", pFieldDesc->m_pName);
}

/*
 * 32-bit types
 */

void
CPrXMLEncoderStream::put_u32 (
  llrp_u32_t                    Value,
  const CFieldDescriptor *      pFieldDesc)
{
    indent();
    printf("<%s>%u</%s>\n",
        pFieldDesc->m_pName, Value, pFieldDesc->m_pName);
}

void
CPrXMLEncoderStream::put_s32 (
  llrp_s32_t                    Value,
  const CFieldDescriptor *      pFieldDesc)
{
    indent();
    printf("<%s>%d</%s>\n",
        pFieldDesc->m_pName, Value, pFieldDesc->m_pName);
}

void
CPrXMLEncoderStream::put_u32v (
  llrp_u32v_t                   Value,
  const CFieldDescriptor *      pFieldDesc)
{
    indent();
    printf("<%s>", pFieldDesc->m_pName);
    for(int i = 0; i < Value.m_nValue; i++)
    {
        if(0 < i)
        {
            printf(" ");
        }
        printf("%u", Value.m_pValue[i]);
    }
    printf("</%s>\n", pFieldDesc->m_pName);
}

void
CPrXMLEncoderStream::put_s32v (
  llrp_s32v_t                   Value,
  const CFieldDescriptor *      pFieldDesc)
{
    indent();
    printf("<%s>", pFieldDesc->m_pName);
    for(int i = 0; i < Value.m_nValue; i++)
    {
        if(0 < i)
        {
            printf(" ");
        }
        printf("%d", Value.m_pValue[i]);
    }
    printf("</%s>\n", pFieldDesc->m_pName);
}

/*
 * 64-bit types
 */

void
CPrXMLEncoderStream::put_u64 (
  llrp_u64_t                    Value,
  const CFieldDescriptor *      pFieldDesc)
{
    indent();
#ifdef WIN32
    printf("<%s>%I64u</%s>\n",
        pFieldDesc->m_pName, Value, pFieldDesc->m_pName);
#else
    printf("<%s>%llu</%s>\n",
        pFieldDesc->m_pName, Value, pFieldDesc->m_pName);
#endif
}

void
CPrXMLEncoderStream::put_s64 (
  llrp_s64_t                    Value,
  const CFieldDescriptor *      pFieldDesc)
{
    indent();
#ifdef WIN32
    printf("<%s>%I64d</%s>\n",
        pFieldDesc->m_pName, Value, pFieldDesc->m_pName);
#else
    printf("<%s>%lld</%s>\n",
        pFieldDesc->m_pName, Value, pFieldDesc->m_pName);
#endif
}

void
CPrXMLEncoderStream::put_u64v (
  llrp_u64v_t                   Value,
  const CFieldDescriptor *      pFieldDesc)
{
    indent();
    printf("<%s>", pFieldDesc->m_pName);
    for(int i = 0; i < Value.m_nValue; i++)
    {
        if(0 < i)
        {
            printf(" ");
        }
#ifdef WIN32
        printf("%I64u", Value.m_pValue[i]);
#else
        printf("%llu", Value.m_pValue[i]);
#endif
    }
    printf("</%s>\n", pFieldDesc->m_pName);
}

void
CPrXMLEncoderStream::put_s64v (
  llrp_s64v_t                   Value,
  const CFieldDescriptor *      pFieldDesc)
{
    indent();
    printf("<%s>", pFieldDesc->m_pName);
    for(int i = 0; i < Value.m_nValue; i++)
    {
        if(0 < i)
        {
            printf(" ");
        }
#ifdef WIN32
        printf("%I64d", Value.m_pValue[i]);
#else
        printf("%lld", Value.m_pValue[i]);
#endif
    }
    printf("</%s>\n", pFieldDesc->m_pName);
}

/*
 * Special types
 */

void
CPrXMLEncoderStream::put_u1 (
  llrp_u1_t                     Value,
  const CFieldDescriptor *      pFieldDesc)
{
    indent();
    printf("<%s>%d</%s>\n",
        pFieldDesc->m_pName, Value & 1, pFieldDesc->m_pName);
}

void
CPrXMLEncoderStream::put_u1v (
  llrp_u1v_t                    Value,
  const CFieldDescriptor *      pFieldDesc)
{
    int                         nByteCount;

    nByteCount = (Value.m_nBit + 7u) / 8u;

    indent();
    printf("<%s Count='%d'>", pFieldDesc->m_pName, Value.m_nBit);
    for(int i = 0; i < nByteCount; i++)
    {
        printf("%02X", Value.m_pValue[i]);
    }
    printf("</%s>\n", pFieldDesc->m_pName);
}

void
CPrXMLEncoderStream::put_u2 (
  llrp_u2_t                     Value,
  const CFieldDescriptor *      pFieldDesc)
{
    indent();
    printf("<%s>%d</%s>\n",
        pFieldDesc->m_pName, Value & 3, pFieldDesc->m_pName);
}

void
CPrXMLEncoderStream::put_u96 (
  llrp_u96_t                    Value,
  const CFieldDescriptor *      pFieldDesc)
{
    indent();
    printf("<%s>", pFieldDesc->m_pName);
    for(int i = 0; i < 12; i++)
    {
        printf("%02X", Value.m_aValue[i]);
    }
    printf("</%s>\n", pFieldDesc->m_pName);
}

void
CPrXMLEncoderStream::put_utf8v (
  llrp_utf8v_t                  Value,
  const CFieldDescriptor *      pFieldDesc)
{
    indent();
    printf("<%s>", pFieldDesc->m_pName);
    for(int i = 0; i < Value.m_nValue; i++)
    {
        int         c = Value.m_pValue[i];

        if(' ' <= c && c < 0x7F)
        {
            printf("%c", c);
        }
        else
        {
            printf("\\%03o", c);
        }
    }
    printf("</%s>\n", pFieldDesc->m_pName);
}

void
CPrXMLEncoderStream::put_bytesToEnd (
  llrp_bytesToEnd_t             Value,
  const CFieldDescriptor *      pFieldDesc)
{
    indent();
    printf("<%s>", pFieldDesc->m_pName);
    for(int i = 0; i < Value.m_nValue; i++)
    {
        if(0 < i)
        {
            printf(" ");
        }
        printf("%02X", Value.m_pValue[i]);
    }
    printf("</%s>\n", pFieldDesc->m_pName);
}

/*
 * Enumerated types of various sizes
 */

void
CPrXMLEncoderStream::put_e1 (
  int                           eValue,
  const CFieldDescriptor *      pFieldDesc)
{
    put_enum(eValue, pFieldDesc);
}

void
CPrXMLEncoderStream::put_e2 (
  int                           eValue,
  const CFieldDescriptor *      pFieldDesc)
{
    put_enum(eValue, pFieldDesc);
}

void
CPrXMLEncoderStream::put_e8 (
  int                           eValue,
  const CFieldDescriptor *      pFieldDesc)
{
    put_enum(eValue, pFieldDesc);
}

void
CPrXMLEncoderStream::put_e16 (
  int                           eValue,
  const CFieldDescriptor *      pFieldDesc)
{
    put_enum(eValue, pFieldDesc);
}

void
CPrXMLEncoderStream::put_e32 (
  int                           eValue,
  const CFieldDescriptor *      pFieldDesc)
{
    put_enum(eValue, pFieldDesc);
}

/*
 * Reserved types are some number of bits
 */

void
CPrXMLEncoderStream::put_reserved (
  unsigned int                  nBits)
{
    indent();
    printf("<!-- reserved %d bits -->\n", nBits);
}


CPrXMLEncoderStream::CPrXMLEncoderStream (
  CPrXMLEncoder *               pEncoder)
{
    m_pEncoder                  = pEncoder;
    m_pEnclosingEncoderStream   = NULL;
    m_pRefType                  = NULL;
    m_nDepth                    = 1;
}

CPrXMLEncoderStream::CPrXMLEncoderStream (
  CPrXMLEncoderStream *         pEnclosingEncoderStream)
{
    m_pEncoder                  = pEnclosingEncoderStream->m_pEncoder;
    m_pEnclosingEncoderStream   = pEnclosingEncoderStream;
    m_pRefType                  = NULL;
    m_nDepth                    = pEnclosingEncoderStream->m_nDepth+1;
}

void
CPrXMLEncoderStream::putElement (
  const CElement *              pElement)
{
    m_pRefType = pElement->m_pType;

    indent(-1);
    if(m_pRefType->m_bIsMessage)
    {
#if 1
        printf("<%s MessageID='%u'>\n",
            m_pRefType->m_pName,
            ((const CMessage *)pElement)->getMessageID());
#else
        printf("<%s MessageID='%u'\n",
            m_pRefType->m_pName,
            ((const CMessage *)pElement)->getMessageID());
        printf("      xmlns='LLRP'\n");
        printf("      xmlns:xsi='%s'\n",
            "http://www.w3.org/2001/XMLSchema-instance");
        printf("      xsi:schemaLocation='LLRP LLRP.xsd'>\n");
#endif
    }
    else
    {
        printf("<%s>\n", m_pRefType->m_pName);
    }

    pElement->encode(this);

    indent(-1);
    printf("</%s>\n", m_pRefType->m_pName);
}

void
CPrXMLEncoderStream::put_enum (
  int                           eValue,
  const CFieldDescriptor *      pFieldDesc)
{
    const SEnumTableEntry *     pEntry;

    indent();
    printf("<%s>", pFieldDesc->m_pName);

    for(pEntry = pFieldDesc->m_pEnumTable; NULL != pEntry->pName; pEntry++)
    {
        if(pEntry->Value == eValue)
        {
            break;
        }
    }

    if(NULL != pEntry->pName)
    {
        printf("%s", pEntry->pName);
    }
    else
    {
        printf("%d", eValue);
    }

    printf("</%s>\n", pFieldDesc->m_pName);
}

void
CPrXMLEncoderStream::indent (
  int                           adjust)
{
    int                         n = m_nDepth + adjust;

    for(int i = 0; i < n; i++)
    {
        printf("  ");
    }
}

}; /* namespace LLRP */
