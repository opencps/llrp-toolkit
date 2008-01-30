
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
 ** @file  dx201.c
 **
 ** @brief Simple test of LTKC that talks to an actual reader
 **
 ** This is diagnostic 201 for the LLRP Tool Kit for C (LTKC).
 **
 ** DX201 controls an LLRP reader to do a very simple operation.
 **     - Uses mostly the default settings of the reader
 **     - Simple inventory operation
 **     - Uses all antennas
 **     - Stops when there are no new tags for 3.5 seconds
 **     - Stops after 12.5 seconds no matter what
 **     - Reports after operation stops
 **
 ** It is best to be sure DX10x tests work properly before trying
 ** this test. The one facility DX201 uses that the DX10x tests
 ** don't is the connection management interface (LLRP_tSConnection).
 **
 ** This program can be tested using tools like valgrind (please
 ** see http://en.wikipedia.org/wiki/Valgrind) that detect memory leaks.
 **
 ** This program can be run with zero, one, or two verbose options (-v).
 **     no -v -- Only prints the tag report and errors
 **     -v    -- Also prints one line progress messages
 **     -vv   -- Also prints all LLRP messages as XML text
 **
 ** This is a test of LTKC and also a reasonable example to
 ** help people get started with LTKC.
 **
 *****************************************************************************/


#include <stdio.h>
#ifdef linux
#include <unistd.h>
#endif

#include "ltkc.h"


/*
 * BEGIN forward declarations
 */
int
main (
  int                           ac,
  char *                        av[]);

void
usage (
  char *                        pProgName);

int
run (
  const char *                  pReaderHostName);

int
checkConnectionStatus (void);

int
scrubConfiguration (void);

int
resetConfigurationToFactoryDefaults (void);

int
deleteAllROSpecs (void);

int
deleteAllAccessSpecs (void);

int
prepareReader(void);

int
getAllCapabilities (void);

int
getAllConfiguration (void);

int
configureNotificationStates (void);

int
addROSpec (void);

int
enableROSpec (void);

int
startAndMonitorOperation (void);

void
printTagReportData (
  LLRP_tSRO_ACCESS_REPORT *     pRO_ACCESS_REPORT);

void
printOneTagReportData (
  LLRP_tSTagReportData *        pTagReportData);

int
checkLLRPStatus (
  LLRP_tSLLRPStatus *           pLLRPStatus,
  char *                        pWhatStr);

LLRP_tSMessage *
transact (
  LLRP_tSMessage *              pSendMsg);

LLRP_tSMessage *
recvMessage (
  int                           nMaxMS);

int
sendMessage (
  LLRP_tSMessage *              pSendMsg);

void
freeMessage (
  LLRP_tSMessage *              pMessage);

void
printXMLMessage (
  LLRP_tSMessage *              pMessage);

/*
 * END forward declarations
 */


/*
 * Global variables
 */
/** Verbose level, incremented by each -v on command line */
int                             g_Verbose;
/** ModeIndex requested [012345], -1 means no mode given and use the default */
int                             g_ModeIndex = -1;
/** Quiet flag -q, don't print the tag reports */
int                             g_Quiet;
/** Count of tag reports (RO_ACCESS_REPORT) */
int                             g_nTagReport;
/** Connection to the LLRP reader */
LLRP_tSConnection *             g_pConnectionToReader;



/**
 *****************************************************************************
 **
 ** @brief  Command main routine
 **
 ** Command synopsis:
 **
 **     dx201 [-vvq012345] READERHOSTNAME
 **
 ** Options:
 **     -v      Verbose, each -v increments verbosity
 **     -q      Quiet, don't print tag reports (used for characterization)
 **     -012345 Select mode, if not given the default mode is used
 **
 ** @exitcode   0               Everything *seemed* to work.
 **             1               Bad usage
 **             2               Run failed
 **
 *****************************************************************************/

int
main (
  int                           ac,
  char *                        av[])
{
    char *                      pReaderHostName;
    int                         rc;
#ifdef linux
    char *                      pMemHiwatAtStart = (char*)sbrk(0);
#endif

    /*
     * Process comand arguments, determine reader name
     * and verbosity level.
     */
    if(ac == 2)
    {
        pReaderHostName = av[1];
    }
    else if(ac == 3)
    {
        char *                  p = av[1];

        while(*p)
        {
            switch(*p++)
            {
            case '-':   /* linux conventional option warn char */
            case '/':   /* Windows/DOS conventional option warn char */
                break;

            case 'v':
            case 'V':
                g_Verbose++;
                break;

            case 'q':
            case 'Q':
                g_Quiet++;
                break;

            case '0': case '1': case '2': case '3': case '4': case '5':
                g_ModeIndex = p[-1] - '0';
                break;

            default:
                usage(av[0]);
                /* no return */
                break;
            }
        }

        pReaderHostName = av[2];
    }
    else
    {
        usage(av[0]);
        /* no return */
    }

    /*
     * Run application, capture return value for exit status
     */
    rc = run(pReaderHostName);

    printf("INFO: Done\n");

#ifdef linux
    {
        char *              pMemHiwatAtEnd = (char*)sbrk(0);

        printf("INFO: Needed %d bytes of heap\n",
            pMemHiwatAtEnd - pMemHiwatAtStart);
    }
#endif

    /*
     * Exit with the right status.
     */
    if(0 == rc)
    {
        exit(0);
    }
    else
    {
        exit(2);
    }
    /*NOTREACHED*/
}


/**
 *****************************************************************************
 **
 ** @brief  Print usage message and exit
 **
 ** @param[in]  nProgName       Program name string
 **
 ** @return     none, exits
 **
 *****************************************************************************/

void
usage (
  char *                        pProgName)
{
    printf("Usage: %s [-v] READERHOSTNAME\n", pProgName);
    printf("\n");
    printf("Each -v increases verbosity level\n");
    exit(1);
}


/**
 *****************************************************************************
 **
 ** @brief  Run the application
 **
 ** The steps:
 **     - Instantiate connection
 **     - Connect to LLRP reader (TCP)
 **     - Make sure the connection status is good
 **     - Clear (scrub) the reader configuration
 **     - Configure for what we want to do
 **     - Run one inventory operation
 **     - Again, clear (scrub) the reader configuration
 **     - Disconnect from reader
 **     - Destruct connection
 **
 ** @param[in]  pReaderHostName String with reader name
 **
 ** @return      0              Everything worked.
 **             -1              Failed allocation of type registry
 **             -2              Failed construction of connection
 **             -3              Could not connect to reader
 **              1              Reader connection status bad
 **              2              Cleaning reader config failed
 **              3              Preparing reader (ADD_ROSPEC) failed
 **              4              Something went wrong running the ROSpec
 **
 *****************************************************************************/

int
run (
  const char *                  pReaderHostName)
{
    LLRP_tSTypeRegistry *       pTypeRegistry;
    LLRP_tSConnection *         pConn;
    int                         rc;

    /*
     * Allocate the type registry. This is needed
     * by the connection to decode.
     */
    pTypeRegistry = LLRP_getTheTypeRegistry();
    if(NULL == pTypeRegistry)
    {
        printf("ERROR: getTheTypeRegistry failed\n");
        return -1;
    }

    /*
     * Construct a connection (LLRP_tSConnection).
     * Using a 32kb max frame size for send/recv.
     * The connection object is ready for business
     * but not actually connected to the reader yet.
     */
    pConn = LLRP_Conn_construct(pTypeRegistry, 32u*1024u);
    if(NULL == pConn)
    {
        printf("ERROR: Conn_construct failed\n");
        return -2;
    }

    /*
     * Open the connection to the reader
     */
    if(g_Verbose)
    {
        printf("INFO: Connecting to %s....\n", pReaderHostName);
    }

    rc = LLRP_Conn_openConnectionToReader(pConn, pReaderHostName);
    if(0 != rc)
    {
        printf("ERROR: connect: %s (%d)\n", pConn->pConnectErrorStr, rc);
        LLRP_Conn_destruct(pConn);
        return -3;
    }

    /*
     * Record the pointer to the connection object so other
     * routines can use it.
     */
    g_pConnectionToReader = pConn;

    if(g_Verbose)
    {
        printf ("INFO: Connected, checking status....\n");
    }

    /*
     * Commence the sequence and check for errors as we go.
     * See comments for each routine for details.
     * Each routine prints messages per verbose level.
     */
    rc = 1;
    if(0 == checkConnectionStatus())
    {
        rc = 2;
        if(0 == scrubConfiguration())
        {
            rc = 3;
            if(0 == prepareReader())
            {
                rc = 4;
                if(0 == startAndMonitorOperation())
                {
                    rc = 0;
                }
            }

            /*
             * After we're done, try to leave the reader
             * in a clean state for next use. This is best
             * effort and no checking of the result is done.
             */
            if(g_Verbose)
            {
                printf("INFO: Clean up reader configuration...\n");
            }
            scrubConfiguration();
        }
    }

    if(g_Verbose)
    {
        printf ("INFO: Finished\n");
    }

    /*
     * Close the connection and release its resources
     */
    LLRP_Conn_closeConnectionToReader(pConn);
    LLRP_Conn_destruct(pConn);

    /*
     * Done with the registry.
     */
    LLRP_TypeRegistry_destruct(pTypeRegistry);

    /*
     * Maybe tattle on the number of tags. Normally tags are reported.
     * The -q command option prevents that and instead asks for just a
     * total. With the -v command option we're probably reporting tags
     * and a final count.
     */
    if(g_Verbose || g_Quiet)
    {
        printf("INFO: %d Tag reports\n", g_nTagReport);
    }

    /*
     * When we get here all allocated memory should have been deallocated.
     */

    return rc;
}


/**
 *****************************************************************************
 **
 ** @brief  Await and check the connection status message from the reader
 **
 ** We are expecting a READER_EVENT_NOTIFICATION message that
 ** tells us the connection is OK. The reader is suppose to
 ** send the message promptly upon connection.
 **
 ** If there is already another LLRP connection to the
 ** reader we'll get a bad Status.
 **
 ** The message should be something like:
 **
 **     <READER_EVENT_NOTIFICATION MessageID='0'>
 **       <ReaderEventNotificationData>
 **         <UTCTimestamp>
 **           <Microseconds>1184491439614224</Microseconds>
 **         </UTCTimestamp>
 **         <ConnectionAttemptEvent>
 **           <Status>Success</Status>
 **         </ConnectionAttemptEvent>
 **       </ReaderEventNotificationData>
 **     </READER_EVENT_NOTIFICATION>
 **
 ** @return     ==0             Everything OK
 **             !=0             Something went wrong
 **
 *****************************************************************************/

int
checkConnectionStatus (void)
{
    LLRP_tSMessage *            pMessage;
    LLRP_tSREADER_EVENT_NOTIFICATION *pNtf;
    LLRP_tSReaderEventNotificationData *pNtfData;
    LLRP_tSConnectionAttemptEvent *pEvent;

    /*
     * Expect the notification within 10 seconds.
     * It is suppose to be the very first message sent.
     */
    pMessage = recvMessage(10000);

    /*
     * recvMessage() returns NULL if something went wrong.
     */
    if(NULL == pMessage)
    {
        /* recvMessage() already tattled. */
        goto fail;
    }

    /*
     * Check to make sure the message is of the right type.
     * The type label (pointer) in the message should be
     * the type descriptor for READER_EVENT_NOTIFICATION.
     */
    if(&LLRP_tdREADER_EVENT_NOTIFICATION != pMessage->elementHdr.pType)
    {
        goto fail;
    }

    /*
     * Now that we are sure it is a READER_EVENT_NOTIFICATION,
     * traverse to the ReaderEventNotificationData parameter.
     */
    pNtf = (LLRP_tSREADER_EVENT_NOTIFICATION *) pMessage;
    pNtfData = pNtf->pReaderEventNotificationData;
    if(NULL == pNtfData)
    {
        goto fail;
    }

    /*
     * The ConnectionAttemptEvent parameter must be present.
     */
    pEvent = pNtfData->pConnectionAttemptEvent;
    if(NULL == pEvent)
    {
        goto fail;
    }

    /*
     * The status in the ConnectionAttemptEvent parameter
     * must indicate connection success.
     */
    if(LLRP_ConnectionAttemptStatusType_Success != pEvent->eStatus)
    {
        goto fail;
    }

    /*
     * Done with the message
     */
    freeMessage(pMessage);

    if(g_Verbose)
    {
        printf("INFO: Connection status OK\n");
    }

    /*
     * Victory.
     */
    return 0;

  fail:
    /*
     * Something went wrong. Tattle. Clean up. Return error.
     */
    printf("ERROR: checkConnectionStatus failed\n");
    freeMessage(pMessage);
    return -1;
}


/**
 *****************************************************************************
 **
 ** @brief  Scrub the reader configuration
 **
 ** The steps:
 **     - Try to reset configuration to factory defaults,
 **       this feature is optional and may not be supported
 **       by the reader.
 **     - Delete all ROSpecs
 **     - Delete all AccessSpecs
 **
 ** This deliberately has the steps as separate 'if' statements
 ** to aid easier debugging.
 **
 ** Upon successful return, the reader is as close to
 ** brand new as we can make it.
 **
 ** @return     ==0             Everything OK
 **             !=0             Something went wrong
 **
 *****************************************************************************/

int
scrubConfiguration (void)
{
    if(0 != resetConfigurationToFactoryDefaults())
    {
        return -1;
    }

    if(0 != deleteAllROSpecs())
    {
        return -2;
    }

    if(0 != deleteAllAccessSpecs())
    {
        return -3;
    }

    return 0;
}


/**
 *****************************************************************************
 **
 ** @brief  Send a SET_READER_CONFIG message that resets the
 **         reader to factory defaults.
 **
 ** NB: The ResetToFactoryDefault semantics vary between readers.
 **     It might have no effect because it is optional.
 **
 ** The message is:
 **
 **     <SET_READER_CONFIG MessageID='101'>
 **       <ResetToFactoryDefault>1</ResetToFactoryDefault>
 **     </SET_READER_CONFIG>
 **
 ** @return     ==0             Everything OK
 **             !=0             Something went wrong
 **
 *****************************************************************************/

int
resetConfigurationToFactoryDefaults (void)
{
    LLRP_tSSET_READER_CONFIG    Cmd = {
        .hdr.elementHdr.pType   = &LLRP_tdSET_READER_CONFIG,
        .hdr.MessageID          = 101,

        .ResetToFactoryDefault  = 1
    };
    LLRP_tSMessage *            pRspMsg;
    LLRP_tSSET_READER_CONFIG_RESPONSE *pRsp;

    /*
     * Send the message, expect the response of certain type
     */
    pRspMsg = transact(&Cmd.hdr);
    if(NULL == pRspMsg)
    {
        /* transact already tattled */
        return -1;
    }

    /*
     * Cast to a SET_READER_CONFIG_RESPONSE message.
     */
    pRsp = (LLRP_tSSET_READER_CONFIG_RESPONSE *) pRspMsg;

    /*
     * Check the LLRPStatus parameter.
     */
    if(0 != checkLLRPStatus(pRsp->pLLRPStatus,
                            "resetConfigurationToFactoryDefaults"))
    {
        /* checkLLRPStatus already tattled */
        freeMessage(pRspMsg);
        return -1;
    }

    /*
     * Done with the response message.
     */
    freeMessage(pRspMsg);

    /*
     * Tattle progress, maybe
     */
    if(g_Verbose)
    {
        printf("INFO: Configuration reset to factory defaults\n");
    }

    /*
     * Victory.
     */
    return 0;
}


/**
 *****************************************************************************
 **
 ** @brief  Delete all ROSpecs using DELETE_ROSPEC message
 **
 ** Per the spec, the DELETE_ROSPEC message contains an ROSpecID
 ** of 0 to indicate we want all ROSpecs deleted.
 **
 ** The message is
 **
 **     <DELETE_ROSPEC MessageID='102'>
 **       <ROSpecID>0</ROSpecID>
 **     </DELETE_ROSPEC>
 **
 ** @return     ==0             Everything OK
 **             !=0             Something went wrong
 **
 *****************************************************************************/

int
deleteAllROSpecs (void)
{
    LLRP_tSDELETE_ROSPEC        Cmd = {
        .hdr.elementHdr.pType   = &LLRP_tdDELETE_ROSPEC,
        .hdr.MessageID          = 102,

        .ROSpecID               = 0     /* All */
    };
    LLRP_tSMessage *            pRspMsg;
    LLRP_tSDELETE_ROSPEC_RESPONSE *pRsp;

    /*
     * Send the message, expect the response of certain type
     */
    pRspMsg = transact(&Cmd.hdr);
    if(NULL == pRspMsg)
    {
        /* transact already tattled */
        return -1;
    }

    /*
     * Cast to a DELETE_ROSPEC_RESPONSE message.
     */
    pRsp = (LLRP_tSDELETE_ROSPEC_RESPONSE *) pRspMsg;

    /*
     * Check the LLRPStatus parameter.
     */
    if(0 != checkLLRPStatus(pRsp->pLLRPStatus, "deleteAllROSpecs"))
    {
        /* checkLLRPStatus already tattled */
        freeMessage(pRspMsg);
        return -1;
    }

    /*
     * Done with the response message.
     */
    freeMessage(pRspMsg);

    /*
     * Tattle progress, maybe
     */
    if(g_Verbose)
    {
        printf("INFO: All ROSpecs are deleted\n");
    }

    /*
     * Victory.
     */
    return 0;
}


/**
 *****************************************************************************
 **
 ** @brief  Delete all AccessSpecs using DELETE_ACCESSSPEC message
 **
 ** Per the spec, the DELETE_ACCESSSPEC message contains an AccessSpecID
 ** of 0 to indicate we want all AccessSpecs deleted.
 **
 ** The message is
 **
 **     <DELETE_ACCESSSPEC MessageID='103'>
 **       <AccessSpecID>0</AccessSpecID>
 **     </DELETE_ACCESSSPEC>
 **
 ** @return     ==0             Everything OK
 **             !=0             Something went wrong
 **
 *****************************************************************************/

int
deleteAllAccessSpecs (void)
{
    LLRP_tSDELETE_ACCESSSPEC    Cmd = {
        .hdr.elementHdr.pType   = &LLRP_tdDELETE_ACCESSSPEC,
        .hdr.MessageID          = 103,

        .AccessSpecID           = 0     /* All */
    };
    LLRP_tSMessage *            pRspMsg;
    LLRP_tSDELETE_ACCESSSPEC_RESPONSE *pRsp;

    /*
     * Send the message, expect the response of certain type
     */
    pRspMsg = transact(&Cmd.hdr);
    if(NULL == pRspMsg)
    {
        /* transact already tattled */
        return -1;
    }

    /*
     * Cast to a DELETE_ACCESSSPEC_RESPONSE message.
     */
    pRsp = (LLRP_tSDELETE_ACCESSSPEC_RESPONSE *) pRspMsg;

    /*
     * Check the LLRPStatus parameter.
     */
    if(0 != checkLLRPStatus(pRsp->pLLRPStatus, "deleteAllAccessSpecs"))
    {
        /* checkLLRPStatus already tattled */
        freeMessage(pRspMsg);
        return -1;
    }

    /*
     * Done with the response message.
     */
    freeMessage(pRspMsg);

    /*
     * Tattle progress, maybe
     */
    if(g_Verbose)
    {
        printf("INFO: All AccessSpecs are deleted\n");
    }

    /*
     * Victory.
     */
    return 0;
}


/**
 *****************************************************************************
 **
 ** @brief  Prepare the reader for our inventory operation
 **
 ** The steps:
 **     - Get the capabilities, not strictly necessary but a real
 **       application would do this to adapt to the reader.
 **     - Get the reader configuration, not strictly necessary
 **       but a real reader would check the config
 **     - Configure the notifications so that we get the
 **       notifications we want
 **     - Get the configuration again to verify the effect
 **       of the set configuration message we sent
 **     - Add our ROSpec
 **     - Enable it
 **
 ** This deliberately has the steps as separate 'if' statements
 ** to aid easier debugging.
 **
 ** Upon successful return, the reader is configured and
 ** ready to do inventory operations.
 **
 ** @return     ==0             Everything OK
 **             !=0             Something went wrong
 **
 *****************************************************************************/

int
prepareReader(void)
{
    if(0 != getAllCapabilities())
    {
        return -1;
    }

    if(0 != getAllConfiguration())
    {
        return -2;
    }

    if(0 != configureNotificationStates())
    {
        return -3;
    }

    if(0 != getAllConfiguration())
    {
        return -4;
    }

    if(0 != addROSpec())
    {
        return -5;
    }

    if(0 != enableROSpec())
    {
        return -6;
    }

    return 0;
}


/**
 *****************************************************************************
 **
 ** @brief  Get the reader capabilities using GET_READER_CAPABILTIES message
 **
 ** The RequestedData selects all capabilities.
 ** It's mostly here as an example.
 ** If you run this program with -vv (verbose level 2)
 ** you can look at the XML of the capabilities that
 ** come back from the reader.
 **
 ** The message is
 **
 **     <GET_READER_CAPABILITIES MessageID='120'>
 **       <RequestedData>All</RequestedData>
 **     </GET_READER_CAPABILITIES>
 **
 ** @return     ==0             Everything OK
 **             !=0             Something went wrong
 **
 *****************************************************************************/

int
getAllCapabilities (void)
{
    LLRP_tSGET_READER_CAPABILITIES Cmd = {
        .hdr.elementHdr.pType   = &LLRP_tdGET_READER_CAPABILITIES,
        .hdr.MessageID          = 120,

        .eRequestedData         = LLRP_GetReaderCapabilitiesRequestedData_All,
    };
    LLRP_tSMessage *            pRspMsg;
    LLRP_tSGET_READER_CAPABILITIES_RESPONSE *pRsp;

    /*
     * Send the message, expect the response of certain type
     */
    pRspMsg = transact(&Cmd.hdr);
    if(NULL == pRspMsg)
    {
        /* transact already tattled */
        return -1;
    }

    /*
     * Cast to a GET_READER_CAPABILITIES_RESPONSE message.
     */
    pRsp = (LLRP_tSGET_READER_CAPABILITIES_RESPONSE *) pRspMsg;

    /*
     * Check the LLRPStatus parameter.
     */
    if(0 != checkLLRPStatus(pRsp->pLLRPStatus, "getAllCapabilities"))
    {
        /* checkLLRPStatus already tattled */
        freeMessage(pRspMsg);
        return -1;
    }

    /*
     * Done with the response message.
     */
    freeMessage(pRspMsg);

    /*
     * Tattle progress, maybe
     */
    if(g_Verbose)
    {
        printf("INFO: Got capabilities\n");
    }

    /*
     * Victory.
     */
    return 0;
}


/**
 *****************************************************************************
 **
 ** @brief  Get the reader configuration using GET_READER_CONFIG message
 **
 ** The RequestedData selects all configuration parameters.
 ** It's mostly here as an example.
 ** If you run this program with -vv (verbose level 2)
 ** you can look at the XML of the configuration that
 ** comes back from the reader.
 **
 ** The message is
 **
 **     <GET_READER_CONFIG MessageID='121'>
 **       <RequestedData>All</RequestedData>
 **     </GET_READER_CONFIG>
 **
 ** @return     ==0             Everything OK
 **             !=0             Something went wrong
 **
 *****************************************************************************/

int
getAllConfiguration (void)
{
    LLRP_tSGET_READER_CONFIG Cmd = {
        .hdr.elementHdr.pType   = &LLRP_tdGET_READER_CONFIG,
        .hdr.MessageID          = 121,

        .eRequestedData         = LLRP_GetReaderConfigRequestedData_All,
    };
    LLRP_tSMessage *            pRspMsg;
    LLRP_tSGET_READER_CONFIG_RESPONSE *pRsp;

    /*
     * Send the message, expect the response of certain type
     */
    pRspMsg = transact(&Cmd.hdr);
    if(NULL == pRspMsg)
    {
        /* transact already tattled */
        return -1;
    }

    /*
     * Cast to a GET_READER_CONFIG_RESPONSE message.
     */
    pRsp = (LLRP_tSGET_READER_CONFIG_RESPONSE *) pRspMsg;

    /*
     * Check the LLRPStatus parameter.
     */
    if(0 != checkLLRPStatus(pRsp->pLLRPStatus, "getAllConfiguration"))
    {
        /* checkLLRPStatus already tattled */
        freeMessage(pRspMsg);
        return -1;
    }

    /*
     * Done with the response message.
     */
    freeMessage(pRspMsg);

    /*
     * Tattle progress, maybe
     */
    if(g_Verbose)
    {
        printf("INFO: Got configuration\n");
    }

    /*
     * Victory.
     */
    return 0;
}


/**
 *****************************************************************************
 **
 ** @brief  Configure event notifications using SET_READER_CONFIG message
 **
 ** The events we want are:
 **     - Hopping events when the reader changes channel
 **     - ROSpec events, when ROSpecs start and stop
 **     - AISpec events, when AISpecs stop
 **     - Antenna events, when an antenna fails
 **     - Buffer events, so we can see when we are not keeping up
 **     - Reader exceptions, so we can see when something goes wrong
 **
 ** This is a good example of how to do a list of subparameters.
 **
 ** The message is
 **
 **     <SET_READER_CONFIG MessageID='150'>
 **       <ResetToFactoryDefault>0</ResetToFactoryDefault>
 **       <ReaderEventNotificationSpec>
 **         <EventNotificationState>
 **           <EventType>Upon_Hopping_To_Next_Channel</EventType>
 **           <NotificationState>1</NotificationState>
 **         </EventNotificationState>
 **         <EventNotificationState>
 **           <EventType>GPI_Event</EventType>
 **           <NotificationState>0</NotificationState>
 **         </EventNotificationState>
 **         <EventNotificationState>
 **           <EventType>ROSpec_Event</EventType>
 **           <NotificationState>1</NotificationState>
 **         </EventNotificationState>
 **         <EventNotificationState>
 **           <EventType>Report_Buffer_Fill_Warning</EventType>
 **           <NotificationState>1</NotificationState>
 **         </EventNotificationState>
 **         <EventNotificationState>
 **           <EventType>Reader_Exception_Event</EventType>
 **           <NotificationState>1</NotificationState>
 **         </EventNotificationState>
 **         <EventNotificationState>
 **           <EventType>RFSurvey_Event</EventType>
 **           <NotificationState>0</NotificationState>
 **         </EventNotificationState>
 **         <EventNotificationState>
 **           <EventType>AISpec_Event</EventType>
 **           <NotificationState>1</NotificationState>
 **         </EventNotificationState>
 **         <EventNotificationState>
 **           <EventType>AISpec_Event_With_Details</EventType>
 **           <NotificationState>0</NotificationState>
 **         </EventNotificationState>
 **         <EventNotificationState>
 **           <EventType>Antenna_Event</EventType>
 **           <NotificationState>1</NotificationState>
 **         </EventNotificationState>
 **       </ReaderEventNotificationSpec>
 **     </SET_READER_CONFIG>
 **
 ** @return     ==0             Everything OK
 **             !=0             Something went wrong
 **
 *****************************************************************************/

int
configureNotificationStates (void)
{
    LLRP_tSEventNotificationState aEventNotificationState[] = {
      [0] = {
        .hdr.elementHdr.pType   = &LLRP_tdEventNotificationState,
        .hdr.pNextSubParameter  = &aEventNotificationState[1].hdr,

        .eEventType             =
                    LLRP_NotificationEventType_Upon_Hopping_To_Next_Channel,
        .NotificationState      = 1,
      },
      [1] = {
        .hdr.elementHdr.pType   = &LLRP_tdEventNotificationState,
        .hdr.pNextSubParameter  = &aEventNotificationState[2].hdr,

        .eEventType             =
                    LLRP_NotificationEventType_GPI_Event,
        .NotificationState      = 0,
      },
      [2] = {
        .hdr.elementHdr.pType   = &LLRP_tdEventNotificationState,
        .hdr.pNextSubParameter  = &aEventNotificationState[3].hdr,

        .eEventType             =
                    LLRP_NotificationEventType_ROSpec_Event,
        .NotificationState      = 1,
      },
      [3] = {
        .hdr.elementHdr.pType   = &LLRP_tdEventNotificationState,
        .hdr.pNextSubParameter  = &aEventNotificationState[4].hdr,

        .eEventType             =
                    LLRP_NotificationEventType_Report_Buffer_Fill_Warning,
        .NotificationState      = 1,
      },
      [4] = {
        .hdr.elementHdr.pType   = &LLRP_tdEventNotificationState,
        .hdr.pNextSubParameter  = &aEventNotificationState[5].hdr,

        .eEventType             =
                    LLRP_NotificationEventType_Reader_Exception_Event,
        .NotificationState      = 1,
      },
      [5] = {
        .hdr.elementHdr.pType   = &LLRP_tdEventNotificationState,
        .hdr.pNextSubParameter  = &aEventNotificationState[6].hdr,

        .eEventType             =
                    LLRP_NotificationEventType_RFSurvey_Event,
        .NotificationState      = 0,
      },
      [6] = {
        .hdr.elementHdr.pType   = &LLRP_tdEventNotificationState,
        .hdr.pNextSubParameter  = &aEventNotificationState[7].hdr,

        .eEventType             =
                    LLRP_NotificationEventType_AISpec_Event,
        .NotificationState      = 1,
      },
      [7] = {
        .hdr.elementHdr.pType   = &LLRP_tdEventNotificationState,
        .hdr.pNextSubParameter  = &aEventNotificationState[8].hdr,

        .eEventType             =
                    LLRP_NotificationEventType_AISpec_Event_With_Details,
        .NotificationState      = 0,
      },
      [8] = {
        .hdr.elementHdr.pType   = &LLRP_tdEventNotificationState,
        .hdr.pNextSubParameter  = NULL,     /* end of list */

        .eEventType             =
                    LLRP_NotificationEventType_Antenna_Event,
        .NotificationState      = 1,
      },
    };
    LLRP_tSReaderEventNotificationSpec ReaderEventNotificationSpec = {
        .hdr.elementHdr.pType   = &LLRP_tdReaderEventNotificationSpec,

        .listEventNotificationState = aEventNotificationState,
    };
    LLRP_tSSET_READER_CONFIG    Cmd = {
        .hdr.elementHdr.pType   = &LLRP_tdSET_READER_CONFIG,
        .hdr.MessageID          = 150,

        .ResetToFactoryDefault  = 0,
        .pReaderEventNotificationSpec = &ReaderEventNotificationSpec,
    };
    LLRP_tSMessage *            pRspMsg;
    LLRP_tSSET_READER_CONFIG_RESPONSE *pRsp;

    /*
     * Send the message, expect the response of certain type
     */
    pRspMsg = transact(&Cmd.hdr);
    if(NULL == pRspMsg)
    {
        /* transact already tattled */
        return -1;
    }

    /*
     * Cast to a SET_READER_CONFIG_RESPONSE message.
     */
    pRsp = (LLRP_tSSET_READER_CONFIG_RESPONSE *) pRspMsg;

    /*
     * Check the LLRPStatus parameter.
     */
    if(0 != checkLLRPStatus(pRsp->pLLRPStatus, "configureNotificationStates"))
    {
        freeMessage(pRspMsg);
        return -1;
    }

    /*
     * Done with the response message.
     */
    freeMessage(pRspMsg);

    /*
     * Tattle progress, maybe
     */
    if(g_Verbose)
    {
        printf("INFO: Notifications are configured\n");
    }

    /*
     * Victory.
     */
    return 0;
}


/**
 *****************************************************************************
 **
 ** @brief  Add our ROSpec using ADD_ROSPEC message
 **
 ** This ROSpec waits for a START_ROSPEC message,
 ** then takes inventory on all antennas until no new tags have
 ** been seen in 3.5 seconds. The operation never runs longer
 ** than 12.5 seconds.
 **
 ** The tag report is generated after the ROSpec is done.
 **
 ** This example is deliberately streamlined.
 ** Nothing here configures the antennas, RF, or Gen2.
 ** The current defaults are used. Remember we just reset
 ** the reader to factory defaults (above). Normally an
 ** application would be more precise in configuring the
 ** reader and in its ROSpecs.
 **
 ** Experience suggests that typical ROSpecs are about
 ** double this in size.
 **
 ** The message is
 **
 **     <ADD_ROSPEC MessageID='201'>
 **       <ROSpec>
 **         <ROSpecID>123</ROSpecID>
 **         <Priority>0</Priority>
 **         <CurrentState>Disabled</CurrentState>
 **         <ROBoundarySpec>
 **           <ROSpecStartTrigger>
 **             <ROSpecStartTriggerType>Null</ROSpecStartTriggerType>
 **           </ROSpecStartTrigger>
 **           <ROSpecStopTrigger>
 **             <ROSpecStopTriggerType>Null</ROSpecStopTriggerType>
 **             <DurationTriggerValue>0</DurationTriggerValue>
 **           </ROSpecStopTrigger>
 **         </ROBoundarySpec>
 **         <AISpec>
 **           <AntennaIDs>0</AntennaIDs>
 **           <AISpecStopTrigger>
 **             <AISpecStopTriggerType>Duration</AISpecStopTriggerType>
 **             <DurationTrigger>30000</DurationTrigger>
 **           </AISpecStopTrigger>
 **           <InventoryParameterSpec>
 **             <InventoryParameterSpecID>1234</InventoryParameterSpecID>
 **             <ProtocolID>EPCGlobalClass1Gen2</ProtocolID>
 **             <AntennaConfiguration>
 **               <AntennaID>0</AntennaID>
 **               <C1G2InventoryCommand>
 **                 <TagInventoryStateAware>false</TagInventoryStateAware>
 ** BEGIN C1G2RFControl only if mode specified on command line
 **                  <C1G2RFControl>
 **                   <ModeIndex>$MODE</ModeIndex>
 **                   <Tari>0</Tari>
 **                 </C1G2RFControl>
 ** END   C1G2RFControl only if mode specified on command line
 **                 <C1G2SingulationControl>
 **                   <Session>2</Session>
 **                   <TagPopulation>32</TagPopulation>
 **                   <TagTransitTime>0</TagTransitTime>
 **                 </C1G2SingulationControl>
 **               </C1G2InventoryCommand>
 **             </AntennaConfiguration>
 **           </InventoryParameterSpec>
 **         </AISpec>
 **         <ROReportSpec>
 **           <ROReportTrigger>Upon_N_Tags_Or_End_Of_ROSpec</ROReportTrigger>
 **           <N>0</N>
 **           <TagReportContentSelector>
 **             <EnableROSpecID>0</EnableROSpecID>
 **             <EnableSpecIndex>0</EnableSpecIndex>
 **             <EnableInventoryParameterSpecID>0</EnableInventoryParameterSpecID>
 **             <EnableAntennaID>1</EnableAntennaID>
 **             <EnableChannelIndex>0</EnableChannelIndex>
 **             <EnablePeakRSSI>1</EnablePeakRSSI>
 **             <EnableFirstSeenTimestamp>1</EnableFirstSeenTimestamp>
 **             <EnableLastSeenTimestamp>1</EnableLastSeenTimestamp>
 **             <EnableTagSeenCount>1</EnableTagSeenCount>
 **             <EnableAccessSpecID>0</EnableAccessSpecID>
 **           </TagReportContentSelector>
 **         </ROReportSpec>
 **       </ROSpec>
 **     </ADD_ROSPEC>
 **
 ** @return     ==0             Everything OK
 **             !=0             Something went wrong
 **
 *****************************************************************************/

int
addROSpec (void)
{
    LLRP_tSROSpecStartTrigger   ROSpecStartTrigger = {
        .hdr.elementHdr.pType   = &LLRP_tdROSpecStartTrigger,

        .eROSpecStartTriggerType = LLRP_ROSpecStartTriggerType_Null,
    };
    LLRP_tSROSpecStopTrigger    ROSpecStopTrigger = {
        .hdr.elementHdr.pType   = &LLRP_tdROSpecStopTrigger,

        .eROSpecStopTriggerType = LLRP_ROSpecStopTriggerType_Null,
        .DurationTriggerValue   = 0     /* n/a */
    };
    LLRP_tSROBoundarySpec       ROBoundarySpec = {
        .hdr.elementHdr.pType   = &LLRP_tdROBoundarySpec,

        .pROSpecStartTrigger    = &ROSpecStartTrigger,
        .pROSpecStopTrigger     = &ROSpecStopTrigger,
    };
    llrp_u16_t                  AntennaIDs[1] = { 0 };  /* All */
#if 1
    LLRP_tSAISpecStopTrigger    AISpecStopTrigger = {
        .hdr.elementHdr.pType   = &LLRP_tdAISpecStopTrigger,

        .eAISpecStopTriggerType = LLRP_AISpecStopTriggerType_Duration,
        .DurationTrigger        = 30000,
    };
#else
    LLRP_tSTagObservationTrigger TagObservationTrigger = {
        .hdr.elementHdr.pType   = &LLRP_tdTagObservationTrigger,

        .eTriggerType           = 
                LLRP_TagObservationTriggerType_Upon_Seeing_No_More_New_Tags_For_Tms_Or_Timeout,
        .NumberOfTags           = 0,        /* unlimited */
        .NumberOfAttempts       = 0,        /* n/a */
        .T                      = 3500,     /* 3.5 sec */
        .Timeout                = 12500,    /* 12.5 sec */
    };
    LLRP_tSAISpecStopTrigger    AISpecStopTrigger = {
        .hdr.elementHdr.pType   = &LLRP_tdAISpecStopTrigger,

        .eAISpecStopTriggerType = LLRP_AISpecStopTriggerType_Tag_Observation,
        .DurationTrigger        = 0,
        .pTagObservationTrigger = &TagObservationTrigger,
    };
#endif
    LLRP_tSC1G2RFControl        C1G2RFControl = {
        .hdr.elementHdr.pType   = &LLRP_tdC1G2RFControl,

        .ModeIndex              = g_ModeIndex,
    };
    LLRP_tSC1G2SingulationControl C1G2SingulationControl = {
        .hdr.elementHdr.pType   = &LLRP_tdC1G2SingulationControl,

        .Session                = 2,
        .TagPopulation          = 32,
        .TagTransitTime         = 0,
    };
    LLRP_tSC1G2InventoryCommand C1G2InventoryCommand = {
        .hdr.elementHdr.pType   = &LLRP_tdC1G2InventoryCommand,

        /*
         * .pC1G2RFControl might be set below if
         * a g_ModeIndex was given on the command line.
         */
        .pC1G2SingulationControl = &C1G2SingulationControl,
    };
    LLRP_tSAntennaConfiguration AntennaConfiguration = {
        .hdr.elementHdr.pType   = &LLRP_tdAntennaConfiguration,

        .listAirProtocolInventoryCommandSettings =
                                  &C1G2InventoryCommand.hdr,
    };
    LLRP_tSInventoryParameterSpec InventoryParameterSpec = {
        .hdr.elementHdr.pType   = &LLRP_tdInventoryParameterSpec,

        .InventoryParameterSpecID = 1234,
        .eProtocolID            = LLRP_AirProtocols_EPCGlobalClass1Gen2,

        .listAntennaConfiguration = &AntennaConfiguration,
    };
    LLRP_tSAISpec               AISpec = {
        .hdr.elementHdr.pType   = &LLRP_tdAISpec,

        .AntennaIDs = {
            .nValue                 = 1,
            .pValue                 = AntennaIDs
        },
        .pAISpecStopTrigger     = &AISpecStopTrigger,
        .listInventoryParameterSpec = &InventoryParameterSpec,
    };
    LLRP_tSTagReportContentSelector TagReportContentSelector = {
        .hdr.elementHdr.pType   = &LLRP_tdTagReportContentSelector,

        .EnableROSpecID         = 0,
        .EnableSpecIndex        = 0,
        .EnableInventoryParameterSpecID = 0,
        .EnableAntennaID        = 1,
        .EnableChannelIndex     = 0,
        .EnablePeakRSSI         = 1,
        .EnableFirstSeenTimestamp = 1,
        .EnableLastSeenTimestamp = 1,
        .EnableTagSeenCount     = 1,
        .EnableAccessSpecID     = 0,
    };
    LLRP_tSROReportSpec         ROReportSpec = {
        .hdr.elementHdr.pType   = &LLRP_tdROReportSpec,

        .eROReportTrigger       =
                      LLRP_ROReportTriggerType_Upon_N_Tags_Or_End_Of_ROSpec,
        .N                      = 1,
        .pTagReportContentSelector = &TagReportContentSelector,
    };
    LLRP_tSROSpec               ROSpec = {
        .hdr.elementHdr.pType   = &LLRP_tdROSpec,

        .ROSpecID               = 123,
        .Priority               = 0,
        .eCurrentState          = LLRP_ROSpecState_Disabled,
        .pROBoundarySpec        = &ROBoundarySpec,
        .listSpecParameter      = &AISpec.hdr,
        .pROReportSpec          = &ROReportSpec,
    };
    LLRP_tSADD_ROSPEC           Cmd = {
        .hdr.elementHdr.pType   = &LLRP_tdADD_ROSPEC,

        .hdr.MessageID          = 201,
        .pROSpec                = &ROSpec,
    };
    LLRP_tSMessage *            pRspMsg;
    LLRP_tSADD_ROSPEC_RESPONSE *pRsp;

    /*
     * If the mode was specified link in the AntennaConfiguration
     * that was prepared, above. The absence of the AntennaConfiguration
     * tells the reader to use the default mode.
     */
    if(0 <= g_ModeIndex)
    {
        C1G2InventoryCommand.pC1G2RFControl = &C1G2RFControl;
    }

    /*
     * Send the message, expect the response of certain type
     */
    pRspMsg = transact(&Cmd.hdr);
    if(NULL == pRspMsg)
    {
        /* transact already tattled */
        return -1;
    }

    /*
     * Cast to a ADD_ROSPEC_RESPONSE message.
     */
    pRsp = (LLRP_tSADD_ROSPEC_RESPONSE *) pRspMsg;

    /*
     * Check the LLRPStatus parameter.
     */
    if(0 != checkLLRPStatus(pRsp->pLLRPStatus, "addROSpec"))
    {
        /* checkLLRPStatus already tattled */
        freeMessage(pRspMsg);
        return -1;
    }

    /*
     * Done with the response message.
     */
    freeMessage(pRspMsg);

    /*
     * Tattle progress, maybe
     */
    if(g_Verbose)
    {
        printf("INFO: ROSpec added\n");
    }

    /*
     * Victory.
     */
    return 0;
}


/**
 *****************************************************************************
 **
 ** @brief  Enable our ROSpec using ENABLE_ROSPEC message
 **
 ** Enable the ROSpec that was added above.
 **
 ** The message we send is:
 **     <ENABLE_ROSPEC MessageID='202'>
 **       <ROSpecID>123</ROSpecID>
 **     </ENABLE_ROSPEC>
 **
 ** @return     ==0             Everything OK
 **             !=0             Something went wrong
 **
 *****************************************************************************/

int
enableROSpec (void)
{
    LLRP_tSENABLE_ROSPEC        Cmd = {
        .hdr.elementHdr.pType   = &LLRP_tdENABLE_ROSPEC,
        .hdr.MessageID          = 202,

        .ROSpecID               = 123,
    };
    LLRP_tSMessage *            pRspMsg;
    LLRP_tSENABLE_ROSPEC_RESPONSE *pRsp;

    /*
     * Send the message, expect the response of certain type
     */
    pRspMsg = transact(&Cmd.hdr);
    if(NULL == pRspMsg)
    {
        /* transact already tattled */
        return -1;
    }

    /*
     * Cast to a ENABLE_ROSPEC_RESPONSE message.
     */
    pRsp = (LLRP_tSENABLE_ROSPEC_RESPONSE *) pRspMsg;

    /*
     * Check the LLRPStatus parameter.
     */
    if(0 != checkLLRPStatus(pRsp->pLLRPStatus, "enableROSpec"))
    {
        /* checkLLRPStatus already tattled */
        freeMessage(pRspMsg);
        return -1;
    }

    /*
     * Done with the response message.
     */
    freeMessage(pRspMsg);

    /*
     * Tattle progress, maybe
     */
    if(g_Verbose)
    {
        printf("INFO: ROSpec enabled\n");
    }

    /*
     * Victory.
     */
    return 0;
}


/**
 *****************************************************************************
 **
 ** @brief  Start the ROSpec using START_ROSPEC message then
 **         monitor the operation until it is done
 **
 ** The steps:
 **     - Send the START_ROSPEC message, but not using transact()
 **     - Receive all messages coming back from the reader
 **     - We can be absolutely sure of the order the messages will arrive
 **         - Expect a START_ROSPEC_RESPONSE message
 **         - Expect a READER_EVENT_NOTIFICATION with
 **           a ROSpecEvent parameter indicating that the
 **           ROSpec started
 **         - Expect many READER_EVENT_NOTIFICATION messages each
 **           with HoppingEvent parameters as the reader
 **           tries different channels. These are not specifically
 **           handled here. If you run this program with -vv
 **           (verbose level 2) you can look at the XML of the
 **           configuration that comes back from the reader.
 **         - Expect at least one RO_ACCESS_REPORT message
 **           and print the tag report
 **         - Expect a READER_EVENT_NOTIFICATION with
 **           a ROSpecEvent parameter indicating that the
 **           ROSpec stopped. This is when the loop exits.
 **
 **
 ** This could use transact() to send the START_ROSPEC.
 ** The LLRP_tSConnection mechanism preserves notifications
 ** while looking for response messages, like
 ** START_ROSPEC_RESPONSE. Afterward the notifications
 ** can be received.
 **
 ** This example shows how to just send a message and
 ** then combine fielding the resposne with fielding
 ** notifications.
 **
 ** @return     ==0             Everything OK
 **             !=0             Something went wrong
 **
 *****************************************************************************/

int
startAndMonitorOperation (void)
{
    LLRP_tSSTART_ROSPEC         Cmd = {
        .hdr.elementHdr.pType   = &LLRP_tdSTART_ROSPEC,
        .hdr.MessageID          = 203,

        .ROSpecID               = 123,
    };
    int                         bDone = 0;
    int                         RetVal = 0;

    /*
     * Tattle as we start.
     */
    printf("INFO: Inventory operation starting\n");

    /*
     * Just send the START_ROSPEC message, do not wait for the resposne.
     */
    if(0 != sendMessage(&Cmd.hdr))
    {
        /* sendMessage already tattled */
        return -1;
    }

    /*
     * Keep receiving messages until done or until
     * something bad happens.
     */
    while(!bDone)
    {
        LLRP_tSMessage *        pMessage;
        const LLRP_tSTypeDescriptor *pType;

        /*
         * Wait up to 20 seconds for a message. They should
         * be much more frequent because there will be several
         * hopping events per second.
         */
        pMessage = recvMessage(20000);
        if(NULL == pMessage)
        {
            /*
             * Did not receive a message within a reasonable
             * amount of time. recvMessage() already tattled
             */
            RetVal = -2;
            bDone = 1;
            continue;
        }

        /*
         * What happens depends on what kind of message
         * received. Use the type label (pType) to
         * discriminate message types.
         */
        pType = pMessage->elementHdr.pType;

        /*
         * Is it the response from the START_ROSPEC?
         * If so, just make sure the status is good.
         * If the status is not good there is no
         * ROSpec running and things are done.
         */
        if(&LLRP_tdSTART_ROSPEC_RESPONSE == pType)
        {
            LLRP_tSSTART_ROSPEC_RESPONSE *pRsp;

            /*
             * Cast to a START_ROSPEC_RESPONSE message.
             */
            pRsp = (LLRP_tSSTART_ROSPEC_RESPONSE *) pMessage;

            /*
             * Check the LLRPStatus parameter.
             */
            if(0 != checkLLRPStatus(pRsp->pLLRPStatus, "startROSpec"))
            {
                /* checkLLRPStatus already tattled */
                RetVal = -3;
                bDone = 1;
            }
            else
            {
                /* Good start. Tattle, maybe. */
                if(g_Verbose)
                {
                    printf("INFO: Start command acknowledged\n");
                }
            }
        }

        /*
         * Is it a reader event? If so, which ones?
         */
        else if(&LLRP_tdREADER_EVENT_NOTIFICATION == pType)
        {
            LLRP_tSREADER_EVENT_NOTIFICATION *pNtf;
            LLRP_tSReaderEventNotificationData *pNtfData;

            pNtf = (LLRP_tSREADER_EVENT_NOTIFICATION *) pMessage;
            pNtfData = pNtf->pReaderEventNotificationData;

            /*
             * Is there an ROSpecEvent? These tell us when the ROSpec
             * starts and stops. When it stops things are done.
             */
            if(NULL != pNtfData->pROSpecEvent)
            {
                LLRP_tSROSpecEvent *    pROEvent;

                pROEvent = pNtfData->pROSpecEvent;
                switch(pROEvent->eEventType)
                {
                default:
                    printf("ERROR: Unknown RO event type\n");
                    RetVal = -4;
                    bDone = 1;
                    break;

                case LLRP_ROSpecEventType_Start_Of_ROSpec:
                    if(g_Verbose)
                    {
                        printf("INFO: RO running\n");
                    }
                    break;

                case LLRP_ROSpecEventType_End_Of_ROSpec:
                    if(g_Verbose)
                    {
                        printf("INFO: RO stopped\n");
                    }
                    RetVal = 0;
                    bDone = 1;
                    break;
                }
            }

            /*
             * Other reader events may be present.
             * Notifications enabled include AISpec events,
             * antenna events, and hopping events.
             * Run this programs with -vv to see the
             * XML for the other events.
             *
             * For this example we just ignore them.
             */
        }

        /*
         * Is it a tag report? If so, print it out.
         */
        else if(&LLRP_tdRO_ACCESS_REPORT == pType)
        {
            LLRP_tSRO_ACCESS_REPORT *pNtf;

            pNtf = (LLRP_tSRO_ACCESS_REPORT *) pMessage;

            printTagReportData(pNtf);
        }

        /*
         * Hmmm. Something unexpected. Just tattle and keep going.
         */
        else
        {
            printf("WARNING: Ignored unexpected message during monitor: %s\n",
                pType->pName);
        }

        /*
         * Done with the received message
         */
        freeMessage(pMessage);
    }

    /*
     * Tattle about how things turned out.
     */
    if(RetVal != 0)
    {
        printf("ERROR: Inventory operation terminated (%d)\n", RetVal);
    }
    else if(g_Verbose)
    {
        printf("INFO: Inventory operation finished\n");
    }

    return RetVal;
}


/**
 *****************************************************************************
 **
 ** @brief  Helper routine to print a tag report
 **
 ** The report is printed in list order, which is arbitrary.
 **
 ** TODO: It would be cool to sort the list by EPC and antenna,
 **       then print it.
 **
 ** @return     void
 **
 *****************************************************************************/

void
printTagReportData (
  LLRP_tSRO_ACCESS_REPORT *     pRO_ACCESS_REPORT)
{
    LLRP_tSTagReportData *      pTagReportData;
    unsigned int                nEntry = 0;

    /*
     * Count the number of tag reports.
     */
    g_nTagReport++;

    /*
     * If individual tag reports are not wanted, just return.
     */
    if(g_Quiet)
    {
        return;
    }

    /*
     * Loop through and count the number of entries
     */
    for(
        pTagReportData = pRO_ACCESS_REPORT->listTagReportData;
        NULL != pTagReportData;
        pTagReportData = (LLRP_tSTagReportData *)
                                    pTagReportData->hdr.pNextSubParameter)
    {
        nEntry++;
    }

    printf("INFO: %u tag report entries\n", nEntry);

    /*
     * Loop through again and print each entry.
     */
    for(
        pTagReportData = pRO_ACCESS_REPORT->listTagReportData;
        NULL != pTagReportData;
        pTagReportData = (LLRP_tSTagReportData *)
                                    pTagReportData->hdr.pNextSubParameter)
    {
        printOneTagReportData (pTagReportData);
    }
}


/**
 *****************************************************************************
 **
 ** @brief  Helper routine to print one tag report entry on one line
 **
 ** The format is, approximately:
 **
 **     1234-5678-9ABC-DEF0-1234-5678  ant1  -55dBm   27seen
 **
 ** @return     void
 **
 *****************************************************************************/

void
printOneTagReportData (
  LLRP_tSTagReportData *        pTagReportData)
{
    const LLRP_tSTypeDescriptor *pType;
    char                    aBuf[64];

    /*
     * Print the EPC. It could be an 96-bit EPC_96 parameter
     * or an variable length EPCData parameter.
     */
    if(NULL != pTagReportData->pEPCParameter)
    {
        char *              p = aBuf;
        llrp_u8_t *         pValue = NULL;
        unsigned int        n, i;

        pType = pTagReportData->pEPCParameter->elementHdr.pType;
        if(&LLRP_tdEPC_96 == pType)
        {
            LLRP_tSEPC_96 * pE96;

            pE96 = (LLRP_tSEPC_96 *) pTagReportData->pEPCParameter;
            pValue = pE96->EPC.aValue;
            n = 12u;
        }
        else if(&LLRP_tdEPCData == pType)
        {
            LLRP_tSEPCData *pEPCData;

            pEPCData = (LLRP_tSEPCData *) pTagReportData->pEPCParameter;
            pValue = pEPCData->EPC.pValue;
            n = (pEPCData->EPC.nBit + 7u) / 8u;
        }

        if(NULL != pValue)
        {
            for(i = 0; i < n; i++)
            {
                if(0 < i && i%2 == 0)
                {
                    *p++ = '-';
                }
                sprintf(p, "%02X", pValue[i]);
                while(*p) p++;
            }
        }
        else
        {
            strcpy(aBuf, "---unknown-epc-data-type---");
        }
    }
    else
    {
        strcpy(aBuf, "---missing-epc-data---");
    }
    printf("%-32s", aBuf);

    /*
     * Print the antenna ID number
     */
    if(NULL != pTagReportData->pAntennaID)
    {
        sprintf(aBuf, "ant%d", pTagReportData->pAntennaID->AntennaID);
    }
    else
    {
        sprintf(aBuf, "ant?");
    }
    printf(" %-5s", aBuf);

    /*
     * Print the peak RSSI
     */
    if(NULL != pTagReportData->pPeakRSSI)
    {
        sprintf(aBuf, "%3ddBm", pTagReportData->pPeakRSSI->PeakRSSI);
    }
    else
    {
        sprintf(aBuf, "  ?dBm");
    }
    printf(" %-7s", aBuf);

    /*
     * Print the number of times the tag was seen
     */
    if(NULL != pTagReportData->pTagSeenCount)
    {
        sprintf(aBuf, "%4dseen", pTagReportData->pTagSeenCount->TagCount);
    }
    else
    {
        sprintf(aBuf, "   ?seen");
    }
    printf(" %-8s", aBuf);

    /*
     * End of line
     */
    printf("\n");
}


/**
 *****************************************************************************
 **
 ** @brief  Helper routine to check an LLRPStatus parameter
 **         and tattle on errors
 **
 ** Helper routine to interpret the LLRPStatus subparameter
 ** that is in all responses. It tattles on an error, if one,
 ** and tries to safely provide details.
 **
 ** This simplifies the code, above, for common check/tattle
 ** sequences.
 **
 ** @return     ==0             Everything OK
 **             !=0             Something went wrong, already tattled
 **
 *****************************************************************************/

int
checkLLRPStatus (
  LLRP_tSLLRPStatus *           pLLRPStatus,
  char *                        pWhatStr)
{
    /*
     * The LLRPStatus parameter is mandatory in all responses.
     * If it is missing there should have been a decode error.
     * This just makes sure (remember, this program is a
     * diagnostic and suppose to catch LTKC mistakes).
     */
    if(NULL == pLLRPStatus)
    {
        printf("ERROR: %s missing LLRP status\n", pWhatStr);
        return -1;
    }

    /*
     * Make sure the status is M_Success.
     * If it isn't, print the error string if one.
     * This does not try to pretty-print the status
     * code. To get that, run this program with -vv
     * and examine the XML output.
     */
    if(LLRP_StatusCode_M_Success != pLLRPStatus->eStatusCode)
    {
        if(0 == pLLRPStatus->ErrorDescription.nValue)
        {
            printf("ERROR: %s failed, no error description given\n",
                pWhatStr);
        }
        else
        {
            printf("ERROR: %s failed, %.*s\n",
                pWhatStr,
                pLLRPStatus->ErrorDescription.nValue,
                pLLRPStatus->ErrorDescription.pValue);
        }
        return -2;
    }

    /*
     * Victory. Everything is fine.
     */
    return 0;
}


/**
 *****************************************************************************
 **
 ** @brief  Wrapper routine to do an LLRP transaction
 **
 ** Wrapper to transact a request/resposne.
 **     - Print the outbound message in XML if verbose level is at least 2
 **     - Send it using the LLRP_Conn_transact()
 **     - LLRP_Conn_transact() receives the response or recognizes an error
 **     - Tattle on errors, if any
 **     - Print the received message in XML if verbose level is at least 2
 **     - If the response is ERROR_MESSAGE, the request was sufficiently
 **       misunderstood that the reader could not send a proper reply.
 **       Deem this an error, free the message.
 **
 ** The message returned resides in allocated memory. It is the
 ** caller's obligtation to free it.
 **
 ** @return     ==NULL          Something went wrong, already tattled
 **             !=NULL          Pointer to a message
 **
 *****************************************************************************/

LLRP_tSMessage *
transact (
  LLRP_tSMessage *              pSendMsg)
{
    LLRP_tSConnection *         pConn = g_pConnectionToReader;
    LLRP_tSMessage *            pRspMsg;

    /*
     * Print the XML text for the outbound message if
     * verbosity is 2 or higher.
     */
    if(1 < g_Verbose)
    {
        /* If -qq command option, do XML encode but don't actually print */
        if(2 > g_Quiet)
        {
            printf("\n===================================\n");
            printf("INFO: Transact sending\n");
        }
        printXMLMessage(pSendMsg);
    }

    /*
     * Send the message, expect the response of certain type.
     * If LLRP_Conn_transact() returns NULL then there was
     * an error. In that case we try to print the error details.
     */
    pRspMsg = LLRP_Conn_transact(pConn, pSendMsg, 5000);
    if(NULL == pRspMsg)
    {
        const LLRP_tSErrorDetails *pError = LLRP_Conn_getTransactError(pConn);

        printf("ERROR: %s transact failed, %s\n",
            pSendMsg->elementHdr.pType->pName,
            pError->pWhatStr ? pError->pWhatStr : "no reason given");

        if(NULL != pError->pRefType)
        {
            printf("ERROR: ... reference type %s\n",
                pError->pRefType->pName);
        }

        if(NULL != pError->pRefField)
        {
            printf("ERROR: ... reference field %s\n",
                pError->pRefField->pName);
        }

        return NULL;
    }

    /*
     * Print the XML text for the inbound message if
     * verbosity is 2 or higher.
     */
    if(1 < g_Verbose)
    {
        /* If -qq command option, do XML encode but don't actually print */
        if(2 > g_Quiet)
        {
            printf("\n- - - - - - - - - - - - - - - - - -\n");
            printf("INFO: Transact received response\n");
        }
        printXMLMessage(pRspMsg);
    }

    /*
     * If it is an ERROR_MESSAGE (response from reader
     * when it can't understand the request), tattle
     * and declare defeat.
     */
    if(&LLRP_tdERROR_MESSAGE == pRspMsg->elementHdr.pType)
    {
        printf("ERROR: Received ERROR_MESSAGE instead of %s\n",
            pSendMsg->elementHdr.pType->pResponseType->pName);
        freeMessage(pRspMsg);
        pRspMsg = NULL;
    }

    return pRspMsg;
}


/**
 *****************************************************************************
 **
 ** @brief  Wrapper routine to receive a message
 **
 ** This can receive notifications as well as responses.
 **     - Recv a message using the LLRP_Conn_recvMessage()
 **     - Tattle on errors, if any
 **     - Print the message in XML if verbose level is at least 2
 **
 ** The message returned resides in allocated memory. It is the
 ** caller's obligtation to free it.
 **
 ** @param[in]  nMaxMS          -1 => block indefinitely
 **                              0 => just peek at input queue and
 **                                   socket queue, return immediately
 **                                   no matter what
 **                             >0 => ms to await complete frame
 **
 ** @return     ==NULL          Something went wrong, already tattled
 **             !=NULL          Pointer to a message
 **
 *****************************************************************************/

LLRP_tSMessage *
recvMessage (
  int                           nMaxMS)
{
    LLRP_tSConnection *         pConn = g_pConnectionToReader;
    LLRP_tSMessage *            pMessage;

    /*
     * Receive the message subject to a time limit
     */
    pMessage = LLRP_Conn_recvMessage(pConn, nMaxMS);

    /*
     * If LLRP_Conn_recvMessage() returns NULL then there was
     * an error. In that case we try to print the error details.
     */
    if(NULL == pMessage)
    {
        const LLRP_tSErrorDetails *pError = LLRP_Conn_getRecvError(pConn);

        printf("ERROR: recvMessage failed, %s\n",
            pError->pWhatStr ? pError->pWhatStr : "no reason given");

        if(NULL != pError->pRefType)
        {
            printf("ERROR: ... reference type %s\n",
                pError->pRefType->pName);
        }

        if(NULL != pError->pRefField)
        {
            printf("ERROR: ... reference field %s\n",
                pError->pRefField->pName);
        }

        return NULL;
    }

    /*
     * Print the XML text for the inbound message if
     * verbosity is 2 or higher.
     */
    if(1 < g_Verbose)
    {
        /* If -qq command option, do XML encode but don't actually print */
        if(2 > g_Quiet)
        {
            printf("\n===================================\n");
            printf("INFO: Message received\n");
        }
        printXMLMessage(pMessage);
    }

    return pMessage;
}


/**
 *****************************************************************************
 **
 ** @brief  Wrapper routine to send a message
 **
 ** Wrapper to send a message.
 **     - Print the message in XML if verbose level is at least 2
 **     - Send it using the LLRP_Conn_sendMessage()
 **     - Tattle on errors, if any
 **
 ** @param[in]  pSendMsg        Pointer to message to send
 **
 ** @return     ==0             Everything OK
 **             !=0             Something went wrong, already tattled
 **
 *****************************************************************************/

int
sendMessage (
  LLRP_tSMessage *              pSendMsg)
{
    LLRP_tSConnection *         pConn = g_pConnectionToReader;

    /*
     * Print the XML text for the outbound message if
     * verbosity is 2 or higher.
     */
    if(1 < g_Verbose)
    {
        /* If -qq command option, do XML encode but don't actually print */
        if(2 > g_Quiet)
        {
            printf("\n===================================\n");
            printf("INFO: Sending\n");
        }
        printXMLMessage(pSendMsg);
    }

    /*
     * If LLRP_Conn_sendMessage() returns other than LLRP_RC_OK
     * then there was an error. In that case we try to print
     * the error details.
     */
    if(LLRP_RC_OK != LLRP_Conn_sendMessage(pConn, pSendMsg))
    {
        const LLRP_tSErrorDetails *pError = LLRP_Conn_getSendError(pConn);

        printf("ERROR: %s sendMessage failed, %s\n",
            pSendMsg->elementHdr.pType->pName,
            pError->pWhatStr ? pError->pWhatStr : "no reason given");

        if(NULL != pError->pRefType)
        {
            printf("ERROR: ... reference type %s\n",
                pError->pRefType->pName);
        }

        if(NULL != pError->pRefField)
        {
            printf("ERROR: ... reference field %s\n",
                pError->pRefField->pName);
        }

        return -1;
    }

    /*
     * Victory
     */
    return 0;
}


/**
 *****************************************************************************
 **
 ** @brief  Wrapper to free a message.
 **
 ** All it does is cast pMessage and let
 ** LLRP_Element_destruct() do the work.
 **
 ** @param[in]  pMessage        Pointer to message to destruct
 **
 ** @return     void
 **
 *****************************************************************************/

void
freeMessage (
  LLRP_tSMessage *              pMessage)
{
    LLRP_Element_destruct(&pMessage->elementHdr);
}


/**
 *****************************************************************************
 **
 ** @brief  Helper to print a message as XML text
 **
 ** Print a LLRP message as XML text
 **
 ** @param[in]  pMessage        Pointer to message to print
 **
 ** @return     void
 **
 *****************************************************************************/

void
printXMLMessage (
  LLRP_tSMessage *              pMessage)
{
    char                        aBuf[100*1024];

    /*
     * Convert the message to an XML string.
     * This fills the buffer with either the XML string
     * or an error message. The return value could
     * be checked.
     */

    LLRP_toXMLString(&pMessage->elementHdr, aBuf, sizeof aBuf);

    /*
     * Print the XML Text to the standard output.
     * For characterization, command line option -qq
     * prevents the XML text actually being printed and
     * so the CPU utilization of XMLTextEncoder can be measured
     * without the noise.
     */
    if(2 > g_Quiet)
    {
        printf("%s", aBuf);
    }
}
