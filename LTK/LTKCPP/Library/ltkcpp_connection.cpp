
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
 ** @file  ltkcpp_connection.cpp
 **
 ** @brief Functions to communicate using LLRP messages over a socket
 **
 ** This provides a simple interface to send and receive LLRP messages
 ** using a network socket.
 **
 ** This is simplified. It is possible, even likely, that some
 ** applications could require much more sophistication. To that
 ** end this source file serves as an example to illustrate how
 ** framing, encoding, decoding, and response matching are done.
 **
 *****************************************************************************/


#include <assert.h>


#if defined(linux) || defined(__APPLE__)
#include <poll.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <fcntl.h>
#include <time.h>
#endif
#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <time.h>
#endif

#include "ltkcpp_platform.h"
#include "ltkcpp_base.h"
#include "ltkcpp_frame.h"
#include "ltkcpp_connection.h"


#define LLRP1_TCP_PORT   (5084u)


namespace LLRP
{

/*
 * On Linux a socket is a simple type (int). On Windows it
 * is a pointer to a specific type defined in a WinSock
 * header (.h) file. Rather than make that header file a
 * prerequisite to every source file that includes this header
 * file (ltkcpp_connection.h), the following CPlatformSocket
 * class opaquely wraps the platform-specific socket.
 *
 * The CConnection class references it by pointer only.
 * The content of the CPlatformSocket is only known
 * within the implementation of CConnection.
 */
#if defined(linux) || defined(__APPLE__)
class CPlatformSocket
{
  public:
    int                         m_sock;

    CPlatformSocket(int sock);
};

CPlatformSocket::CPlatformSocket (int sock)
{
    m_sock = sock;
}

#endif
#ifdef WIN32
class CPlatformSocket
{
  public:
    SOCKET                      m_sock;

