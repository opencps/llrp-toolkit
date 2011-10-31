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
 ****************************************************************************/


#include <assert.h>
#include <ctype.h>
#include <string.h>
#include <time.h>
#include <stdio.h>

#include "ltkc_platform.h"
#include "ltkc_base.h"

#include "libxml/parser.h"
#include "libxml/tree.h"

#include "ltkc_xmltext.h"

/*
 * BEGIN forward decls
 */

/*#define XML_NODE_DEBUG(ptr) print_element_names(__LINE__, ptr); */
#define XML_NODE_DEBUG(ptr) 
   
extern char *strptime(const char *s,const char *fmt, struct tm *tp);

/* don't declare static as it creates a warning when
 * debug is off */
void
print_element_names(int line, xmlNode * pnode)
{
    printf("XMLDUMP (Line %d) node type: %d, name: %s line %d", 
            line, pnode->type, pnode->name, pnode->line);
    if(pnode->type == XML_TEXT_NODE) 
    {
        if(!xmlIsBlankNode(pnode))
        {
        printf(" -- %s\n", pnode->content);
        }
        else
        {
        printf(" EMPTY \n");
        }
    }
    else
    {
        printf("\n");
    }
}


LLRP_tSLibXMLTextDecoder *
LLRP_LibXMLTextDecoder_construct (
  const LLRP_tSTypeRegistry *   pTypeRegistry,
  unsigned char *               pBuffer,
  unsigned int                  nBuffer);

LLRP_tSLibXMLTextDecoder *
LLRP_LibXMLTextDecoder_construct_file (
  const LLRP_tSTypeRegistry *   pTypeRegistry,
  char *                        fname);

LLRP_tSLibXMLTextDecoder *
LLRP_LibXMLTextDecoder_construct_nodetree (
  const LLRP_tSTypeRegistry *   pTypeRegistry,
  struct _xmlNode *             pNodeTree);

static void
decoderDestruct (
  LLRP_tSDecoder *              pBaseDecoder);

static LLRP_tSMessage *
topDecodeMessage (
  LLRP_tSDecoder *              pBaseDecoder);

static void
streamConstruct_outermost (
  LLRP_tSLibXMLTextDecoderStream *   pDecoderStream,
  LLRP_tSLibXMLTextDecoder *         pDecoder);

static void
streamConstruct_nested (
  LLRP_tSLibXMLTextDecoderStream *   pDecoderStream,
  LLRP_tSLibXMLTextDecoderStream *   pEnclosingDecoderStream);

static LLRP_tSElement *
decodeElement (
  LLRP_tSLibXMLTextDecoderStream * pDecoderStream,
  int                           bAllowMessage,
  int                           bSkipBlanks);


/* the decoder field interface */

static llrp_u8_t
get_u8 (
  LLRP_tSDecoderStream *        pBaseDecoderStream,
  const LLRP_tSFieldDescriptor *pFieldDescriptor);

static llrp_s8_t
get_s8 (
  LLRP_tSDecoderStream *        pBaseDecoderStream,
  const LLRP_tSFieldDescriptor *pFieldDescriptor);

static llrp_u8v_t
get_u8v (
  LLRP_tSDecoderStream *        pBaseDecoderStream,
  const LLRP_tSFieldDescriptor *pFieldDescriptor);

static llrp_s8v_t
get_s8v (
  LLRP_tSDecoderStream *        pBaseDecoderStream,
  const LLRP_tSFieldDescriptor *pFieldDescriptor);

static llrp_u16_t
get_u16 (
  LLRP_tSDecoderStream *        pBaseDecoderStream,
  const LLRP_tSFieldDescriptor *pFieldDescriptor);

static llrp_s16_t
get_s16 (
  LLRP_tSDecoderStream *        pBaseDecoderStream,
  const LLRP_tSFieldDescriptor *pFieldDescriptor);

static llrp_u16v_t
get_u16v (
  LLRP_tSDecoderStream *        pBaseDecoderStream,
  const LLRP_tSFieldDescriptor *pFieldDescriptor);

static llrp_s16v_t
get_s16v (
  LLRP_tSDecoderStream *        pBaseDecoderStream,
  const LLRP_tSFieldDescriptor *pFieldDescriptor);

static llrp_u32_t
get_u32 (
  LLRP_tSDecoderStream *        pBaseDecoderStream,
  const LLRP_tSFieldDescriptor *pFieldDescriptor);

static llrp_s32_t
get_s32 (
  LLRP_tSDecoderStream *        pBaseDecoderStream,
  const LLRP_tSFieldDescriptor *pFieldDescriptor);

static llrp_u32v_t
get_u32v (
  LLRP_tSDecoderStream *        pBaseDecoderStream,
  const LLRP_tSFieldDescriptor *pFieldDescriptor);

static llrp_s32v_t
get_s32v (
  LLRP_tSDecoderStream *        pBaseDecoderStream,
  const LLRP_tSFieldDescriptor *pFieldDescriptor);

static llrp_u64_t
get_u64 (
  LLRP_tSDecoderStream *        pBaseDecoderStream,
  const LLRP_tSFieldDescriptor *pFieldDescriptor);

static llrp_s64_t
get_s64 (
  LLRP_tSDecoderStream *        pBaseDecoderStream,
  const LLRP_tSFieldDescriptor *pFieldDescriptor);

static llrp_u64v_t
get_u64v (
  LLRP_tSDecoderStream *        pBaseDecoderStream,
  const LLRP_tSFieldDescriptor *pFieldDescriptor);

static llrp_s64v_t
get_s64v (
  LLRP_tSDecoderStream *        pBaseDecoderStream,
  const LLRP_tSFieldDescriptor *pFieldDescriptor);

static llrp_u1_t
get_u1 (
  LLRP_tSDecoderStream *        pBaseDecoderStream,
  const LLRP_tSFieldDescriptor *pFieldDescriptor);

static llrp_u1v_t
get_u1v (
  LLRP_tSDecoderStream *        pBaseDecoderStream,
  const LLRP_tSFieldDescriptor *pFieldDescriptor);

static llrp_u2_t
get_u2 (
  LLRP_tSDecoderStream *        pBaseDecoderStream,
  const LLRP_tSFieldDescriptor *pFieldDescriptor);

static llrp_u96_t
get_u96 (
  LLRP_tSDecoderStream *        pBaseDecoderStream,
  const LLRP_tSFieldDescriptor *pFieldDescriptor);

static llrp_utf8v_t
get_utf8v (
  LLRP_tSDecoderStream *        pBaseDecoderStream,
  const LLRP_tSFieldDescriptor *pFieldDescriptor);

static llrp_bytesToEnd_t
get_bytesToEnd (
  LLRP_tSDecoderStream *        pBaseDecoderStream,
  const LLRP_tSFieldDescriptor *pFieldDescriptor);

static llrp_u8v_t
get_e8v (
  LLRP_tSDecoderStream *        pBaseDecoderStream,
  const LLRP_tSFieldDescriptor *pFieldDescriptor);

static int
get_e1 (
  LLRP_tSDecoderStream *        pBaseDecoderStream,
  const LLRP_tSFieldDescriptor *pFieldDescriptor);

static int
get_e2 (
  LLRP_tSDecoderStream *        pBaseDecoderStream,
  const LLRP_tSFieldDescriptor *pFieldDescriptor);

static int
get_e8 (
  LLRP_tSDecoderStream *        pBaseDecoderStream,
  const LLRP_tSFieldDescriptor *pFieldDescriptor);

static int
get_e16 (
  LLRP_tSDecoderStream *        pBaseDecoderStream,
  const LLRP_tSFieldDescriptor *pFieldDescriptor);

static int
get_e32 (
  LLRP_tSDecoderStream *        pBaseDecoderStream,
  const LLRP_tSFieldDescriptor *pFieldDescriptor);

static void
get_reserved (
  LLRP_tSDecoderStream *        pBaseDecoderStream,
  unsigned int                  nBit);

/* some static misc helpers */

static llrp_bool_t
verifyVectorAllocation (
  LLRP_tSLibXMLTextDecoderStream *   pDecoderStream,
  const void *                  pValue,
  const LLRP_tSFieldDescriptor *pFieldDescriptor);

static int
isInsignificantNode (
  xmlNodePtr                    pnode);

/* some class member field helpers */

static int 
getFieldStringPtr(
  LLRP_tSLibXMLTextDecoderStream * pDecoderStream,
  const LLRP_tSFieldDescriptor *pFieldDescriptor,
  const llrp_u8_t **            pbuf,
  const llrp_u8_t **            pend);

static llrp_s64v_t
getSpacedVectorField (
  LLRP_tSLibXMLTextDecoderStream * pDecoderStream,
  const LLRP_tSFieldDescriptor *pFieldDescriptor,
  llrp_s64_t                    minValue,
  llrp_s64_t                    maxValue);

