
/*
 ***************************************************************************
 *  Copyright 2007,2010 Impinj, Inc.
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
 ** @file  ltkcpp_xmltextdecode.cpp
 **
 ** @brief Classes to decode LTK-XML 
 **
 ** These classes implement the basic Decoder classes
 ** to convert LTKCPP objects to and from LTK_XML syntax
 **
 *****************************************************************************/

#include <assert.h>
#include <ctype.h>
#include <string.h>
#include <time.h>
#include <stdio.h>

#include "ltkcpp_platform.h"
#include "ltkcpp_base.h"

#include "libxml/parser.h"
#include "libxml/tree.h"

#include "ltkcpp_xmltext.h"


namespace LLRP
{

CXMLTextDecoder::CXMLTextDecoder (
  const CTypeRegistry *         pTypeRegistry,
  char *                        pBuffer,
  int                           nBuffer) : CDecoder(pTypeRegistry)

{
    /* set the line numbers for error reporting */
    xmlLineNumbersDefault(1);

    /* Read in the XML from the buffer into the XML Reader */
    m_pDoc = xmlReadMemory((char*) pBuffer, nBuffer, "noName.xml", NULL, 
                                  XML_PARSE_COMPACT | XML_PARSE_NONET);

    if(NULL != m_pDoc)
    {
        /* Initialize the other decoder state variables */
        m_pxmlNodeTree = xmlDocGetRootElement(m_pDoc);
    }
    else
    {
        fprintf(stderr, "could not parse XML memory buffer");
    }
}

CXMLTextDecoder::CXMLTextDecoder (
  const CTypeRegistry *         pTypeRegistry,
  struct _xmlNode *             pNodeTree) : CDecoder(pTypeRegistry)
{
    /* set the line numbers for error reporting */
    xmlLineNumbersDefault(1);

