/*
 ***************************************************************************
 *  Copyright 2009 Impinj, Inc.
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
 * File Name:       CustomMessage.cs
 *
 * Author:          Impinj
 * Organization:    Impinj
 * Date:            June, 2009
 *
 * Description:     This file contains interfaces, base classes and parser
 *                  for custom messages
***************************************************************************
*/

using System;
using System.Collections;
using System.Collections.Generic;
using System.Text;
using System.Xml;
using System.Reflection;
using System.IO;

using Org.LLRP.LTK.LLRPV1.DataType;

namespace Org.LLRP.LTK.LLRPV1
{
    /// <summary>
    /// Custom Message Class
    /// </summary>
    public class MSG_CUSTOM_MESSAGE : Message
    {
        protected UInt32 VendorIdentifier;
        protected byte MessageSubtype;
        protected ByteArray Data;

        private Int16 VendorIdentifier_len = 0;
        private Int16 MessageSubtype_len = 0;
        private Int16 Data_len;

        public MSG_CUSTOM_MESSAGE()
        {
            msgType = 1023;
            MSG_ID = MessageID.getNewMessageID();  //Give each message a unique ID by default
        }

        /// <summary>
        /// Vendor ID
        /// </summary>
        public UInt32 VendorID
        {
            get { return VendorIdentifier; }
            set { VendorIdentifier = value; }
        }

        /// <summary>
        /// Sub type
        /// </summary>
        public byte SubType
        {
            get { return MessageSubtype; }
            set { MessageSubtype = value; }
        }

        /// <summary>
        /// Convert to XML string
        /// </summary>
        /// <returns>XML string</returns>
        public override string ToString()
        {
            int len;
            string xml_str = "<CUSTOM_MESSAGE";
            xml_str += string.Format(" xmlns=\"{0}\"\n",
                        LLRPConstants.NAMESPACE_URI);
            xml_str += string.Format(" xmlns:llrp=\"{0}\"\n",
                        LLRPConstants.NAMESPACE_URI);
            xml_str += " xmlns:xsi= \"http://www.w3.org/2001/XMLSchema-instance\"\n";
            xml_str += string.Format(" xsi:schemaLocation=\"{0} {1}\"\n",
                        LLRPConstants.NAMESPACE_URI,
                        LLRPConstants.NAMESPACE_SCHEMALOCATION);
            xml_str += " Version=\"" + version.ToString();
            xml_str += "\" MessageID=\"" + MSG_ID.ToString() + "\"" + ">" + "\r\n";

            if (VendorIdentifier != null)
            {
                try
                {
                    xml_str += "  <VendorIdentifier>";
                    xml_str += Util.ConvertValueTypeToString(VendorIdentifier, "u32", "");
                    xml_str += "</VendorIdentifier>\r\n";
                }
                catch { }
            }

            if (MessageSubtype != null)
            {
                try
                {
                    xml_str += "  <MessageSubtype>";
                    xml_str += Util.ConvertValueTypeToString(MessageSubtype, "u8", "");
                    xml_str += "</MessageSubtype>\r\n";
                }
                catch { }
            }

            if (Data != null)
            {
                try
                {
                    xml_str += "  <Data>";
                    xml_str += Util.ConvertArrayTypeToString(Data, "bytesToEnd", "Hex");
                    xml_str += "</Data>\r\n";
                }
                catch { }
            }

            xml_str += "</CUSTOM_MESSAGE>";

            return xml_str;
        }

        /// <summary>
        /// Deserialize a xml string to a MSG_CUSTOM_MESSAGE
        /// </summary>
        /// <param name="str">Input Xml string</param>
        /// <returns>MSG_CUSTOM_MESSAGE</returns>
        public new static MSG_CUSTOM_MESSAGE FromString(string str)
        {
            string val;

            XmlDocument xdoc = new XmlDocument();
            xdoc.LoadXml(str);
            XmlNode node = (XmlNode)xdoc.DocumentElement;

            MSG_CUSTOM_MESSAGE msg = new MSG_CUSTOM_MESSAGE();
            try
            {
                msg.MSG_ID = Convert.ToUInt32(XmlUtil.GetNodeAttrValue(node, "MessageID"));
            }
            catch { }

            val = XmlUtil.GetNodeValue(node, "VendorIdentifier");
            msg.VendorIdentifier = (UInt32)(Util.ParseValueTypeFromString(val, "u32", ""));

            val = XmlUtil.GetNodeValue(node, "MessageSubtype");
            msg.MessageSubtype = (byte)(Util.ParseValueTypeFromString(val, "u8", ""));

            val = XmlUtil.GetNodeValue(node, "Data");
            msg.Data = (ByteArray)(Util.ParseArrayTypeFromString(val, "bytesToEnd", "Hex"));

            return msg;
        }