static llrp_s64v_t
getFixedVectorField (
  LLRP_tSLibXMLTextDecoderStream * pDecoderStream,
  const LLRP_tSFieldDescriptor *pFieldDescriptor,
  unsigned int                  vectorSize,
  llrp_s64_t                    minValue,
  llrp_s64_t                    maxValue);

static llrp_s64_t
getIntegerField (
  LLRP_tSLibXMLTextDecoderStream * pDecoderStream,
  const LLRP_tSFieldDescriptor *pFieldDescriptor,
  llrp_s64_t                    minValue,
  llrp_s64_t                    maxValue);

static llrp_s64_t getInteger(
  LLRP_tSLibXMLTextDecoderStream * pDecoderStream,
  const LLRP_tSFieldDescriptor* pFieldDescriptor, 
  const llrp_u8_t *             pbuf,
  const llrp_u8_t *             pend,
  LLRP_tSErrorDetails          *pError,
  llrp_s64_t                    minValue,
  llrp_s64_t                    maxValue);

/* static string helpers */

static const llrp_u8_t *
getSingleChar(
  const llrp_u8_t *             pbuf,
  const llrp_u8_t *             pend,
  llrp_s64_t *                  pValue);

static const llrp_u8_t *
getSingleU1(
  const llrp_u8_t *             pbuf,
  const llrp_u8_t *             pend,
  llrp_s64_t *                  pValue);

static const llrp_u8_t *
getSingleEnum(
  const LLRP_tSFieldDescriptor* pFieldDescriptor, 
  const llrp_u8_t *             pbuf,
  const llrp_u8_t *             pend,
  llrp_s64_t *                  pValue);

static const llrp_u8_t *
getSingleDecimal(
  const llrp_u8_t *             pbuf,
  const llrp_u8_t *             pend,
  llrp_s64_t *                  pValue);

static const llrp_u8_t *
getSingleHexidecimal(
  const llrp_u8_t *             pbuf,
  const llrp_u8_t *             pend,
  llrp_s64_t *                  pValue);

static int
cleanString(
  const llrp_u8_t **            ppbuf,
  const llrp_u8_t **            ppend);

static const llrp_u8_t *
getSingleTimestamp(
  const llrp_u8_t *             pbuf,
  const llrp_u8_t *             pend,
  llrp_s64_t *                  pValue);

static llrp_u16_t 
countElements(
  const char *                  pval, 
  int                           nval);

/*
 * END forward decls
 */


#define MAX_U8      ((1ull << 8u) - 1u)
#define MIN_U8      0ull
#define MAX_S8      ((1ull << 7u) - 1u)
#define MIN_S8      (-1ull - MAX_S8)

#define MAX_U16     ((1ull << 16u) - 1u)
#define MIN_U16     0ull
#define MAX_S16     ((1ull << 15u) - 1u)
#define MIN_S16     (-1ull - MAX_S16)

#define MAX_U32     ((1ull << 32u) - 1u)
#define MIN_U32     0ull
#define MAX_S32     ((1ull << 31u) - 1u)
#define MIN_S32     (-1ull - MAX_S32)

#define MAX_U64     ((1ull << 63u) - 1u)    // one bit lost
#define MIN_U64     0ull
#define MAX_S64     ((1ull << 63u) - 1u)
#define MIN_S64     (-1ull - MAX_S64)

static LLRP_tSDecoderOps
s_LibXMLTextDecoderOps =
{
    .pfDestruct             = decoderDestruct,
    .pfDecodeMessage        = topDecodeMessage,
};

static LLRP_tSDecoderStreamOps
s_LibXMLTextDecoderStreamOps =
{
    .pfGet_u8               = get_u8,
    .pfGet_s8               = get_s8,
    .pfGet_u8v              = get_u8v,
    .pfGet_s8v              = get_s8v,

    .pfGet_u16              = get_u16,
    .pfGet_s16              = get_s16,
    .pfGet_u16v             = get_u16v,
    .pfGet_s16v             = get_s16v,

    .pfGet_u32              = get_u32,
    .pfGet_s32              = get_s32,
    .pfGet_u32v             = get_u32v,
    .pfGet_s32v             = get_s32v,

    .pfGet_u64              = get_u64,
    .pfGet_s64              = get_s64,
    .pfGet_u64v             = get_u64v,
    .pfGet_s64v             = get_s64v,

    .pfGet_u1               = get_u1,
    .pfGet_u1v              = get_u1v,
    .pfGet_u2               = get_u2,
    .pfGet_u96              = get_u96,
    .pfGet_utf8v            = get_utf8v,
    .pfGet_bytesToEnd       = get_bytesToEnd,

    .pfGet_e1               = get_e1,
    .pfGet_e2               = get_e2,
    .pfGet_e8               = get_e8,
    .pfGet_e16              = get_e16,
    .pfGet_e32              = get_e32,
    .pfGet_e8v              = get_e8v,

    .pfGet_reserved         = get_reserved,
};

LLRP_tSLibXMLTextDecoder *
LLRP_LibXMLTextDecoder_construct (
  const LLRP_tSTypeRegistry *   pTypeRegistry,
  unsigned char *               pInput,
  unsigned int                  nInput)
{
    LLRP_tSLibXMLTextDecoder *     pDecoder;

    pDecoder = malloc(sizeof *pDecoder);
    if(NULL == pDecoder)
    {
        return pDecoder;
    }

    memset(pDecoder, 0, sizeof *pDecoder);

    /* set the line numbers for error reporting */
    xmlLineNumbersDefault(1);

    /* Read in the XML from the buffer into the XML Reader */
    pDecoder->doc = xmlReadMemory((char*) pInput, nInput, "noName.xml", NULL, 
                                  XML_PARSE_COMPACT | XML_PARSE_NONET);

    if(NULL == pDecoder->doc)
    {
        fprintf(stderr, "could not parse XML memory buffer");
    /* TODO proper error extraction from library ??? */
        free(pDecoder);
        return NULL;
    }

    /* Initialize the other decoder state variables */
    pDecoder->pxmlNodeTree = xmlDocGetRootElement(pDecoder->doc);

    pDecoder->decoderHdr.pDecoderOps = &s_LibXMLTextDecoderOps;
    pDecoder->decoderHdr.pRegistry = pTypeRegistry;

    return pDecoder;
}

LLRP_tSLibXMLTextDecoder *
LLRP_LibXMLTextDecoder_construct_file (
  const LLRP_tSTypeRegistry *   pTypeRegistry,
  char *                        fname)
{
    LLRP_tSLibXMLTextDecoder *  pDecoder;

    pDecoder = malloc(sizeof *pDecoder);
    if(NULL == pDecoder)
    {
        return pDecoder;
    }

    memset(pDecoder, 0, sizeof *pDecoder);

    /* set the line numbers for error reporting */
    xmlLineNumbersDefault(1);

    /* Read in the XML from the buffer into the XML Reader */
    pDecoder->doc = xmlReadFile(fname, 
                                NULL, 
                                XML_PARSE_COMPACT | XML_PARSE_NONET);

    if(NULL == pDecoder->doc)
    {
        fprintf(stderr, "could not parse XML file ");
    /* TODO proper error extraction from library ??? */
        free(pDecoder);
        return NULL;
    }

    /* Initialize the other decoder state variables */
    pDecoder->pxmlNodeTree = xmlDocGetRootElement(pDecoder->doc);

    pDecoder->decoderHdr.pDecoderOps = &s_LibXMLTextDecoderOps;
    pDecoder->decoderHdr.pRegistry = pTypeRegistry;

    return pDecoder;
}

LLRP_tSLibXMLTextDecoder *
LLRP_LibXMLTextDecoder_construct_nodetree (
  const LLRP_tSTypeRegistry *   pTypeRegistry,
  struct _xmlNode *             pNodeTree)
{
    LLRP_tSLibXMLTextDecoder *  pDecoder;

    if(NULL == pNodeTree)
    {
        return NULL;
    }

    pDecoder = malloc(sizeof *pDecoder);
    if(NULL == pDecoder)
    {
        return pDecoder;
    }

    memset(pDecoder, 0, sizeof *pDecoder);

    /* set the line numbers for error reporting */
    xmlLineNumbersDefault(1);


    /* Initialize the other decoder state variables */
    pDecoder->pxmlNodeTree = pNodeTree;

    pDecoder->decoderHdr.pDecoderOps = &s_LibXMLTextDecoderOps;
    pDecoder->decoderHdr.pRegistry = pTypeRegistry;

    return pDecoder;
}

