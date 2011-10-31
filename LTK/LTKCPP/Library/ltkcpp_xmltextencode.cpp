
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


#include <stdio.h>
#include <stdarg.h>
#include <time.h>

#include "ltkcpp_platform.h"
#include "ltkcpp_base.h"
#include "ltkcpp_xmltext.h"



namespace LLRP
{

/*
 * Structures used by discoverNamespaces() and putElement().
 */
#define MAX_NS      10
struct NamespaceList
{
    const CNamespaceDescriptor *apNamespaceDescriptor[MAX_NS];
    int                         nNamespaceDescriptor;
};
typedef struct NamespaceList    tNamespaceList;

/* forward */
static int
discoverNamespaces (
  const CElement *              pElement,
  void *                        pArg);


CXMLTextEncoder::CXMLTextEncoder (
  char *                        pBuffer,
  int                           nBuffer)
{
    m_pBuffer = pBuffer;
    m_nBuffer = nBuffer;
    m_iNext = 0;
    m_bOverflow = 0;
}

CXMLTextEncoder::~CXMLTextEncoder (void)
{
}

void
CXMLTextEncoder::encodeElement (
  const CElement *              pElement)
{
    CXMLTextEncoderStream         MyEncoderStream(this);

    MyEncoderStream.putElement(pElement);
}

void
CXMLTextEncoderStream::putRequiredSubParameter (
  const CParameter *            pParameter,
  const CTypeDescriptor *       pRefType)
{
    if(NULL == pParameter)
    {
        appendFormat("warning: missing %s\n",
            (NULL == pRefType) ? "<something>" : pRefType->m_pName);
        return;
    }

    CXMLTextEncoderStream         NestEncoderStream(this);

    NestEncoderStream.putElement(pParameter);
}

void
CXMLTextEncoderStream::putOptionalSubParameter (
  const CParameter *            pParameter,
  const CTypeDescriptor *       pRefType)
{
    if(NULL == pParameter)
    {
        return;
    }

    CXMLTextEncoderStream         NestEncoderStream(this);

    NestEncoderStream.putElement(pParameter);
}

void
CXMLTextEncoderStream::putRequiredSubParameterList (
  const tListOfParameters *     pParameterList,
  const CTypeDescriptor *       pRefType)
{
    if(pParameterList->empty())
    {
        appendFormat("warning: missing list of %s\n",
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
CXMLTextEncoderStream::putOptionalSubParameterList (
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
CXMLTextEncoderStream::put_u8 (
  llrp_u8_t                     Value,
  const CFieldDescriptor *      pFieldDescriptor)
{
    const char *                pFieldName = pFieldDescriptor->m_pName;

    appendOpenTag(pFieldName);
    switch(pFieldDescriptor->m_eFieldFormat)
    {
    case CFieldDescriptor::FMT_NORMAL:
    case CFieldDescriptor::FMT_DEC:
    default:
        appendFormat("%u", Value);
        break;

    case CFieldDescriptor::FMT_HEX:
        appendFormat("%02X", Value);
        break;
    }
    appendCloseTag(pFieldName);
}

void
CXMLTextEncoderStream::put_s8 (
  llrp_s8_t                     Value,
  const CFieldDescriptor *      pFieldDescriptor)
{
    const char *                pFieldName = pFieldDescriptor->m_pName;

    appendOpenTag(pFieldName);
    switch(pFieldDescriptor->m_eFieldFormat)
    {
    case CFieldDescriptor::FMT_NORMAL:
    case CFieldDescriptor::FMT_DEC:
    default:
        appendFormat("%d", Value);
        break;

    case CFieldDescriptor::FMT_HEX:
        appendFormat("%02X", 0xFF & Value);
        break;
    }
    appendCloseTag(pFieldName);
}

void
CXMLTextEncoderStream::put_u8v (
  llrp_u8v_t                    Value,
  const CFieldDescriptor *      pFieldDescriptor)
{
    const char *                pFieldName = pFieldDescriptor->m_pName;

    appendOpenTag(pFieldName);
    for(int i = 0; i < Value.m_nValue; i++)
    {
        switch(pFieldDescriptor->m_eFieldFormat)
        {
        case CFieldDescriptor::FMT_NORMAL:
        case CFieldDescriptor::FMT_DEC:
        default:
            if(0 < i)
            {
                appendFormat(" ");
            }
            appendFormat("%u", Value.m_pValue[i]);
            break;

        case CFieldDescriptor::FMT_HEX:
            appendFormat("%02X", 0xFF & Value.m_pValue[i]);
            break;
        }
    }
    appendCloseTag(pFieldName);
}

void
CXMLTextEncoderStream::put_s8v (
  llrp_s8v_t                    Value,
  const CFieldDescriptor *      pFieldDescriptor)
{
    const char *                pFieldName = pFieldDescriptor->m_pName;

    appendOpenTag(pFieldName);
    for(int i = 0; i < Value.m_nValue; i++)
    {
        switch(pFieldDescriptor->m_eFieldFormat)
        {
        case CFieldDescriptor::FMT_NORMAL:
        case CFieldDescriptor::FMT_DEC:
        default:
            if(0 < i)
            {
                appendFormat(" ");
            }
            appendFormat("%d", Value.m_pValue[i]);
            break;

        case CFieldDescriptor::FMT_HEX:
            appendFormat("%02X", 0xFF & Value.m_pValue[i]);
            break;
        }
    }
    appendCloseTag(pFieldName);
}

/*
 * 16-bit types
 */

void
CXMLTextEncoderStream::put_u16 (
  llrp_u16_t                    Value,
  const CFieldDescriptor *      pFieldDescriptor)
{
    const char *                pFieldName = pFieldDescriptor->m_pName;

    appendOpenTag(pFieldName);
    switch(pFieldDescriptor->m_eFieldFormat)
    {
    case CFieldDescriptor::FMT_NORMAL:
    case CFieldDescriptor::FMT_DEC:
    default:
        appendFormat("%u", Value);
        break;

    case CFieldDescriptor::FMT_HEX:
        appendFormat("%04X", Value);
        break;
    }
    appendCloseTag(pFieldName);
}

void
CXMLTextEncoderStream::put_s16 (
  llrp_s16_t                    Value,
  const CFieldDescriptor *      pFieldDescriptor)
{
    const char *                pFieldName = pFieldDescriptor->m_pName;

    appendOpenTag(pFieldName);
    switch(pFieldDescriptor->m_eFieldFormat)
    {
    case CFieldDescriptor::FMT_NORMAL:
    case CFieldDescriptor::FMT_DEC:
    default:
        appendFormat("%d", Value);
        break;

    case CFieldDescriptor::FMT_HEX:
        appendFormat("%04X", 0xFFFF & Value);
        break;
    }
    appendCloseTag(pFieldName);
}

void
CXMLTextEncoderStream::put_u16v (
  llrp_u16v_t                   Value,
  const CFieldDescriptor *      pFieldDescriptor)
{
    const char *                pFieldName = pFieldDescriptor->m_pName;

    appendOpenTag(pFieldName);
    for(int i = 0; i < Value.m_nValue; i++)
    {
        if(0 < i)
        {
            appendFormat(" ");
        }
        switch(pFieldDescriptor->m_eFieldFormat)
        {
        case CFieldDescriptor::FMT_NORMAL:
        case CFieldDescriptor::FMT_DEC:
        default:
            appendFormat("%u", Value.m_pValue[i]);
            break;

        case CFieldDescriptor::FMT_HEX:
            appendFormat("%04X", 0xFFFF & Value.m_pValue[i]);
            break;
        }
    }
    appendCloseTag(pFieldName);
}

void
CXMLTextEncoderStream::put_s16v (
  llrp_s16v_t                   Value,
  const CFieldDescriptor *      pFieldDescriptor)
{
    const char *                pFieldName = pFieldDescriptor->m_pName;

    appendOpenTag(pFieldName);
    for(int i = 0; i < Value.m_nValue; i++)
    {
        if(0 < i)
        {
            appendFormat(" ");
        }
        switch(pFieldDescriptor->m_eFieldFormat)
        {
        case CFieldDescriptor::FMT_NORMAL:
        case CFieldDescriptor::FMT_DEC:
        default:
            appendFormat("%d", Value.m_pValue[i]);
            break;

        case CFieldDescriptor::FMT_HEX:
            appendFormat("%04X", 0xFFFF & Value.m_pValue[i]);
            break;
        }
    }
    appendCloseTag(pFieldName);
}

/*
 * 32-bit types
 */

void
CXMLTextEncoderStream::put_u32 (
  llrp_u32_t                    Value,
  const CFieldDescriptor *      pFieldDescriptor)
{
    const char *                pFieldName = pFieldDescriptor->m_pName;

    appendOpenTag(pFieldName);
    switch(pFieldDescriptor->m_eFieldFormat)
    {
    case CFieldDescriptor::FMT_NORMAL:
    case CFieldDescriptor::FMT_DEC:
    default:
        appendFormat("%u", Value);
        break;

    case CFieldDescriptor::FMT_HEX:
        appendFormat("%08X", Value);
        break;
    }
    appendCloseTag(pFieldName);
}

void
CXMLTextEncoderStream::put_s32 (
  llrp_s32_t                    Value,
  const CFieldDescriptor *      pFieldDescriptor)
{
    const char *                pFieldName = pFieldDescriptor->m_pName;

    appendOpenTag(pFieldName);
    switch(pFieldDescriptor->m_eFieldFormat)
    {
    case CFieldDescriptor::FMT_NORMAL:
    case CFieldDescriptor::FMT_DEC:
    default:
        appendFormat("%d", Value);
        break;

    case CFieldDescriptor::FMT_HEX:
        appendFormat("%08X", Value);
        break;
    }
    appendCloseTag(pFieldName);
}

void
CXMLTextEncoderStream::put_u32v (
  llrp_u32v_t                   Value,
  const CFieldDescriptor *      pFieldDescriptor)
{
    const char *                pFieldName = pFieldDescriptor->m_pName;

    appendOpenTag(pFieldName);
    for(int i = 0; i < Value.m_nValue; i++)
    {
        if(0 < i)
        {
            appendFormat(" ");
        }
        switch(pFieldDescriptor->m_eFieldFormat)
        {
        case CFieldDescriptor::FMT_NORMAL:
        case CFieldDescriptor::FMT_DEC:
        default:
            appendFormat("%u", Value.m_pValue[i]);
            break;

        case CFieldDescriptor::FMT_HEX:
            appendFormat("%08X", Value.m_pValue[i]);
            break;
        }
    }
    appendCloseTag(pFieldName);
}

void
CXMLTextEncoderStream::put_s32v (
  llrp_s32v_t                   Value,
  const CFieldDescriptor *      pFieldDescriptor)
{
    const char *                pFieldName = pFieldDescriptor->m_pName;

    appendOpenTag(pFieldName);
    for(int i = 0; i < Value.m_nValue; i++)
    {
        if(0 < i)
        {
            appendFormat(" ");
        }
        switch(pFieldDescriptor->m_eFieldFormat)
        {
        case CFieldDescriptor::FMT_NORMAL:
        case CFieldDescriptor::FMT_DEC:
        default:
            appendFormat("%d", Value.m_pValue[i]);
            break;

        case CFieldDescriptor::FMT_HEX:
            appendFormat("%08X", Value.m_pValue[i]);
            break;
        }
    }
    appendCloseTag(pFieldName);
}

/*
 * 64-bit types
 */

void
CXMLTextEncoderStream::put_u64 (
  llrp_u64_t                    Value,
  const CFieldDescriptor *      pFieldDescriptor)
{
    const char *                pFieldName = pFieldDescriptor->m_pName;

    appendOpenTag(pFieldName);
    switch(pFieldDescriptor->m_eFieldFormat)
    {
    case CFieldDescriptor::FMT_NORMAL:
    case CFieldDescriptor::FMT_DEC:
    default:
#ifdef WIN32
        appendFormat("%I64u", Value);
#else
        appendFormat("%llu", Value);
#endif
        break;

    case CFieldDescriptor::FMT_HEX:
#ifdef WIN32
        appendFormat("%016I64X", Value);
#else
        appendFormat("%016llX", Value);
#endif
        break;

    case CFieldDescriptor::FMT_DATETIME:
        {
            char                aBuf[64];
            time_t              CurSec  = (time_t)(Value / 1000000u);
            llrp_u32_t          CurUSec = (llrp_u32_t)(Value % 1000000u);
            struct tm *         pGMTime;

            pGMTime = gmtime(&CurSec);
            strftime(aBuf, sizeof aBuf, "%Y-%m-%dT%H:%M:%S", pGMTime);
            appendFormat("%s.%06dZ", aBuf, CurUSec);
        }
        break;
    }
    appendCloseTag(pFieldName);
}

void
CXMLTextEncoderStream::put_s64 (
  llrp_s64_t                    Value,
  const CFieldDescriptor *      pFieldDescriptor)
{
    const char *                pFieldName = pFieldDescriptor->m_pName;

    appendOpenTag(pFieldName);
    switch(pFieldDescriptor->m_eFieldFormat)
    {
    case CFieldDescriptor::FMT_NORMAL:
    case CFieldDescriptor::FMT_DEC:
    default:
#ifdef WIN32
        appendFormat("%I64d", Value);
#else
        appendFormat("%lld", Value);
#endif
        break;

    case CFieldDescriptor::FMT_HEX:
#ifdef WIN32
        appendFormat("%016I64X", Value);
#else
        appendFormat("%016llX", Value);
#endif
        break;
    }
    appendCloseTag(pFieldName);
}

void
CXMLTextEncoderStream::put_u64v (
  llrp_u64v_t                   Value,
  const CFieldDescriptor *      pFieldDescriptor)
{
    const char *                pFieldName = pFieldDescriptor->m_pName;

    appendOpenTag(pFieldName);
    for(int i = 0; i < Value.m_nValue; i++)
    {
        if(0 < i)
        {
            appendFormat(" ");
        }
        switch(pFieldDescriptor->m_eFieldFormat)
        {
        case CFieldDescriptor::FMT_NORMAL:
        case CFieldDescriptor::FMT_DEC:
        default:
#ifdef WIN32
            appendFormat("%I64u", Value.m_pValue[i]);
#else
            appendFormat("%llu", Value.m_pValue[i]);
#endif
            break;

        case CFieldDescriptor::FMT_HEX:
#ifdef WIN32
            appendFormat("%016I64X", Value.m_pValue[i]);
#else
            appendFormat("%016llX", Value.m_pValue[i]);
#endif
            break;
        }
    }
    appendCloseTag(pFieldName);
}

void
CXMLTextEncoderStream::put_s64v (
  llrp_s64v_t                   Value,
  const CFieldDescriptor *      pFieldDescriptor)
{
    const char *                pFieldName = pFieldDescriptor->m_pName;

    appendOpenTag(pFieldName);
    for(int i = 0; i < Value.m_nValue; i++)
    {
        if(0 < i)
        {
            appendFormat(" ");
        }
        switch(pFieldDescriptor->m_eFieldFormat)
        {
        case CFieldDescriptor::FMT_NORMAL:
        case CFieldDescriptor::FMT_DEC:
        default:
#ifdef WIN32
            appendFormat("%I64d", Value.m_pValue[i]);
#else
            appendFormat("%lld", Value.m_pValue[i]);
#endif
            break;

        case CFieldDescriptor::FMT_HEX:
#ifdef WIN32
            appendFormat("%016I64X", Value.m_pValue[i]);
#else
            appendFormat("%016llX", Value.m_pValue[i]);
#endif
            break;
        }
    }
    appendCloseTag(pFieldName);
}

/*
 * Special types
 */

void
CXMLTextEncoderStream::put_u1 (
  llrp_u1_t                     Value,
  const CFieldDescriptor *      pFieldDescriptor)
{
    const char *                pFieldName = pFieldDescriptor->m_pName;

    appendOpenTag(pFieldName);
    switch(pFieldDescriptor->m_eFieldFormat)
    {
    case CFieldDescriptor::FMT_NORMAL:
    default:
        appendFormat("%s", (Value & 1) ? "true" : "false");
        break;

    case CFieldDescriptor::FMT_DEC:
    case CFieldDescriptor::FMT_HEX:
        appendFormat("%d", Value & 1);
        break;
    }
    appendCloseTag(pFieldName);
}

void
CXMLTextEncoderStream::put_u1v (
  llrp_u1v_t                    Value,
  const CFieldDescriptor *      pFieldDescriptor)
{
    const char *                pFieldName = pFieldDescriptor->m_pName;
    int                         nByte;

    nByte = (Value.m_nBit + 7u) / 8u;

    indent();
    appendFormat("<");
    appendPrefixedTagName(pFieldName);
    appendFormat(" Count='%d'>", Value.m_nBit);

    for(int i = 0; i < nByte; i++)
    {
        appendFormat("%02X", Value.m_pValue[i]);
    }

    appendCloseTag(pFieldName);
}

void
CXMLTextEncoderStream::put_u2 (
  llrp_u2_t                     Value,
  const CFieldDescriptor *      pFieldDescriptor)
{
    const char *                pFieldName = pFieldDescriptor->m_pName;

    appendOpenTag(pFieldName);
    appendFormat("%d", Value & 3);
    appendCloseTag(pFieldName);
}

void
CXMLTextEncoderStream::put_u96 (
  llrp_u96_t                    Value,
  const CFieldDescriptor *      pFieldDescriptor)
{
    const char *                pFieldName = pFieldDescriptor->m_pName;

    appendOpenTag(pFieldName);
    for(int i = 0; i < 12; i++)
    {
        appendFormat("%02X", Value.m_aValue[i]);
    }
    appendCloseTag(pFieldName);
}

void
CXMLTextEncoderStream::put_utf8v (
  llrp_utf8v_t                  Value,
  const CFieldDescriptor *      pFieldDescriptor)
{
    const char *                pFieldName = pFieldDescriptor->m_pName;

    appendOpenTag(pFieldName);
    for(int i = 0; i < Value.m_nValue; i++)
    {
        int         c = Value.m_pValue[i];

        if(0 == c && i+1 == Value.m_nValue)
        {
            continue;
        }
        if(' ' <= c && c < 0x7F)
        {
            appendFormat("%c", c);
        }
        else
        {
            appendFormat("\\%03o", c);
        }
    }
    appendCloseTag(pFieldName);
}

void
CXMLTextEncoderStream::put_bytesToEnd (
  llrp_bytesToEnd_t             Value,
  const CFieldDescriptor *      pFieldDescriptor)
{
    const char *                pFieldName = pFieldDescriptor->m_pName;

    appendOpenTag(pFieldName);
    for(int i = 0; i < Value.m_nValue; i++)
    {
        appendFormat("%02X", Value.m_pValue[i]);
    }
    appendCloseTag(pFieldName);
}

/*
 * Enumerated types of various sizes
 */

void
CXMLTextEncoderStream::put_e1 (
  int                           eValue,
  const CFieldDescriptor *      pFieldDescriptor)
{
    put_enum(eValue, pFieldDescriptor);
}

void
CXMLTextEncoderStream::put_e2 (
  int                           eValue,
  const CFieldDescriptor *      pFieldDescriptor)
{
    put_enum(eValue, pFieldDescriptor);
}

void
CXMLTextEncoderStream::put_e8 (
  int                           eValue,
  const CFieldDescriptor *      pFieldDescriptor)
{
    put_enum(eValue, pFieldDescriptor);
}

void
CXMLTextEncoderStream::put_e16 (
  int                           eValue,
  const CFieldDescriptor *      pFieldDescriptor)
{
    put_enum(eValue, pFieldDescriptor);
}

void
CXMLTextEncoderStream::put_e32 (
  int                           eValue,
  const CFieldDescriptor *      pFieldDescriptor)
{
    put_enum(eValue, pFieldDescriptor);
}

void
CXMLTextEncoderStream::put_e8v (
  llrp_u8v_t                    Value,
  const CFieldDescriptor *      pFieldDescriptor)
{
    const char *                pFieldName = pFieldDescriptor->m_pName;

    appendOpenTag(pFieldName);
    for(int i = 0; i < Value.m_nValue; i++)
    {
        int                     eValue = Value.m_pValue[i];
        const SEnumTableEntry * pEntry;

        for(pEntry = pFieldDescriptor->m_pEnumTable;
            NULL != pEntry->pName;
            pEntry++)
        {
            if(pEntry->Value == eValue)
            {
                break;
            }
        }

        if(0 < i)
        {
            appendFormat(" ");
        }

        if(NULL != pEntry->pName)
        {
            appendFormat("%s", pEntry->pName);
        }
        else
        {
            appendFormat("%d", eValue);
        }
    }
    appendCloseTag(pFieldName);
}

/*
 * Reserved types are some number of bits
 */

void
CXMLTextEncoderStream::put_reserved (
  unsigned int                  nBits)
{
    indent();
    appendFormat("<!-- reserved %d bits -->\n", nBits);
}


CXMLTextEncoderStream::CXMLTextEncoderStream (
  CXMLTextEncoder *               pEncoder)
{
    m_pEncoder                  = pEncoder;
    m_pEnclosingEncoderStream   = NULL;
    m_pRefType                  = NULL;
    m_nDepth                    = 1;
}

CXMLTextEncoderStream::CXMLTextEncoderStream (
  CXMLTextEncoderStream *         pEnclosingEncoderStream)
{
    m_pEncoder                  = pEnclosingEncoderStream->m_pEncoder;
    m_pEnclosingEncoderStream   = pEnclosingEncoderStream;
    m_pRefType                  = NULL;
    m_nDepth                    = pEnclosingEncoderStream->m_nDepth+1;
}

void
CXMLTextEncoderStream::putElement (
  const CElement *              pElement)
{
    m_pRefType = pElement->m_pType;

    indent(-1);
    appendFormat("<");
    appendPrefixedTagName(m_pRefType->m_pName);
    if(m_pRefType->m_bIsMessage)
    {
        appendFormat(" MessageID='%u'",
            ((const CMessage *)pElement)->getMessageID());
    }

    if(NULL == m_pEnclosingEncoderStream)
    {
        tNamespaceList          NamespaceList;
        const CNamespaceDescriptor *pNamespaceDescriptor;
        int                     iNSD;

        memset(&NamespaceList, 0, sizeof NamespaceList);

        pElement->walk(discoverNamespaces, (void*)&NamespaceList,
            0, 12);

        /* Emit the namespace cookie for each */
        for(iNSD = 0; iNSD < NamespaceList.nNamespaceDescriptor; iNSD++)
        {
            pNamespaceDescriptor = NamespaceList.apNamespaceDescriptor[iNSD];

            appendFormat("\n");
            indent(0);
            appendFormat("xmlns:%s='%s'",
                pNamespaceDescriptor->m_pPrefix,
                pNamespaceDescriptor->m_pURI);
            /*
             * If this is the default namespace then emit the assigment.
             */
            if(0 == strcmp(pNamespaceDescriptor->m_pPrefix, "llrp"))
            {
                appendFormat("\n");
                indent(0);
                appendFormat("xmlns='%s'", pNamespaceDescriptor->m_pURI);
            }
        }
    }
    appendFormat(">\n");

    pElement->encode(this);

    indent(-1);
    appendCloseTag(m_pRefType->m_pName);
}

static int
discoverNamespaces (
  const CElement *              pElement,
  void *                        pArg)
{
    tNamespaceList *            pNSL = (tNamespaceList *) pArg;
    const CNamespaceDescriptor *pNamespaceDescriptor;
    int                         iNSD;

    pNamespaceDescriptor = pElement->m_pType->m_pNamespaceDescriptor;

    for(iNSD = 0; iNSD < pNSL->nNamespaceDescriptor; iNSD++)
    {
        if(pNSL->apNamespaceDescriptor[iNSD] == pNamespaceDescriptor)
        {
            /* Already have it */
            return 0;
        }
    }

    /* if we get here this namespace isn't already in the list */
    if(MAX_NS > pNSL->nNamespaceDescriptor)
    {
        iNSD = pNSL->nNamespaceDescriptor++;
        pNSL->apNamespaceDescriptor[iNSD] = pNamespaceDescriptor;
    }

    return 0;
}


void
CXMLTextEncoderStream::put_enum (
  int                           eValue,
  const CFieldDescriptor *      pFieldDescriptor)
{
    const char *                pFieldName = pFieldDescriptor->m_pName;
    const SEnumTableEntry *     pEntry;

    appendOpenTag(pFieldName);

    for(pEntry = pFieldDescriptor->m_pEnumTable;
        NULL != pEntry->pName;
        pEntry++)
    {
        if(pEntry->Value == eValue)
        {
            break;
        }
    }

    if(NULL != pEntry->pName)
    {
        appendFormat("%s", pEntry->pName);
    }
    else
    {
        appendFormat("%d", eValue);
    }

    appendCloseTag(pFieldName);
}

void
CXMLTextEncoderStream::indent (
  int                           adjust)
{
    int                         n = m_nDepth + adjust;

    for(int i = 0; i < n; i++)
    {
        appendFormat("  ");
    }
}

void
CXMLTextEncoderStream::appendOpenTag (
  const char *                  pName)
{
    indent(0);
    appendFormat("<");
    appendPrefixedTagName(pName);
    appendFormat(">");
}

void
CXMLTextEncoderStream::appendCloseTag (
  const char *                  pName)
{
    appendFormat("</");
    appendPrefixedTagName(pName);
    appendFormat(">\n");
}

void
CXMLTextEncoderStream::appendPrefixedTagName (
  const char *                  pName)
{
    const CTypeDescriptor *     pRefType = m_pRefType;
    const char *                pPrefix =
                                  pRefType->m_pNamespaceDescriptor->m_pPrefix;

    if(0 != strcmp("llrp", pPrefix))
    {
        appendFormat("%s:%s", pPrefix, pName);
    }
    else
    {
        appendFormat("%s", pName);
    }
}

void
CXMLTextEncoderStream::appendFormat (
  const char *                        pFmtStr,
                                ...)
{
    char                        aHoldBuf[256u];
    int                         nHoldBuf;
    va_list                     ap;

    /* If overflow already detected, bail */
    if(m_pEncoder->m_bOverflow)
    {
        return;
    }

    va_start(ap, pFmtStr);
#ifdef WIN32
    _vsnprintf(aHoldBuf, sizeof aHoldBuf, pFmtStr, ap);
#else
    vsnprintf(aHoldBuf, sizeof aHoldBuf, pFmtStr, ap);
#endif
    va_end(ap);

    nHoldBuf = (int)strlen(aHoldBuf);

    if(m_pEncoder->m_iNext + nHoldBuf >= m_pEncoder->m_nBuffer)
    {
        m_pEncoder->m_bOverflow = 1;
        return;
    }

    strcpy(&m_pEncoder->m_pBuffer[m_pEncoder->m_iNext], aHoldBuf);

    m_pEncoder->m_iNext += nHoldBuf;
}


/**
 *****************************************************************************
 **
 ** @brief  Format an element as XML text
 **
 ** Basic steps
 **     - Construct an XML encoder that fills a buffer
 **     - Encode the message through the XML encoder
 **     - Destruct the XML encoder
 **
 ** @param[in]  pElement        Pointer to message/parameter to format
 **
 ** @return     EResultCode
 **
 *****************************************************************************/

EResultCode
CElement::toXMLString (
  char *                        pBuffer,
  int                           nBuffer)
{
    return LLRP::toXMLString(this, pBuffer, nBuffer);
}

EResultCode
toXMLString (
  const CElement *              pElement,
  char *                        pBuffer,
  int                           nBuffer)
{
    CXMLTextEncoder *           pXMLEncoder;
    const CErrorDetails *       pError;

    /*
     * Make sure the element is not NULL.
     */
    if(NULL == pElement)
    {
        strcpy(pBuffer, "ERROR: NULL pMessage to printXMLMessage\n");
        return RC_MiscError;
    }

    /*
     * Construct an XML encoder
     */
    pXMLEncoder = new CXMLTextEncoder(pBuffer, nBuffer);
    if(NULL == pXMLEncoder)
    {
        printf("ERROR: XMLTextEncoder_construct failed\n");
        return RC_MiscError;
    }

    /*
     * Now let the encoding mechanism do its thing.
     */
    pXMLEncoder->encodeElement(pElement);

    /*
     * Check the outcome in the error details.
     * If there is a problem, return the error rather
     * than the assumed to be useless string.
     */
    pError = &pXMLEncoder->m_ErrorDetails;

    if(RC_OK != pError->m_eResultCode)
    {
        sprintf(pBuffer, "ERROR: %s XML text failed, %s\n",
            pElement->m_pType->m_pName,
            pError->m_pWhatStr ? pError->m_pWhatStr : "no reason given");

        delete pXMLEncoder;

        return pError->m_eResultCode;
    }

    /*
     * Check if the XML fit in the buffer.
     */
    if(pXMLEncoder->m_bOverflow)
    {
        strcpy(pBuffer, "ERROR: Buffer overflow\n");
        delete pXMLEncoder;
        return RC_MiscError;
    }

    /*
     * Done with the XML encoder.
     */
    delete pXMLEncoder;

    return RC_OK;
}


}; /* namespace LLRP */
