
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
 ** @file  llrp2xml.cpp
 **
 ** @brief Converts an LLRP binary stream to an LTK-XML packet sequence
 **
 ** This is diagnostic 101 for the LLRP Tool Kit for C++ (LTKCPP).
 **
 ** llrp2xml reads an input file that contains consecutive LLRP frames,
 ** sometimes called the "binary encoding". Its output is printed
 ** on stdout.
 **
 ** For each input frame:
 **     - Decode the frame into an LLRP message object
 **     - Encode as XML text (essentially print) the message to stdout
 **
 ** This program can be tested using tools like valgrind (please
 ** see http://en.wikipedia.org/wiki/Valgrind) that detect memory leaks.
 **
 ** There are "golden" test files under the ../../Tests/dx101/ directory.
 ** Normal use is something like
 **
 **     llrp2xml ../../Tests/dx101/dx101_in.bin > dx101_out.tmp
 **
 ** Then to verify proper function, the output file is compared
 **
 **     cmp ../../Tests/dx101/dx101_out.txt dx101_out.tmp
 **
 ** When the files compare it means the dx101 and the LTKC are correct.
 **
 *****************************************************************************/


#include <stdio.h>

#include "ltkcpp.h"


using namespace LLRP;


/* Buffer sizes */
#define FRAME_BUF_SIZE          (4u*1024u*1024u)
#define XML_TEXT_BUF_SIZE       (10u * FRAME_BUF_SIZE)


/* forward declaration */
void
dump (
  unsigned char *               pBuffer,
  unsigned int                  nBuffer);


/*
 * XML header and footer enclosing the sequence of messages.
 */
static char
g_aPacketSequenceHeader[] =
{
  "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
  "\n"
  "<ps:packetSequence\n"
  "  xmlns='http://www.llrp.org/ltk/schema/core/encoding/xml/1.0'\n"
  "  xmlns:xsi='http://www.w3.org/2001/XMLSchema-instance'\n"
  "  xmlns:ps='http://www.llrp.org/ltk/schema/testing/encoding/xml/0.6'\n"
  "  xsi:schemaLocation='http://www.llrp.org/ltk/schema/core/encoding/xml/1.0\n"
  "                      http://www.llrp.org/ltk/schema/core/encoding/xml/1.0/llrp.xsd\n'>"
                        
};

static char
g_aPacketSequenceFooter[] =
{
  "\n</ps:packetSequence>\n"
};

/* This is the message format that is agreed upon when messages fail
** to decode. It's somewhat arbitrary, but allows us to do easy
** comparisons */
static char * g_errMsgStr = "<ERROR_MESSAGE MessageID=\"0\" Version=\"0\">\n" \
                            "  <LLRPStatus>\n" \
                            "    <StatusCode>M_Success</StatusCode>\n" \
                            "    <ErrorDescription></ErrorDescription>\n" \
                            "  </LLRPStatus>\n" \
                            "</ERROR_MESSAGE>\n";

/*
 * These used to be allocated as local (auto) variables.
 * But they are really, really big and Linux has a 10mb
 * stack limit. So they had to be moved here.
 */
unsigned char                   aInBuffer[FRAME_BUF_SIZE];
char                            aXMLTextBuf[XML_TEXT_BUF_SIZE];

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
    FILE *                      infp;

    /*
     * Check arg count
     */
    if(ac != 2)
    {
        fprintf(stderr, "ERROR: Bad usage\nusage: %s INPUTFILE\n", av[0]);
        exit(1);
    }

    /*
     * Open input file
     */
#ifdef WIN32
    infp = fopen(av[1], "rb");
#else
    infp = fopen(av[1], "r");
#endif
    if(NULL == infp)
    {
        perror(av[1]);
        exit(2);
    }

    printf("%s\n", g_aPacketSequenceHeader);

    /*
     * Construct the type registry. This is needed for decode.
     */
    pTypeRegistry = getTheTypeRegistry();

    /*
     * Loop iterates for each input frame
     */
    for(;;)
    {
//        unsigned char           aInBuffer[FRAME_BUF_SIZE];
        unsigned int            nInBuffer = sizeof aInBuffer;
        bool                    bEOF;

        /*
         * Zero fill the buffer to make things easier
         * for printing the buffer on the debugger.
         */
        memset(aInBuffer, 0, nInBuffer);

        /*
         * Set status variables before entering the frame read loop.
         */
        nInBuffer = 0;
        bEOF = FALSE;

        /*
         * Loop iterates for each individual file read.
         * The size of each read is guided by LLRP_FrameExtract.
         */
        for(;;)
        {
            /*
             * Ask LLRP_FrameExtract() how we are doing
             * on building a frame. It'll tell us the
             * status and possibly the number of bytes
             * still needed.
             */
            CFrameExtract       MyFrameExtract(aInBuffer, nInBuffer);

            /*
             * If there is a framing error we have to declare
             * defeat. There is no way to realign the input
             * stream to a frame boundary. This could mean
             * the input file is bad or that the extract
             * function is broken.
             */
            if(CFrameExtract::FRAME_ERROR == MyFrameExtract.m_eStatus)
            {
                fprintf(stderr, "ERROR: Frame error, bail!\n");
                bEOF = TRUE;
                break;
            }

            /*
             * If we need more bytes read them in. This may
             * not request the entire frame. It might be
             * only asking form enough of the frame so that
             * LLRP_FrameExtract() can determine the actual
             * size of the frame.
             */
            if(CFrameExtract::NEED_MORE == MyFrameExtract.m_eStatus)
            {
                int             rc;

                if (sizeof aInBuffer <
                        nInBuffer + MyFrameExtract.m_nBytesNeeded)
                {
                    fprintf(stderr,"Input frame too big\n");
                    exit(3);
                }

                rc = (int)fread(&aInBuffer[nInBuffer], 1u,
                            MyFrameExtract.m_nBytesNeeded, infp);
                if(rc <= 0)
                {
                    if(ferror(infp))
                    {
                        fprintf(stderr,"ERROR: bad file read status\n");
                    }
                    bEOF = TRUE;
                    break;
                }
                nInBuffer += rc;
                continue;
            }

            /*
             * The only remaining extract status we recognize
             * is READY. If it's anything else, give up.
             * This probably means that the frame extract
             * function is broken.
             */
            if(CFrameExtract::READY != MyFrameExtract.m_eStatus)
            {
                fprintf(stderr, "ERROR: Unrecognized extract status, bail!\n");
                bEOF = TRUE;
                break;
            }

            break;
        }

        /*
         * Did the inner loop detect and end-of-file or other
         * reason to stop?
         */
        if(bEOF)
        {
            if(0 < nInBuffer)
            {
                fprintf(stderr, "ERROR: EOF w/ %u bytes in buffer\n", nInBuffer);
            }
            break;
        }

        /* Put a blank line between messages */
        printf ("\n");

        /*
         * Construct a frame decoder. It references the
         * type registry and the input buffer.
         */
        CFrameDecoder           MyFrameDecoder(pTypeRegistry,
                                        aInBuffer, nInBuffer);

        /*
         * Now ask the frame decoder to actually decode
         * the message. It returns NULL for an error.
         */
        CMessage *              pMessage;

        pMessage = MyFrameDecoder.decodeMessage();

        /*
         * Did the decode fail?
         */
        if(NULL == pMessage)
        {
            const CErrorDetails *pError;

            pError = &MyFrameDecoder.m_ErrorDetails;

            fprintf(stderr, "ERROR: Decoder error, result=%d\n",
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

	    printf("%s\n", g_errMsgStr);

            continue;
        }

        /*
         * pMessage points to the root of an object
         * tree representing the LLRP message.
         */

        /*
         * Print as XML text the LLRP message to stdout.
         */
        {
            CXMLTextEncoder     MyXMLEncoder(aXMLTextBuf, sizeof aXMLTextBuf);

            MyXMLEncoder.encodeElement(pMessage);
            if(!MyXMLEncoder.m_bOverflow)
            {
                printf("%s", aXMLTextBuf);
            }
            else
            {
                fprintf(stderr, "<!-- Buffer overflow -->\n");
	        printf("%s\n", g_errMsgStr);
            }
        }
    
        delete pMessage;
    }

    printf("%s\n", g_aPacketSequenceFooter);

    /*
     * Done with the type registry.
     */
    delete pTypeRegistry;

    /*
     * Done with the input file.
     */
    fclose(infp);

    /*
     * When we get here everything that was allocated
     * should now be deallocated.
     */
    return 0;
}

/**
 *****************************************************************************
 **
 ** @brief  Print a buffer in hex
 **
 ** And don't we always need one of these.
 **     - 16 bytes per line
 **     - extra space every four bytes
 **     - full lines have a three digit sum, used to speed visually
 **       comparing entire lines.
 **
 ** @param[in]  pBuffer         Pointer to buffer
 ** @param[in]  nBuffer         Number of valid bytes in buffer
 **
 ** @return     none
 **
 *****************************************************************************/

void
dump (
  unsigned char *               pBuffer,
  unsigned int                  nBuffer)
{
    unsigned int                chk = 0;
    unsigned int                i;

    for(i = 0; i < nBuffer; i++)
    {
        if(i%4 == 0)
        {
            printf(" ");
        }
        printf(" %02X", pBuffer[i]);
        chk += pBuffer[i];

        if(i%16 == 15)
        {
            printf("  sum=%03X\n", chk);
            chk = 0;
        }
    }
    printf("\n");
}