static void
decoderDestruct (
  LLRP_tSDecoder *              pBaseDecoder)
{
    LLRP_tSLibXMLTextDecoder *  pDecoder =
                                    (LLRP_tSLibXMLTextDecoder*)pBaseDecoder;

    if(NULL != pDecoder->doc)
    {
        if(NULL != pDecoder->doc)
        {
            xmlFreeDoc(pDecoder->doc);    
        }
        pDecoder->pxmlNodeTree = NULL;
    }

    free(pDecoder);
}

static LLRP_tSMessage *
topDecodeMessage (
  LLRP_tSDecoder *              pBaseDecoder)
{
    LLRP_tSLibXMLTextDecoder *     pDecoder;
    LLRP_tSLibXMLTextDecoderStream DecoderStream;
    LLRP_tSMessage *               pMessage;

    pDecoder = (LLRP_tSLibXMLTextDecoder *) pBaseDecoder;

    streamConstruct_outermost(&DecoderStream, pDecoder);

    pMessage = (LLRP_tSMessage *) decodeElement(&DecoderStream, TRUE, TRUE);

    return pMessage;
}

static void
streamConstruct_outermost (
  LLRP_tSLibXMLTextDecoderStream * pDecoderStream,
  LLRP_tSLibXMLTextDecoder *       pDecoder)
{
    memset(pDecoderStream, 0, sizeof *pDecoderStream);
    pDecoderStream->decoderStreamHdr.pDecoderStreamOps =
                                          &s_LibXMLTextDecoderStreamOps;

    pDecoderStream->pDecoder                = pDecoder;
    pDecoderStream->pEnclosingDecoderStream = NULL;
    pDecoderStream->pRefType                = NULL;
    pDecoderStream->pTargetNode             = pDecoder->pxmlNodeTree;
    pDecoderStream->pCurrentChildNode       = NULL;
}

static void
streamConstruct_nested (
  LLRP_tSLibXMLTextDecoderStream * pDecoderStream,
  LLRP_tSLibXMLTextDecoderStream * pEnclosingDecoderStream)
{
    LLRP_tSLibXMLTextDecoder *  pDecoder;

    pDecoder = pEnclosingDecoderStream->pDecoder;

    memset(pDecoderStream, 0, sizeof *pDecoderStream);
    pDecoderStream->decoderStreamHdr.pDecoderStreamOps =
                                          &s_LibXMLTextDecoderStreamOps;

    pDecoderStream->pDecoder                = pDecoder;
    pDecoderStream->pEnclosingDecoderStream = pEnclosingDecoderStream;
    pDecoderStream->pRefType                = pEnclosingDecoderStream->pRefType;
    pDecoderStream->pTargetNode             = pEnclosingDecoderStream->pCurrentChildNode;    
    pDecoderStream->pCurrentChildNode       = NULL;
}

static LLRP_tSElement *
decodeElement (
  LLRP_tSLibXMLTextDecoderStream * pDecoderStream,
  int                           bAllowMessage,
  int                           bSkipBlanks)
{
    LLRP_tSLibXMLTextDecoder *  pDecoder  = pDecoderStream->pDecoder;
    LLRP_tSErrorDetails *       pError    = &pDecoder->decoderHdr.ErrorDetails;
    const LLRP_tSTypeRegistry * pRegistry = pDecoder->decoderHdr.pRegistry;
    LLRP_tSDecoderStream *      pBaseDecoderStream =
                                        &pDecoderStream->decoderStreamHdr;
    const LLRP_tSTypeDescriptor *pTypeDescriptor;
    llrp_u32_t                  MessageID = 0;
    LLRP_tSElement *            pElement;

    if(LLRP_RC_OK != pError->eResultCode)
    {
        return NULL;
    }

    /* get the first interesting node as the target node */
    if(bSkipBlanks)
    {
        while ((NULL != pDecoderStream->pTargetNode) &&
               ( isInsignificantNode(pDecoderStream->pTargetNode) || 
                 xmlIsBlankNode(pDecoderStream->pTargetNode)))
        {
            pDecoderStream->pTargetNode = pDecoderStream->pTargetNode->next;
        }
    }

    /* no interestig target node found. THis is OK as its an empty decode */
    if(NULL == pDecoderStream->pTargetNode)
    {
        return NULL;
    }

    /* first interesting node had better be an element */
    if(pDecoderStream->pTargetNode->type != XML_ELEMENT_NODE)
    {
        /* TODO proper error here. We were expecting an element and got something else */
        pError->eResultCode = LLRP_RC_XMLInvalidNodeType;
        pError->pWhatStr    = "unexpected XML node type";
        pError->pRefType    = pDecoderStream->pRefType;
        pError->pRefField   = NULL;
        pError->OtherDetail = (int) pDecoderStream->pTargetNode->line;
        return NULL;
    }

    XML_NODE_DEBUG(pDecoderStream->pTargetNode);

    /* look up the type descriptor*/
    pTypeDescriptor = LLRP_TypeRegistry_lookupByName(pRegistry,
                                        (char*) pDecoderStream->pTargetNode->name);

    if(NULL == pTypeDescriptor)
    {
        /* TODO good errora */
        pError->eResultCode = LLRP_RC_UnknownParameterType;
        pError->pWhatStr    = "unknown message or parameter type";
        pError->pRefType    = pDecoderStream->pRefType;
        pError->pRefField   = NULL;
        pError->OtherDetail = (int) pDecoderStream->pTargetNode->line;
        return NULL;
    }

    pDecoderStream->pRefType = pTypeDescriptor;

    /* process messages a bit specially */
    if(pTypeDescriptor->bIsMessage)
    {
        xmlChar *               pMessageIDStr;

        if(!bAllowMessage)
        {
            pError->eResultCode = LLRP_RC_MiscError;
            pError->pWhatStr    = "message as subparameter";
            pError->pRefType    = pDecoderStream->pRefType;
            pError->pRefField   = NULL;
            pError->OtherDetail = (int) pDecoderStream->pTargetNode->line;
            return NULL;
        }

        /* get the messageID Attribute */
        pMessageIDStr = xmlGetProp(pDecoderStream->pTargetNode, 
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
                pError->eResultCode = LLRP_RC_MiscError;
                pError->pWhatStr    = "malformed MessageID";
                pError->pRefType    = pDecoderStream->pRefType;
                pError->pRefField   = NULL;
                pError->OtherDetail = (int) pDecoderStream->pTargetNode->line;
                return NULL;
            }
        }
    }

    /* create our element to hold the information */
    pElement = LLRP_Element_construct(pTypeDescriptor);

    if(NULL == pElement)
    {
        pError->eResultCode = LLRP_RC_MessageAllocationFailed;
        pError->pWhatStr    = "element allocation failed";
        pError->pRefType    = pDecoderStream->pRefType;
        pError->pRefField   = NULL;
        pError->OtherDetail = (int) pDecoderStream->pTargetNode->line;
        return NULL;
    }

    /* write the message ID into our message (only if its a message)*/
    if(pTypeDescriptor->bIsMessage)
    {
        ((LLRP_tSMessage *) pElement)->MessageID = MessageID;
    }

    /* now switch to decode the children which are the
     * fields and sub-parameters of this element */
    pDecoderStream->pCurrentChildNode = pDecoderStream->pTargetNode->children;

    /* decode the fields first */
    pTypeDescriptor->pfDecodeFields(pElement, pBaseDecoderStream);

    if(LLRP_RC_OK != pError->eResultCode)
    {
        LLRP_Element_destruct(pElement);
        return NULL;
    }

    /*
     * Subparameters.
     *
     * loop through all the remaining children of the target node and try to find sub-parameters.
     * We are done when we don't have any more or error
     */
    for(; pDecoderStream->pCurrentChildNode != NULL; 
            pDecoderStream->pCurrentChildNode = pDecoderStream->pCurrentChildNode->next)
    {
        LLRP_tSLibXMLTextDecoderStream  NestStream;
        LLRP_tSElement *                pSubElement;
        LLRP_tSParameter *              pParameter;

        if(isInsignificantNode(pDecoderStream->pCurrentChildNode))
        {
                continue;
        }

        if(xmlIsBlankNode(pDecoderStream->pCurrentChildNode))
        {
                continue;
        }

        streamConstruct_nested(&NestStream, pDecoderStream);

        pSubElement = decodeElement(&NestStream, FALSE, FALSE);

        /* nothing more to decode as a sub-element or failure */
        if(NULL == pSubElement)
        {
            break;
        }

        pParameter = (LLRP_tSParameter *) pSubElement;

        pParameter->elementHdr.pParent = pElement;
        LLRP_Element_addSubParameterToAllList(pElement, pParameter);
    }

    if(LLRP_RC_OK != pError->eResultCode)
    {
        LLRP_Element_destruct(pElement);
        return NULL;
    }

    pTypeDescriptor->pfAssimilateSubParameters(pElement, pError);

    if(LLRP_RC_OK != pError->eResultCode)
    {
        LLRP_Element_destruct(pElement);
        return NULL;
    }

    return pElement;
}

