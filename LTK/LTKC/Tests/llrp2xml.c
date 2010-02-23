
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
 ** @file  llrp2xml.c
 **
 ** @brief Converts LLRP binary packet stream to an LTK-XML packet sequence
 **
 ** This is a diagnostic tool for the LLRP Tool Kit for C (LTKC).
 **
 ** llrp2xml reads an input file that contains consecutive LLRP frames,
 ** sometimes called the "binary encoding". Its output is printed
 ** on stdout and conforms to the LTK-XML packet sequence format.
 **
 ** For each input frame:
 **     - Decode the frame into an LLRP message object
 **     -     on success: outputs the object as LTK-SML to stdout
 **     -     on failure: outputs ERROR_MESSAGE as LTK-XML to stdout
 **
 ** This program can be tested using tools like valgrind (please
 ** see http://en.wikipedia.org/wiki/Valgrind) that detect memory leaks.
 **
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

#include "ltkc.h"

/* Buffer sizes */
#define FRAME_BUF_SIZE          (4u*1024u*1024u)
#define XML_TEXT_BUF_SIZE       (10u * FRAME_BUF_SIZE)




/* forward declaration */
void
dump (
  unsigned char *               pBuffer,
  unsigned int                  nBuffer);

/* This is the message format that is agreed upon when messages fail
** to decode. It's somewhat arbitrary, but allows us to do easy
** comparisons */
static char * errMsgStr = "<ERROR_MESSAGE MessageID=\"0\" Version=\"0\">\n" \
                          "  <LLRPStatus>\n" \
                          "    <StatusCode>M_Success</StatusCode>\n" \
                          "    <ErrorDescription></ErrorDescription>\n" \
                          "  </LLRPStatus>\n" \
                          "</ERROR_MESSAGE>\n";

#if 0
/* the corresponding binary packet */
static unsigned char errMsgBinary[18] = {0x04, 0x64, 0x00, 0x00, 
                                         0x00, 0x12, 0x00, 0x00, 
                                         0x00, 0x00, 0x01, 0x1F, 
                                         0x00, 0x08, 0x00, 0x00, 
                                         0x00, 0x00};
#endif

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
  "                      http://www.llrp.org/ltk/schema/core/encoding/xml/1.0/llrp.xsd'>\n"
};

static char
g_aPacketSequenceFooter[] =
{
  "\n</ps:packetSequence>\n"
};


/*
 * These used to be allocated as local (auto) variables.
 * But they are really, really big and Linux has a 10mb
 * stack limit. So they had to be moved here.
 */
unsigned char                   aInBuffer[FRAME_BUF_SIZE];
char                            aXMLTextBuf[XML_TEXT_BUF_SIZE];
unsigned char                   aOutBuffer[FRAME_BUF_SIZE];


/**
 *****************************************************************************
 **
 ** @brief  Command main routine
 **
 ** Command synopsis:
 **
 **     llrp2xml INPUTFILE
 **
 ** @exitcode   0               Everything *seemed* to work.
 **             1               Bad usage
 **             2               Could not open input file
 **
 *****************************************************************************/