    CPlatformSocket(SOCKET sock);
};

CPlatformSocket::CPlatformSocket (SOCKET sock)
{
    m_sock = sock;
}

#endif


/**
 *****************************************************************************
 **
 ** @brief  Construct a new LLRP connection instance
 **
 ** @param[in]  pTypeRegistry   The LLRP registry of known message/parameter
 **                             types. Includes standard and custom.
 **                             Used during decode.
 ** @param[in]  nBufferSize     Size of each the receive and send buffers.
 **                             Use size larger than the largest frame
 **                             you expect. 0 selects a default value.
 **
 ** @return     !=NULL          Pointer to connection instance
 **             ==NULL          Error, always an allocation failure
 **                             most likely nBufferSize is weird
 **
 *****************************************************************************/

CConnection::CConnection (
  const CTypeRegistry *         pTypeRegistry,
  unsigned int                  nBufferSize)
{
    /*
     * Apply default buffer size
     */
    if(0 == nBufferSize)
    {
        nBufferSize = 128u*1024u;
    }

    /*
     * Make sure the buffer size is sane. This is arbitrary.
     * The smallest message is 10 bytes, but it ain't anything
     * useful. 1024u covers a surprising number of messages.
     */
    if(1024u > nBufferSize || 1u*1024u*1024u < nBufferSize)
    {
        throw "Insane buffer size";
    }

    /*
     * Capture variables. m_pPlatformSocket=NULL indicates there
     * is no connection yet.
     */
    m_pPlatformSocket = NULL;
    m_pTypeRegistry = pTypeRegistry;
    m_nBufferSize = nBufferSize;

    memset(&m_Recv, 0, sizeof m_Recv);
    memset(&m_Send, 0, sizeof m_Send);

    /*
     * Allocate and check each the recv and send buffers.
     */
    m_Recv.pBuffer = new llrp_byte_t[nBufferSize];
    m_Send.pBuffer = new llrp_byte_t[nBufferSize];

    /*
     * Zero-fill buffers just so debugger printing is tidy
     */
    memset(m_Recv.pBuffer, 0, nBufferSize);
    memset(m_Send.pBuffer, 0, nBufferSize);

#ifdef WIN32
    /*
     * On Windows have to enable the WinSock library
     */
    {
        WSADATA SocketLibraryInitData;
        WSAStartup(0xFFFF, &SocketLibraryInitData);
    }
#endif

    /*
     * Victory
     */
}


/**
 *****************************************************************************
 **
 ** @brief  Destruct a LLRP connection instance
 **
 *****************************************************************************/

CConnection::~CConnection (void)
{
    /*
     * Close the connection, if one
     */
    closeConnectionToReader();

    /*
     * Destruct any messages on the input queue
     */
    for (
        std::list<CMessage *>::iterator msg = m_listInputQueue.begin();
        msg != m_listInputQueue.end();
        msg++)
    {
        delete *msg;
    }

    /*
     * free each the receive and send bufers
     */
    delete[] m_Recv.pBuffer;
    delete[] m_Send.pBuffer;

#ifdef WIN32
    /*
     * On Windows have to disable (dereference) the WinSock library
     */
    {
        WSACleanup();
    }
#endif

}


/**
 *****************************************************************************
 **
 ** @brief  Open the connection to the reader
 **
 ** The steps:
 **     - Look up the host name
 **     - Create a socket
 **     - Connect to the host address
 **     - Condition the socket (set options)
 **
 ** @param[in]  pReaderHostName String containing the reader host name
 **
 ** @return     ==0             Connected OK, ready for business
 **             !=0             Error, check getConnectError() for reason
 **
 *****************************************************************************/

int
CConnection::openConnectionToReader (
  const char *                  pReaderHostName)
{
#if defined(linux) || defined(__APPLE__)
    int                         Sock;
#endif
#ifdef WIN32
    SOCKET                      Sock;
#endif
    struct addrinfo             aiHints;
    struct addrinfo *           aiList;
    int                         Flag;
    struct sockaddr_in          Sin;
    int                         rc;

    /*
     * Clear the connect error string
     */
    m_pConnectErrorStr = NULL;

    /*
     * Make sure there isn't already a connection.
     */
    if(NULL != m_pPlatformSocket)
    {
        m_pConnectErrorStr = "already connected";
        return -1;
    }

    /*
     * Look up host using getaddrinfo().
     * This could be configured a lot of different ways.
     * There is /etc/hosts, DNS, NIS, etc, etc.
     * Suffice to say it is big, bulky, and susceptible to stall.
     */
    memset(&aiHints, 0, sizeof(aiHints));
    aiHints.ai_family = AF_INET;
    aiList = NULL;

    rc = getaddrinfo(pReaderHostName, NULL, &aiHints, &aiList);
    if(0 != rc)
    {
        m_pConnectErrorStr = "host lookup failed";
        return -1;
    }

    /*
     * Convert the address to sockaddr_in format
     */
    memset(&Sin, 0, sizeof Sin);
    Sin.sin_family = AF_INET;
    Sin.sin_addr = ((struct sockaddr_in *)(aiList->ai_addr))->sin_addr;
    Sin.sin_port = htons(LLRP1_TCP_PORT);

    /*
     * Done withe the host addrinfo
     */
    freeaddrinfo(aiList);

    /*
     * Create the socket.
     */
    Sock = socket(AF_INET, SOCK_STREAM, 0);
#if defined(linux) || defined(__APPLE__)
    if(0 > Sock)
#endif /* linux */
#ifdef WIN32
    if(NULL == Sock)
#endif /* WIN32 */
    {
        m_pConnectErrorStr = "socket() failed";
        return -3;
    }

    /*
     * Connect the socket to reader. This can stall.
     */
    rc = connect(Sock, (struct sockaddr *)&Sin, sizeof Sin);
    if(0 > rc)
    {
        /* Connect failed */
        m_pConnectErrorStr = "connection failed";
#if defined(linux) || defined(__APPLE__)
        close(Sock);
#endif
#ifdef WIN32
        closesocket(Sock);
#endif
        return -4;
    }

    /*
     * Best effort to set no delay. If this doesn't work
     * (no reason it shouldn't) we do not declare defeat.
     */
    Flag = 1;

#if defined(linux) || defined(__APPLE__)
    setsockopt(Sock, IPPROTO_TCP, TCP_NODELAY, (void*)&Flag, sizeof Flag);
#endif
#ifdef WIN32
    setsockopt(Sock, IPPROTO_TCP, TCP_NODELAY, (char*)&Flag, sizeof Flag);
#endif

    /*
     * Wrap the platform-specific socket in the platform-specific class.
     */
    m_pPlatformSocket = new CPlatformSocket(Sock);

    /*
     * Victory
     */
    return 0;
}


/**
 *****************************************************************************
 **
 ** @brief  Get the string that explains openReaderConnection() error
 **
 ** @return     ==NULL          No error
 **             !=NULL          Short string description of error
 **
 *****************************************************************************/

const char *
CConnection::getConnectError (void)
{
    return m_pConnectErrorStr;
}


/**
 *****************************************************************************
 **
 ** @brief  Close connection to reader, allow reuse of instance
 **
 ** @return     ==0             Connected OK, ready for business
 **             !=0             Error, check getConnectError() for reason
 **
 *****************************************************************************/

int
CConnection::closeConnectionToReader (void)
{
    if(NULL == m_pPlatformSocket)
    {
        m_pConnectErrorStr = "not connected";
        return -1;
    }

#if defined(linux) || defined(__APPLE__)
    shutdown(m_pPlatformSocket->m_sock, SHUT_RDWR);
    close(m_pPlatformSocket->m_sock);
    m_pPlatformSocket->m_sock = -1;
#endif
#ifdef WIN32
    closesocket(m_pPlatformSocket->m_sock);
    m_pPlatformSocket->m_sock = NULL;
#endif

    delete m_pPlatformSocket;
    m_pPlatformSocket = NULL;
    return 0;
}


/**
 *****************************************************************************
 **
 ** @brief  Transact a LLRP request and response to a connection
 **
 ** This is a combination of sendMessage() and recvResponse().
 ** The MessageID is taken from the outgoing messages.
 ** It's best to not use MessageID 0.
 **
 ** @param[in]  pSendMessage    Pointer to the LLRP message to send.
 ** @param[in]  nMaxMS          -1 => block indefinitely
 **                              0 => just peek at input queue and
 **                                   socket queue, return immediately
 **                                   no matter what
 **                             >0 => ms to await complete frame
 **
 ** @return     ==NULL          Something failed.  Use getTransactError()
 **                             for best guess at what went wrong.
 **             !=NULL          Response message
 **
 *****************************************************************************/

CMessage *
CConnection::transact (
  CMessage *                    pSendMessage,
  int                           nMaxMS)
{
    const CTypeDescriptor *     pResponseType;
    EResultCode                 lrc;
    CMessage *                  pResponseMessage;

    /*
     * Determine the response type. The type descriptor
     * of the outgoing request message points to the
     * type descriptor of the response. Since we are
     * totally dependent upon it, fail if there
     * is no response type pointer value.
     */
    pResponseType = pSendMessage->m_pType->m_pResponseType;
    if(NULL == pResponseType)
    {
        CErrorDetails *         pError = &m_Send.ErrorDetails;

        pError->clear();
        pError->resultCodeAndWhatStr(RC_MissingResponseType,
            "send message has no response type");
        return NULL;
    }

    /*
     * Send the request
     */
    lrc = sendMessage(pSendMessage);
    if(RC_OK != lrc)
    {
        return NULL;
    }

    /*
     * Receive the response subject to timeout
     */
    pResponseMessage = recvResponse(nMaxMS,
                            pResponseType, pSendMessage->getMessageID());

    /*
     * Whatever recvResponse() returned is the result.
     */
    return pResponseMessage;
}


/**
 *****************************************************************************
 **
 ** @brief  Get the details that explains transact() error
 **
 ** This is not 100% reliable. It looks at the send error and
 ** if that seems OK it uses the recv error. Uses of sendMessage(),
 ** recvMessage() or recvResponse() since transact() will
 ** distort the error.
 **
 ** @return                     Pointer to const error details
 **
 *****************************************************************************/

const CErrorDetails *
CConnection::getTransactError (void)
{
    const CErrorDetails *       pError;

    pError = getSendError();
    if(RC_OK == pError->m_eResultCode)
    {
        pError = getRecvError();
    }

    return pError;
}


/**
 *****************************************************************************
 **
 ** @brief  Send a LLRP message to a connection
 **
 ** @param[in]  pMessage        Pointer to the LLRP message to send.
 **
 ** @return     RC_OK               Frame sent
 **             RC_SendIOError      I/O error in write().
 **                                 Probably means fd is bad.
 **             RC_...              Encoder error.
 **                                 Check getSendError() for why.
 **
 *****************************************************************************/

EResultCode
CConnection::sendMessage (
  CMessage *                    pMessage)
{
    CErrorDetails *             pError = &m_Send.ErrorDetails;
    CFrameEncoder *             pEncoder;

    /*
     * Clear the error details in the send state.
     */
    pError->clear();

    /*
     * Make sure the socket is open.
     */
    if(NULL == m_pPlatformSocket)
    {
        pError->resultCodeAndWhatStr(RC_MiscError, "not connected");
        return pError->m_eResultCode;
    }

    /*
     * Construct a frame encoder. It needs to know the buffer
     * base and maximum size.
     */
    pEncoder = new CFrameEncoder(m_Send.pBuffer, m_nBufferSize);

    /*
     * Check that the encoder actually got created.
     */
    if(NULL == pEncoder)
    {
        pError->resultCodeAndWhatStr(RC_MiscError,
                "encoder constructor failed");
        return pError->m_eResultCode;
    }

    /*
     * Encode the message. Return value is ignored.
     * We check the encoder's ErrorDetails for results.
     */
    pEncoder->encodeElement(pMessage);

    /*
     * Regardless of what happened capture the error details
     * and the number of bytes placed in the buffer.
     */
    m_Send.ErrorDetails = pEncoder->m_ErrorDetails;
    m_Send.nBuffer = pEncoder->getLength();

    /*
     * Bye bye li'l encoder.
     */
    delete pEncoder;

    /*
     * If the encoding appears complete write the frame
     * to the connection. NB: this is not ready for
     * non-blocking I/O (EWOULDBLOCK).
     */
    if(RC_OK == pError->m_eResultCode)
    {
        int             rc;

        rc = send(m_pPlatformSocket->m_sock, (char*)m_Send.pBuffer,
            m_Send.nBuffer, 0);
        if(rc != (int)m_Send.nBuffer)
        {
            /* Yikes! */
            pError->resultCodeAndWhatStr(RC_SendIOError, "send IO error");
        }
    }

    /*
     * Done.
     */
    return pError->m_eResultCode;
}


/**
 *****************************************************************************
 **
 ** @brief  Get the details that explains sendMessage() error
 **
 ** @return                     Pointer to const error details
 **
 *****************************************************************************/

const CErrorDetails *
CConnection::getSendError (void)
{
    return &m_Send.ErrorDetails;
}


/**
 *****************************************************************************
 **
 ** @brief  Receive a message from a connection
 **
 ** The message could have already arrived and be pending
 ** on the input queue.
 **
 ** @param[in]  nMaxMS          -1 => block indefinitely
 **                              0 => just peek at input queue and
 **                                   socket queue, return immediately
 **                                   no matter what
 **                             >0 => ms to await complete frame
 **
 ** @return     ==NULL          No message available per parameters.
 **                             Check getRecvError() for why
 **             !=NULL          Input message
 **
 *****************************************************************************/

CMessage *
CConnection::recvMessage (
  int                           nMaxMS)
{
    time_t                      timeLimit = calculateTimeLimit(nMaxMS);
    EResultCode                 lrc;
    CMessage *                  pMessage;

    /*
     * Make sure the socket is open.
     */
    if(NULL == m_pPlatformSocket)
    {
        CErrorDetails *         pError = &m_Recv.ErrorDetails;

        pError->resultCodeAndWhatStr(RC_MiscError, "not connected");
        return NULL;
    }

    /*
     * Loop until victory or some sort of exception happens
     */
    for(;;)
    {
        /*
         * Check the input queue to see if there is already
         * a message pending.
         */
        if(!m_listInputQueue.empty())
        {
            pMessage = m_listInputQueue.front();
            m_listInputQueue.pop_front();
            return pMessage;
        }

        /*
         * No message available. Advance the receiver state
         * and see if a message is produced.
         */
        lrc = recvAdvance(nMaxMS, timeLimit);
        if(lrc != RC_OK)
        {
            return NULL;
        }
    }
}


/**
 *****************************************************************************
 **
 ** @brief  Get the details that explains recvMessage()
 **         or recvResponse() error
 **
 ** @return                     Pointer to const error details
 **
 *****************************************************************************/

const CErrorDetails *
CConnection::getRecvError (void)
{
    return &m_Recv.ErrorDetails;
}


/**
 *****************************************************************************
 **
 ** @brief  Receive a specific message from a connection
 **
 ** The message is identified by type and message ID.
 **
 ** This is used to receive a response to a request, but
 ** could be used for other things, too. A message matches
 ** and is returned if the type matches AND if the message
 ** ID matches. Either or both can be wildcards.
 **
 ** The sought message could have already arrived and be pending
 ** on the input queue. While we are looking for the response
 ** notifications might arrive. They are held in the input
 ** queue while we continue to look for the sought message.
 **
 ** About timeLimit....
 ** The timeLimit is the last time() we'll try to receive
 ** the sought message and prevents "spinning".
 ** It is conceivable that a steady stream of messages
 ** other than the one sought could arrive, and the time
 ** between those messages could be smaller the nMaxMS.
 ** When that happens, recvAdvance() would not time out
 ** during its poll/select. It could "spin". The time out
 ** must be detected by other means and that's the
 ** purpose of timeLimit.
 **
 ** @param[in]  nMaxMS          -1 => block indefinitely
 **                              0 => just peek at input queue and
 **                                   socket queue, return immediately
 **                                   no matter what
 **                             >0 => ms to await complete frame
 ** @param[in]  pResponseType   The type descriptor of the sought
 **                             or NULL to match all messages.
 **                             If not NULL, ERROR_MESSAGE will
 **                             also match.
 ** @param[in]  ResponseMessageID The MessageID of sought message
 **                             or 0 to match all messages.
 **
 ** @return     ==NULL          No message available per parameters.
 **                             Check getRecvError() for why
 **             !=NULL          Input message
 **
 *****************************************************************************/

CMessage *
CConnection::recvResponse (
  int                           nMaxMS,
  const CTypeDescriptor *       pResponseType,
  llrp_u32_t                    ResponseMessageID)
{
    time_t                      timeLimit = calculateTimeLimit(nMaxMS);
    const CTypeDescriptor *     pErrorMsgType;
    EResultCode                 lrc;
    CMessage *                  pMessage;

    /*
     * Make sure the socket is open.
     */
    if(NULL == m_pPlatformSocket)
    {
        CErrorDetails *         pError = &m_Recv.ErrorDetails;

        pError->resultCodeAndWhatStr(RC_MiscError, "not connected");
        return NULL;
    }

    /*
     * Look up the ERROR_MESSAGE type descriptor now.
     */
    pErrorMsgType = m_pTypeRegistry->lookupMessage(100u);

    /*
     * Loop until victory or some sort of exception happens
     */
    for(;;)
    {
        /*
         * Check the input queue to see if the sought
         * message is present.
         */
        for (
            std::list<CMessage *>::iterator msg = m_listInputQueue.begin();
            msg != m_listInputQueue.end();
            msg++)
        {
            pMessage = *msg;

            /*
             * Are we looking for a particular message type?
             */
            if(NULL != pResponseType)
            {
                /*
                 * See if it is the sought response type or
                 * an ERROR_MESSAGE.
                 */
                if(pMessage->m_pType != pResponseType &&
                   pMessage->m_pType != pErrorMsgType)
                {
                    /* Type does not match. Keep looking. */
                    continue;
                }
            }

            /*
             * Are we looking for a particular message ID?
             */
            if(0 != ResponseMessageID)
            {
                if(pMessage->getMessageID() != ResponseMessageID)
                {
                    /* Message ID does not match. Keep looking. */
                    continue;
                }
            }

            /*
             * Found it. Unlink it from the queue and return it.
             */
            m_listInputQueue.remove(pMessage);
            return pMessage;
        }

        /*
         * Sought message is not in the queue. Advance the
         * receiver state and see if the message is produced.
         */
        lrc = recvAdvance(nMaxMS, timeLimit);
        if(lrc != RC_OK)
        {
            return NULL;
        }

        /*
         * Loop to the top and try again.
         */
    }
}


/**
 *****************************************************************************
 **
 ** @brief  Internal routine to advance receiver
 **
 ** Try to receive some bytes from the socket and build a frame.
 ** When there is a complete frame decode it and enqueue it.
 ** Stops when a frame is added or when time runs out.
 **
 ** The timeLimit prevents "spinning". See recvResponse() above.
 **
 ** @param[in]  nMaxMS          -1 => block indefinitely
 **                              0 => just peek at input queue and
 **                                   socket queue, return immediately
 **                                   no matter what
 **                             >0 => ms to await complete frame
 ** @param[in]  timeLimit        0 => never stop
 **                             >0 => latest time() to try
 **
 ** @return     RC_OK               Frame received
 **             RC_RecvEOF          End-of-file condition on socket.
 **             RC_RecvIOError      I/O error in poll() or recv().
 **                                 Probably means socket is bad.
 **             RC_RecvFramingError LLRP_FrameExtract() detected an
 **                                 impossible situation. Recovery unlikely.
 **             RC_RecvTimeout      Frame didn't complete within allowed time
 **             RC_RecvBufferOverflow
 **                                 LLRP_FrameExtract() detected an inbound
 **                                 message that would overflow the receive
 **                                 buffer.
 **             RC_...              Decoder error.
 **
 *****************************************************************************/

EResultCode
CConnection::recvAdvance (
  int                           nMaxMS,
  time_t                        timeLimit)
{
    CErrorDetails *             pError = &m_Recv.ErrorDetails;

    /*
     * Clear the error details in the receiver state.
     */
    pError->clear();

    /*
     * Loop until victory or some sort of exception happens
     */
    for(;;)
    {
        int                     rc;

        /*
         * Note that the frame is in progress.
         * Existing buffer content, if any, is deemed
         * invalid or incomplete.
         */
        m_Recv.bFrameValid = FALSE;

        /*
         * Check to see if we have a frame in the buffer.
         * If not, how many more bytes do we need?
         *
         * LLRP_FrameExtract() status
         *
         * FRAME_ERROR          Impossible situation, like message
         *                      length too small or the like.
         *                      Recovery in this situation is
         *                      unlikely and probably the app
         *                      should drop the connection.
         *
         * FRAME_READY          Frame is complete. Details are
         *                      available for pre-decode decisions.
         *
         * FRAME_NEED_MORE      Need more input bytes to finish the frame.
         *                      The m_nBytesNeeded field is how many more.
         */
        m_Recv.FrameExtract = CFrameExtract(m_Recv.pBuffer, m_Recv.nBuffer);

        /*
         * Framing error?
         */
        if(CFrameExtract::FRAME_ERROR == m_Recv.FrameExtract.m_eStatus)
        {
            pError->resultCodeAndWhatStr(RC_RecvFramingError,
                    "framing error in message stream");
            break;
        }

        /*
         * Need more bytes? extractRc>0 means we do and extractRc is the
         * number of bytes immediately required.
         */
        if(CFrameExtract::NEED_MORE == m_Recv.FrameExtract.m_eStatus)
        {
            unsigned int        nRead = m_Recv.FrameExtract.m_nBytesNeeded;
            unsigned char *     pBufPos = &m_Recv.pBuffer[m_Recv.nBuffer];

            /*
             * Before we do anything that might block,
             * check to see if the time limit is exceeded.
             */
            if(0 != timeLimit)
            {
                if(time(NULL) > timeLimit)
                {
                    /* Timeout */
                    pError->resultCodeAndWhatStr(RC_RecvTimeout,
                            "timeout");
                    break;
                }
            }

            /*
             * The frame extractor needs more data, make sure the
             * frame size fits in the receive buffer.
             */
            if(m_Recv.nBuffer + nRead > m_nBufferSize)
            {
                pError->resultCodeAndWhatStr(RC_RecvBufferOverflow,
                        "buffer overflow");
                break;
            }

            /*
             * If this is not a block indefinitely request use poll()
             * to see if there is data in time.
             */
            if(nMaxMS >= 0)
            {
#if defined(linux) || defined(__APPLE__)
                struct pollfd   pfd;

                pfd.fd = m_pPlatformSocket->m_sock;
                pfd.events = POLLIN;
                pfd.revents = 0;

                rc = poll(&pfd, 1, nMaxMS);
#endif /* linux */
#ifdef WIN32
                fd_set          readfds;
                struct timeval  timeout;

                timeout.tv_sec = nMaxMS / 1000u;
                timeout.tv_usec = (nMaxMS % 1000u) * 1000u;

                FD_ZERO(&readfds);
                FD_SET(m_pPlatformSocket->m_sock, &readfds);
                rc = select(-1, &readfds, NULL, NULL, &timeout);

#endif /* WIN32 */
                if(0 > rc)
                {
                    /* Error */
                    pError->resultCodeAndWhatStr(RC_RecvIOError,
                            "poll failed");
                    break;
                }
                if(0 == rc)
                {
                    /* Timeout */
                    pError->resultCodeAndWhatStr(RC_RecvTimeout,
                            "timeout");
                    break;
                }
            }

            /*
             * Read (recv) some number of bytes from the socket.
             */
            rc = recv(m_pPlatformSocket->m_sock, (char*)pBufPos, nRead, 0);
            if(0 > rc)
            {
                /*
                 * Error. Note this could be EWOULDBLOCK if the
                 * file descriptor is using non-blocking I/O.
                 * So we return the error but do not tear-up
                 * the receiver state.
                 */
                pError->resultCodeAndWhatStr(RC_RecvIOError,
                        "recv IO error");
                break;
            }

            if(0 == rc)
            {
                /* EOF */
                pError->resultCodeAndWhatStr(RC_RecvEOF,
                        "recv end-of-file");
                break;
            }

            /*
             * When we get here, rc>0 meaning some bytes were read.
             * Update the number of bytes present.
             * Then loop to the top and retry the FrameExtract().
             */
            m_Recv.nBuffer += rc;

            continue;
        }

        /*
         * Is the frame ready?
         * If a valid frame is present, decode and enqueue it.
         */
        if(CFrameExtract::READY == m_Recv.FrameExtract.m_eStatus)
        {
            /*
             * Frame appears complete. Time to try to decode it.
             */
            CFrameDecoder *     pDecoder;
            CMessage *          pMessage;

            /*
             * Construct a new frame decoder. It needs the registry
             * to facilitate decoding.
             */
            pDecoder = new CFrameDecoder(m_pTypeRegistry,
                    m_Recv.pBuffer, m_Recv.nBuffer);

            /*
             * Make sure we really got one. If not, weird problem.
             */
            if(pDecoder == NULL)
            {
                /* All we can do is discard the frame. */
                m_Recv.nBuffer = 0;
                m_Recv.bFrameValid = FALSE;
                pError->resultCodeAndWhatStr(RC_MiscError,
                        "decoder constructor failed");
                break;
            }

            /*
             * Now ask the nice, brand new decoder to decode the frame.
             * It returns NULL for some kind of error.
             */
            pMessage = pDecoder->decodeMessage();

            /*
             * Always capture the error details even when it works.
             * Whatever happened, we are done with the decoder.
             */
            m_Recv.ErrorDetails = pDecoder->m_ErrorDetails;

            /*
             * Bye bye and thank you li'l decoder.
             */
            delete pDecoder;

            /*
             * If NULL there was an error. Clean up the
             * receive state. Return the error.
             */
            if(NULL == pMessage)
            {
                /*
                 * Make sure the return is not RC_OK
                 */
                if(RC_OK == pError->m_eResultCode)
                {
                    pError->resultCodeAndWhatStr(RC_MiscError,
                            "NULL message but no error");
                }

                /*
                 * All we can do is discard the frame.
                 */
                m_Recv.nBuffer = 0;
                m_Recv.bFrameValid = FALSE;

                break;
            }

            /*
             * Yay! It worked. Enqueue the message.
             */
            m_listInputQueue.push_back(pMessage);

            /*
             * Note that the frame is valid. Consult
             * Recv.FrameExtract.m_MessageLength.
             * Clear the buffer count to be ready for next time.
             */
            m_Recv.bFrameValid = TRUE;
            m_Recv.nBuffer = 0;

            break;
        }

        /*
         * If we get here there was an FrameExtract status
         * we didn't expect.
         */

        /*NOTREACHED*/
        assert(0);
    }

    return pError->m_eResultCode;
}


/**
 *****************************************************************************
 **
 ** @brief  Internal routine to calculate time limit
 **
 ** Based on nMaxMS, the subscriber specified max time to
 ** await receipt of a (specific) message, determine the
 ** last time() to try.
 **
 ** The timeLimit prevents "spinning". See recvResponse() above.
 **
 ** @param[in]  nMaxMS          -1 => block indefinitely
 **                              0 => just peek at input queue and
 **                                   socket queue, return immediately
 **                                   no matter what
 **                             >0 => ms to await complete frame
 **
 ** @return     timeLimit        0 => never stop
 **                             >0 => latest time() to try
 **
 *****************************************************************************/

time_t
CConnection::calculateTimeLimit (
  int                           nMaxMS)
{
    if(0 == nMaxMS)
    {
        /* When just peeking, try for at most one second */
        return time(NULL) + 1;
    }
    else if(0 < nMaxMS)
    {
        /*
         * Try for a at most a certain period of time.
         *
         * timeLimit = now + ceil(nMaxMS/1000) + 1
         *
         * The last +1 compensates for not knowing
         * when the next time() tick will happen.
         *
         * For example, if now is SECONDS.999 seconds
         * the next tick will happen in 1 millisecond.
         * Suppose nMaxMS is 500ms (1/2 second).
         * Even rounding 500ms up to 1 second, the
         * time limit without the +1 would be
         * SECONDS+1 -- 1ms away. That's too soon.
         *
         * The extra +1 makes the maximum timeout
         * longer than required. But the timeLimit
         * is a safeguard anyway and usually the
         * timeout will occur when the user wants.
         */
        return time(NULL) + ((nMaxMS + 1999u) / 1000u);
    }
    else
    {
        /* Try indefinitely */
        return 0;
    }
}

}; /* namespace LLRP */

