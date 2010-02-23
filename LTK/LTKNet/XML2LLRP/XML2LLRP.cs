using System;
using System.Collections.Generic;
using System.Text;
using System.IO;

using System.Xml;

using Org.LLRP.LTK.LLRPV1;
using LTKD = Org.LLRP.LTK.LLRPV1.DataType;

namespace LTK
{
    class XML2LLRP
    {
        static private int nextElement(ref XmlTextReader tr)
        {
            while (true)
            {
                tr.Read();
                switch (tr.ReadState)
                {
                    case ReadState.EndOfFile:
                        return (0);
                    case ReadState.Error:
                    case ReadState.Initial:
                    case ReadState.Closed:
                        return (-1);
                    case ReadState.Interactive:
                        if (tr.NodeType == XmlNodeType.Element)
                        {
                            return (1);
                        }
                        break;
                }
            }
        }

        static private void DumpHex(ref byte[] msg)
        {
            string hex = "";
            Console.WriteLine("Len: " + msg.Length);
            foreach (byte b in msg)
            {
                int tmp = (int)b;
                hex += String.Format("{0:x2} ", (uint)System.Convert.ToUInt32(tmp.ToString()));
            }
            Console.WriteLine(hex);
            Console.Out.Flush();
        }

        static void Main(string[] args)
        {
            Stream outp;
            Stream inp;
            TextWriter errorWriter = Console.Error;

            if (args.Length == 2)
            {
                inp = new FileStream(args[0], FileMode.Open, FileAccess.Read);
                outp = new FileStream(args[1], FileMode.OpenOrCreate, FileAccess.Write);
            }
            else if (args.Length == 1)
            {
                inp = new FileStream(args[0], FileMode.Open, FileAccess.Read);
                outp = Console.OpenStandardOutput();
            }
            else
            {
                inp = Console.OpenStandardInput();
                outp = Console.OpenStandardOutput();
            }

            System.Xml.XmlTextReader xr = new System.Xml.XmlTextReader(inp);
            xr.MoveToContent();
            if (!xr.LocalName.Equals("packetSequence"))
            {
                throw new System.FormatException("Not a packetSequence");
            }
            int result = nextElement(ref xr);

            UInt32 msg_no = 0;
            //StreamWriter sw = new StreamWriter(outp);
            while (result == 1)
            {
                if (xr.LocalName.Equals("packetSequence"))
                {
                    break;
                }

                StringWriter sw = new StringWriter();
                XmlTextWriter tw = new XmlTextWriter(sw);
                XmlDocument doc = new XmlDocument();
                doc.Load(xr.ReadSubtree());
                doc.WriteContentTo(tw);

                //turn me on to find out where each packet starts
                //errorWriter.Write("packet {0}  \n", msg_no);

                LTKD.Message msg;
                ENUM_LLRP_MSG_TYPE dummy;

                try
                {
                    LLRPXmlParser.ParseXMLToLLRPMessage(sw.ToString(), out msg, out dummy);
                }
                catch (Exception e)
                {
                    String desc = "ParseXMLToLLRPMessage failure on Packet #" + msg_no + ", " + e.Message;
                    Console.Error.WriteLine(desc);
                    String err_msg =
                        "<ERROR_MESSAGE MessageID=\"0\" Version=\"0\">\r\n" +
                        "  <LLRPStatus>\r\n" +
                        "    <StatusCode>R_DeviceError</StatusCode>\r\n" +
                        "    <ErrorDescription>" + desc + "</ErrorDescription>\r\n" +
                        "  </LLRPStatus>\r\n" +
                        "</ERROR_MESSAGE>\r\n";

                    LLRPXmlParser.ParseXMLToLLRPMessage(err_msg, out msg, out dummy);
                }

                byte[] packet = LTKD.Util.ConvertBitArrayToByteArray(msg.ToBitArray());
                outp.Write(packet, 0, packet.Length);

                /* next XML subdocument */
                result = nextElement(ref xr);
                msg_no++;
            }

        }
    }
}