int
main (int ac, char *av[])
{
    LLRP_tSTypeRegistry *       pTypeRegistry;
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
    infp = fopen(av[1], "r");
    if(NULL == infp)
    {
        fprintf(stderr, "ERROR: Can't open file %s\n", av[1]);
        exit(2);
    }

    fprintf (stdout, "%s\n", g_aPacketSequenceHeader);

    /*
     * Construct the type registry. This is needed for decode.
     */
    pTypeRegistry = LLRP_getTheTypeRegistry();


    /*
     * Loop iterates for each input frame
     */
    for(;;)
    {
        unsigned int            nInBuffer = sizeof aInBuffer;
        int                     bEOF;
        LLRP_tSFrameDecoder *   pDecoder;
        LLRP_tSMessage *        pMessage;
        LLRP_tSXMLTextEncoder * pEncoder;

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
            LLRP_tSFrameExtract MyFrameExtract;

            /*
             * Ask LLRP_FrameExtract() how we are doing
             * on building a frame. It'll tell us the
             * status and possibly the number of bytes
             * still needed.
             */
            MyFrameExtract = LLRP_FrameExtract(aInBuffer, nInBuffer);

            /*
             * If there is a framing error we have to declare
             * defeat. There is no way to realign the input
             * stream to a frame boundary. This could mean
             * the input file is bad or that the extract
             * function is broken.
             */
            if(LLRP_FRAME_ERROR == MyFrameExtract.eStatus)
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
            if(LLRP_FRAME_NEED_MORE == MyFrameExtract.eStatus)
            {
                int             rc;

                if (sizeof aInBuffer <
                        nInBuffer + MyFrameExtract.nBytesNeeded)
                {
                    fprintf (stderr, "Input frame too big\n");
                    exit(3);
                }

                rc = fread(&aInBuffer[nInBuffer], 1u,
                            MyFrameExtract.nBytesNeeded, infp);
                if(rc <= 0)
                {
                    if(ferror(infp))
                    {
                        fprintf(stderr, "ERROR: bad file read status\n");
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
            if(LLRP_FRAME_READY != MyFrameExtract.eStatus)
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
        fprintf (stdout, "\n");

        /*
         * Construct a frame decoder. It references the
         * type registry and the input buffer.
         */
        pDecoder = LLRP_FrameDecoder_construct(pTypeRegistry,
                                               aInBuffer, nInBuffer);

        /*
         * Now ask the frame decoder to actually decode
         * the message. It returns NULL for an error.
         */
        pMessage = LLRP_Decoder_decodeMessage(&pDecoder->decoderHdr);

        /*
         * Did the decode fail?
         */
        if(NULL == pMessage)
        {
            const LLRP_tSErrorDetails *pError;

            pError = &pDecoder->decoderHdr.ErrorDetails;

#ifdef LLRP2XML_DEBUG
            fprintf(stderr, "ERROR: Decoder error, result=%d\n",
                    pError->eResultCode);

            if(NULL != pError->pRefType)
            {
                fprintf(stderr, "ERROR ... refType=%s\n",
                        pError->pRefType->pName);
            }
            if(NULL != pError->pRefField)
            {
                fprintf(stderr, "ERROR ... refField=%s\n",
                        pError->pRefField->pName);
            }
#endif /* LLRP2XML_DEBUG */

            /* if decode fails, write the error message */
            fprintf(stdout, "%s", errMsgStr);

            LLRP_Decoder_destruct(&pDecoder->decoderHdr);
            continue;
        }

        /*
         * pMessage points to the root of an object
         * tree representing the LLRP message.
         * We are done with the frame decoder.
         */
        LLRP_Decoder_destruct(&pDecoder->decoderHdr);

        /*
         * Print as XML text the LLRP message to stdout.
         */
        pEncoder = LLRP_XMLTextEncoder_construct((unsigned char*) aXMLTextBuf,
                sizeof aXMLTextBuf);

        LLRP_Encoder_encodeElement(&pEncoder->encoderHdr,
                &pMessage->elementHdr);
        if(!pEncoder->bOverflow)
        {
            printf("%s", aXMLTextBuf);
        }
        else
        {
            fprintf(stderr, "<!-- Buffer overflow -->\n");
            /* if decode fails, write the error message */
            fprintf(stdout, "%s", errMsgStr);
        }
        LLRP_Encoder_destruct(&pEncoder->encoderHdr);

        /*
         * Destruct the message. This must deallocate
         * everything that was allocated during decode.
         */
        LLRP_Element_destruct(&pMessage->elementHdr);
    }

    fprintf (stdout, "%s\n", g_aPacketSequenceFooter);

    /*
     * Done with the type registry.
     */
    LLRP_TypeRegistry_destruct(pTypeRegistry);

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

