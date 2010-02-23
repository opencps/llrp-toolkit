/*
 ***************************************************************************
 *  Copyright 2008 Impinj, Inc.
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

/*
***************************************************************************
 * File Name:       Transaction.cs
 * 
 * Author:          Impinj
 * Organization:    Impinj
 * Date:            September, 2007
 * 
 * Description:     This file contains simple network send and receive 
 *                  command.
***************************************************************************
*/

using System;
using System.Collections.Generic;
using System.Text;
using System.Threading;
using System.Data;
using System.Collections;
using Org.LLRP.LTK.LLRPV1;
using Org.LLRP.LTK.LLRPV1.DataType;

namespace Org.LLRP.LTK.LLRPV1
{
    class Transaction
    {
        private UInt32 msg_id;
        private CommunicationInterface commIF;
        private ENUM_LLRP_MSG_TYPE msg_response_type;
        private byte[] rsp_data;
        private ManualResetEvent rsp_event;
        private ManualResetEvent err_event;

        /// <summary>
        /// Construct an LLRP transaction object for the purpose of performing a transaction
        /// via LLRP.  
        /// </summary>
        /// <param name="ci">The communications interface to use for the transaction</param>
        /// <param name="send_msg_id">the message ID of the sent message (also of the response)</param>
        /// <param name="response_type">the expected response type</param>
        public Transaction(CommunicationInterface ci, UInt32 send_msg_id, ENUM_LLRP_MSG_TYPE response_type)
        {
            msg_id = send_msg_id;
            commIF = ci;
            msg_response_type = response_type;
            rsp_event = new ManualResetEvent(false);
            err_event = new ManualResetEvent(false);
        }

        /// <summary>
        /// A private method to handle frames from the communications interface during a transaction
        /// </summary>
        /// <param name="ver">LLRP protocol version</param>
        /// <param name="msg_type">Message type of the received message</param>
        /// <param name="id">Message ID of th received message</param>
        /// <param name="data">The raw message data as a byte[]</param>
        private void ProcessFrame(Int16 ver, Int16 msg_type, Int32 id, byte[] data)
        {
            /* is this a frame that is a reponse to our message */
            if ((((ENUM_LLRP_MSG_TYPE)msg_type) == msg_response_type) && (id == msg_id))
            {
                /* save the data and signal the event */
                rsp_data = new byte[data.Length];
                Array.Copy(data, rsp_data, data.Length);
                rsp_event.Set();
            }

            // all transactions will be aborted if we receive an error 
            if (((ENUM_LLRP_MSG_TYPE)msg_type) == ENUM_LLRP_MSG_TYPE.ERROR_MESSAGE)
            {
                /* save the data and signal the event */
                rsp_data = new byte[data.Length];
                Array.Copy(data, rsp_data, data.Length);
                err_event.Set();
            }
        }

        /// <summary>
        /// Send data
        /// </summary>
        /// <param name="ci">Communication interface</param>
        /// <param name="data">Data to be sent, byte array</param>
        public static void Send(CommunicationInterface ci, byte[] data)
        {
            ci.Send(data);
        }

        /// <summary>
        /// Receive data
        /// </summary>
        /// <param name="ci">Communication interface</param>
        /// <param name="buffer">Buffer for receiving data</param>
        /// <returns></returns>
        public static int Receive(CommunicationInterface ci, out byte[] buffer)
        {
            return ci.Receive(out buffer);
        }

        /// <summary>
        /// A helper to just send a message without a transaction
        /// </summary>
        /// <param name="msg">Message to send</param>
        public void Send(Message msg)
        {
            // Convert the outgoing message to an array of bytes 
            bool[] bit_array = msg.ToBitArray();
            byte[] data = Util.ConvertBitArrayToByteArray(bit_array);

            commIF.Send(data);
        }

        /// <summary>
        /// Performs a message transaction with the reader.  Sends the msg and waitws
        /// for a response, an error message or a timeout
        /// </summary>
        /// <param name="msg">Message to send</param>
        /// <param name="msg_err">Output contains error message if sent by reader</param>
        /// <param name="time_out">Timeout value to abort waiting for response</param>
        /// <returns>Response Message or NULL if error or timeout </returns>
        public Message Transact(Message msg, out MSG_ERROR_MESSAGE msg_err, int time_out)
        {
            msg_err = null;
            Message rsp_msg = null;

            // subscribe to the comminucations interface delegate to look for the response
            commIF.OnFrameReceived += new delegateMessageReceived(this.ProcessFrame);

            Send(msg);

            // wait for either response or error 
            WaitHandle[] wait = new WaitHandle[] { rsp_event, err_event };
            int wait_result = WaitHandle.WaitAny(wait, time_out);

            switch (wait_result)
            {
                case 0:
                    /* got a response. already checked the mssage type */
                    LLRPBinaryDecoder.Decode(ref rsp_data, out rsp_msg);
                    break;
                case 1:
                    {
                        int cursor = 0;
                        int length;
                        BitArray bArr;

                        /* got an error */
                        bArr = Util.ConvertByteArrayToBitArray(rsp_data);
                        length = bArr.Count;
                        msg_err = MSG_ERROR_MESSAGE.FromBitArray(ref bArr, ref cursor, length);
                    }
                    break;
            }

            // remove the event handler when  our transaction is over 
            commIF.OnFrameReceived -= new delegateMessageReceived(this.ProcessFrame);

            return rsp_msg;
        }
    }
}