/* determine whether we can ingore node always */
static int
isInsignificantNode (
  xmlNodePtr                    pnode)
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

        /* not sure yet about these TODO */
        case XML_ELEMENT_DECL:
        case XML_ATTRIBUTE_DECL:
        case XML_NAMESPACE_DECL:

        /* anything new we should error */
        default:
            return 0;
    }
}

static llrp_u8_t
get_u8 (
  LLRP_tSDecoderStream *        pBaseDecoderStream,
  const LLRP_tSFieldDescriptor *pFieldDescriptor)
{
    LLRP_tSLibXMLTextDecoderStream *pDecoderStream;
    llrp_u8_t                   Value;

    pDecoderStream = (LLRP_tSLibXMLTextDecoderStream *) pBaseDecoderStream;

    Value = (llrp_u8_t) getIntegerField(pDecoderStream, pFieldDescriptor,
                            MIN_U8, MAX_U8);

    return Value;
}

static llrp_s8_t
get_s8 (
  LLRP_tSDecoderStream *        pBaseDecoderStream,
  const LLRP_tSFieldDescriptor *pFieldDescriptor)
{
    LLRP_tSLibXMLTextDecoderStream *pDecoderStream;
    llrp_s8_t                   Value;

    pDecoderStream = (LLRP_tSLibXMLTextDecoderStream *) pBaseDecoderStream;

    Value = (llrp_s8_t) getIntegerField(pDecoderStream, pFieldDescriptor,
                            MIN_S8, MAX_S8);

    return Value;
}

static llrp_u8v_t
get_u8v (
  LLRP_tSDecoderStream *        pBaseDecoderStream,
  const LLRP_tSFieldDescriptor *pFieldDescriptor)
{
    LLRP_tSLibXMLTextDecoderStream *pDecoderStream;
    LLRP_tSErrorDetails *       pError;
    llrp_u8v_t                  Value;
    llrp_s64v_t                 Tmp;

    pDecoderStream = (LLRP_tSLibXMLTextDecoderStream *) pBaseDecoderStream;
    pError = &pDecoderStream->pDecoder->decoderHdr.ErrorDetails;

    memset(&Value, 0, sizeof Value);

    switch(pFieldDescriptor->eFieldFormat)
    {
        /* assume normal format here is decimal */
        case LLRP_FMT_NORMAL:
        case LLRP_FMT_DEC:
        Tmp = getSpacedVectorField(pDecoderStream, pFieldDescriptor,
                                       MIN_U8, MAX_U8);
        break;
        case LLRP_FMT_HEX:
        Tmp = getFixedVectorField(pDecoderStream, pFieldDescriptor,
                                      2, MIN_U8, MAX_U8);	    
        break;
        default:
        case LLRP_FMT_UTF8:
        case LLRP_FMT_DATETIME:
            if(NULL != pError)
            {
                pError->eResultCode = LLRP_RC_MiscError;
                pError->pWhatStr    = "Format type not support for field";
                pError->pRefType    = pDecoderStream->pRefType;
                pError->pRefField   = pFieldDescriptor;
                pError->OtherDetail = (int) pDecoderStream->pCurrentChildNode->line;
            }	    
        break;
    }    

    if(Tmp.nValue)
    {
        llrp_u16_t              nValue;

        nValue = Tmp.nValue;
        Value = LLRP_u8v_construct(nValue);
        if(verifyVectorAllocation(pDecoderStream, Value.pValue,
                                  pFieldDescriptor))
        {
            unsigned int    Ix;

            for(Ix = 0; Ix < nValue; Ix++)
            {
                Value.pValue[Ix] = (llrp_u8_t) Tmp.pValue[Ix];
            }
        }
        LLRP_s64v_clear(&Tmp);
    }

    return Value;
}

static llrp_s8v_t
get_s8v (
  LLRP_tSDecoderStream *        pBaseDecoderStream,
  const LLRP_tSFieldDescriptor *pFieldDescriptor)
{
   LLRP_tSLibXMLTextDecoderStream *pDecoderStream;
    LLRP_tSErrorDetails *       pError;
    llrp_s8v_t                  Value;
    llrp_s64v_t                 Tmp;

    pDecoderStream = (LLRP_tSLibXMLTextDecoderStream *) pBaseDecoderStream;
    pError = &pDecoderStream->pDecoder->decoderHdr.ErrorDetails;

    memset(&Value, 0, sizeof Value);
     switch(pFieldDescriptor->eFieldFormat)
    {
        /* assume normal format here is decimal */
        case LLRP_FMT_NORMAL:
        case LLRP_FMT_DEC:
        Tmp = getSpacedVectorField(pDecoderStream, pFieldDescriptor,
                                   MIN_S8, MAX_S8);
        break;
        case LLRP_FMT_HEX:
        Tmp = getFixedVectorField(pDecoderStream, pFieldDescriptor,
                                  2, MIN_S8, MAX_S8);	    
        break;
        default:
        case LLRP_FMT_UTF8:
        case LLRP_FMT_DATETIME:
            if(NULL != pError)
            {
                pError->eResultCode = LLRP_RC_MiscError;
                pError->pWhatStr    = "Format type not support for field";
                pError->pRefType    = pDecoderStream->pRefType;
                pError->pRefField   = pFieldDescriptor;
                pError->OtherDetail = (int) pDecoderStream->pCurrentChildNode->line;
            }	    
        break;
    }    
    if(Tmp.nValue)
    {
        llrp_u16_t              nValue;

        nValue = Tmp.nValue;
        Value = LLRP_s8v_construct(nValue);
        if(verifyVectorAllocation(pDecoderStream, Value.pValue,
                                  pFieldDescriptor))
        {
            unsigned int    Ix;

            for(Ix = 0; Ix < nValue; Ix++)
            {
                Value.pValue[Ix] = (llrp_s8_t) Tmp.pValue[Ix];
            }
        }
        LLRP_s64v_clear(&Tmp);
    }

    return Value;
}

static llrp_u16_t
get_u16 (
  LLRP_tSDecoderStream *        pBaseDecoderStream,
  const LLRP_tSFieldDescriptor *pFieldDescriptor)
{
    LLRP_tSLibXMLTextDecoderStream *pDecoderStream;
    llrp_u16_t                  Value;

    pDecoderStream = (LLRP_tSLibXMLTextDecoderStream *) pBaseDecoderStream;

    Value = (llrp_u16_t) getIntegerField(pDecoderStream, pFieldDescriptor,
                                         MIN_U16, MAX_U16);

    return Value;
}

static llrp_s16_t
get_s16 (
  LLRP_tSDecoderStream *        pBaseDecoderStream,
  const LLRP_tSFieldDescriptor *pFieldDescriptor)
{
    LLRP_tSLibXMLTextDecoderStream *pDecoderStream;
    llrp_s16_t                  Value;

    pDecoderStream = (LLRP_tSLibXMLTextDecoderStream *) pBaseDecoderStream;

    Value = (llrp_s16_t) getIntegerField(pDecoderStream, pFieldDescriptor,
                                         MIN_S16, MAX_S16);

    return Value;
}

static llrp_u16v_t
get_u16v (
  LLRP_tSDecoderStream *        pBaseDecoderStream,
  const LLRP_tSFieldDescriptor *pFieldDescriptor)
{
    LLRP_tSLibXMLTextDecoderStream *pDecoderStream;
    llrp_u16v_t                  Value;
    pDecoderStream = (LLRP_tSLibXMLTextDecoderStream *) pBaseDecoderStream;
    llrp_s64v_t                 Tmp;

    memset(&Value, 0, sizeof Value);

    Tmp = getSpacedVectorField(pDecoderStream, pFieldDescriptor,
                               MIN_U16, MAX_U16);
    if(Tmp.nValue)
    {
        llrp_u16_t              nValue;

        nValue = Tmp.nValue;
        Value = LLRP_u16v_construct(nValue);
        if(verifyVectorAllocation(pDecoderStream, Value.pValue,
                                  pFieldDescriptor))
        {
            unsigned int    Ix;

            for(Ix = 0; Ix < nValue; Ix++)
            {
                Value.pValue[Ix] = (llrp_u16_t) Tmp.pValue[Ix];
            }
        }
        LLRP_s64v_clear(&Tmp);
    }

    return Value;
}

