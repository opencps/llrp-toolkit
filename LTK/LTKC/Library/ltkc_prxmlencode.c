
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


#include "ltkc_platform.h"
#include "ltkc_base.h"
#include "ltkc_prxml.h"


/*
 * BEGIN forward declarations
 */

LLRP_tSPrXMLEncoder *
LLRP_PrXMLEncoder_construct (
  FILE *                        outfp);

static void
encoderDestruct (
  LLRP_tSEncoder *              pBaseEncoder);

static void
encodeElement (
  LLRP_tSEncoder *              pBaseEncoder,
  const LLRP_tSElement *        pElement);

static void
putRequiredSubParameter (
  LLRP_tSEncoderStream *        pBaseEncoderStream,
  const LLRP_tSParameter *      pParameter,
  const LLRP_tSTypeDescriptor * pRefType);

static void
putOptionalSubParameter (
  LLRP_tSEncoderStream *        pBaseEncoderStream,
  const LLRP_tSParameter *      pParameter,
  const LLRP_tSTypeDescriptor * pRefType);

static void
putRequiredSubParameterList (
  LLRP_tSEncoderStream *        pBaseEncoderStream,
  const LLRP_tSParameter *      pParameterList,
  const LLRP_tSTypeDescriptor * pRefType);

static void
putOptionalSubParameterList (
  LLRP_tSEncoderStream *        pBaseEncoderStream,
  const LLRP_tSParameter *      pParameterList,
  const LLRP_tSTypeDescriptor * pRefType);

static void
put_u8 (
  LLRP_tSEncoderStream *        pBaseEncoderStream,
  const llrp_u8_t               Value,
  const LLRP_tSFieldDescriptor *pFieldDescriptor);

static void
put_s8 (
  LLRP_tSEncoderStream *        pBaseEncoderStream,
  const llrp_s8_t               Value,
  const LLRP_tSFieldDescriptor *pFieldDescriptor);

static void
put_u8v (
  LLRP_tSEncoderStream *        pBaseEncoderStream,
  const llrp_u8v_t              Value,
  const LLRP_tSFieldDescriptor *pFieldDescriptor);

static void
put_s8v (
  LLRP_tSEncoderStream *        pBaseEncoderStream,
  const llrp_s8v_t              Value,
  const LLRP_tSFieldDescriptor *pFieldDescriptor);

static void
put_u16 (
  LLRP_tSEncoderStream *        pBaseEncoderStream,
  const llrp_u16_t              Value,
  const LLRP_tSFieldDescriptor *pFieldDescriptor);

static void
put_s16 (
  LLRP_tSEncoderStream *        pBaseEncoderStream,
  const llrp_s16_t              Value,
  const LLRP_tSFieldDescriptor *pFieldDescriptor);

static void
put_u16v (
  LLRP_tSEncoderStream *        pBaseEncoderStream,
  const llrp_u16v_t             Value,
  const LLRP_tSFieldDescriptor *pFieldDescriptor);

static void
put_s16v (
  LLRP_tSEncoderStream *        pBaseEncoderStream,
  const llrp_s16v_t             Value,
  const LLRP_tSFieldDescriptor *pFieldDescriptor);

static void
put_u32 (
  LLRP_tSEncoderStream *        pBaseEncoderStream,
  const llrp_u32_t              Value,
  const LLRP_tSFieldDescriptor *pFieldDescriptor);

static void
put_s32 (
  LLRP_tSEncoderStream *        pBaseEncoderStream,
  const llrp_s32_t              Value,
  const LLRP_tSFieldDescriptor *pFieldDescriptor);

static void
put_u32v (
  LLRP_tSEncoderStream *        pBaseEncoderStream,
  const llrp_u32v_t             Value,
  const LLRP_tSFieldDescriptor *pFieldDescriptor);

static void
put_s32v (
  LLRP_tSEncoderStream *        pBaseEncoderStream,
  const llrp_s32v_t             Value,
  const LLRP_tSFieldDescriptor *pFieldDescriptor);

static void
put_u64 (
  LLRP_tSEncoderStream *        pBaseEncoderStream,
  const llrp_u64_t              Value,
  const LLRP_tSFieldDescriptor *pFieldDescriptor);

static void
put_s64 (
  LLRP_tSEncoderStream *        pBaseEncoderStream,
  const llrp_s64_t              Value,
  const LLRP_tSFieldDescriptor *pFieldDescriptor);

static void
put_u64v (
  LLRP_tSEncoderStream *        pBaseEncoderStream,
  const llrp_u64v_t             Value,
  const LLRP_tSFieldDescriptor *pFieldDescriptor);

static void
put_s64v (
  LLRP_tSEncoderStream *        pBaseEncoderStream,
  const llrp_s64v_t             Value,
  const LLRP_tSFieldDescriptor *pFieldDescriptor);

