using System;
using System.Collections.Generic;
using System.Text;
using System.IO;
using System.Runtime.InteropServices;
using System.Net;

using Org.LLRP.LTK.LLRPV1;
using Org.LLRP.LTK.LLRPV1.DataType;

namespace LTK
{
    class LLRP2XML
    {

        static private void DumpHex(ref byte [] msg)
        {
            string hex = "";
            Console.WriteLine("Len: " + msg.Length);
            foreach (byte b in msg)
            {
                int tmp = (int)b;
                hex += String.Format("{0:x2} ", (uint)System.Convert.ToUInt32(tmp.ToString()));
            }
            Console.WriteLine (hex);
            Console.Out.Flush();
        }

        private static void read_msg (Stream source, byte[] hdr, ref LLRPBinaryDecoder.LLRP_Envelope env, out byte [] packet)
        {

            /* read remaining bytes */
            if (env.msg_len < LLRPBinaryDecoder.MIN_HDR || env.msg_len > 4000000)
            {
                throw new MalformedPacket("Message length (" + env.msg_len + ") out-of-range");
            }
            int remainder = (int)env.msg_len - LLRPBinaryDecoder.MIN_HDR;
            packet = new byte[env.msg_len];
            Array.Copy(hdr, packet, LLRPBinaryDecoder.MIN_HDR);
            int bytes_read = source.Read(packet, LLRPBinaryDecoder.MIN_HDR, remainder);
            if (bytes_read < remainder)
            {
                throw new MalformedPacket("Reached EOF before end of message");
            }
        }

        static void Main(string[] args)
        {
            Stream s;
            UInt32 byteCount = 0;
            TextWriter errorWriter = Console.Error;

            if (args.GetLength(0) >= 1)
            {
                s = new FileStream(args[0], FileMode.Open, FileAccess.Read);
            }
            else
            {
                s = Console.OpenStandardInput();
            }

            Console.WriteLine("<packetSequence>\r\n");
            UInt32 msg_no = 0;
            while (true)
            {

                /* read message header */
                byte[] hdr = new byte[LLRPBinaryDecoder.MIN_HDR];
                int how_many = s.Read(hdr, 0, LLRPBinaryDecoder.MIN_HDR);
                if (how_many == 0)
                {
                    break;
                }
                else if (how_many < LLRPBinaryDecoder.MIN_HDR)
                {
                    throw new MalformedPacket ("Header fragment at end-of-file");
                }


                /* get the full message */
                LLRPBinaryDecoder.LLRP_Envelope env;
                LLRPBinaryDecoder.Decode_Envelope(hdr, out env);
                
                // turn me on to find out where each packet starts
                //errorWriter.Write("packet {0} offset {1}({1:x}) ", msg_no, byteCount);
                //errorWriter.Write(env.msg_type.ToString() + "\n");
                
                byte[] packet;
                read_msg(s, hdr, ref env, out packet);
                byteCount += env.msg_len;
                Message msg;

                try
                {
                    LLRPBinaryDecoder.Decode(ref packet, out msg);
                    try
                    {
                        Console.Write(msg.ToString() + "\r\n");
                    }
                    catch (Exception e)
                    {
                        Console.WriteLine(e.Message + "\r\nAt: " + e.StackTrace);
                        Console.Write(
                            "<ERROR_MESSAGE MessageID=\"0\" Version=\"0\">\r\n" +
                            "  <LLRPStatus>\r\n" +
                            "    <StatusCode>R_DeviceError</StatusCode>\r\n" +
                            "    <ErrorDescription>ToString failure on Packet #" + msg_no + "</ErrorDescription>\r\n" +
                            "  </LLRPStatus>\r\n" +
                            "</ERROR_MESSAGE>\r\n"
                        );
                    }
                }
                catch (Exception e)
                {
                    Console.WriteLine(e.Message + "\r\nAt: " + e.StackTrace);
                    Console.Write(
                        "<ERROR_MESSAGE MessageID=\"0\" Version=\"0\">\r\n" +
                        "  <LLRPStatus>\r\n" +
                        "    <StatusCode>R_DeviceError</StatusCode>\r\n" +
                        "    <ErrorDescription>Decode failure on Packet #" + msg_no + "</ErrorDescription>\r\n" +
                        "  </LLRPStatus>\r\n" +
                        "</ERROR_MESSAGE>\r\n"
                    );

                }

                msg_no++;
            }

            Console.WriteLine("</packetSequence>\r\n"); 
        }
    }



}