static llrp_s16v_t
get_s16v (
  LLRP_tSDecoderStream *        pBaseDecoderStream,
  const LLRP_tSFieldDescriptor *pFieldDescriptor)
{
    LLRP_tSLibXMLTextDecoderStream *pDecoderStream;
    llrp_s16v_t                  Value;
    pDecoderStream = (LLRP_tSLibXMLTextDecoderStream *) pBaseDecoderStream;
    llrp_s64v_t                 Tmp;

    memset(&Value, 0, sizeof Value);

    Tmp = getSpacedVectorField(pDecoderStream, pFieldDescriptor,
                               MIN_S16, MAX_S16);
    if(Tmp.nValue)
    {
        llrp_u16_t              nValue;

        nValue = Tmp.nValue;
        Value = LLRP_s16v_construct(nValue);
        if(verifyVectorAllocation(pDecoderStream, Value.pValue,
                                  pFieldDescriptor))
        {
            unsigned int    Ix;

            for(Ix = 0; Ix < nValue; Ix++)
            {
                Value.pValue[Ix] = (llrp_s16_t) Tmp.pValue[Ix];
            }
        }
        LLRP_s64v_clear(&Tmp);
    }

    return Value;
}

static llrp_u32_t
get_u32 (
  LLRP_tSDecoderStream *        pBaseDecoderStream,
  const LLRP_tSFieldDescriptor *pFieldDescriptor)
{
    LLRP_tSLibXMLTextDecoderStream *pDecoderStream;
    llrp_u32_t                  Value;

    pDecoderStream = (LLRP_tSLibXMLTextDecoderStream *) pBaseDecoderStream;

    Value = (llrp_u32_t) getIntegerField(pDecoderStream, pFieldDescriptor,
                                         MIN_U32, MAX_U32);

    return Value;
}

static llrp_s32_t
get_s32 (
  LLRP_tSDecoderStream *        pBaseDecoderStream,
  const LLRP_tSFieldDescriptor *pFieldDescriptor)
{
    LLRP_tSLibXMLTextDecoderStream *pDecoderStream;
    llrp_s32_t                  Value;

    pDecoderStream = (LLRP_tSLibXMLTextDecoderStream *) pBaseDecoderStream;

    Value = (llrp_s32_t) getIntegerField(pDecoderStream, pFieldDescriptor,
                                         MIN_S32, MAX_S32);

    return Value;
}

static llrp_u32v_t
get_u32v (
  LLRP_tSDecoderStream *        pBaseDecoderStream,
  const LLRP_tSFieldDescriptor *pFieldDescriptor)
{
    LLRP_tSLibXMLTextDecoderStream *pDecoderStream;
    llrp_u32v_t                 Value;
    pDecoderStream = (LLRP_tSLibXMLTextDecoderStream *) pBaseDecoderStream;
    llrp_s64v_t                 Tmp;

    memset(&Value, 0, sizeof Value);

    Tmp = getSpacedVectorField(pDecoderStream, pFieldDescriptor,
                               MIN_U32, MAX_U32);
    if(Tmp.nValue)
    {
        llrp_u16_t              nValue;

        nValue = Tmp.nValue;
        Value = LLRP_u32v_construct(nValue);
        if(verifyVectorAllocation(pDecoderStream, Value.pValue,
                                  pFieldDescriptor))
        {
            unsigned int    Ix;

            for(Ix = 0; Ix < nValue; Ix++)
            {
                Value.pValue[Ix] = (llrp_u32_t) Tmp.pValue[Ix];
            }
        }
        LLRP_s64v_clear(&Tmp);
    }

    return Value;
}

static llrp_s32v_t
get_s32v (
  LLRP_tSDecoderStream *        pBaseDecoderStream,
  const LLRP_tSFieldDescriptor *pFieldDescriptor)
{
    LLRP_tSLibXMLTextDecoderStream *pDecoderStream;
    llrp_s32v_t                 Value;
    pDecoderStream = (LLRP_tSLibXMLTextDecoderStream *) pBaseDecoderStream;
    llrp_s64v_t                 Tmp;

    memset(&Value, 0, sizeof Value);

    Tmp = getSpacedVectorField(pDecoderStream, pFieldDescriptor,
                               MIN_S32, MAX_S32);
    if(Tmp.nValue)
    {
        llrp_u16_t              nValue;

        nValue = Tmp.nValue;
        Value = LLRP_s32v_construct(nValue);
        if(verifyVectorAllocation(pDecoderStream, Value.pValue,
                                  pFieldDescriptor))
        {
            unsigned int    Ix;

            for(Ix = 0; Ix < nValue; Ix++)
            {
                Value.pValue[Ix] = (llrp_s32_t) Tmp.pValue[Ix];
            }
        }
        LLRP_s64v_clear(&Tmp);
    }

    return Value;
}

static llrp_u64_t
get_u64 (
  LLRP_tSDecoderStream *        pBaseDecoderStream,
  const LLRP_tSFieldDescriptor *pFieldDescriptor)
{
    LLRP_tSLibXMLTextDecoderStream *pDecoderStream;
    llrp_u64_t                  Value;

    pDecoderStream = (LLRP_tSLibXMLTextDecoderStream *) pBaseDecoderStream;

    Value = (llrp_u64_t) getIntegerField(pDecoderStream, pFieldDescriptor,
                                         MIN_U64, MAX_U64);

    return Value;
}

static llrp_s64_t
get_s64 (
  LLRP_tSDecoderStream *        pBaseDecoderStream,
  const LLRP_tSFieldDescriptor *pFieldDescriptor)
{
    LLRP_tSLibXMLTextDecoderStream *pDecoderStream;
    llrp_s64_t                  Value;

    pDecoderStream = (LLRP_tSLibXMLTextDecoderStream *) pBaseDecoderStream;

    Value = (llrp_s64_t) getIntegerField(pDecoderStream, pFieldDescriptor,
                                         MIN_S64, MAX_S64);

    return Value;
}

static llrp_u64v_t
get_u64v (
  LLRP_tSDecoderStream *        pBaseDecoderStream,
  const LLRP_tSFieldDescriptor *pFieldDescriptor)
{
    LLRP_tSLibXMLTextDecoderStream *pDecoderStream;
    llrp_u64v_t                 Value;
    pDecoderStream = (LLRP_tSLibXMLTextDecoderStream *) pBaseDecoderStream;
    llrp_s64v_t                 Tmp;


    memset(&Value, 0, sizeof Value);

    Tmp = getSpacedVectorField(pDecoderStream, pFieldDescriptor,
                               MIN_U64, MAX_U64);
    if(Tmp.nValue)
    {
        llrp_u16_t              nValue;

        nValue = Tmp.nValue;
        Value = LLRP_u64v_construct(nValue);
        if(verifyVectorAllocation(pDecoderStream, Value.pValue,
                            pFieldDescriptor))
        {
            unsigned int    Ix;

            for(Ix = 0; Ix < nValue; Ix++)
            {
                Value.pValue[Ix] = (llrp_u64_t) Tmp.pValue[Ix];
            }
        }
        LLRP_s64v_clear(&Tmp);
    }

    return Value;
}

static llrp_s64v_t
get_s64v (
  LLRP_tSDecoderStream *        pBaseDecoderStream,
  const LLRP_tSFieldDescriptor *pFieldDescriptor)
{
    LLRP_tSLibXMLTextDecoderStream *pDecoderStream;
    llrp_s64v_t                  Value;
    pDecoderStream = (LLRP_tSLibXMLTextDecoderStream *) pBaseDecoderStream;


    memset(&Value, 0, sizeof Value);

    Value = getSpacedVectorField(pDecoderStream, pFieldDescriptor,
                                 MIN_S64, MAX_S64);
 
    return Value;
}

static llrp_u1_t
get_u1 (
  LLRP_tSDecoderStream *        pBaseDecoderStream,
  const LLRP_tSFieldDescriptor *pFieldDescriptor)
{
    LLRP_tSLibXMLTextDecoderStream *pDecoderStream;
    llrp_u1_t                   Value;

    pDecoderStream = (LLRP_tSLibXMLTextDecoderStream *) pBaseDecoderStream;

    Value = (llrp_u1_t) getIntegerField(pDecoderStream, pFieldDescriptor,
                                        0, 1);

    return Value;
}