    /* Read in the XML from the buffer into the XML Reader */
    m_pDoc = NULL; 
    /* Initialize the other decoder state variables */
    m_pxmlNodeTree = pNodeTree;
}

CXMLTextDecoder::CXMLTextDecoder (
  const CTypeRegistry *         pTypeRegistry,
  char *                        fname) : CDecoder(pTypeRegistry)
{
    /* set the line numbers for error reporting */
    xmlLineNumbersDefault(1);

    /* Read in the XML from the file into the XML Reader */
    m_pDoc = xmlReadFile(fname, 
                         NULL, 
                         XML_PARSE_COMPACT | XML_PARSE_NONET);

    if(NULL != m_pDoc)
    {
        /* Initialize the other decoder state variables */
        m_pxmlNodeTree = xmlDocGetRootElement(m_pDoc);
    }
    else
    {
        fprintf(stderr, "could not parse XML file");
    }
}

CXMLTextDecoder::~CXMLTextDecoder (void)
{
    if(NULL != m_pDoc)
    {
        xmlFreeDoc(m_pDoc);    
        m_pDoc = NULL;
    }
    m_pxmlNodeTree = NULL;
}

CMessage *
CXMLTextDecoder::decodeMessage (void)
{
    CMessage *                  pMessage;
    CXMLTextDecoderStream       DecoderStream(this);
    CErrorDetails              *pError = &m_ErrorDetails;
  
    if(NULL == m_pxmlNodeTree)
    {
        pError->m_eResultCode = RC_MiscError;
        pError->m_pWhatStr    = "Unable to Extract XML Node Tree";
        pError->m_pRefType    = NULL;
        pError->m_pRefField   = NULL;
        pError->m_OtherDetail = (int) 0;
        return NULL;
    }
    pMessage = (CMessage *) DecoderStream.decodeElement(TRUE, TRUE);

    return pMessage;
}

CXMLTextDecoderStream::CXMLTextDecoderStream (
  CXMLTextDecoder *             pDecoder)
{

    m_pDecoder                = pDecoder;
    m_pEnclosingDecoderStream = NULL;
    m_pRefType                = NULL;
    m_pTargetNode             = pDecoder->m_pxmlNodeTree;
    m_pCurrentChildNode       = NULL;
}

CXMLTextDecoderStream::CXMLTextDecoderStream (
  CXMLTextDecoderStream *       pEnclosingDecoderStream)
{
    m_pDecoder                = pEnclosingDecoderStream->m_pDecoder;
    m_pEnclosingDecoderStream = pEnclosingDecoderStream;
    m_pRefType                = pEnclosingDecoderStream->m_pRefType;
    m_pTargetNode             = pEnclosingDecoderStream->m_pCurrentChildNode; 
    m_pCurrentChildNode       = NULL;
}

CElement *
CXMLTextDecoderStream::decodeElement (
  int                           bAllowMessage,
  int                           bSkipBlanks)
{
    CErrorDetails              *pError = &m_pDecoder->m_ErrorDetails;
    const CTypeDescriptor*      pTypeDescriptor;
    llrp_u32_t                  MessageID = 0;
    CElement *                  pElement;

    if(RC_OK != pError->m_eResultCode)
    {
        return NULL;
    }

    /* get the first interesting node as the target node */
    if(bSkipBlanks)
    {
        while ((NULL != m_pTargetNode) &&
               ( isInsignificantNode(m_pTargetNode) || 
                 xmlIsBlankNode(m_pTargetNode)))
        {
            m_pTargetNode = m_pTargetNode->next;
        }
    }

    /* no interestig target node found. THis is OK as its an empty decode */
    if(NULL == m_pTargetNode)
    {
        return NULL;
    }

    /* first interesting node had better be an element */
    if(m_pTargetNode->type != XML_ELEMENT_NODE)
    {
        pError->m_eResultCode = RC_XMLInvalidNodeType;
        pError->m_pWhatStr    = "unexpected XML node type";
        pError->m_pRefType    = m_pRefType;
        pError->m_pRefField   = NULL;
        pError->m_OtherDetail = (int) m_pTargetNode->line;
        return NULL;
    }

    /* look up the type descriptor*/
    pTypeDescriptor = m_pDecoder->m_pRegistry->lookupByName(
                                           (char*) m_pTargetNode->name);

    if(NULL == pTypeDescriptor)
    {
        pError->m_eResultCode = RC_UnknownParameterType;
        pError->m_pWhatStr    = "unknown message or parameter type";
        pError->m_pRefType    = m_pRefType;
        pError->m_pRefField   = NULL;
        pError->m_OtherDetail = (int) m_pTargetNode->line;
        return NULL;
    }

    m_pRefType = pTypeDescriptor;

    /* process messages a bit specially */
    if(pTypeDescriptor->m_bIsMessage)
    {
        xmlChar *               pMessageIDStr;

        if(!bAllowMessage)
        {
            pError->m_eResultCode = RC_MiscError;
            pError->m_pWhatStr    = "message as subparameter";
            pError->m_pRefType    = m_pRefType;
            pError->m_pRefField   = NULL;
            pError->m_OtherDetail = (int) m_pTargetNode->line;
            return NULL;
        }

        /* get the messageID Attribute */
        pMessageIDStr = xmlGetProp(m_pTargetNode, 
                                   (xmlChar*) "MessageID");
        if(NULL != pMessageIDStr)
        {
            char *                  pArg = (char *) pMessageIDStr;
            char *                  pTail = pArg + strlen((char*) pMessageIDStr);
            char *                  pTmp;
	        cleanString((const llrp_u8_t **) &pArg, (const llrp_u8_t **) &pTail);
            MessageID = strtoul(pArg, &pTmp, 10);
            xmlFree(pMessageIDStr);

            if(pTail != pTmp)
            {
                /* malformed */
                pError->m_eResultCode = RC_MiscError;
                pError->m_pWhatStr    = "malformed MessageID";
                pError->m_pRefType    = m_pRefType;
                pError->m_pRefField   = NULL;
                pError->m_OtherDetail = (int) m_pTargetNode->line;
                return NULL;
            }
        }
    }

    /* create our element to hold the information */
    pElement = pTypeDescriptor->constructElement();

    if(NULL == pElement)
    {
        pError->m_eResultCode = RC_MessageAllocationFailed;
        pError->m_pWhatStr    = "element allocation failed";
        pError->m_pRefType    = m_pRefType;
        pError->m_pRefField   = NULL;
        pError->m_OtherDetail = (int) m_pTargetNode->line;
        return NULL;
    }

    /* only write the message ID for messages */
    if(pTypeDescriptor->m_bIsMessage)
    {
        ((CMessage *) pElement)->setMessageID(MessageID);
    }

    /* now switch to decode the children which are the
     * fields and sub-parameters of this element */
    m_pCurrentChildNode = m_pTargetNode->children;

    /* decode the fields first */
    pTypeDescriptor->m_pfDecodeFields(this, pElement);

    if(RC_OK != pError->m_eResultCode)
    {
        delete pElement;
        return NULL;
    }

    /*
     * Subparameters.
     *
     * loop through all the remaining children of the target node and try to find sub-parameters.
     * We are done when we don't have any more or error
     */
    for(; m_pCurrentChildNode != NULL; 
            m_pCurrentChildNode = m_pCurrentChildNode->next)
    {
        CElement *              pSubElement;
        CParameter *            pParameter;

        if(isInsignificantNode(m_pCurrentChildNode))
        {
                continue;
        }

        if(xmlIsBlankNode(m_pCurrentChildNode))
        {
                continue;
        }

        CXMLTextDecoderStream   NestStream(this);

        pSubElement = NestStream.decodeElement(FALSE, FALSE);

        /* nothing more to decode as a sub-element or failure */
        if(NULL == pSubElement)
        {
            break;
        }

        pParameter = (CParameter *) pSubElement;

        pParameter->m_pParent = pElement;
        pElement->addSubParameterToAllList(pParameter);
    }

    if(RC_OK != pError->m_eResultCode)
    {
        delete pElement;
        return NULL;
    }

    pElement->assimilateSubParameters(pError);

    if(RC_OK != pError->m_eResultCode)
    {
        delete pElement;
        return NULL;
    }

    return pElement;
}

/** @brief Decodes an llrp_u8_t (unsigned 8-bit number) into the specified field descriptor */
llrp_u8_t
CXMLTextDecoderStream::get_u8 (
  const CFieldDescriptor *      pFieldDesc)
{
    llrp_u8_t Value;
    Value = (llrp_u8_t) getIntegerField(pFieldDesc, MIN_U8, MAX_U8);
    return Value;
}

/** @brief Decodes a llrp_s8_t (signed 8-bit number) into the specified field descriptor */
llrp_s8_t
CXMLTextDecoderStream::get_s8 (
  const CFieldDescriptor *      pFieldDesc)
{
    llrp_s8_t Value;
    Value = (llrp_s8_t) getIntegerField(pFieldDesc, MIN_S8, MAX_S8);
    return Value;
}

/** @brief Decodes an llrp_u8v_t into the specified field descriptor */
llrp_u8v_t
CXMLTextDecoderStream::get_u8v (
  const CFieldDescriptor *      pFieldDesc)
{
    CErrorDetails *             pError;
    llrp_u8v_t                  Value;
    llrp_s64v_t                 Tmp;

    pError = &m_pDecoder->m_ErrorDetails;

    switch(pFieldDesc->m_eFieldFormat)
    {
        /* assume normal format here is decimal */
        case CFieldDescriptor::FMT_NORMAL:
        case CFieldDescriptor::FMT_DEC:
        Tmp = getSpacedVectorField(pFieldDesc, MIN_U8, MAX_U8);
        break;
        case CFieldDescriptor::FMT_HEX:
        Tmp = getFixedVectorField(pFieldDesc, 2, MIN_U8, MAX_U8);	    
        break;
        default:
        case CFieldDescriptor::FMT_UTF8:
        case CFieldDescriptor::FMT_DATETIME:
            if(NULL != pError)
            {
                pError->m_eResultCode = RC_MiscError;
                pError->m_pWhatStr    = "Format type not support for field";
                pError->m_pRefType    = m_pRefType;
                pError->m_pRefField   = pFieldDesc;
                pError->m_OtherDetail = (int) m_pCurrentChildNode->line;
            }	    
        break;
    }    

    if(Tmp.m_nValue)
    {
        llrp_u16_t              nValue;

        nValue = Tmp.m_nValue;
        Value = llrp_u8v_t(nValue);
        if(verifyVectorAllocation(Value.m_pValue, pFieldDesc))
        {
            unsigned int    Ix;

            for(Ix = 0; Ix < nValue; Ix++)
            {
                Value.m_pValue[Ix] = (llrp_u8_t) Tmp.m_pValue[Ix];
            }
        }
    }

    return Value;
}

/** @brief Decodes an llrp_s8v_t into the specified field descriptor */
llrp_s8v_t
CXMLTextDecoderStream::get_s8v (
  const CFieldDescriptor *      pFieldDesc)
{
    CErrorDetails *             pError;
    llrp_s8v_t                  Value;
    llrp_s64v_t                 Tmp;

    pError = &m_pDecoder->m_ErrorDetails;

    switch(pFieldDesc->m_eFieldFormat)
    {
        /* assume normal format here is decimal */
        case CFieldDescriptor::FMT_NORMAL:
        case CFieldDescriptor::FMT_DEC:
        Tmp = getSpacedVectorField(pFieldDesc, MIN_S8, MAX_S8);
        break;
        case CFieldDescriptor::FMT_HEX:
        Tmp = getFixedVectorField(pFieldDesc, 2, MIN_S8, MAX_S8);	    
        break;
        default:
        case CFieldDescriptor::FMT_UTF8:
        case CFieldDescriptor::FMT_DATETIME:
            if(NULL != pError)
            {
                pError->m_eResultCode = RC_MiscError;
                pError->m_pWhatStr    = "Format type not support for field";
                pError->m_pRefType    = m_pRefType;
                pError->m_pRefField   = pFieldDesc;
                pError->m_OtherDetail = (int) m_pCurrentChildNode->line;
            }	    
        break;
    }    

    if(Tmp.m_nValue)
    {
        llrp_u16_t              nValue;

        nValue = Tmp.m_nValue;
        Value = llrp_s8v_t(nValue);
        if(verifyVectorAllocation(Value.m_pValue, pFieldDesc))
        {
            unsigned int    Ix;

            for(Ix = 0; Ix < nValue; Ix++)
            {
                Value.m_pValue[Ix] = (llrp_s8_t) Tmp.m_pValue[Ix];
            }
        }
    }

    return Value;
}

/*
 * 16-bit types
 */

/** @brief Decodes an llrp_u16_t (unsigned 16-bit number) into the specified field descriptor */
llrp_u16_t
CXMLTextDecoderStream::get_u16 (
  const CFieldDescriptor *      pFieldDesc)
{
    llrp_u16_t Value;
    Value = (llrp_u16_t) getIntegerField(pFieldDesc, MIN_U16, MAX_U16);
    return Value;
}

/** @brief Decodes a llrp_s16_t (signed 16-bit number) into the specified field descriptor */
llrp_s16_t
CXMLTextDecoderStream::get_s16 (
  const CFieldDescriptor *      pFieldDesc)
{
    llrp_s16_t Value;
    Value = (llrp_s16_t) getIntegerField(pFieldDesc, MIN_S16, MAX_S16);
    return Value;
}

/** @brief Decodes a llrp_u16v_t into the specified field descriptor */
llrp_u16v_t
CXMLTextDecoderStream::get_u16v (
  const CFieldDescriptor *      pFieldDesc)
{
    CErrorDetails *             pError;
    llrp_u16v_t                  Value;
    llrp_s64v_t                 Tmp;

    pError = &m_pDecoder->m_ErrorDetails;

    switch(pFieldDesc->m_eFieldFormat)
    {
        /* assume normal format here is decimal */
        case CFieldDescriptor::FMT_NORMAL:
        case CFieldDescriptor::FMT_DEC:
        case CFieldDescriptor::FMT_HEX:
            Tmp = getSpacedVectorField(pFieldDesc, MIN_U16, MAX_U16);
            break;
        default:
        case CFieldDescriptor::FMT_UTF8:
        case CFieldDescriptor::FMT_DATETIME:
            if(NULL != pError)
            {
                pError->m_eResultCode = RC_MiscError;
                pError->m_pWhatStr    = "Format type not support for field";
                pError->m_pRefType    = m_pRefType;
                pError->m_pRefField   = pFieldDesc;
                pError->m_OtherDetail = (int) m_pCurrentChildNode->line;
            }	    
        break;
    }    

    if(Tmp.m_nValue)
    {
        llrp_u16_t              nValue;

        nValue = Tmp.m_nValue;
        Value = llrp_u16v_t(nValue);
        if(verifyVectorAllocation(Value.m_pValue, pFieldDesc))
        {
            unsigned int    Ix;

            for(Ix = 0; Ix < nValue; Ix++)
            {
                Value.m_pValue[Ix] = (llrp_u16_t) Tmp.m_pValue[Ix];
            }
        }
    }
    return Value;
}

/** @brief Decodes an llrp_u16v_t into the specified field descriptor */
llrp_s16v_t
CXMLTextDecoderStream::get_s16v (
  const CFieldDescriptor *      pFieldDesc)
{
    CErrorDetails *             pError;
    llrp_s16v_t                  Value;
    llrp_s64v_t                 Tmp;

    pError = &m_pDecoder->m_ErrorDetails;

    switch(pFieldDesc->m_eFieldFormat)
    {
        /* assume normal format here is decimal */
        case CFieldDescriptor::FMT_NORMAL:
        case CFieldDescriptor::FMT_DEC:
        case CFieldDescriptor::FMT_HEX:
            Tmp = getSpacedVectorField(pFieldDesc, MIN_S16, MAX_S16);
            break;
        default:
        case CFieldDescriptor::FMT_UTF8:
        case CFieldDescriptor::FMT_DATETIME:
            if(NULL != pError)
            {
                pError->m_eResultCode = RC_MiscError;
                pError->m_pWhatStr    = "Format type not support for field";
                pError->m_pRefType    = m_pRefType;
                pError->m_pRefField   = pFieldDesc;
                pError->m_OtherDetail = (int) m_pCurrentChildNode->line;
            }	    
        break;
    }    

    if(Tmp.m_nValue)
    {
        llrp_u16_t              nValue;

        nValue = Tmp.m_nValue;
        Value = llrp_s16v_t(nValue);
        if(verifyVectorAllocation(Value.m_pValue, pFieldDesc))
        {
            unsigned int    Ix;

            for(Ix = 0; Ix < nValue; Ix++)
            {
                Value.m_pValue[Ix] = (llrp_s16_t) Tmp.m_pValue[Ix];
            }
        }
    }
    return Value;
}

/*
 * 32-bit types
 */

/** @brief Decodes an llrp_u32_t (unsigned 32-bit number) into the specified field descriptor */
llrp_u32_t
CXMLTextDecoderStream::get_u32 (
  const CFieldDescriptor *      pFieldDesc)
{
    llrp_u32_t Value;
    Value = (llrp_u32_t) getIntegerField(pFieldDesc, MIN_U32, MAX_U32);
    return Value;
}

/** @brief Decodes an llrp_s32_t (signed 32-bit number) into the specified field descriptor */
llrp_s32_t
CXMLTextDecoderStream::get_s32 (
  const CFieldDescriptor *      pFieldDesc)
{
    llrp_s32_t Value;
    Value = (llrp_s32_t) getIntegerField(pFieldDesc, MIN_S32, MAX_S32);
    return Value;
}

/** @brief Decodes an llrp_u32v_t into the specified field descriptor */
llrp_u32v_t
CXMLTextDecoderStream::get_u32v (
  const CFieldDescriptor *      pFieldDesc)
{
    CErrorDetails *             pError;
    llrp_u32v_t                 Value;
    llrp_s64v_t                 Tmp;

    pError = &m_pDecoder->m_ErrorDetails;

    switch(pFieldDesc->m_eFieldFormat)
    {
        /* assume normal format here is decimal */
        case CFieldDescriptor::FMT_NORMAL:
        case CFieldDescriptor::FMT_DEC:
        case CFieldDescriptor::FMT_HEX:
            Tmp = getSpacedVectorField(pFieldDesc, MIN_U32, MAX_U32);
            break;
        default:
        case CFieldDescriptor::FMT_UTF8:
        case CFieldDescriptor::FMT_DATETIME:
            if(NULL != pError)
            {
                pError->m_eResultCode = RC_MiscError;
                pError->m_pWhatStr    = "Format type not support for field";
                pError->m_pRefType    = m_pRefType;
                pError->m_pRefField   = pFieldDesc;
                pError->m_OtherDetail = (int) m_pCurrentChildNode->line;
            }	    
        break;
    }    

    if(Tmp.m_nValue)
    {
        llrp_u16_t              nValue;

        nValue = Tmp.m_nValue;
        Value = llrp_u32v_t(nValue);
        if(verifyVectorAllocation(Value.m_pValue, pFieldDesc))
        {
            unsigned int    Ix;

            for(Ix = 0; Ix < nValue; Ix++)
            {
                Value.m_pValue[Ix] = (llrp_u32_t) Tmp.m_pValue[Ix];
            }
        }
    }
    return Value;
}

/** @brief Decodes an llrp_s32v_t into the specified field descriptor */
llrp_s32v_t
CXMLTextDecoderStream::get_s32v (
  const CFieldDescriptor *      pFieldDesc)
{
    CErrorDetails *             pError;
    llrp_s32v_t                 Value;
    llrp_s64v_t                 Tmp;

    pError = &m_pDecoder->m_ErrorDetails;

    switch(pFieldDesc->m_eFieldFormat)
    {
        /* assume normal format here is decimal */
        case CFieldDescriptor::FMT_NORMAL:
        case CFieldDescriptor::FMT_DEC:
        case CFieldDescriptor::FMT_HEX:
            Tmp = getSpacedVectorField(pFieldDesc, MIN_S32, MAX_S32);
            break;
        default:
        case CFieldDescriptor::FMT_UTF8:
        case CFieldDescriptor::FMT_DATETIME:
            if(NULL != pError)
            {
                pError->m_eResultCode = RC_MiscError;
                pError->m_pWhatStr    = "Format type not support for field";
                pError->m_pRefType    = m_pRefType;
                pError->m_pRefField   = pFieldDesc;
                pError->m_OtherDetail = (int) m_pCurrentChildNode->line;
            }	    
        break;
    }    

    if(Tmp.m_nValue)
    {
        llrp_u16_t              nValue;

        nValue = Tmp.m_nValue;
        Value = llrp_s32v_t(nValue);
        if(verifyVectorAllocation(Value.m_pValue, pFieldDesc))
        {
            unsigned int    Ix;

            for(Ix = 0; Ix < nValue; Ix++)
            {
                Value.m_pValue[Ix] = (llrp_s32_t) Tmp.m_pValue[Ix];
            }
        }
    }
    return Value;
}

/*
 * 64-bit types
 */

/** @brief Decodes an llrp_u64_t (unsigned 64-bit number) into the specified field descriptor */
llrp_u64_t
CXMLTextDecoderStream::get_u64 (
  const CFieldDescriptor *      pFieldDesc)
{
    llrp_u64_t Value;
    Value = (llrp_u64_t) getIntegerField(pFieldDesc, MIN_U64, MAX_U64);
    return Value;
}

/** @brief Decodes an llrp_s64_t (signed 64-bit number) into the specified field descriptor */
llrp_s64_t
CXMLTextDecoderStream::get_s64 (
  const CFieldDescriptor *      pFieldDesc)
{
    llrp_s64_t Value;
    Value = (llrp_s64_t) getIntegerField(pFieldDesc, MIN_S64, MAX_S64);
    return Value;
}

/** @brief Decodes an llrp_u64v_t into the specified field descriptor */
llrp_u64v_t
CXMLTextDecoderStream::get_u64v (
  const CFieldDescriptor *      pFieldDesc)
{
    CErrorDetails *             pError;
    llrp_u64v_t                 Value;
    llrp_s64v_t                 Tmp;

    pError = &m_pDecoder->m_ErrorDetails;

    switch(pFieldDesc->m_eFieldFormat)
    {
        /* assume normal format here is decimal */
        case CFieldDescriptor::FMT_NORMAL:
        case CFieldDescriptor::FMT_DEC:
        case CFieldDescriptor::FMT_HEX:
        case CFieldDescriptor::FMT_DATETIME:
            Tmp = getSpacedVectorField(pFieldDesc, MIN_U64, MAX_U64);
            break;
        default:
        case CFieldDescriptor::FMT_UTF8:
            if(NULL != pError)
            {
                pError->m_eResultCode = RC_MiscError;
                pError->m_pWhatStr    = "Format type not support for field";
                pError->m_pRefType    = m_pRefType;
                pError->m_pRefField   = pFieldDesc;
                pError->m_OtherDetail = (int) m_pCurrentChildNode->line;
            }	    
        break;
    }    

    if(Tmp.m_nValue)
    {
        llrp_u16_t              nValue;

        nValue = Tmp.m_nValue;
        Value = llrp_u64v_t(nValue);
        if(verifyVectorAllocation(Value.m_pValue, pFieldDesc))
        {
            unsigned int    Ix;

            for(Ix = 0; Ix < nValue; Ix++)
            {
                Value.m_pValue[Ix] = (llrp_u64_t) Tmp.m_pValue[Ix];
            }
        }
    }
    return Value;
}

/** @brief Decodes an llrp_s64v_t into the specified field descriptor */
llrp_s64v_t
CXMLTextDecoderStream::get_s64v (
  const CFieldDescriptor *      pFieldDesc)
{
    llrp_s64v_t Value;
    Value = getSpacedVectorField(pFieldDesc, MIN_S64, MAX_S64); 
    return Value;
}

/*
 * Special types
 */

/** @brief Decodes an llrp_u1_t (unsigned 1 bit number) into the specified field descriptor */
llrp_u1_t
CXMLTextDecoderStream::get_u1 (
  const CFieldDescriptor *      pFieldDesc)
{
    llrp_u1_t Value;
    Value = (llrp_u1_t) getIntegerField(pFieldDesc, 0, 1);
    return Value;
}

/** @brief Decodes an llrp_u1v_t into the specified field descriptor */
llrp_u1v_t
CXMLTextDecoderStream::get_u1v (
  const CFieldDescriptor *      pFieldDesc)
{
    CErrorDetails *             pError;
    llrp_u1v_t                  Value;
    llrp_s64v_t                 Tmp;

    pError = &m_pDecoder->m_ErrorDetails;
    Tmp = getFixedVectorField(pFieldDesc, 2, 0, 255);

    if(Tmp.m_nValue)
    {
        llrp_u16_t              nValue;

        nValue = Tmp.m_nValue;
        Value = llrp_u1v_t(nValue*8);
        if(verifyVectorAllocation(Value.m_pValue, pFieldDesc))
        {
            unsigned int        Ix;
            xmlChar *           pCountStr;

            for(Ix = 0; Ix < nValue; Ix++)
            {
                Value.m_pValue[Ix] = (llrp_u1_t) Tmp.m_pValue[Ix];
            }
            /* bit length could be shorter than this. Use this if there
             * is no attribute */
            Value.m_nBit = Tmp.m_nValue * 8;

            /* find the XML attribute for Count and fix the array */
            if(NULL != m_pLastFieldNode)
            {
                pCountStr = xmlGetProp(m_pLastFieldNode, 
                                       (xmlChar*) "Count");
                if(NULL != pCountStr)
                {
                    char *      pArg = (char *) pCountStr;
                    char *      pTail = pArg + strlen((char*) pCountStr);
                    llrp_u16_t  nBits;

                    nBits = (llrp_u16_t) strtoul(pArg, &pTail, 10);
                    if( (*pTail != 0) ||
                        (nBits > (Tmp.m_nValue * 8)))
                    {
                        /* malformed */
                        pError->m_eResultCode = RC_MiscError;
                        pError->m_pWhatStr    = "malformed Count Attribute";
                        pError->m_pRefType    = m_pRefType;
                        pError->m_pRefField   = pFieldDesc;
                        pError->m_OtherDetail = (int) m_pLastFieldNode->line;
                    }                
                    else
                    {
                        llrp_u16_t lastByteBits;
                        llrp_u8_t  lastByteMask;
                                    
                        Value.m_nBit = nBits;			

                        /* zero off the last bits if they were non-zero */
                        lastByteBits = nBits % 8;			
                        if(lastByteBits)
                        {
                            /* convert the bit position to a mask */
                            lastByteBits = 8 - lastByteBits;
                            lastByteMask = (1 << lastByteBits) - 1;
                            /* invert mask */
                            lastByteMask = ~lastByteMask;
                            /* clear bits */
                            Value.m_pValue[Ix] &= lastByteMask;
                        }
                    }
                    xmlFree(pCountStr);
                }
            }
        }
    }
    return Value;
}

/** @brief Decodes an llrp_u2_t into the specified field descriptor */
llrp_u2_t
CXMLTextDecoderStream::get_u2 (
  const CFieldDescriptor *      pFieldDesc)
{
    llrp_u2_t Value;
    Value = (llrp_u2_t) getIntegerField(pFieldDesc, 0, 3);
    return Value;
}

/** @brief Decodes an llrp_u96_t into the specified field descriptor */
llrp_u96_t
CXMLTextDecoderStream::get_u96 (
  const CFieldDescriptor *      pFieldDesc)
{
    CErrorDetails *             pError; 
    llrp_u96_t                  Value;
    llrp_s64v_t                 Tmp;
    unsigned int    Ix;

    pError = &m_pDecoder->m_ErrorDetails;

    Tmp = getFixedVectorField( pFieldDesc, 2, 0, 255);

    if((RC_OK != pError->m_eResultCode) && (Tmp.m_nValue != 12))
    {
        if(NULL != pError)
        {
            pError->m_eResultCode = RC_MiscError;
            pError->m_pWhatStr    = "Illegal length u96 field";
            pError->m_pRefType    = m_pRefType;
            pError->m_pRefField   = pFieldDesc;
            pError->m_OtherDetail = (int) m_pCurrentChildNode->line;
        }
        return Value;    
    }

    /* convert to this type */
    for(Ix = 0; Ix < Tmp.m_nValue; Ix++)
    {
        Value.m_aValue[Ix] = (llrp_u8_t) Tmp.m_pValue[Ix];
    }
  
    return Value;
}

/** @brief Decodes an llrp_utf8v_t into the specified field descriptor */
llrp_utf8v_t
CXMLTextDecoderStream::get_utf8v (
  const CFieldDescriptor *      pFieldDesc)
{
    llrp_utf8v_t                Value;
    llrp_u16_t                  nValue;
    const llrp_u8_t *           pbuf;
    const llrp_u8_t *           pend;

    if(getFieldStringPtr(pFieldDesc, &pbuf, &pend))
    {
        nValue = (llrp_u16_t) strlen((char*) pbuf);
        Value = llrp_utf8v_t(nValue);

        if(verifyVectorAllocation(Value.m_pValue, pFieldDesc))
        {
            unsigned int    Ix;

            for(Ix = 0; Ix < nValue; Ix++)
            {
                Value.m_pValue[Ix] = pbuf[Ix];
            }
        }    
    }
    else
    {
        /* its valid for UTF8 strings to be empty */
        Value.m_nValue=0;
        Value.m_pValue=NULL;
    }
    return Value;
}

/** @brief Decodes an llrp_bytesToEnd_t into the specified field descriptor */
llrp_bytesToEnd_t
CXMLTextDecoderStream::get_bytesToEnd (
  const CFieldDescriptor *      pFieldDesc)
{
    llrp_bytesToEnd_t           Value;
    llrp_s64v_t                 Tmp;
    unsigned int                Ix;

    Tmp = getFixedVectorField( pFieldDesc, 2, 0, 255);

    if(verifyVectorAllocation(Tmp.m_pValue, pFieldDesc))
    {
        /* build and convert */
        Value = llrp_bytesToEnd_t(Tmp.m_nValue);
        if(verifyVectorAllocation(Tmp.m_pValue, pFieldDesc))
        {
            /* convert to this type */
            for(Ix = 0; Ix < Tmp.m_nValue; Ix++)
            {
                Value.m_pValue[Ix] = (llrp_byte_t) Tmp.m_pValue[Ix];
            }
            Value.m_nValue = Tmp.m_nValue;
        }
    }
    return Value;
}

/*
 * Enumerated types of various sizes
 */

/** @brief Decodes a 1 bit enumerated field into the specified field descriptor */
int
CXMLTextDecoderStream::get_e1 (
  const CFieldDescriptor *      pFieldDesc)
{
    int                         Value;
    Value = (int) getIntegerField(pFieldDesc, 0, 1);
    return Value;
}

/** @brief Decodes a 2 bit enumerated field into the specified field descriptor */
int
CXMLTextDecoderStream::get_e2 (
  const CFieldDescriptor *      pFieldDesc)
{
    int                         Value;
    Value = (int) getIntegerField(pFieldDesc, 0, 3);
    return Value;
}

/** @brief Decodes a 8 bit enumerated field into the specified field descriptor */
int
CXMLTextDecoderStream::get_e8 (
  const CFieldDescriptor *      pFieldDesc)
{
    int                         Value;
    Value = (int) getIntegerField(pFieldDesc, MIN_U8, MAX_U8);
    return Value;
}

/** @brief Decodes a 16 bit enumerated field into the specified field descriptor */
int
CXMLTextDecoderStream::get_e16 (
  const CFieldDescriptor *      pFieldDesc)
{
    int                         Value;
    Value = (int) getIntegerField(pFieldDesc, MIN_U16, MAX_U16);
    return Value;
}

/** @brief Decodes a 32 bit enumerated field into the specified field descriptor */
int
CXMLTextDecoderStream::get_e32 (
  const CFieldDescriptor *      pFieldDesc)
{
    int                         Value;
    Value = (int) getIntegerField(pFieldDesc, MIN_U32, MAX_U32);
    return Value;
}

/** @brief Decodes an enumerated u8v field into the specified field descriptor */
llrp_u8v_t
CXMLTextDecoderStream::get_e8v (
  const CFieldDescriptor *      pFieldDesc)
{
    llrp_u8v_t                  Value;
    llrp_s64v_t                 Tmp;

    Tmp = getSpacedVectorField( pFieldDesc, MIN_U8, MAX_U8);
    if(Tmp.m_nValue)
    {
        llrp_u16_t              nValue;

        nValue = Tmp.m_nValue;
        Value = llrp_u8v_t(nValue);
        if(verifyVectorAllocation(Value.m_pValue,pFieldDesc))
        {
            unsigned int    Ix;

            for(Ix = 0; Ix < nValue; Ix++)
            {
                Value.m_pValue[Ix] = (llrp_u8_t) Tmp.m_pValue[Ix];
            }
        }
    }
    return Value;
}

/* @brief skips a number of reserved bits and discards during the encode/decode process */
void
CXMLTextDecoderStream::get_reserved (
  unsigned int                  nBits)
{
    return;
}

llrp_bool_t
CXMLTextDecoderStream::verifyVectorAllocation (
  const void *                  pValue,
  const CFieldDescriptor *      pFieldDescriptor)
{
    if(NULL == pValue)
    {
        CErrorDetails *         pError = &m_pDecoder->m_ErrorDetails;

        pError->m_eResultCode = RC_FieldAllocationFailed;
        pError->m_pWhatStr    = "field allocation failed";
        pError->m_pRefType    = m_pRefType;
        pError->m_pRefField   = pFieldDescriptor;
        pError->m_OtherDetail = (int) m_pCurrentChildNode->line;

        return FALSE;
    }
    else
    {
        return TRUE;
    }
}

int
CXMLTextDecoderStream::isInsignificantNode (
  struct _xmlNode *             pnode)
{
    switch(pnode->type)
    {
        /* don't process comments or notations */
        case XML_COMMENT_NODE:
        case XML_NOTATION_NODE:
        /* ignore includes */
        case XML_XINCLUDE_START:
       case XML_XINCLUDE_END:		
       /* don't read processing instructions */
        case XML_PI_NODE:
        return 1;

        /* hopefully we only see these type of nodes */
        case XML_ELEMENT_NODE:
        case XML_ATTRIBUTE_NODE:
        case XML_TEXT_NODE:

        /* we should error if we see a cdata  */
        case XML_CDATA_SECTION_NODE:

        /* we don't understand entities, so we should error */
        case XML_ENTITY_REF_NODE:
        case XML_ENTITY_NODE:
        case XML_ENTITY_DECL:

        /* we should have already gotten the doc node , but should error out on these*/
        case XML_DOCUMENT_NODE:
        case XML_DOCUMENT_TYPE_NODE:
        case XML_DOCUMENT_FRAG_NODE:
        case XML_HTML_DOCUMENT_NODE:
        case XML_DTD_NODE:

        /* not sure yet about these, but I think they are irellevant */
        case XML_ELEMENT_DECL:
        case XML_ATTRIBUTE_DECL:
        case XML_NAMESPACE_DECL:

        /* anything new we should error */
        default:
            return 0;
    }
}

/* some class member field helpers */
int 
CXMLTextDecoderStream::getFieldStringPtr(
  const CFieldDescriptor *      pFieldDescriptor,
  const llrp_u8_t **            pbuf,
  const llrp_u8_t **            pend)
{
    CErrorDetails *             pError = &m_pDecoder->m_ErrorDetails;
    xmlNode *                   pSave;

    *pbuf = NULL;
    *pend = NULL;

    /* clear this out as its not valid anymore */
    m_pLastFieldNode = NULL;

    if(RC_OK != pError->m_eResultCode)
    {
        return 0;
    }

   /* get the first interesting node */
    while ((NULL != m_pCurrentChildNode) &&
           (isInsignificantNode(m_pCurrentChildNode) || 
            xmlIsBlankNode(m_pCurrentChildNode)))
    {
        m_pCurrentChildNode = m_pCurrentChildNode->next;
    }    

    if(NULL == m_pCurrentChildNode)
    {
        pError->m_eResultCode = RC_FieldUnderrun;
        pError->m_pWhatStr    = "underrun at field";
        pError->m_pRefType    = m_pRefType;
        pError->m_pRefField   = pFieldDescriptor;
        pError->m_OtherDetail = (int) m_pCurrentChildNode->line;
        return 0;
    }

    /* first interesting node had better be an element */
    if(m_pCurrentChildNode->type != XML_ELEMENT_NODE)
    {
        pError->m_eResultCode = RC_XMLInvalidNodeType;
        pError->m_pWhatStr    = "unexpected field value";
        pError->m_pRefType    = m_pRefType;
        pError->m_pRefField   = pFieldDescriptor;
        pError->m_OtherDetail = (int) m_pCurrentChildNode->line;	    
        return 0;
    }

    /* better match our field value */
    if(0 != strcmp((char*) m_pCurrentChildNode->name, 
                    pFieldDescriptor->m_pName))
    {
        pError->m_eResultCode = RC_XMLMissingField;
        pError->m_pWhatStr    = "missing field value";
        pError->m_pRefType    = m_pRefType;
        pError->m_pRefField   = pFieldDescriptor;
        pError->m_OtherDetail = (int) m_pCurrentChildNode->line;	    
        return 0;
    }

    /* save this in case someone wants to explore attributes */
    m_pLastFieldNode = m_pCurrentChildNode;

    /* use this to decode the field values */
    pSave = m_pCurrentChildNode->children;

    /* advance to the next one for the next field */
    m_pCurrentChildNode = m_pCurrentChildNode->next;

   /* get the first interesting node */
    while ((NULL !=pSave) &&
           (isInsignificantNode(pSave) ||
         xmlIsBlankNode(pSave)))
    {
        pSave = pSave->next;
    }    

    if(NULL == pSave)
    {
        /* its possible that this is an empty field value, which is
         * allowed in many cases in LLRP-XML. Let the condition
         * percolate and error out specifically where it is not
         * allowed */
        return 0;	
    }

    if(XML_TEXT_NODE != pSave->type)
    {
        pError->m_eResultCode = RC_XMLInvalidNodeType;
        pError->m_pWhatStr    = "invalid XML Node found during field decode";
        pError->m_pRefType    = m_pRefType;
        pError->m_pRefField   = pFieldDescriptor;
        pError->m_OtherDetail = (int) pSave->line;
        return 0;	 	    
    }

    *pbuf = pSave->content;

    /* make sure there are no more interesting nodes */
    pSave = pSave->next;
    while ((NULL !=pSave) &&
           (isInsignificantNode(pSave) ||
            xmlIsBlankNode(pSave)))
    {
        pSave = pSave->next;
    }   

    if(pSave != NULL)
    {
        pError->m_eResultCode = RC_XMLExtraNode;
        pError->m_pWhatStr    = "extra XML node found";
        pError->m_pRefType    = m_pRefType;
        pError->m_pRefField   = pFieldDescriptor;
        pError->m_OtherDetail = (int) pSave->line;
        return 0;	 
    }

    *pend = *pbuf + strlen((char*) *pbuf);
    return 1;
}

llrp_s64v_t
CXMLTextDecoderStream::getSpacedVectorField (
  const CFieldDescriptor *      pFieldDescriptor,
  llrp_s64_t                    minValue,
  llrp_s64_t                    maxValue)
{
    CErrorDetails *             pError = &m_pDecoder->m_ErrorDetails;
    const llrp_u8_t *           pTok;
    const llrp_u8_t *           pEnd;    
    const llrp_u8_t *           pTokEnd;    
    llrp_u16_t                  length;
    llrp_u16_t                  elementCount;
    llrp_s64v_t                 Value;
    llrp_u16_t                  count;

    if(RC_OK != pError->m_eResultCode)
    {
    /* already errd */
        return Value;
    }

    /* get the NULL terminates string from XML */
    if(!getFieldStringPtr(pFieldDescriptor, &pTok, &pEnd))
    {
      /* many of these (e.g. ReadData) are allowed to be empty */
       return Value;
    }

    length = cleanString(&pTok, &pEnd);
    elementCount = countElements((char *) pTok, length);

    /* create the final storage entity */ 
    Value = llrp_s64v_t(elementCount);

    for(count = 0;count <elementCount ; count++)
    {
    /* find the next starting token */
        while(isspace(*pTok) && (pTok < pEnd))
        {
            pTok++;
        }

        /* find the next ending token */
        for(pTokEnd = pTok; (!isspace(*pTokEnd)) && (*pTokEnd != '\0'); pTokEnd++)
        {
           /* nothing */
        } 

        /* get a single integer */
        Value.m_pValue[count] = getInteger(pFieldDescriptor, 
                                           pTok, 
                                           pTokEnd, 
                                           pError, 
                                           minValue, 
                                           maxValue);
        if(RC_OK != pError->m_eResultCode)
        {
            break;
        }
        /* get the next value */
        pTok = pTokEnd;
    }

    /* mark the length */
    Value.m_nValue = count;

    return Value;
}

llrp_s64v_t
CXMLTextDecoderStream::getFixedVectorField (
  const CFieldDescriptor *      pFieldDescriptor,
  unsigned int                  vectorSize,
  llrp_s64_t                    minValue,
  llrp_s64_t                    maxValue)
{
    CErrorDetails *             pError = &m_pDecoder->m_ErrorDetails;
    const llrp_u8_t *           pTok;
    const llrp_u8_t *           pEnd;    
    llrp_u16_t                  length;
    llrp_u16_t                  elementCount;
    llrp_s64v_t                 Value;
    llrp_u16_t                  count;

    if(RC_OK != pError->m_eResultCode)
    {
    /* already errd */
        return Value;
    }

    if(!getFieldStringPtr(pFieldDescriptor, &pTok, &pEnd))
    {
        /* these values are allowed to be empty */
        Value.m_nValue = 0;
        Value.m_pValue = NULL;
        return Value;
    }

    length = cleanString(&pTok, &pEnd);

    /* size should match exactly */
    if (length % vectorSize)
    {
        pError->m_eResultCode = RC_FieldAllocationFailed;
        pError->m_pWhatStr    = "field size must be multiple of basic type";
        pError->m_pRefType    = m_pRefType;
        pError->m_pRefField   = pFieldDescriptor;
        pError->m_OtherDetail = (int) m_pCurrentChildNode->line;
        return Value;
    }

    /* round up for element count */
    elementCount = (length +  vectorSize - 1)/vectorSize;

    /* create the final storage entity */ 
    Value = llrp_s64v_t(elementCount);

    for(count = 0;count <elementCount ; count++, pTok += vectorSize)
    {
        pEnd = pTok + vectorSize;

        /* get a single integer */
        Value.m_pValue[count] = getInteger(pFieldDescriptor, 
                                           pTok, 
                                           pEnd, 
                                           pError, 
                                           minValue, 
                                           maxValue);

        if(RC_OK != pError->m_eResultCode)
        {
            break;
        }
    }

    /* mark the length */
    Value.m_nValue = count;

    return Value;
}


llrp_s64_t
CXMLTextDecoderStream::getIntegerField (
  const CFieldDescriptor *      pFieldDescriptor,
  llrp_s64_t                    minValue,
  llrp_s64_t                    maxValue)
{
    CErrorDetails *             pError = &m_pDecoder->m_ErrorDetails;
    const llrp_u8_t *           pbuf;
    const llrp_u8_t *           pend;
    llrp_s64_t                  Value;

    memset(&Value, 0x00, sizeof(Value));

    if(RC_OK != pError->m_eResultCode)
    {
        /* already errd */
        return Value;
    }

    if(getFieldStringPtr(pFieldDescriptor, &pbuf, &pend))
    {
        cleanString(&pbuf, &pend);
        Value = getInteger(pFieldDescriptor, 
                           pbuf, 
                           pend, 
                           pError, 
                           minValue, 
                           maxValue);
    }
    else
    {
        pError->m_eResultCode = RC_FieldUnderrun;
        pError->m_pWhatStr    = "underrun at field no characters";
        pError->m_pRefType    = m_pRefType;
        pError->m_pRefField   = pFieldDescriptor;
        pError->m_OtherDetail = (int) m_pCurrentChildNode->line; 
        return 0;	
    }

    return Value;
}


llrp_s64_t CXMLTextDecoderStream::getInteger(
  const CFieldDescriptor *      pFieldDescriptor,
  const llrp_u8_t *             pbuf,
  const llrp_u8_t *             pend,
  CErrorDetails *               pError,
  llrp_s64_t                    minValue,
  llrp_s64_t                    maxValue)
{
    llrp_s64_t                  Value;
    const llrp_u8_t           * endPtr = NULL;


    if(pFieldDescriptor->m_eFieldType == CFieldDescriptor::FT_U1)
    /* special case for U1 types as they can be
     * 'true' and 'false' strings */
    {
        endPtr = getSingleU1(pbuf, pend, &Value);
    }
    else if((pFieldDescriptor->m_eFieldType == CFieldDescriptor::FT_E1 ) || 
            (pFieldDescriptor->m_eFieldType == CFieldDescriptor::FT_E2 ) ||
            (pFieldDescriptor->m_eFieldType == CFieldDescriptor::FT_E8 ) || 
            (pFieldDescriptor->m_eFieldType == CFieldDescriptor::FT_E16 ) || 
            (pFieldDescriptor->m_eFieldType == CFieldDescriptor::FT_E32 ) ||
            (pFieldDescriptor->m_eFieldType == CFieldDescriptor::FT_E8V ) )
    /* special case for enumerated fields as they are strings in XML*/
    {
        endPtr = getSingleEnum(pFieldDescriptor, pbuf, pend, &Value);  
    }
    else switch(pFieldDescriptor->m_eFieldFormat)
    {
        /* assume default is decimal for new formats */
        default:
        case CFieldDescriptor::FMT_NORMAL:
        case CFieldDescriptor::FMT_DEC:
            endPtr = getSingleDecimal(pbuf, pend, &Value);
        break;
        case CFieldDescriptor::FMT_HEX:
            endPtr = getSingleHexidecimal(pbuf, pend, &Value);
        break;
        case CFieldDescriptor::FMT_UTF8:
        {
            const llrp_u8_t  *ptr;
            /* not sure what to do for UTF 8 encoding as integer */
            for(ptr = pbuf; ptr < pend; ptr++)
            {
                Value = 256*Value + *ptr;
            } 
        }
        break;
        case CFieldDescriptor::FMT_DATETIME:
            endPtr = getSingleTimestamp(pbuf, pend, &Value);
        break;
    }    

    if(endPtr == pbuf)
    /* if we didn't get any thing, it means there are illegal characters
     * and we could not import a field value */
    {
        pError->m_eResultCode = RC_XMLInvalidFieldCharacters;
        pError->m_pWhatStr    = "Illegal field value";
        pError->m_pRefType    = m_pRefType;
        pError->m_pRefField   = pFieldDescriptor;
        pError->m_OtherDetail = (int) m_pCurrentChildNode->line; 
        Value = 0;		
    }
    else if (endPtr != pend)
    /* if these don't match, there is still extra stuff
     * at the end.  This is always an error */
    {
        pError->m_eResultCode = RC_FieldOverrun;
        pError->m_pWhatStr    = "overrun at field extra characters";
        pError->m_pRefType    = m_pRefType;
        pError->m_pRefField   = pFieldDescriptor;
        pError->m_OtherDetail = (int) m_pCurrentChildNode->line;
        Value = 0;		
    }
    /* make sure the value is in range */ 
    else if((Value > maxValue) || (Value < minValue))
    /* we have an out of range value. This is always an error */
    {
        pError->m_eResultCode = RC_XMLOutOfRange;
        pError->m_pWhatStr    = "out of range value";
        pError->m_pRefType    = m_pRefType;
        pError->m_pRefField   = pFieldDescriptor;
        pError->m_OtherDetail = (int) m_pCurrentChildNode->line;
        Value = 0;
    }    
    return Value;
}

/* static string helpers */

const llrp_u8_t *
CXMLTextDecoderStream::getSingleU1(
  const llrp_u8_t *             pbuf,
  const llrp_u8_t *             pend,
  llrp_s64_t *                  pValue)
{
    const llrp_u8_t *           endPtr = pbuf;
    const int                   len = (int) (pend - pbuf);

    if((len >= 4) && (0 == strncasecmp("true", (char*) pbuf, 4)))
    {
        *pValue = 1;
        endPtr = pbuf + 4; 
    }
    else if ((len >= 5) && (0 == strncasecmp("false", (char*) pbuf, 5)))
    {
        *pValue = 0;
        endPtr = pbuf + 5; 
    } 
    else if ((len >= 1) && (*pbuf == '0'))
    {
        *pValue = 0;
        endPtr = pbuf + 1;
    }
    else if ((len >= 1) && (*pbuf == '1'))
    {
        *pValue = 0;
        endPtr = pbuf + 1;
    }
    return endPtr; 	
}

const llrp_u8_t *
CXMLTextDecoderStream::getSingleEnum(
  const CFieldDescriptor *      pFieldDescriptor,
  const llrp_u8_t *             pbuf,
  const llrp_u8_t *             pend,
  llrp_s64_t *                  pValue)
{
    const llrp_u8_t *           endPtr = pbuf;
    const int                   length = (int) (pend - pbuf);
    const SEnumTableEntry *     pEntry;    

    *pValue = 0;

    for(
        pEntry = pFieldDescriptor->m_pEnumTable;
        NULL != pEntry->pName;
        pEntry++)
    {
        int len = (int) strlen(pEntry->pName);
        if((len == length) && 
           (memcmp(pbuf, pEntry->pName, len) == 0))
        {
            break;
        }
    }

    if(NULL == pEntry->pName)
    {
        return pbuf;
    }
   
    *pValue = pEntry->Value;
    endPtr = pbuf + length;		
    return endPtr;
}

const llrp_u8_t *
CXMLTextDecoderStream::getSingleDecimal(
  const llrp_u8_t *             pbuf,
  const llrp_u8_t *             pend,
  llrp_s64_t *                  pValue)
{
    int bNeg = 0;
    const llrp_u8_t *           endPtr;

    *pValue = 0;

    for(endPtr = pbuf; endPtr < pend; endPtr++)
    {
        if(*endPtr == '-')
        {
           bNeg = 1;
        } else if (isdigit(*endPtr))
        {
           *pValue = 10 * *pValue + (*endPtr - '0');
        }
        else
        {
           /* a character not allowed in a decimal integer */
           break;        
        }
    }
    if(bNeg)
    {
        *pValue *= -1;
    }

    return endPtr;
}


const llrp_u8_t *
CXMLTextDecoderStream::getSingleHexidecimal(
  const llrp_u8_t *             pbuf,
  const llrp_u8_t *             pend,
  llrp_s64_t *                  pValue)
{
    const llrp_u8_t *           endPtr;

    *pValue = 0;
    for(endPtr = pbuf; endPtr < pend; endPtr++)
    {
        if(isdigit(*endPtr))
        {
           *pValue = 16 * *pValue + (*endPtr - '0');
        }
        else if (*endPtr >= 'A' && *endPtr <= 'F')
        {
            *pValue = 16 * *pValue + (*endPtr - 'A' + 10);
        }
        else if (*endPtr >= 'a' && *endPtr <= 'f')
        {
            *pValue = 16 * *pValue + (*endPtr - 'a' + 10);
        }
        else 
        {
           /* a character not allowed in a decimal integer */
           break;         
        }
    }
    return endPtr;
}


int
CXMLTextDecoderStream::cleanString(
  const llrp_u8_t **            ppbuf,
  const llrp_u8_t **            ppend)
{
    /* skip leading spaces */
    while( isspace(**ppbuf) && (*ppbuf <= *ppend))
    {
      (*ppbuf)++;
    }
    /* skip trailing spaces except last separator */
    /* last separator could be space or NULL. */
    while((*ppend > *ppbuf) && (isspace(*(*ppend-1)) || (*(*ppend-1) == '\0')))
    {
       (*ppend)--;
    }
    return (int) (*ppend - *ppbuf);
}

/* gets a single char value from a non-NULL terminated buffer buffer.
 * If it fails to get one, reutrn value should == pbuf, other
 * wise EndPtr should point to one past amount consumed */
static const llrp_u8_t *
getSingleChar(
  const llrp_u8_t *             pbuf,
  const llrp_u8_t *             pend,
  llrp_s64_t *                  pValue)
{
    const llrp_u8_t *           endPtr = pbuf;
    const int                   len = pend - pbuf;

    if(len >= 1) 
    {
        *pValue = *pbuf;
         endPtr = pbuf + 1;
    }

    return endPtr;
}

const llrp_u8_t *
CXMLTextDecoderStream::getSingleTimestamp(
  const llrp_u8_t *             pbuf,
  const llrp_u8_t *             pend,
  llrp_s64_t *                  pValue)
{
    const llrp_u8_t *           endPtr;
    const llrp_u8_t *           tmpPtr;
    struct tm importTime;
    llrp_s64_t micros = 0;
    llrp_s64_t temp = 0;
    time_t tt;
    int utc = 0;
    llrp_s64_t offset = 0;

    memset(&importTime, 0x00, sizeof(importTime));

    tmpPtr = pbuf;
    endPtr = (llrp_u8_t*) strptime((char*) tmpPtr, "%Y-%m-%dT%T", &importTime);

    if(endPtr == NULL) 
    {
        goto timeParseError;
    }
    
    /* we have a valid timestamp, but there may be more stuff */
    tmpPtr = endPtr;
    if((endPtr = getSingleChar(tmpPtr, pend, &temp)) == tmpPtr) 
    {
        goto timeParseDone;
    }

    if(temp == '.')
    { 
        int length;

        /* try to get the microsecond value */
        tmpPtr = endPtr;
        if((endPtr = getSingleDecimal(tmpPtr, pend, &micros)) == tmpPtr)
        {
            goto timeParseError;
        }

        length = endPtr - tmpPtr;
        /* may not contain 6 decimal places */
        while(length < 6)
        {
            micros *= 10;
            length++;
        }

        if(length > 6) 
        {
            goto timeParseError;
        }

        /* look for more stuff that is optional */
        tmpPtr = endPtr;
        if((endPtr = getSingleChar(tmpPtr, pend, &temp)) == tmpPtr) 
        {
            goto timeParseDone;
        }
    }

    /* timestamp could either have offset or Z for UTC (Zulu) */
    if(temp == '-' || temp == '+') 
    {
        int scale = 1;
        /* offset formats are hh or hh:mm from UTC */
	if(temp == '+')
        {
            scale = -1;
            /* The time is offset positively from UTC, which means we have to 
            ** subtract to get to UTC time */
        }
	utc = 1;
        tmpPtr = endPtr;
        if((endPtr = getSingleDecimal(tmpPtr, pend, &temp)) == (tmpPtr+2))
        {
            goto timeParseError;
        }
   
        offset = scale*temp*60*60;
       
        tmpPtr = endPtr;
        if((endPtr = getSingleChar(tmpPtr, pend, &temp)) == tmpPtr) 
        {
            goto timeParseDone;
        }
     
        if(temp != ':') {
            goto timeParseError;
        }

        tmpPtr = endPtr;
        if((endPtr = getSingleDecimal(tmpPtr, pend, &temp)) == (tmpPtr+2))
        {
            goto timeParseError;
        }

        offset += (scale * temp * 60);
        
    }
    else if ((temp == 'z') || (temp == 'Z')) 
    {
	utc = 1;
    }
    /* fall through */

timeParseDone:

    if(utc) 
    {
        tt = timegm(&importTime);
    } else 
    {
        tt = timelocal(&importTime);
    }

    /* make this into time since epoch at GMT (UTC) */
    if(tt != (time_t) -1)
    {
        /* LLRP format is 64-bit microseconds UTC. Conver  */
        *pValue = 1000000ll * (llrp_s64_t) (tt + offset) + micros;
        return endPtr;
    }
    /* fall through if time wasn't valid */
timeParseError:
    endPtr = pbuf;
    return endPtr;
}


llrp_u16_t 
CXMLTextDecoderStream::countElements(
  const char *                  pval, 
  int                           nval)
{
    const llrp_u8_t *           pTok;
    const llrp_u8_t *           pEnd;
    llrp_u8_t                   lastChar;
    llrp_u16_t                  elements = 0;

    /* find out how many possible tokens there are.
     * This should limit our size of array we need */

    pTok = (llrp_u8_t*) &pval[0];
    pEnd = pTok + nval;

    /* skip leading spaces */
    for(lastChar = ' '; pTok < pEnd; pTok++)
    {
        if((!isspace(*pTok)) && (isspace(lastChar)))
        {
            elements++;
        }
        lastChar = *pTok;
    } 

    return elements;
}

};
