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

/*
***************************************************************************
 * File Name:       CustomParameter.cs
 *
 * Author:          Impinj
 * Organization:    Impinj
 * Date:            September, 2007
 *
 * Description:     This file contains interfaces, base classes and parser
 *                  for custom parameters
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
    //LLRP Custom Parameters definitions
    public interface ICustom_Parameter : IParameter { }

    /// <summary>
    /// Custom Parameter Class
    /// </summary>
    public class PARAM_Custom : Parameter, ICustom_Parameter
    {
        protected UInt32 VendorIdentifier;
        protected UInt32 ParameterSubtype;
        protected ByteArray Data;

        private Int16 VendorIdentifier_len = 0;
        private Int16 ParameterSubtype_len = 0;
        private Int16 Data_len;

        public PARAM_Custom()
        {
            typeID = 1023;
        }

        /// <summary>
        /// Vendor ID
        /// </summary>
        public UInt32 VendorID
        {
            get { return VendorIdentifier; }
        }

        /// <summary>
        /// Sub type
        /// </summary>
        public UInt32 SubType
        {
            get { return ParameterSubtype; }
        }

        /// <summary>
        /// Convert to XML string
        /// </summary>
        /// <returns>XML string</returns>
        public override string ToString()
        {
            string xml_str = "<Custom>\r\n";
            xml_str += "  <VendorIdentifier>" + VendorIdentifier.ToString() + "</VendorIdentifier>\r\n";
            xml_str += "  <ParameterSubtype>" + ParameterSubtype.ToString() + "</ParameterSubtype>\r\n";
            xml_str += "  <Data>" + Data.ToHexString() + "</Data>\r\n";
            xml_str += "</Custom>\r\n";
            return xml_str;
        }

        /// <summary>
        /// Convert and copy to a exist to bit array. updates position indicator
        /// </summary>
        /// <param name="bit_array">bit array to be copied to</param>
        /// <param name="cursor">position to be updated</param>
        public override void ToBitArray(ref bool[] bit_array, ref int cursor)
        {
            int cursor_old = cursor;
            BitArray bArr;

            cursor += 6;
            bArr = Util.ConvertIntToBitArray(typeID, 10);
            bArr.CopyTo(bit_array, cursor);
            cursor += 26;

            try
            {
                BitArray tempBitArr = Util.ConvertObjToBitArray(VendorIdentifier, 32);
                tempBitArr.CopyTo(bit_array, cursor);
                cursor += tempBitArr.Length;
            }
            catch { }

            try
            {
                BitArray tempBitArr = Util.ConvertObjToBitArray(ParameterSubtype, 32);
                tempBitArr.CopyTo(bit_array, cursor);
                cursor += tempBitArr.Length;
            }
            catch { }

            try
            {
                BitArray tempBitArr = Util.ConvertObjToBitArray(Data, Data.Count * 8);
                tempBitArr.CopyTo(bit_array, cursor);
                cursor += tempBitArr.Length;
            }
            catch { }

            UInt32 param_len = (UInt32)(cursor - cursor_old) / 8;
            bArr = Util.ConvertIntToBitArray(param_len, 16);
            bArr.CopyTo(bit_array, cursor_old + 16);
        }

        /// <summary>
        /// Decode a BitArray to Custom Parameter
        /// </summary>
        /// <param name="bit_array">BitArray to be decoded.</param>
        /// <param name="cursor">Current bit position to be processed.</param>
        /// <param name="length">Total length of the BitArray.</param>
        /// <returns></returns>
        public new static PARAM_Custom FromBitArray(ref BitArray bit_array, ref int cursor, int length)
        {
            if (cursor >= length) return null;

            PARAM_Custom obj = new PARAM_Custom();
            object obj_val;
            int orig_cursor = cursor;
            int param_type = 0;

            cursor += 6;
            param_type = (int)(UInt64)Util.CalculateVal(ref bit_array, ref cursor, 10);
            if (param_type != obj.TypeID)
            {
                cursor = orig_cursor;
                return null;
            }

            obj.length = (UInt16)(int)Util.DetermineFieldLength(ref bit_array, ref cursor);

            if (cursor > length) throw new Exception("Input data is not complete message");

            Util.ConvertBitArrayToObj(ref bit_array, ref cursor, out obj_val, typeof(UInt32), 32);
            obj.VendorIdentifier = (UInt32)obj_val;

            if (cursor > length) throw new Exception("Input data is not complete message");

            Util.ConvertBitArrayToObj(ref bit_array, ref cursor, out obj_val, typeof(UInt32), 32);
            obj.ParameterSubtype = (UInt32)obj_val;

            if (cursor > length) throw new Exception("Input data is not complete message");

            int field_len = (obj.length * 8 - (cursor - orig_cursor)) / 8;
            Util.ConvertBitArrayToObj(ref bit_array, ref cursor, out obj_val, typeof(ByteArray), field_len);
            obj.Data = (ByteArray)obj_val;

            return obj;
        }

        /// <summary>
        /// Deserialize a XmlNode to custom parameter
        /// </summary>
        /// <param name="node">Xml node to be deserialized</param>
        /// <returns>Custom Parameter</returns>
        public static PARAM_Custom FromXmlNode(XmlNode node)
        {
            string val;
            PARAM_Custom param = new PARAM_Custom();


            val = XmlUtil.GetNodeValue(node, "VendorIdentifier");

            param.VendorIdentifier = Convert.ToUInt32(val);

            val = XmlUtil.GetNodeValue(node, "ParameterSubtype");

            param.ParameterSubtype = Convert.ToUInt32(val);

            val = XmlUtil.GetNodeValue(node, "Data");

            param.Data = (ByteArray)(Util.ParseArrayTypeFromString(val, "bytesToEnd", "Hex"));

            return param;
        }
    }

    /// <summary>
    /// Class for dynamic load vendor extended classes
    /// </summary>
    public class CustomParamDecodeFactory
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
                    if (tp.BaseType != typeof(PARAM_Custom)) continue;

                    string type_full_name = tp.Namespace + "." + tp.Name;
                    object obj = asm.CreateInstance(type_full_name);

                    PARAM_Custom temp_param = (PARAM_Custom)obj;
                    string key = temp_param.VendorID + "-" + temp_param.SubType;
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
        public static ICustom_Parameter DecodeCustomParameter(ref BitArray bit_array, ref int cursor, int length)
        {
            if (cursor >= length) return null;

            int old_cursor = cursor;
            PARAM_Custom param = PARAM_Custom.FromBitArray(ref bit_array, ref cursor, length);
            if (param != null)
            {
                string key = param.VendorID + "-" + param.SubType;
                if (vendorExtensionIDTypeHash != null )
                {
                    int new_cursor = cursor;

                    try
                    {
                        Type tp = (Type)vendorExtensionIDTypeHash[key];

                        MethodInfo mis = tp.GetMethod("FromBitArray");
                        if (mis == null) return null;

                        cursor = old_cursor;
                        object[] parameters = new object[] { bit_array, cursor, length };

                        object obj = mis.Invoke(null, parameters);
                        cursor = (int)parameters[1];

                        return (ICustom_Parameter)obj;
                    }
                    catch
                    {
                        cursor = new_cursor;
                    }
                }

                return param;
            }

            return null;
        }

        /// <summary>
        /// Decode a general Xml node to vendor extended parameters.
        /// </summary>
        /// <param name="node">Xml node to be decoded.</param>
        /// <returns>Custom Parameter</returns>
        public static ICustom_Parameter DecodeXmlNodeToCustomParameter(XmlNode node)
        {
            // Our hash is not namespace aware.
            string[] temp = node.Name.Split(new char[] {':'});
            string type_name = "PARAM_" + temp[temp.Length - 1];

            if (type_name == "PARAM_Custom")
            {
                return (ICustom_Parameter)PARAM_Custom.FromXmlNode(node);
            }
            else if (null != vendorExtensionNameTypeHash)
            {
                try
                {
                    Type tp = (Type)vendorExtensionNameTypeHash[type_name];
                    if (tp != null)
                    {
                        MethodInfo mis = tp.GetMethod("FromXmlNode");
                        if (mis == null) return null;

                        object[] parameters = new object[] { node };
                        object obj = mis.Invoke(null, parameters);

                        return (ICustom_Parameter)obj;
                    }
                }
                catch { }
            }

            return null;
        }
    }
}