static llrp_u1v_t
get_u1v (
  LLRP_tSDecoderStream *        pBaseDecoderStream,
  const LLRP_tSFieldDescriptor *pFieldDescriptor)
{
    LLRP_tSLibXMLTextDecoderStream *pDecoderStream;
    LLRP_tSErrorDetails *       pError;
    llrp_u1v_t                  Value;
    llrp_s64v_t                 Tmp;

    memset(&Value, 0, sizeof Value);

    pDecoderStream = (LLRP_tSLibXMLTextDecoderStream *) pBaseDecoderStream;
    pError = &pDecoderStream->pDecoder->decoderHdr.ErrorDetails;
    Tmp = getFixedVectorField(pDecoderStream, pFieldDescriptor, 2, 0, 255);

    if(Tmp.nValue)
    {
        llrp_u16_t              nValue;

        nValue = Tmp.nValue;
        Value = LLRP_u1v_construct(nValue*8);
        if(verifyVectorAllocation(pDecoderStream, Value.pValue,
                                  pFieldDescriptor))
        {
            unsigned int        Ix;
            xmlChar *           pCountStr;

            for(Ix = 0; Ix < nValue; Ix++)
            {
                Value.pValue[Ix] = (llrp_u1_t) Tmp.pValue[Ix];
            }
            /* bit length could be shorter than this. Use this if there
             * is no attribute */
            Value.nBit = Tmp.nValue * 8;

            /* find the XML attribute for Count and fix the array */
            if(NULL != pDecoderStream->pLastFieldNode)
            {
                pCountStr = xmlGetProp(pDecoderStream->pLastFieldNode, 
                                       (xmlChar*) "Count");
                if(NULL != pCountStr)
                {
                    char *      pArg = (char *) pCountStr;
                    char *      pTail = pArg + strlen((char*) pCountStr);
                    llrp_u16_t  nBits;

                    nBits = strtoul(pArg, &pTail, 10);
                    if( (*pTail != 0) ||
                        (nBits > (Tmp.nValue * 8)))
                    {
                        /* malformed */
                        pError->eResultCode = LLRP_RC_MiscError;
                        pError->pWhatStr    = "malformed Count Attribute";
                        pError->pRefType    = pDecoderStream->pRefType;
                        pError->pRefField   = pFieldDescriptor;
                        pError->OtherDetail =
                        (int) pDecoderStream->pLastFieldNode->line;
                    }                
                    else
                    {
                        llrp_u16_t lastByteBits;
                        llrp_u8_t  lastByteMask;
                                    
                        Value.nBit = nBits;			

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
                            Value.pValue[Ix] &= lastByteMask;
                        }
                    }
                    xmlFree(pCountStr);
                }
            }
        }
        LLRP_s64v_clear(&Tmp);
    }

    return Value;
}

static llrp_u2_t
get_u2 (
  LLRP_tSDecoderStream *        pBaseDecoderStream,
  const LLRP_tSFieldDescriptor *pFieldDescriptor)
{
    LLRP_tSLibXMLTextDecoderStream *pDecoderStream;
    llrp_u2_t                   Value;

    pDecoderStream = (LLRP_tSLibXMLTextDecoderStream *) pBaseDecoderStream;

    Value = (llrp_u2_t) getIntegerField(pDecoderStream, pFieldDescriptor,
                                        0, 3);
    return Value;
}

static llrp_u96_t
get_u96 (
  LLRP_tSDecoderStream *        pBaseDecoderStream,
  const LLRP_tSFieldDescriptor *pFieldDescriptor)
{
    LLRP_tSLibXMLTextDecoderStream *pDecoderStream;
    LLRP_tSErrorDetails *       pError; 
    llrp_u96_t                  Value;
    llrp_s64v_t                 Tmp;
    unsigned int    Ix;

    pDecoderStream = (LLRP_tSLibXMLTextDecoderStream *) pBaseDecoderStream;
    pError = &pDecoderStream->pDecoder->decoderHdr.ErrorDetails;

    memset(&Value, 0, sizeof Value);

    Tmp = getFixedVectorField(pDecoderStream, pFieldDescriptor,
                              2, 0, 255);

    if((LLRP_RC_OK != pError->eResultCode) && (Tmp.nValue != 12))
    {
        if(NULL != pError)
        {
            pError->eResultCode = LLRP_RC_MiscError;
            pError->pWhatStr    = "Illegal length u96 field";
            pError->pRefType    = pDecoderStream->pRefType;
            pError->pRefField   = pFieldDescriptor;
            pError->OtherDetail = (int) pDecoderStream->pCurrentChildNode->line;

            LLRP_s64v_clear(&Tmp);
            return Value;
        }
    }

    /* convert to this type */
    for(Ix = 0; Ix < Tmp.nValue; Ix++)
    {
        Value.aValue[Ix] = Tmp.pValue[Ix];
    }
  
    LLRP_s64v_clear(&Tmp);
    return Value;
}

/* gets a utf8 string from the decoder stream.
 * NOTE: there is only one format for utf8 strings 
 */
static llrp_utf8v_t
get_utf8v (
  LLRP_tSDecoderStream *        pBaseDecoderStream,
  const LLRP_tSFieldDescriptor *pFieldDescriptor)
{
    LLRP_tSLibXMLTextDecoderStream *pDecoderStream;
    llrp_utf8v_t                Value;
    llrp_u16_t                  nValue;
    const llrp_u8_t *           pbuf;
    const llrp_u8_t *           pend;

    pDecoderStream = (LLRP_tSLibXMLTextDecoderStream *) pBaseDecoderStream;
    if(getFieldStringPtr(pDecoderStream, pFieldDescriptor, &pbuf, &pend))
    {
        nValue = strlen((char*) pbuf);
        Value = LLRP_utf8v_construct(nValue);

        if(verifyVectorAllocation(pDecoderStream, Value.pValue,
                                  pFieldDescriptor))
        {
            unsigned int    Ix;

            for(Ix = 0; Ix < nValue; Ix++)
            {
                Value.pValue[Ix] = pbuf[Ix];
            }
        }    
    }
    else
    {
        /* its valid for UTF8 strings to be empty */
        Value.nValue=0;
        Value.pValue=NULL;
    }
    return Value;
}

static llrp_bytesToEnd_t
get_bytesToEnd (
  LLRP_tSDecoderStream *        pBaseDecoderStream,
  const LLRP_tSFieldDescriptor *pFieldDescriptor)
{
    LLRP_tSLibXMLTextDecoderStream *pDecoderStream;
    llrp_bytesToEnd_t           Value;
    llrp_s64v_t                 Tmp;
    unsigned int                Ix;

    memset(&Value, 0, sizeof Value);

    pDecoderStream = (LLRP_tSLibXMLTextDecoderStream *) pBaseDecoderStream;
    Tmp = getFixedVectorField(pDecoderStream, pFieldDescriptor,
                              2, 0, 255);

    if(verifyVectorAllocation(pDecoderStream, Tmp.pValue,
                              pFieldDescriptor))
    {
        /* build and convert */
        Value = LLRP_bytesToEnd_construct(Tmp.nValue);
        if(verifyVectorAllocation(pDecoderStream, Tmp.pValue,
                                  pFieldDescriptor))
        {
            /* convert to this type */
            for(Ix = 0; Ix < Tmp.nValue; Ix++)
            {
                Value.pValue[Ix] = Tmp.pValue[Ix];
            }
            Value.nValue = Tmp.nValue;
        }
    }
  
    LLRP_s64v_clear(&Tmp);
    return Value;
}

static int
get_e1 (
  LLRP_tSDecoderStream *        pBaseDecoderStream,
  const LLRP_tSFieldDescriptor *pFieldDescriptor)
{
    LLRP_tSLibXMLTextDecoderStream *pDecoderStream;
    int                         Value;

    pDecoderStream = (LLRP_tSLibXMLTextDecoderStream *) pBaseDecoderStream;

    Value = (int) getIntegerField(pDecoderStream, pFieldDescriptor,
                                  0, 1);

    return Value;
}

static int
get_e2 (
  LLRP_tSDecoderStream *        pBaseDecoderStream,
  const LLRP_tSFieldDescriptor *pFieldDescriptor)
{
    LLRP_tSLibXMLTextDecoderStream *pDecoderStream;
    int                         Value;

    pDecoderStream = (LLRP_tSLibXMLTextDecoderStream *) pBaseDecoderStream;

    Value = (int) getIntegerField(pDecoderStream, pFieldDescriptor,
                                  0, 3);

    return Value;
}

static int
get_e8 (
  LLRP_tSDecoderStream *        pBaseDecoderStream,
  const LLRP_tSFieldDescriptor *pFieldDescriptor)
{
    LLRP_tSLibXMLTextDecoderStream *pDecoderStream;
    int                         Value;

    pDecoderStream = (LLRP_tSLibXMLTextDecoderStream *) pBaseDecoderStream;

    Value = (int) getIntegerField(pDecoderStream, pFieldDescriptor,
                                  MIN_U8, MAX_U8);

    return Value;
}

static int
get_e16 (
  LLRP_tSDecoderStream *        pBaseDecoderStream,
  const LLRP_tSFieldDescriptor *pFieldDescriptor)
{
    LLRP_tSLibXMLTextDecoderStream *pDecoderStream;
    int                         Value;

    pDecoderStream = (LLRP_tSLibXMLTextDecoderStream *) pBaseDecoderStream;

    Value = (int) getIntegerField(pDecoderStream, pFieldDescriptor,
                                  MIN_U16, MAX_U16);

    return Value;
}