static void
put_u1 (
  LLRP_tSEncoderStream *        pBaseEncoderStream,
  const llrp_u1_t               Value,
  const LLRP_tSFieldDescriptor *pFieldDescriptor);

static void
put_u1v (
  LLRP_tSEncoderStream *        pBaseEncoderStream,
  const llrp_u1v_t              Value,
  const LLRP_tSFieldDescriptor *pFieldDescriptor);

static void
put_u2 (
  LLRP_tSEncoderStream *        pBaseEncoderStream,
  const llrp_u2_t               Value,
  const LLRP_tSFieldDescriptor *pFieldDescriptor);

static void
put_u96 (
  LLRP_tSEncoderStream *        pBaseEncoderStream,
  const llrp_u96_t              Value,
  const LLRP_tSFieldDescriptor *pFieldDescriptor);

static void
put_utf8v (
  LLRP_tSEncoderStream *        pBaseEncoderStream,
  const llrp_utf8v_t            Value,
  const LLRP_tSFieldDescriptor *pFieldDescriptor);

static void
put_bytesToEnd (
  LLRP_tSEncoderStream *        pBaseEncoderStream,
  const llrp_bytesToEnd_t       Value,
  const LLRP_tSFieldDescriptor *pFieldDescriptor);

static void
put_e1 (
  LLRP_tSEncoderStream *        pBaseEncoderStream,
  const int                     Value,
  const LLRP_tSFieldDescriptor *pFieldDescriptor);

static void
put_e2 (
  LLRP_tSEncoderStream *        pBaseEncoderStream,
  const int                     Value,
  const LLRP_tSFieldDescriptor *pFieldDescriptor);

static void
put_e8 (
  LLRP_tSEncoderStream *        pBaseEncoderStream,
  const int                     Value,
  const LLRP_tSFieldDescriptor *pFieldDescriptor);

static void
put_e16 (
  LLRP_tSEncoderStream *        pBaseEncoderStream,
  const int                     Value,
  const LLRP_tSFieldDescriptor *pFieldDescriptor);

static void
put_e32 (
  LLRP_tSEncoderStream *        pBaseEncoderStream,
  const int                     Value,
  const LLRP_tSFieldDescriptor *pFieldDescriptor);

static void
put_enum (
  LLRP_tSEncoderStream *        pBaseEncoderStream,
  int                           eValue,
  const LLRP_tSFieldDescriptor *pFieldDescriptor);

static void
put_reserved (
  LLRP_tSEncoderStream *        pBaseEncoderStream,
  unsigned int                  nBits);

static void
streamConstruct_outermost (
  LLRP_tSPrXMLEncoderStream *   pEncoderStream,
  LLRP_tSPrXMLEncoder *         pEncoder);

static void
streamConstruct_nested (
  LLRP_tSPrXMLEncoderStream *   pEncoderStream,
  LLRP_tSPrXMLEncoderStream *   pEnclosingEncoderStream);

static void
putElement (
  LLRP_tSPrXMLEncoderStream *   pEncoderStream,
  const LLRP_tSElement *        pElement);

static void
nestSubParameter (
  LLRP_tSPrXMLEncoderStream *   pEncoderStream,
  const LLRP_tSParameter *      pParameter);

static void
indent (
  LLRP_tSPrXMLEncoderStream *   pEncoderStream,
  int                           adjust);

/*
 * END forward declarations
 */



static LLRP_tSEncoderOps
s_PrXMLEncoderOps =
{
    .pfDestruct                 = encoderDestruct,
    .pfEncodeElement            = encodeElement,
};

static LLRP_tSEncoderStreamOps
s_PrXMLEncoderStreamOps =
{
    .pfPutRequiredSubParameter      = putRequiredSubParameter,
    .pfPutOptionalSubParameter      = putOptionalSubParameter,
    .pfPutRequiredSubParameterList  = putRequiredSubParameterList,
    .pfPutOptionalSubParameterList  = putOptionalSubParameterList,

    .pfPut_u8                       = put_u8,
    .pfPut_s8                       = put_s8,
    .pfPut_u8v                      = put_u8v,
    .pfPut_s8v                      = put_s8v,

    .pfPut_u16                      = put_u16,
    .pfPut_s16                      = put_s16,
    .pfPut_u16v                     = put_u16v,
    .pfPut_s16v                     = put_s16v,

    .pfPut_u32                      = put_u32,
    .pfPut_s32                      = put_s32,
    .pfPut_u32v                     = put_u32v,
    .pfPut_s32v                     = put_s32v,

    .pfPut_u64                      = put_u64,
    .pfPut_s64                      = put_s64,
    .pfPut_u64v                     = put_u64v,
    .pfPut_s64v                     = put_s64v,

    .pfPut_u1                       = put_u1,
    .pfPut_u1v                      = put_u1v,
    .pfPut_u2                       = put_u2,
    .pfPut_u96                      = put_u96,
    .pfPut_utf8v                    = put_utf8v,
    .pfPut_bytesToEnd               = put_bytesToEnd,

    .pfPut_e1                       = put_e1,
    .pfPut_e2                       = put_e2,
    .pfPut_e8                       = put_e8,
    .pfPut_e16                      = put_e16,
    .pfPut_e32                      = put_e32,

    .pfPut_reserved                 = put_reserved,
};


