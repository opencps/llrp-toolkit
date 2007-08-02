
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

/**
 *****************************************************************************
 **
 ** @file  dx101.c
 **
 ** @brief Stand alone test of LTKC frame decoder and encoder, and XML encoder
 **
 ** This is diagnostic 101 for the LLRP Tool Kit for C (LTKC).
 **
 ** DX101 reads an input file that contains consecutive LLRP frames,
 ** sometimes called the "binary encoding". Its output is printed
 ** on stdout.
 **
 ** For each input frame:
 **     - Decode the frame into an LLRP message object
 **     - Encode as XML text (essentially print) the message to stdout
 **     - Encode the message into a separate frame buffer
 **     - Compare the two frame buffers and make sure they are the same
 **     - Destruct the LLRP message
 **
 ** This program can be tested using tools like valgrind (please
 ** see http://en.wikipedia.org/wiki/Valgrind) that detect memory leaks.
 **
 ** There are "golden" test files under the ../../Tests/dx101/ directory.
 ** Normal use is something like
 **
 **     dx101 ../../Tests/dx101/dx101_in.bin > dx101_out.tmp
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


/* forward declaration */
void
dump (
  unsigned char *               pBuffer,
  unsigned int                  nBuffer);


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
    LLRP_tSTypeRegistry *       pTypeRegistry;
    FILE *                      infp;

    /*
     * Check arg count
     */
    if(ac != 2)
    {
        printf("ERROR: Bad usage\nusage: %s INPUTFILE\n", av[0]);
        exit(1);
    }

    /*
     * Open input file
     */
    infp = fopen(av[1], "r");
    if(NULL == infp)
    {
        printf("ERROR: Can't open file %s\n", av[1]);
        exit(2);
    }

    printf ("Starting\n");

    /*
     * Construct the type registry. This is needed for decode.
     */
    pTypeRegistry = LLRP_getTheTypeRegistry();

    /*
     * Loop iterates for each input frame
     */
    for(;;)
    {
        unsigned char           aInBuffer[16u*1024u];
        unsigned int            nInBuffer;
        int                     bEOF;
        LLRP_tSFrameDecoder *   pDecoder;
        LLRP_tSMessage *        pMessage;

        /*
         * Zero fill the buffer to make things easier
         * for printing the buffer on the debugger.
         */
        memset(aInBuffer, 0, sizeof aInBuffer);

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
                printf("ERROR: Frame error, bail!\n");
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

                rc = fread(&aInBuffer[nInBuffer], 1u,
                            MyFrameExtract.nBytesNeeded, infp);
                if(rc <= 0)
                {
                    if(ferror(infp))
                    {
                        printf("ERROR: bad file read status\n");
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
                printf("ERROR: Unrecognized extract status, bail!\n");
                bEOF = TRUE;
                break;
            }

            /*
             * The input buffer is ready -- it contains a complete frame.
             * Tattle on key FrameExtract values and exit this
             * frame-read loop.
             */
            printf("Frame nBuf=%u Length=%u ID=%u Type=%u Vers=%u\n",
                nInBuffer,
                MyFrameExtract.MessageLength,
                MyFrameExtract.MessageID,
                MyFrameExtract.MessageType,
                MyFrameExtract.ProtocolVersion);

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
                printf("ERROR: EOF w/ %u bytes in buffer\n", nInBuffer);
            }
            break;
        }

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

            printf("ERROR: Decoder error, result=%d\n",
                pError->eResultCode);
            if(NULL != pError->pRefType)
            {
                printf("ERROR ... refType=%s\n",
                    pError->pRefType->pName);
            }
            if(NULL != pError->pRefField)
            {
                printf("ERROR ... refField=%s\n",
                    pError->pRefField->pName);
            }

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
        {
            LLRP_tSPrXMLEncoder *   pEncoder;

            pEncoder = LLRP_PrXMLEncoder_construct(stdout);
            LLRP_Encoder_encodeElement(&pEncoder->encoderHdr,
                    &pMessage->elementHdr);
            LLRP_Encoder_destruct(&pEncoder->encoderHdr);
        }

        /*
         * Encode the LLRP message into a separate frame buffer.
         * Compare the resulting frame to the input frame.
         * Tattle on any differences.
         */
        {
            unsigned char           aOutBuffer[16u*1024u];
            unsigned int            nOutBuffer;
            LLRP_tSFrameEncoder *   pEncoder;

            /*
             * Zero fill the buffer to make things easier
             * on the debugger.
             */
            memset(aOutBuffer, 0, sizeof aOutBuffer);

            /*
             * Construct a frame encoder. It references
             * the output buffer and knows the maximum size.
             */
            pEncoder = LLRP_FrameEncoder_construct(aOutBuffer,
                sizeof aOutBuffer);

            /*
             * Do the encode.
             * TODO: check the result, tattle on errors.
             */
            LLRP_Encoder_encodeElement(&pEncoder->encoderHdr,
                &pMessage->elementHdr);

            /*
             * Get the byte length of the resulting frame.
             */
            nOutBuffer = pEncoder->iNext;

            /*
             * Check the status, tattle on errors
             */
            if(LLRP_RC_OK != pEncoder->encoderHdr.ErrorDetails.eResultCode)
            {
                const LLRP_tSErrorDetails *pError;

                pError = &pEncoder->encoderHdr.ErrorDetails;
                printf("ERROR: Encoder error, status=%d\n",
                    pError->eResultCode);
                if(NULL != pError->pRefType)
                {
                    printf("ERROR ... refType=%s\n",
                        pError->pRefType->pName);
                }
                if(NULL != pError->pRefField)
                {
                    printf("ERROR ... refField=%s\n",
                        pError->pRefField->pName);
                }
            }

            /*
             * The resulting frame should be an exact
             * match for the input frame.
             */
            if(nOutBuffer != nInBuffer ||
               0 != memcmp(aInBuffer, aOutBuffer, nInBuffer))
            {
                printf("Miscompare\n");

                printf("Input  length=%u\n", nInBuffer);
                dump(aInBuffer, nInBuffer);
                printf("Output length=%u\n", nOutBuffer);
                dump(aOutBuffer, nOutBuffer);
            }

            /*
             * Done with the encoder.
             */
            LLRP_Encoder_destruct(&pEncoder->encoderHdr);
        }

        /*
         * Destruct the message. This must deallocate
         * everything that was allocated during decode.
         */
        LLRP_Element_destruct(&pMessage->elementHdr);
    }

    printf ("Finished\n");

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