static int
get_e32 (
  LLRP_tSDecoderStream *        pBaseDecoderStream,
  const LLRP_tSFieldDescriptor *pFieldDescriptor)
{
    LLRP_tSLibXMLTextDecoderStream *pDecoderStream;
    int                         Value;

    pDecoderStream = (LLRP_tSLibXMLTextDecoderStream *) pBaseDecoderStream;

    Value = (int) getIntegerField(pDecoderStream, pFieldDescriptor,
                                  MIN_U32, MAX_U32);

    return Value;
}


static llrp_u8v_t
get_e8v (
  LLRP_tSDecoderStream *        pBaseDecoderStream,
  const LLRP_tSFieldDescriptor *pFieldDescriptor)
{
    LLRP_tSLibXMLTextDecoderStream *pDecoderStream;
    llrp_u8v_t                  Value;
    pDecoderStream = (LLRP_tSLibXMLTextDecoderStream *) pBaseDecoderStream;
    llrp_s64v_t                 Tmp;

    memset(&Value, 0, sizeof Value);

    Tmp = getSpacedVectorField(pDecoderStream, pFieldDescriptor,
                               MIN_U8, MAX_U8);
    if(Tmp.nValue)
    {
        llrp_u16_t              nValue;

        nValue = Tmp.nValue;
        Value = LLRP_u8v_construct(nValue);
        if(verifyVectorAllocation(pDecoderStream, Value.pValue,
                                  pFieldDescriptor))
        {
            unsigned int    Ix;

            for(Ix = 0; Ix < nValue; Ix++)
            {
                Value.pValue[Ix] = (llrp_u8_t) Tmp.pValue[Ix];
            }
        }
        LLRP_s64v_clear(&Tmp);
    }

    return Value;
}


static void
get_reserved (
  LLRP_tSDecoderStream *        pBaseDecoderStream,
  unsigned int                  nBit)
{
    return;
}

static int 
getFieldStringPtr(
  LLRP_tSLibXMLTextDecoderStream * pDecoderStream,
  const LLRP_tSFieldDescriptor *pFieldDescriptor,
  const llrp_u8_t **            pbuf,
  const llrp_u8_t **            pend)
{
    LLRP_tSErrorDetails *       pError = &pDecoderStream->pDecoder->decoderHdr.ErrorDetails;
    xmlNode *pSave;

    *pbuf = NULL;
    *pend = NULL;

    /* clear this out as its not valid anymore */
    pDecoderStream->pLastFieldNode = NULL;

    if(LLRP_RC_OK != pError->eResultCode)
    {
        return 0;
    }

   /* get the first interesting node */
    while ((NULL != pDecoderStream->pCurrentChildNode) &&
           (isInsignificantNode(pDecoderStream->pCurrentChildNode) || 
            xmlIsBlankNode(pDecoderStream->pCurrentChildNode)))
    {
        pDecoderStream->pCurrentChildNode = pDecoderStream->pCurrentChildNode->next;
    }    

    if(NULL == pDecoderStream->pCurrentChildNode)
    {
        pError->eResultCode = LLRP_RC_FieldUnderrun;
        pError->pWhatStr    = "underrun at field";
        pError->pRefType    = pDecoderStream->pRefType;
        pError->pRefField   = pFieldDescriptor;
        pError->OtherDetail = (int) pDecoderStream->pCurrentChildNode->line;
        return 0;
    }

    XML_NODE_DEBUG(pDecoderStream->pCurrentChildNode);

    /* first interesting node had better be an element */
    if(pDecoderStream->pCurrentChildNode->type != XML_ELEMENT_NODE)
    {
        /* TODO propeo error here. We were expecting an element and got something else */
        pError->eResultCode = LLRP_RC_XMLInvalidNodeType;
        pError->pWhatStr    = "unexpected field value";
        pError->pRefType    = pDecoderStream->pRefType;
        pError->pRefField   = pFieldDescriptor;
        pError->OtherDetail = (int) pDecoderStream->pCurrentChildNode->line;	    
        return 0;
    }

    /* better match our field value */
    if(0 != strcmp((char*) pDecoderStream->pCurrentChildNode->name,  
                    pFieldDescriptor->pName))
    {
        /* TODO propeo error here. We were expecting an element and got something else */
        pError->eResultCode = LLRP_RC_XMLMissingField;
        pError->pWhatStr    = "missing field value";
        pError->pRefType    = pDecoderStream->pRefType;
        pError->pRefField   = pFieldDescriptor;
        pError->OtherDetail = (int) pDecoderStream->pCurrentChildNode->line;	    
        return 0;
    }

    /* save this in case someone wants to explore attributes */
    pDecoderStream->pLastFieldNode = pDecoderStream->pCurrentChildNode;

    /* use this to decode the field values */
    pSave = pDecoderStream->pCurrentChildNode->children;

    /* advance to the next one for the next field */
    pDecoderStream->pCurrentChildNode = pDecoderStream->pCurrentChildNode->next;

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
        pError->eResultCode = LLRP_RC_XMLInvalidNodeType;
        pError->pWhatStr    = "invalid XML Node found during field decode";
        pError->pRefType    = pDecoderStream->pRefType;
        pError->pRefField   = pFieldDescriptor;
        pError->OtherDetail = (int) pSave->line;
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
        pError->eResultCode = LLRP_RC_XMLExtraNode;
        pError->pWhatStr    = "extra XML node found";
        pError->pRefType    = pDecoderStream->pRefType;
        pError->pRefField   = pFieldDescriptor;
        pError->OtherDetail = (int) pSave->line;
        return 0;	 
    }

    *pend = *pbuf + strlen((char*) *pbuf);
    return 1;
}

static llrp_s64_t
getIntegerField (
  LLRP_tSLibXMLTextDecoderStream * pDecoderStream,
  const LLRP_tSFieldDescriptor *pFieldDescriptor,
  llrp_s64_t                    minValue,
  llrp_s64_t                    maxValue)
{
    LLRP_tSErrorDetails *       pError = &pDecoderStream->pDecoder->decoderHdr.ErrorDetails;
    const llrp_u8_t *           pbuf;
    const llrp_u8_t *           pend;
    llrp_s64_t                  Value;

    memset(&Value, 0x00, sizeof(Value));

    if(LLRP_RC_OK != pError->eResultCode)
    {
        /* already errd */
        return Value;
    }

    if(getFieldStringPtr(pDecoderStream, pFieldDescriptor, &pbuf, &pend))
    {
        cleanString(&pbuf, &pend);
        Value = getInteger(pDecoderStream,
                           pFieldDescriptor, 
                           pbuf, 
                           pend, 
                           pError, 
                           minValue, 
                           maxValue);
    }
    else
    {
        pError->eResultCode = LLRP_RC_FieldUnderrun;
        pError->pWhatStr    = "underrun at field no characters";
        pError->pRefType    = pDecoderStream->pRefType;
        pError->pRefField   = pFieldDescriptor;
        pError->OtherDetail = (int) pDecoderStream->pCurrentChildNode->line; 
        return 0;	
    }


    
    return Value;
}