LLRP_tSPrXMLEncoder *
LLRP_PrXMLEncoder_construct (
  FILE *                        outfp)
{
    LLRP_tSPrXMLEncoder *       pEncoder;

    pEncoder = malloc(sizeof *pEncoder);
    if(NULL == pEncoder)
    {
        return pEncoder;
    }

    memset(pEncoder, 0, sizeof *pEncoder);

    pEncoder->encoderHdr.pEncoderOps = &s_PrXMLEncoderOps;
    pEncoder->outfp = outfp;

    return pEncoder;
}

static void
encoderDestruct (
  LLRP_tSEncoder *              pBaseEncoder)
{
    LLRP_tSPrXMLEncoder *       pEncoder = (LLRP_tSPrXMLEncoder*)pBaseEncoder;

    free(pEncoder);
}

static void
encodeElement (
  LLRP_tSEncoder *              pBaseEncoder,
  const LLRP_tSElement *        pElement)
{
    LLRP_tSPrXMLEncoder *       pEncoder = (LLRP_tSPrXMLEncoder*)pBaseEncoder;
    LLRP_tSPrXMLEncoderStream   EncoderStream;

    if(NULL == pElement)
    {
        return;
    }

    streamConstruct_outermost(&EncoderStream, pEncoder);

    putElement(&EncoderStream, pElement);
}

static void
putRequiredSubParameter (
  LLRP_tSEncoderStream *        pBaseEncoderStream,
  const LLRP_tSParameter *      pParameter,
  const LLRP_tSTypeDescriptor * pRefType)
{
    LLRP_tSPrXMLEncoderStream * pEncoderStream =
                            (LLRP_tSPrXMLEncoderStream *) pBaseEncoderStream;
    FILE *                      outfp = pEncoderStream->pEncoder->outfp;

    if(NULL == pParameter)
    {
        fprintf(outfp, "warning: missing %s\n",
            (NULL == pRefType) ? "<something>" : pRefType->pName);
        return;
    }

    nestSubParameter(pEncoderStream, pParameter);
}

static void
putOptionalSubParameter (
  LLRP_tSEncoderStream *        pBaseEncoderStream,
  const LLRP_tSParameter *      pParameter,
  const LLRP_tSTypeDescriptor * pRefType)
{
    LLRP_tSPrXMLEncoderStream * pEncoderStream =
                            (LLRP_tSPrXMLEncoderStream *) pBaseEncoderStream;

    nestSubParameter(pEncoderStream, pParameter);
}

static void
putRequiredSubParameterList (
  LLRP_tSEncoderStream *        pBaseEncoderStream,
  const LLRP_tSParameter *      pParameterList,
  const LLRP_tSTypeDescriptor * pRefType)
{
    LLRP_tSPrXMLEncoderStream * pEncoderStream =
                            (LLRP_tSPrXMLEncoderStream *) pBaseEncoderStream;
    FILE *                      outfp = pEncoderStream->pEncoder->outfp;
    const LLRP_tSParameter *    pParameter;

    if(NULL == pParameterList)
    {
        fprintf(outfp, "warning: missing list of %s\n",
            (NULL == pRefType) ? "<something>" : pRefType->pName);
        return;
    }

    for(
        pParameter = pParameterList;
        NULL != pParameter;
        pParameter = pParameter->pNextSubParameter)
    {
        nestSubParameter(pEncoderStream, pParameter);
    }
}

static void
putOptionalSubParameterList (
  LLRP_tSEncoderStream *        pBaseEncoderStream,
  const LLRP_tSParameter *      pParameterList,
  const LLRP_tSTypeDescriptor * pRefType)
{
    LLRP_tSPrXMLEncoderStream * pEncoderStream =
                            (LLRP_tSPrXMLEncoderStream *) pBaseEncoderStream;
    const LLRP_tSParameter *    pParameter;

    for(
        pParameter = pParameterList;
        NULL != pParameter;
        pParameter = pParameter->pNextSubParameter)
    {
        nestSubParameter(pEncoderStream, pParameter);
    }
}

