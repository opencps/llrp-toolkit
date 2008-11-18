
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
 ** @file   ltkcpp_connection.h
 **
 ** @brief  Class for handling two-way LLRP message traffic
 **
 *****************************************************************************/
 

namespace LLRP
{

/*
 * @brief   Forward class declaration of platform-specific socket wrapper
 *
 * The definition is in ltkcpp_connection.cpp.
 *
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
class CPlatformSocket;

/**
 *****************************************************************************
 **
 ** @brief  LLRP connection class
 **
 ** An LLRP connection consists of:
 **     - A network socket
 **     - An input queue of messages already received. Used to hold
 **       asynchronous messages while awaiting a response.
 **     - Receiver state.
 **         - The receive buffer and count
 **         - Whether a frame is valid. Valid means that the receive
 **           buffer holds a frame and the MessageLength, MessageType,
 **           ProtocolVersion, and MessageID are valid (usable).
 **           Not valid with a nBuffer greater than 0 means that a frame
 **           is being received. Sometimes we want to look at the frame
 **           after it has been (or attempted to be) decoded.
 **         - Top-level frame variables: CFrameExtract
 **         - Details of the last receiver error, including I/O errors,
 **           end-of-file (EOF), timeout, or decode errors.
 **     - Send state
 **         - The send buffer and count
 **         - Details of the last send error, including I/O errors,
 **           or encode errors.
 ** 
 ** @ingroup LTKCoreElement
 **
 *****************************************************************************/

class CConnection
{
  public:
    CConnection (
      const CTypeRegistry *     pTypeRegistry,
      unsigned int              nBufferSize);

    ~CConnection (void);

    int
    openConnectionToReader (
      const char *              pReaderHostName);

    const char *
    getConnectError (void);

    int
    closeConnectionToReader (void);

    CMessage *
    transact (
      CMessage *                pSendMessage,
      int                       nMaxMS);

    const CErrorDetails *
    getTransactError (void);

    EResultCode
    sendMessage (
      CMessage *                pMessage);

    const CErrorDetails *
    getSendError (void);

    CMessage *
    recvMessage (
      int                       nMaxMS);

    CMessage *
    recvResponse (
      int                       nMaxMS,
      const CTypeDescriptor *   pResponseType,
      llrp_u32_t                ResponseMessageID);

    const CErrorDetails *
    getRecvError (void);

  private:
    /** The socket handle, platform specific */
    CPlatformSocket *           m_pPlatformSocket;

    /** Error message if openConnectionToReader() or close...() fail */
    const char *                m_pConnectErrorStr;

    /** The registry to consult for message/parameter types during decode. */
    const CTypeRegistry *       m_pTypeRegistry;

    /** Head of queue of messages already received. Probably events. */
    std::list<CMessage *>       m_listInputQueue;

    /** Size of the send/recv buffers, below, specified at construct() time */
    unsigned int                m_nBufferSize;

    /** Receive state */
    struct RecvState
    {
        /** The buffer. Contains incomming frame. */
        llrp_byte_t *       pBuffer;

        /** Count of bytes currently in buffer */
        unsigned int        nBuffer;

        /** Valid boolean. TRUE means the buffer and frame summary
         ** variables are valid (usable). This is always
         ** FALSE mid receive */
        int                 bFrameValid;

        /** Frame summary variables. Derived by FrameExtract() */
        CFrameExtract       FrameExtract;

        /** Details of last I/O or decoder error. */
        CErrorDetails       ErrorDetails;
    }                           m_Recv;

    /** Send state */
    struct SendState
    {
        /** The buffer. Contains outgoing frame. */
        llrp_byte_t *       pBuffer;

        /** Count of bytes currently in buffer (from last send) */
        unsigned int        nBuffer;

        /** Details of last I/O or encoder error. */
        CErrorDetails       ErrorDetails;
    }                           m_Send;

  private:
    EResultCode
    recvAdvance (
      int                           nMaxMS,
      time_t                        timeLimit);

    time_t
    calculateTimeLimit (
      int                           nMaxMS);
};


}; /* namespace LLRP */