        /// <summary>
        /// Encode message to boolean (bit) array
        /// </summary>
        /// <returns>boolean array</returns>
        public override bool[] ToBitArray()
        {
            int len = 0;
            int cursor = 0;
            bool[] bit_array = new bool[4*1024*1024*8]; // Handle messages up to 4 MB

            BitArray bArr = Util.ConvertIntToBitArray(version, 3);
            cursor += 3;
            bArr.CopyTo(bit_array, cursor);

            cursor += 3;
            bArr = Util.ConvertIntToBitArray(msgType, 10);
            bArr.CopyTo(bit_array, cursor);

            cursor += 10;
            bArr = Util.ConvertIntToBitArray(msgLen, 32);
            bArr.CopyTo(bit_array, cursor);

            cursor += 32;
            bArr = Util.ConvertIntToBitArray(msgID, 32);
            bArr.CopyTo(bit_array, cursor);

            cursor += 32;

            if (VendorIdentifier != null)
            {
                try
                {
                    BitArray tempBitArr = Util.ConvertObjToBitArray(VendorIdentifier, VendorIdentifier_len);
                    tempBitArr.CopyTo(bit_array, cursor);
                    cursor += tempBitArr.Length;
                }
                catch { }
            }

            if (MessageSubtype != null)
            {
                try
                {
                    BitArray tempBitArr = Util.ConvertObjToBitArray(MessageSubtype, MessageSubtype_len);
                    tempBitArr.CopyTo(bit_array, cursor);
                    cursor += tempBitArr.Length;
                }
                catch { }
            }

            if (Data != null)
            {
                try
                {
                    BitArray tempBitArr = Util.ConvertObjToBitArray(Data, Data_len);
                    tempBitArr.CopyTo(bit_array, cursor);
                    cursor += tempBitArr.Length;
                }
                catch { }
            }

            UInt32 msg_len = (UInt32)cursor / 8;
            bArr = Util.ConvertIntToBitArray(msg_len, 32);
            bArr.CopyTo(bit_array, 16);

            bool[] boolArr = new bool[cursor];
            Array.Copy(bit_array, 0, boolArr, 0, cursor);

            return boolArr;
        }

        /// <summary>
        /// Decode bit array to a MSG_CUSTOM_MESSAGE
        /// </summary>
        /// <param name="bit_array">input bit array</param>
        /// <param name="cursor">pointer to current position</param>
        /// <param name="length">data length</param>
        /// <returns>MSG_CUSTOM_MESSAGE</returns>
        public new static MSG_CUSTOM_MESSAGE FromBitArray(ref BitArray bit_array, ref int cursor, int length)
        {
            if (cursor > length) return null;

            UInt16 loop_control_counter = 1;    //used for control choice element parsing loop
            int field_len = 0;
            int msg_type = 0;
            object obj_val;
            ArrayList param_list = new ArrayList();

            MSG_CUSTOM_MESSAGE obj = new MSG_CUSTOM_MESSAGE();

            cursor += 6;
            msg_type = (int)(UInt64)Util.CalculateVal(ref bit_array, ref cursor, 10);

            if (msg_type != obj.msgType)
            {
                cursor -= 16;
                return null;
            }

            obj.msgLen = (UInt32)(UInt64)Util.CalculateVal(ref bit_array, ref cursor, 32);
            obj.msgID = (UInt32)(UInt64)Util.CalculateVal(ref bit_array, ref cursor, 32);

            if (cursor > length) throw new Exception("Input data is not a complete LLRP message");

            field_len = 32;
            Util.ConvertBitArrayToObj(ref bit_array, ref cursor, out obj_val, typeof(UInt32), field_len);
            obj.VendorIdentifier = (UInt32)obj_val;

            if (cursor > length) throw new Exception("Input data is not a complete LLRP message");

            field_len = 8;
            Util.ConvertBitArrayToObj(ref bit_array, ref cursor, out obj_val, typeof(byte), field_len);
            obj.MessageSubtype = (byte)obj_val;

            if (cursor > length) throw new Exception("Input data is not a complete LLRP message");

            field_len = (bit_array.Length - cursor) / 8;
            Util.ConvertBitArrayToObj(ref bit_array, ref cursor, out obj_val, typeof(ByteArray), field_len);
            obj.Data = (ByteArray)obj_val;

            return obj;
        }
    }