static void
put_u8 (
  LLRP_tSEncoderStream *        pBaseEncoderStream,
  const llrp_u8_t               Value,
  const LLRP_tSFieldDescriptor *pFieldDescriptor)
{
    LLRP_tSPrXMLEncoderStream * pEncoderStream =
                            (LLRP_tSPrXMLEncoderStream *) pBaseEncoderStream;
    FILE *                      outfp = pEncoderStream->pEncoder->outfp;
    const char *                pFieldName = pFieldDescriptor->pName;

    indent(pEncoderStream, 0);
    fprintf(outfp, "<%s>%u</%s>\n", pFieldName, Value, pFieldName);
}

static void
put_s8 (
  LLRP_tSEncoderStream *        pBaseEncoderStream,
  const llrp_s8_t               Value,
  const LLRP_tSFieldDescriptor *pFieldDescriptor)
{
    LLRP_tSPrXMLEncoderStream * pEncoderStream =
                            (LLRP_tSPrXMLEncoderStream *) pBaseEncoderStream;
    FILE *                      outfp = pEncoderStream->pEncoder->outfp;
    const char *                pFieldName = pFieldDescriptor->pName;

    indent(pEncoderStream, 0);
    fprintf(outfp, "<%s>%d</%s>\n", pFieldName, Value, pFieldName);
}

static void
put_u8v (
  LLRP_tSEncoderStream *        pBaseEncoderStream,
  const llrp_u8v_t              Value,
  const LLRP_tSFieldDescriptor *pFieldDescriptor)
{
    LLRP_tSPrXMLEncoderStream * pEncoderStream =
                            (LLRP_tSPrXMLEncoderStream *) pBaseEncoderStream;
    FILE *                      outfp = pEncoderStream->pEncoder->outfp;
    const char *                pFieldName = pFieldDescriptor->pName;
    int                         i;

    indent(pEncoderStream, 0);
    fprintf(outfp, "<%s>", pFieldName);
    for(i = 0; i < Value.nValue; i++)
    {
        if(0 < i)
        {
            fprintf(outfp, " ");
        }
        fprintf(outfp, "%u", Value.pValue[i]);
    }
    fprintf(outfp, "</%s>\n", pFieldName);
}

static void
put_s8v (
  LLRP_tSEncoderStream *        pBaseEncoderStream,
  const llrp_s8v_t              Value,
  const LLRP_tSFieldDescriptor *pFieldDescriptor)
{
    LLRP_tSPrXMLEncoderStream * pEncoderStream =
                            (LLRP_tSPrXMLEncoderStream *) pBaseEncoderStream;
    FILE *                      outfp = pEncoderStream->pEncoder->outfp;
    const char *                pFieldName = pFieldDescriptor->pName;
    int                         i;

    indent(pEncoderStream, 0);
    fprintf(outfp, "<%s>", pFieldName);
    for(i = 0; i < Value.nValue; i++)
    {
        if(0 < i)
        {
            fprintf(outfp, " ");
        }
        fprintf(outfp, "%d", Value.pValue[i]);
    }
    fprintf(outfp, "</%s>\n", pFieldName);
}

static void
put_u16 (
  LLRP_tSEncoderStream *        pBaseEncoderStream,
  const llrp_u16_t              Value,
  const LLRP_tSFieldDescriptor *pFieldDescriptor)
{
    LLRP_tSPrXMLEncoderStream * pEncoderStream =
                            (LLRP_tSPrXMLEncoderStream *) pBaseEncoderStream;
    FILE *                      outfp = pEncoderStream->pEncoder->outfp;
    const char *                pFieldName = pFieldDescriptor->pName;

    indent(pEncoderStream, 0);
    fprintf(outfp, "<%s>%u</%s>\n", pFieldName, Value, pFieldName);
}

static void
put_s16 (
  LLRP_tSEncoderStream *        pBaseEncoderStream,
  const llrp_s16_t              Value,
  const LLRP_tSFieldDescriptor *pFieldDescriptor)
{
    LLRP_tSPrXMLEncoderStream * pEncoderStream =
                            (LLRP_tSPrXMLEncoderStream *) pBaseEncoderStream;
    FILE *                      outfp = pEncoderStream->pEncoder->outfp;
    const char *                pFieldName = pFieldDescriptor->pName;

    indent(pEncoderStream, 0);
    fprintf(outfp, "<%s>%d</%s>\n", pFieldName, Value, pFieldName);
}

static void
put_u16v (
  LLRP_tSEncoderStream *        pBaseEncoderStream,
  const llrp_u16v_t             Value,
  const LLRP_tSFieldDescriptor *pFieldDescriptor)
{
    LLRP_tSPrXMLEncoderStream * pEncoderStream =
                            (LLRP_tSPrXMLEncoderStream *) pBaseEncoderStream;
    FILE *                      outfp = pEncoderStream->pEncoder->outfp;
    const char *                pFieldName = pFieldDescriptor->pName;
    int                         i;

    indent(pEncoderStream, 0);
    fprintf(outfp, "<%s>", pFieldName);
    for(i = 0; i < Value.nValue; i++)
    {
        if(0 < i)
        {
            fprintf(outfp, " ");
        }
        fprintf(outfp, "%u", Value.pValue[i]);
    }
    fprintf(outfp, "</%s>\n", pFieldName);
}