static llrp_u16_t 
countElements(
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

/* gets a single U1 value from a non-NULL terminated buffer buffer.
 * If it fails to get one, reutrn value should == pbuf, other
 * wise EndPtr should point to one past amount consumed */
static const llrp_u8_t *
getSingleU1(
  const llrp_u8_t *             pbuf,
  const llrp_u8_t *             pend,
  llrp_s64_t *                  pValue)
{
    const llrp_u8_t *           endPtr = pbuf;
    const int                   len = pend - pbuf;

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

static const llrp_u8_t *
getSingleEnum(
  const LLRP_tSFieldDescriptor* pFieldDescriptor, 
  const llrp_u8_t *             pbuf,
  const llrp_u8_t *             pend,
  llrp_s64_t *                  pValue)
{
    const llrp_u8_t *           endPtr = pbuf;
    const int                   length = pend - pbuf;
    const LLRP_tSEnumTableEntry *pEntry;    

    *pValue = 0;

    for(
        pEntry = pFieldDescriptor->pEnumTable;
        NULL != pEntry->pName;
        pEntry++)
    {
        int len = strlen(pEntry->pName);
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

static const llrp_u8_t *
getSingleDecimal(
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

static const llrp_u8_t *
getSingleHexidecimal(
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

/* clean string and returns the length */
static int
cleanString(
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

static const llrp_u8_t *
getSingleTimestamp(
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

/* gets a single value from a non-null teminated string. 
 * NOTE: It may be NULL terminated or it may have a space. pEnd
 * points to the character after the end of the string (i.e. next
 * separator or null */
static llrp_s64_t getInteger(
  LLRP_tSLibXMLTextDecoderStream * pDecoderStream,
  const LLRP_tSFieldDescriptor* pFieldDescriptor, 
  const llrp_u8_t *             pbuf,
  const llrp_u8_t *             pend,
  LLRP_tSErrorDetails          *pError,
  llrp_s64_t                    minValue,
  llrp_s64_t                    maxValue)
{
    llrp_s64_t                  Value;
    const llrp_u8_t           * endPtr = NULL;

    memset(&Value, 0x00, sizeof(Value));

    if(pFieldDescriptor->eFieldType == LLRP_FT_U1)
    /* special case for U1 types as they can be
     * 'true' and 'false' strings */
    {
        endPtr = getSingleU1(pbuf, pend, &Value);
    }
    else if((pFieldDescriptor->eFieldType == LLRP_FT_E1 ) || 
            (pFieldDescriptor->eFieldType == LLRP_FT_E2 ) ||
            (pFieldDescriptor->eFieldType == LLRP_FT_E8 ) || 
            (pFieldDescriptor->eFieldType == LLRP_FT_E16 ) || 
            (pFieldDescriptor->eFieldType == LLRP_FT_E32 ) ||
            (pFieldDescriptor->eFieldType == LLRP_FT_E8V ) )
    /* special case for enumerated fields as they are strings in XML*/
    {
        endPtr = getSingleEnum(pFieldDescriptor, pbuf, pend, &Value);  
    }
    else switch(pFieldDescriptor->eFieldFormat)
    {
        /* assume default is decimal for new formats */
        default:
        case LLRP_FMT_NORMAL:
        case LLRP_FMT_DEC:
            endPtr = getSingleDecimal(pbuf, pend, &Value);
        break;
        case LLRP_FMT_HEX:
            endPtr = getSingleHexidecimal(pbuf, pend, &Value);
        break;
        case LLRP_FMT_UTF8:
        {
            const llrp_u8_t  *ptr;
            /* not sure what to do for UTF 8 encoding as integer */
            for(ptr = pbuf; ptr < pend; ptr++)
            {
                Value = 256*Value + *ptr;
            } 
        }
        break;
        case LLRP_FMT_DATETIME:
            endPtr = getSingleTimestamp(pbuf, pend, &Value);
        break;
    }    

    if(endPtr == pbuf)
    /* if we didn't get any thing, it means there are illegal characters
     * and we could not import a field value */
    {
        pError->eResultCode = LLRP_RC_XMLInvalidFieldCharacters;
        pError->pWhatStr    = "Illegal field value";
        pError->pRefType    = pDecoderStream->pRefType;
        pError->pRefField   = pFieldDescriptor;
        pError->OtherDetail = (int) pDecoderStream->pCurrentChildNode->line; 
        Value = 0;		
    }
    else if (endPtr != pend)
    /* if these don't match, there is still extra stuff
     * at the end.  This is always an error */
    {
        pError->eResultCode = LLRP_RC_FieldOverrun;
        pError->pWhatStr    = "overrun at field extra characters";
        pError->pRefType    = pDecoderStream->pRefType;
        pError->pRefField   = pFieldDescriptor;
        pError->OtherDetail = (int) pDecoderStream->pCurrentChildNode->line;
        Value = 0;		
    }
    /* make sure the value is in range */ 
    else if((Value > maxValue) || (Value < minValue))
    /* we have an out of range value. This is always an error */
    {
        pError->eResultCode = LLRP_RC_XMLOutOfRange;
        pError->pWhatStr    = "out of range value";
        pError->pRefType    = pDecoderStream->pRefType;
        pError->pRefField   = pFieldDescriptor;
        pError->OtherDetail = (int) pDecoderStream->pCurrentChildNode->line;
        Value = 0;
    }    
    return Value;
}

/* some of the fields are encoded without spaces between
 * the vectors, in this case, the fields are fixed length */
static llrp_s64v_t
getFixedVectorField (
  LLRP_tSLibXMLTextDecoderStream * pDecoderStream,
  const LLRP_tSFieldDescriptor *pFieldDescriptor,
  unsigned int                  vectorSize,
  llrp_s64_t                    minValue,
  llrp_s64_t                    maxValue)
{
    LLRP_tSErrorDetails *       pError = &pDecoderStream->pDecoder->decoderHdr.ErrorDetails;
    const llrp_u8_t *           pTok;
    const llrp_u8_t *           pEnd;    
    llrp_u16_t                  length;
    llrp_u16_t                  elementCount;
    llrp_s64v_t                 Value;
    llrp_u16_t                  count;

    memset(&Value, 0, sizeof Value);

    if(LLRP_RC_OK != pError->eResultCode)
    {
    /* already errd */
        return Value;
    }

    if(!getFieldStringPtr(pDecoderStream, pFieldDescriptor, &pTok, &pEnd))
    {
        /* these values are allowed to be empty */
        Value.nValue = 0;
        Value.pValue = NULL;
        return Value;
    }

    length = cleanString(&pTok, &pEnd);

    /* size should match exactly */
    if (length % vectorSize)
    {
        pError->eResultCode = LLRP_RC_FieldAllocationFailed;
        pError->pWhatStr    = "field size must be multiple of basic type";
        pError->pRefType    = pDecoderStream->pRefType;
        pError->pRefField   = pFieldDescriptor;
        pError->OtherDetail = (int) pDecoderStream->pCurrentChildNode->line;
        return Value;
    }

    /* round up for element count */
    elementCount = (length +  vectorSize - 1)/vectorSize;

    /* create the final storage entity */ 
    Value = LLRP_s64v_construct(elementCount);

    for(count = 0;count <elementCount ; count++, pTok += vectorSize)
    {
        pEnd = pTok + vectorSize;

        /* get a single integer */
        Value.pValue[count] = getInteger(pDecoderStream,
                                         pFieldDescriptor, 
                                         pTok, 
                                         pEnd, 
                                         pError, 
                                         minValue, 
                                         maxValue);
        if(LLRP_RC_OK != pError->eResultCode)
        {
            break;
        }
    }

    /* mark the length */
    Value.nValue = count;

    if((0 == Value.nValue) && (NULL != Value.pValue))
    {
        LLRP_s64v_clear(&Value);
    }

    return Value;
}

static llrp_s64v_t
getSpacedVectorField (
  LLRP_tSLibXMLTextDecoderStream * pDecoderStream,
  const LLRP_tSFieldDescriptor *pFieldDescriptor,
  llrp_s64_t                    minValue,
  llrp_s64_t                    maxValue)
{
    LLRP_tSErrorDetails *       pError = &pDecoderStream->pDecoder->decoderHdr.ErrorDetails;
    const llrp_u8_t *           pTok;
    const llrp_u8_t *           pEnd;    
    const llrp_u8_t *           pTokEnd;    
    llrp_u16_t                  length;
    llrp_u16_t                  elementCount;
    llrp_s64v_t                 Value;
    llrp_u16_t                  count;

    memset(&Value, 0, sizeof Value);

    if(LLRP_RC_OK != pError->eResultCode)
    {
    /* already errd */
        return Value;
    }

    /* get the NULL terminates string from XML */
    if(!getFieldStringPtr(pDecoderStream, pFieldDescriptor, &pTok, &pEnd))
    {
      /* many of these (e.g. ReadData) are allowed to be empty */
       return Value;
    }

    length = cleanString(&pTok, &pEnd);
    elementCount = countElements((char *) pTok, length);

    /* create the final storage entity */ 
    Value = LLRP_s64v_construct(elementCount);

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
        Value.pValue[count] = getInteger(pDecoderStream,
                                         pFieldDescriptor, 
                                         pTok, 
                                         pTokEnd, 
                                         pError, 
                                         minValue, 
                                         maxValue);
        if(LLRP_RC_OK != pError->eResultCode)
        {
            break;
        }
        /* get the next value */
        pTok = pTokEnd;
    }

    /* mark the length */
    Value.nValue = count;

    /* clear this now if we didn't get anything */
    if((0 == Value.nValue) && (NULL != Value.pValue))
    {
        LLRP_s64v_clear(&Value);
    }

    return Value;
}

static llrp_bool_t
verifyVectorAllocation (
  LLRP_tSLibXMLTextDecoderStream * pDecoderStream,
  const void *                  pValue,
  const LLRP_tSFieldDescriptor *pFieldDescriptor)
{
    if(NULL == pValue)
    {
        LLRP_tSLibXMLTextDecoder * pDecoder = pDecoderStream->pDecoder;
        LLRP_tSErrorDetails *   pError = &pDecoder->decoderHdr.ErrorDetails;

        pError->eResultCode = LLRP_RC_FieldAllocationFailed;
        pError->pWhatStr    = "field allocation failed";
        pError->pRefType    = pDecoderStream->pRefType;
        pError->pRefField   = pFieldDescriptor;
        pError->OtherDetail = (int) pDecoderStream->pCurrentChildNode->line;

        return FALSE;
    }
    else
    {
        return TRUE;
    }
}