    /// <summary>
    /// Class for dynamic load vendor extended classes
    /// </summary>
    public class CustomMsgDecodeFactory
    {
        public static Hashtable vendorExtensionIDTypeHash = null;
        public static Hashtable vendorExtensionNameTypeHash = null;
        public static Hashtable vendorExtensionAssemblyHash = null;

        /// <summary>
        /// Register vendor extension assembly
        /// </summary>
        /// <param name="asm"></param>
        public static void LoadVendorExtensionAssembly(Assembly asm)
        {
            if (null == vendorExtensionIDTypeHash)
            {
                vendorExtensionIDTypeHash = new Hashtable();
            }

            if (null == vendorExtensionNameTypeHash)
            {
                vendorExtensionNameTypeHash = new Hashtable();
            }

            if (null == vendorExtensionAssemblyHash)
            {
                vendorExtensionAssemblyHash = new Hashtable();
            }

            // Prevent double registration, and recursion
            string assembly_name = asm.GetName().Name;
            if (!vendorExtensionAssemblyHash.ContainsKey(assembly_name))
            {
                vendorExtensionAssemblyHash.Add(assembly_name, asm);
            }
            else
            {
                return;
            }

            try
            {
                Type[] types = asm.GetTypes();

                foreach (Type tp in types)
                {
                    if (tp.BaseType != typeof(MSG_CUSTOM_MESSAGE)) continue;

                    string type_full_name = tp.Namespace + "." + tp.Name;
                    object obj = asm.CreateInstance(type_full_name);

                    MSG_CUSTOM_MESSAGE temp_msg = (MSG_CUSTOM_MESSAGE)obj;
                    string key = temp_msg.VendorID + "-" + temp_msg.SubType;

                    if (!vendorExtensionIDTypeHash.ContainsKey(key))
                    {
                        vendorExtensionIDTypeHash.Add(key, tp);
                    }

                    if (!vendorExtensionNameTypeHash.ContainsKey(tp.Name))
                    {
                        vendorExtensionNameTypeHash.Add(tp.Name, tp);
                    }
                }
            }
            catch
            {
                Console.WriteLine("LVEA failed", asm);
            }
        }

        /// <summary>
        /// Create vendor extended paramters from BitArray
        /// </summary>
        /// <param name="bit_array">BitArray. Input</param>
        /// <param name="cursor">The current bit position to be processed.</param>
        /// <param name="length">Total length of the array</param>
        /// <returns></returns>
        public new static MSG_CUSTOM_MESSAGE DecodeCustomMessage(ref BitArray bit_array, ref int cursor, int length)
        {
            if (cursor >= length) return null;

            int old_cursor = cursor;

            MSG_CUSTOM_MESSAGE msg = MSG_CUSTOM_MESSAGE.FromBitArray(ref bit_array, ref cursor, length);
            if (null != msg)
            {
                string key = msg.VendorID + "-" + msg.SubType;
                if (null != vendorExtensionIDTypeHash)
                {
                    int new_cursor = cursor;

                    try
                    {
                        Type tp = (Type)vendorExtensionIDTypeHash[key];

                        MethodInfo mis = tp.GetMethod("FromBitArray");
                        if (null == mis) return null;

                        cursor = old_cursor;
                        object[] parameters = new object[] { bit_array, cursor, length };

                        object obj = mis.Invoke(null, parameters);
                        cursor = (int)parameters[1];

                        return (MSG_CUSTOM_MESSAGE)obj;
                    }
                    catch
                    {
                        cursor = new_cursor;
                    }
                }

                return msg;
            }

            return null;
        }

        /// <summary>
        /// Decode a general Xml node to vendor extended parameters.
        /// </summary>
        /// <param name="node">Xml node to be decoded.</param>
        /// <param name="xmlstr">Xml string to be decoded.</param>
        /// <returns>Custom Parameter</returns>
        public new static MSG_CUSTOM_MESSAGE DecodeXmlNodeToCustomMessage(XmlNode node, string xmlstr)
        {
            if (null != vendorExtensionNameTypeHash)
            {
                // Our hash is not namespace aware.
                string[] temp = node.Name.Split(new char[] {':'});
                string type_name = "MSG_" + temp[temp.Length - 1];

                try
                {
                    Type tp = (Type)vendorExtensionNameTypeHash[type_name];
                    if (null != tp)
                    {
                        MethodInfo mis = tp.GetMethod("FromString");
                        if (null == mis) return null;

                        object[] parameters = new object[] { xmlstr };
                        object obj = mis.Invoke(null, parameters);

                        return (MSG_CUSTOM_MESSAGE)obj;
                    }
                }
                catch { }
            }

            return null;
        }
    }
}