static void
put_s16v (
  LLRP_tSEncoderStream *        pBaseEncoderStream,
  const llrp_s16v_t             Value,
  const LLRP_tSFieldDescriptor *pFieldDescriptor)
{
    LLRP_tSPrXMLEncoderStream * pEncoderStream =
                            (LLRP_tSPrXMLEncoderStream *) pBaseEncoderStream;
    FILE *                      outfp = pEncoderStream->pEncoder->outfp;
    const char *                pFieldName = pFieldDescriptor->pName;
    int                         i;

    indent(pEncoderStream, 0);
    fprintf(outfp, "<%s>", pFieldName);
    for(i = 0; i < Value.nValue; i++)
    {
        if(0 < i)
        {
            fprintf(outfp, " ");
        }
        fprintf(outfp, "%d", Value.pValue[i]);
    }
    fprintf(outfp, "</%s>\n", pFieldName);
}

static void
put_u32 (
  LLRP_tSEncoderStream *        pBaseEncoderStream,
  const llrp_u32_t              Value,
  const LLRP_tSFieldDescriptor *pFieldDescriptor)
{
    LLRP_tSPrXMLEncoderStream * pEncoderStream =
                            (LLRP_tSPrXMLEncoderStream *) pBaseEncoderStream;
    FILE *                      outfp = pEncoderStream->pEncoder->outfp;
    const char *                pFieldName = pFieldDescriptor->pName;

    indent(pEncoderStream, 0);
    fprintf(outfp, "<%s>%u</%s>\n", pFieldName, Value, pFieldName);
}

static void
put_s32 (
  LLRP_tSEncoderStream *        pBaseEncoderStream,
  const llrp_s32_t              Value,
  const LLRP_tSFieldDescriptor *pFieldDescriptor)
{
    LLRP_tSPrXMLEncoderStream * pEncoderStream =
                            (LLRP_tSPrXMLEncoderStream *) pBaseEncoderStream;
    FILE *                      outfp = pEncoderStream->pEncoder->outfp;
    const char *                pFieldName = pFieldDescriptor->pName;

    indent(pEncoderStream, 0);
    fprintf(outfp, "<%s>%d</%s>\n", pFieldName, Value, pFieldName);
}

static void
put_u32v (
  LLRP_tSEncoderStream *        pBaseEncoderStream,
  const llrp_u32v_t             Value,
  const LLRP_tSFieldDescriptor *pFieldDescriptor)
{
    LLRP_tSPrXMLEncoderStream * pEncoderStream =
                            (LLRP_tSPrXMLEncoderStream *) pBaseEncoderStream;
    FILE *                      outfp = pEncoderStream->pEncoder->outfp;
    const char *                pFieldName = pFieldDescriptor->pName;
    int                         i;

    indent(pEncoderStream, 0);
    fprintf(outfp, "<%s>", pFieldName);
    for(i = 0; i < Value.nValue; i++)
    {
        if(0 < i)
        {
            fprintf(outfp, " ");
        }
        fprintf(outfp, "%u", Value.pValue[i]);
    }
    fprintf(outfp, "</%s>\n", pFieldName);
}

static void
put_s32v (
  LLRP_tSEncoderStream *        pBaseEncoderStream,
  const llrp_s32v_t             Value,
  const LLRP_tSFieldDescriptor *pFieldDescriptor)
{
    LLRP_tSPrXMLEncoderStream * pEncoderStream =
                            (LLRP_tSPrXMLEncoderStream *) pBaseEncoderStream;
    FILE *                      outfp = pEncoderStream->pEncoder->outfp;
    const char *                pFieldName = pFieldDescriptor->pName;
    int                         i;

    indent(pEncoderStream, 0);
    fprintf(outfp, "<%s>", pFieldName);
    for(i = 0; i < Value.nValue; i++)
    {
        if(0 < i)
        {
            fprintf(outfp, " ");
        }
        fprintf(outfp, "%d", Value.pValue[i]);
    }
    fprintf(outfp, "</%s>\n", pFieldName);
}

static void
put_u64 (
  LLRP_tSEncoderStream *        pBaseEncoderStream,
  const llrp_u64_t              Value,
  const LLRP_tSFieldDescriptor *pFieldDescriptor)
{
    LLRP_tSPrXMLEncoderStream * pEncoderStream =
                            (LLRP_tSPrXMLEncoderStream *) pBaseEncoderStream;
    FILE *                      outfp = pEncoderStream->pEncoder->outfp;
    const char *                pFieldName = pFieldDescriptor->pName;

    indent(pEncoderStream, 0);
    fprintf(outfp, "<%s>%llu</%s>\n", pFieldName, Value, pFieldName);
}

