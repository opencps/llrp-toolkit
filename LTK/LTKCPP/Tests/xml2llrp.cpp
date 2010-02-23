
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
 ** @file  xml2llrp.c
 **
 ** @brief Converts LTK-XML packet sequence to an LLRP binary stream
 **
 ** This is a diagnostic tool for the LLRP Tool Kit for CPP (LTKCPP).
 **
 ** xml2llrp reads an input file that contains an LTK-XML packet sequence.
 **  Its output is printed on stdout and conforms to the LLRP binary format.
 **
 ** For each input LTK-XML message:
 **     - Decode the frame into an LLRP message object
 **     -     on success: outputs the object as a binary packet to stdout
 **     -     on failure: outputs ERROR_MESSAGE to stdout
 **
 ** This program can be tested using tools like valgrind (please
 ** see http://en.wikipedia.org/wiki/Valgrind) that detect memory leaks.
 **
 ** Normal use is something like
 **
 **     xml2llrp ../../Tests/dx301/dx301_a.xml > dx301_out.tmp
 **
 ** Then to verify proper function, the output file is compared
 **
 **     cmp ../../Tests/dx301/dx301_a.bin dx301_out.tmp
 **
 ** When the files compare it means the dx301 and the LTKC are correct.
 **
 *****************************************************************************/


#include <stdio.h>

#include "ltkcpp.h"
#include "libxml/parser.h"
#include "libxml/tree.h"


using namespace LLRP;

// turn this on to learn more about errors 
#undef XML2LLRP_DEBUG

#define FRAME_BUF_SIZE          (4u*1024u*1024u)

/* This is the message format that is agreed upon when messages fail
** to decode. It's somewhat arbitrary, but allows us to do easy
** comparisons */
const char * errMsgStr = "<ERROR_MESSAGE MessageID=\"0\" Version=\"0\">\n" \
                         "  <LLRPStatus>\n" \
                         "    <StatusCode>M_Success</StatusCode>\n" \
                         "    <ErrorDescription></ErrorDescription>\n" \
                         "  </LLRPStatus>\n" \
                         "</ERROR_MESSAGE>\n";

/* the corresponding binary packet */
unsigned char errMsgBinary[18] = {0x04, 0x64, 0x00, 0x00, 
                                  0x00, 0x12, 0x00, 0x00, 
                                  0x00, 0x00, 0x01, 0x1F, 
                                  0x00, 0x08, 0x00, 0x00, 
                                  0x00, 0x00};

/**
 *****************************************************************************
 **
 ** @brief  Command main routine
 **
 ** Command synopsis:
 **
 **     dx101 INPUTFILE
 **
 ** @exitcode   0               Everything *seemed* to work.
 **             1               Bad usage
 **             2               Could not open input file
 **
 *****************************************************************************/

int
main (int ac, char *av[])
{
    CTypeRegistry *             pTypeRegistry;
    CXMLTextDecoder *           pDecoder;
    CMessage *                  pMessage;
    xmlDoc*                     pDoc;
    xmlNode*                    pNode;
    /*
     * Check arg count
     */
    if(ac != 2)
    {
        fprintf(stderr, "ERROR: Bad usage\nusage: %s INPUTFILE\n", av[0]);
        exit(1);
    }

    /*
     * Construct the type registry. This is needed for decode.
     */
    pTypeRegistry = getTheTypeRegistry();


    /* use libXML to read the DOM */
    xmlInitParser();

    /* set the line numbers for error reporting */
    xmlLineNumbersDefault(1);

    pDoc = xmlReadFile(av[1], 
                       NULL, 
                       XML_PARSE_COMPACT | XML_PARSE_NONET);

    if(NULL == pDoc)
    {
        fprintf(stderr, "ERROR: Could not read XML File\n");
        delete pTypeRegistry;
        xmlCleanupParser();
        exit(2);
    }

    pNode = xmlDocGetRootElement(pDoc);

    /* find the first element node and make sure its PS */
    while((pNode) && (XML_ELEMENT_NODE != pNode->type))
    {
        pNode = pNode->next;
    }

    if(NULL == pNode)
    {
        fprintf(stderr, "ERROR: Could not get XML PacketSequence node.  " \
                        "Found no top-level Element nodes \n");
        delete pTypeRegistry;
        xmlCleanupParser();
        exit(3);
    }

    if(strcmp((const char*) pNode->name, "packetSequence") != 0)
    {
        fprintf(stderr, "ERROR: Could not get XML PacketSequence node.  " \
                        "Found %s instead\n", pNode->name);
        xmlFreeDoc(pDoc);
        xmlCleanupParser();
        delete pTypeRegistry;
        exit(4);
    }
    
    /* not sure this is necessary */
    freopen(NULL, "wb", stdout);

    /* packets are all children of the first node */
    pNode = pNode->children;
  
    while(pNode != NULL)
    {
        if(XML_ELEMENT_NODE == pNode->type)
        {
            xmlChar *           pMessageIDStr = NULL;

            /* It helps to know the message ID for debugging */
            pMessageIDStr = xmlGetProp(pNode, (const xmlChar*) "MessageID");

            /*
             * Construct a frame decoder. It references the
             * type registry and the input buffer.
             */
            pDecoder = new CXMLTextDecoder(pTypeRegistry, pNode);

            /*
             * Now ask the frame decoder to actually decode
             * the message. It returns NULL for an error.
             */
             pMessage = pDecoder->decodeMessage();

            /*
             * Did the decode fail?
             */
            if(NULL == pMessage)
            {
                const CErrorDetails *pError;
 
                /* encode error message as binary */
                fwrite(errMsgBinary, 1, sizeof(errMsgBinary), stdout);

                pError = &pDecoder->m_ErrorDetails;
 
#ifdef XML2LLRP_DEBUG
                fprintf(stderr, "ERROR: Decoder error, result=%d\n",
                        pError->m_eResultCode);

                if(NULL != pMessageIDStr)
                {
                    fprintf(stderr, "ERROR ... MessageID=%s\n", 
                           pMessageIDStr);
                }
                if(NULL != pError->m_pRefType)
                { 
                    fprintf(stderr, "ERROR ... refType=%s\n",
                           pError->m_pRefType->m_pName);
                }
                if(NULL != pError->m_pRefField)
                {
                    fprintf(stderr, "ERROR ... refField=%s\n",
                           pError->m_pRefField->m_pName);
                }
                if(NULL != pError->m_pWhatStr)
                {
                    fprintf(stderr, "ERROR ... whatStr=%s\n",
                           pError->m_pWhatStr); 
                }
                if(0 != pError->m_OtherDetail)
                {
                    fprintf(stderr, "ERROR ... XML line number %d\n", 
                            pError->m_OtherDetail);
                }
#endif /* XML2LLRP_DEBUG */

                delete pDecoder;
                xmlFree(pMessageIDStr);
            }
            else
            {
                unsigned char           aOutBuffer[FRAME_BUF_SIZE];
                unsigned int            nOutBuffer;
                CFrameEncoder *         pEncoder;

#ifdef XML2LLRP_DEBUG
                if(NULL == pMessageIDStr)
                {
                    pMessageIDStr = (xmlChar*) "unknown";
                }
                fprintf(stderr, "SUCCESS ... MessageID=%s passed encoding\n", 
                        pMessageIDStr);

#endif  /* XML2LLRP_DEBUG */

                xmlFree(pMessageIDStr);
                delete pDecoder;
                
                /* encode the message as binary */
 
                /*
                 * Zero fill the buffer to make things easier
                 * on the debugger.
                 */
                memset(aOutBuffer, 0, sizeof aOutBuffer);

                /*
                 * Construct a frame encoder. It references
                 * the output buffer and knows the maximum size.
                 */
                pEncoder = new CFrameEncoder(aOutBuffer, 
                                             sizeof aOutBuffer);

                /*
                 * Do the encode.
                 * TODO: check the result, tattle on errors.
                 */
                pEncoder->encodeElement(pMessage);

                /*
                 * Get the byte length of the resulting frame.
                 */
                nOutBuffer = pEncoder->getLength();

                /*
                 * Check the status, tattle on errors
                 */
                if(RC_OK != pEncoder->m_ErrorDetails.m_eResultCode)
                {
                    const CErrorDetails *pError;

                    pError = &pEncoder->m_ErrorDetails;

                    /* encode error message as binary */
                    fwrite(errMsgBinary,1, sizeof(errMsgBinary), stdout);

#ifdef XML2LLRP_DEBUG
                    fprintf(stderr, "Failed to Encode XML message\n");
                    fprintf(stderr, "ERROR: Encoder error, status=%d\n",
                            pError->m_eResultCode);
                    if(NULL != pError->m_pRefType)
                    {
                        fprintf(stderr, "ERROR ... refType=%s\n",
                                pError->m_pRefType->m_pName);
                    }
                    if(NULL != pError->m_pRefField)
                    {
                        fprintf(stderr, "ERROR ... refField=%s\n",
                                pError->m_pRefField->m_pName);
                    }
#endif /* XML2LLRP_DEBUG */
                }
                else
                {
                    fwrite(aOutBuffer, 1, nOutBuffer, stdout);
                }


                /* free the frame encoder */
                delete pEncoder;
            }

            /* free the message we built */
            delete pMessage;
        }
        pNode = pNode->next;
    }

    xmlFreeDoc(pDoc);
    xmlCleanupParser();
    delete pTypeRegistry;

    /*
     * When we get here everything that was allocated
     * should now be deallocated.
     */
    return 0;
}