static void
put_s64 (
  LLRP_tSEncoderStream *        pBaseEncoderStream,
  const llrp_s64_t              Value,
  const LLRP_tSFieldDescriptor *pFieldDescriptor)
{
    LLRP_tSPrXMLEncoderStream * pEncoderStream =
                            (LLRP_tSPrXMLEncoderStream *) pBaseEncoderStream;
    FILE *                      outfp = pEncoderStream->pEncoder->outfp;
    const char *                pFieldName = pFieldDescriptor->pName;

    indent(pEncoderStream, 0);
    fprintf(outfp, "<%s>%lld</%s>\n", pFieldName, Value, pFieldName);
}

static void
put_u64v (
  LLRP_tSEncoderStream *        pBaseEncoderStream,
  const llrp_u64v_t             Value,
  const LLRP_tSFieldDescriptor *pFieldDescriptor)
{
    LLRP_tSPrXMLEncoderStream * pEncoderStream =
                            (LLRP_tSPrXMLEncoderStream *) pBaseEncoderStream;
    FILE *                      outfp = pEncoderStream->pEncoder->outfp;
    const char *                pFieldName = pFieldDescriptor->pName;
    int                         i;

    indent(pEncoderStream, 0);
    fprintf(outfp, "<%s>", pFieldName);
    for(i = 0; i < Value.nValue; i++)
    {
        if(0 < i)
        {
            fprintf(outfp, " ");
        }
        fprintf(outfp, "%llu", Value.pValue[i]);
    }
    fprintf(outfp, "</%s>\n", pFieldName);
}

static void
put_s64v (
  LLRP_tSEncoderStream *        pBaseEncoderStream,
  const llrp_s64v_t             Value,
  const LLRP_tSFieldDescriptor *pFieldDescriptor)
{
    LLRP_tSPrXMLEncoderStream * pEncoderStream =
                            (LLRP_tSPrXMLEncoderStream *) pBaseEncoderStream;
    FILE *                      outfp = pEncoderStream->pEncoder->outfp;
    const char *                pFieldName = pFieldDescriptor->pName;
    int                         i;

    indent(pEncoderStream, 0);
    fprintf(outfp, "<%s>", pFieldName);
    for(i = 0; i < Value.nValue; i++)
    {
        if(0 < i)
        {
            fprintf(outfp, " ");
        }
        fprintf(outfp, "%lld", Value.pValue[i]);
    }
    fprintf(outfp, "</%s>\n", pFieldName);
}

static void
put_u1 (
  LLRP_tSEncoderStream *        pBaseEncoderStream,
  const llrp_u1_t               Value,
  const LLRP_tSFieldDescriptor *pFieldDescriptor)
{
    LLRP_tSPrXMLEncoderStream * pEncoderStream =
                            (LLRP_tSPrXMLEncoderStream *) pBaseEncoderStream;
    FILE *                      outfp = pEncoderStream->pEncoder->outfp;
    const char *                pFieldName = pFieldDescriptor->pName;

    indent(pEncoderStream, 0);
    fprintf(outfp, "<%s>%u</%s>\n", pFieldName, Value & 1, pFieldName);
}

static void
put_u1v (
  LLRP_tSEncoderStream *        pBaseEncoderStream,
  const llrp_u1v_t              Value,
  const LLRP_tSFieldDescriptor *pFieldDescriptor)
{
    LLRP_tSPrXMLEncoderStream * pEncoderStream =
                            (LLRP_tSPrXMLEncoderStream *) pBaseEncoderStream;
    FILE *                      outfp = pEncoderStream->pEncoder->outfp;
    const char *                pFieldName = pFieldDescriptor->pName;
    int                         nByte = (Value.nBit + 7u) / 8u;
    int                         i;

    indent(pEncoderStream, 0);
    fprintf(outfp, "<%s Count='%d'>", pFieldName, Value.nBit);
    for(i = 0; i < nByte; i++)
    {
        fprintf(outfp, "%02X", Value.pValue[i]);
    }
    fprintf(outfp, "</%s>\n", pFieldName);
}

static void
put_u2 (
  LLRP_tSEncoderStream *        pBaseEncoderStream,
  const llrp_u2_t               Value,
  const LLRP_tSFieldDescriptor *pFieldDescriptor)
{
    LLRP_tSPrXMLEncoderStream * pEncoderStream =
                            (LLRP_tSPrXMLEncoderStream *) pBaseEncoderStream;
    FILE *                      outfp = pEncoderStream->pEncoder->outfp;
    const char *                pFieldName = pFieldDescriptor->pName;

    indent(pEncoderStream, 0);
    fprintf(outfp, "<%s>%u</%s>\n", pFieldName, Value & 3, pFieldName);
}

static void
put_u96 (
  LLRP_tSEncoderStream *        pBaseEncoderStream,
  const llrp_u96_t              Value,
  const LLRP_tSFieldDescriptor *pFieldDescriptor)
{
    LLRP_tSPrXMLEncoderStream * pEncoderStream =
                            (LLRP_tSPrXMLEncoderStream *) pBaseEncoderStream;
    FILE *                      outfp = pEncoderStream->pEncoder->outfp;
    const char *                pFieldName = pFieldDescriptor->pName;
    int                         i;

    indent(pEncoderStream, 0);
    fprintf(outfp, "<%s>", pFieldName);
    for(i = 0; i < 12; i++)
    {
        fprintf(outfp, "%02X", Value.aValue[i]);
    }
    fprintf(outfp, "</%s>\n", pFieldName);
}

static void
put_utf8v (
  LLRP_tSEncoderStream *        pBaseEncoderStream,
  const llrp_utf8v_t            Value,
  const LLRP_tSFieldDescriptor *pFieldDescriptor)
{
    LLRP_tSPrXMLEncoderStream * pEncoderStream =
                            (LLRP_tSPrXMLEncoderStream *) pBaseEncoderStream;
    FILE *                      outfp = pEncoderStream->pEncoder->outfp;
    const char *                pFieldName = pFieldDescriptor->pName;
    int                         i;

    indent(pEncoderStream, 0);
    fprintf(outfp, "<%s>", pFieldName);
    for(i = 0; i < Value.nValue; i++)
    {
        int                     c = Value.pValue[i];

        if(' ' <= c && c < 0x7F)
        {
            fprintf(outfp, "%c", c);
        }
        else
        {
            fprintf(outfp, "\\%03o", c);
        }
    }
    fprintf(outfp, "</%s>\n", pFieldName);
}

static void
put_bytesToEnd (
  LLRP_tSEncoderStream *        pBaseEncoderStream,
  const llrp_bytesToEnd_t       Value,
  const LLRP_tSFieldDescriptor *pFieldDescriptor)
{
    LLRP_tSPrXMLEncoderStream * pEncoderStream =
                            (LLRP_tSPrXMLEncoderStream *) pBaseEncoderStream;
    FILE *                      outfp = pEncoderStream->pEncoder->outfp;
    const char *                pFieldName = pFieldDescriptor->pName;
    int                         i;

    indent(pEncoderStream, 0);
    fprintf(outfp, "<%s>", pFieldName);
    for(i = 0; i < Value.nValue; i++)
    {
        if(0 < i)
        {
            fprintf(outfp, " ");
        }
        fprintf(outfp, "%02X", Value.pValue[i]);
    }
    fprintf(outfp, "</%s>\n", pFieldName);
}

static void
put_e1 (
  LLRP_tSEncoderStream *        pBaseEncoderStream,
  const int                     Value,
  const LLRP_tSFieldDescriptor *pFieldDescriptor)
{
    put_enum(pBaseEncoderStream, Value, pFieldDescriptor);
}

static void
put_e2 (
  LLRP_tSEncoderStream *        pBaseEncoderStream,
  const int                     Value,
  const LLRP_tSFieldDescriptor *pFieldDescriptor)
{
    put_enum(pBaseEncoderStream, Value, pFieldDescriptor);
}

static void
put_e8 (
  LLRP_tSEncoderStream *        pBaseEncoderStream,
  const int                     Value,
  const LLRP_tSFieldDescriptor *pFieldDescriptor)
{
    put_enum(pBaseEncoderStream, Value, pFieldDescriptor);
}

static void
put_e16 (
  LLRP_tSEncoderStream *        pBaseEncoderStream,
  const int                     Value,
  const LLRP_tSFieldDescriptor *pFieldDescriptor)
{
    put_enum(pBaseEncoderStream, Value, pFieldDescriptor);
}

static void
put_e32 (
  LLRP_tSEncoderStream *        pBaseEncoderStream,
  const int                     Value,
  const LLRP_tSFieldDescriptor *pFieldDescriptor)
{
    put_enum(pBaseEncoderStream, Value, pFieldDescriptor);
}

static void
put_enum (
  LLRP_tSEncoderStream *        pBaseEncoderStream,
  int                           eValue,
  const LLRP_tSFieldDescriptor *pFieldDescriptor)
{
    LLRP_tSPrXMLEncoderStream * pEncoderStream =
                            (LLRP_tSPrXMLEncoderStream *) pBaseEncoderStream;
    FILE *                      outfp = pEncoderStream->pEncoder->outfp;
    const char *                pFieldName = pFieldDescriptor->pName;
    const LLRP_tSEnumTableEntry *pEntry;

    indent(pEncoderStream, 0);
    fprintf(outfp, "<%s>", pFieldName);

    for(
        pEntry = pFieldDescriptor->pEnumTable;
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
        fprintf(outfp, "%s", pEntry->pName);
    }
    else
    {
        fprintf(outfp, "%d", eValue);
    }

    fprintf(outfp, "</%s>\n", pFieldName);
}

static void
put_reserved (
  LLRP_tSEncoderStream *        pBaseEncoderStream,
  unsigned int                  nBits)
{
    LLRP_tSPrXMLEncoderStream * pEncoderStream =
                            (LLRP_tSPrXMLEncoderStream *) pBaseEncoderStream;
    FILE *                      outfp = pEncoderStream->pEncoder->outfp;

    indent(pEncoderStream, 0);
    fprintf(outfp, "<!-- reserved %d bits -->\n", nBits);
}

static void
streamConstruct_outermost (
  LLRP_tSPrXMLEncoderStream *   pEncoderStream,
  LLRP_tSPrXMLEncoder *         pEncoder)
{
    memset(pEncoderStream, 0, sizeof *pEncoderStream);
    pEncoderStream->encoderStreamHdr.pEncoderStreamOps =
                                &s_PrXMLEncoderStreamOps;

    pEncoderStream->pEncoder                = pEncoder;
    pEncoderStream->pEnclosingEncoderStream = NULL;
    pEncoderStream->pRefType                = NULL;
    pEncoderStream->nDepth                  = 1;
}


static void
streamConstruct_nested (
  LLRP_tSPrXMLEncoderStream *   pEncoderStream,
  LLRP_tSPrXMLEncoderStream *   pEnclosingEncoderStream)
{
    LLRP_tSPrXMLEncoder *       pEncoder;

    pEncoder = pEnclosingEncoderStream->pEncoder;

    memset(pEncoderStream, 0, sizeof *pEncoderStream);
    pEncoderStream->encoderStreamHdr.pEncoderStreamOps =
                                &s_PrXMLEncoderStreamOps;

    pEncoderStream->pEncoder                = pEncoder;
    pEncoderStream->pEnclosingEncoderStream = pEnclosingEncoderStream;
    pEncoderStream->pRefType                = NULL;
    pEncoderStream->nDepth                  =
                            pEnclosingEncoderStream->nDepth+1;
}

static void
putElement (
  LLRP_tSPrXMLEncoderStream *   pEncoderStream,
  const LLRP_tSElement *        pElement)
{
    const LLRP_tSTypeDescriptor *pRefType = pElement->pType;
    FILE *                      outfp = pEncoderStream->pEncoder->outfp;

    pEncoderStream->pRefType = pRefType;

    indent(pEncoderStream, -1);
    if(pRefType->bIsMessage)
    {
#if 1
        fprintf(outfp, "<%s MessageID='%u'>\n",
            pRefType->pName, ((LLRP_tSMessage *)pElement)->MessageID);
#else
        fprintf(outfp, "<%s MessageID='%u'\n",
            pRefType->pName, ((LLRP_tSMessage *)pElement)->MessageID);
        fprintf(outfp, "      xmlns='LLRP'\n");
        fprintf(outfp, "      xmlns:xsi='%s'\n",
            "http://www.w3.org/2001/XMLSchema-instance");
        fprintf(outfp, "      xsi:schemaLocation='LLRP LLRP.xsd'>\n");
#endif
    }
    else
    {
        fprintf(outfp, "<%s>\n", pRefType->pName);
    }
    pRefType->pfEncode(pElement, &pEncoderStream->encoderStreamHdr);
    indent(pEncoderStream, -1);
    fprintf(outfp, "</%s>\n", pRefType->pName);
}

static void
nestSubParameter (
  LLRP_tSPrXMLEncoderStream *   pEncoderStream,
  const LLRP_tSParameter *      pParameter)
{
    const LLRP_tSElement *      pElement = (const LLRP_tSElement *)pParameter;
    LLRP_tSPrXMLEncoderStream   NestEncoderStream;

    if(NULL == pParameter)
    {
        return;
    }

    streamConstruct_nested(&NestEncoderStream, pEncoderStream);

    putElement(&NestEncoderStream, pElement);
}

static void
indent (
  LLRP_tSPrXMLEncoderStream *   pEncoderStream,
  int                           adjust)
{
    FILE *                      outfp = pEncoderStream->pEncoder->outfp;
    int                         n = pEncoderStream->nDepth + adjust;
    int                         i;

    for(i = 0; i < n; i++)
    {
        fprintf(outfp, "  ");
    }
}
