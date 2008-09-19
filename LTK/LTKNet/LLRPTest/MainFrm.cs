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


using System;
using System.Collections;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Runtime.Serialization;
using System.Runtime.Serialization.Formatters.Binary;
using System.Runtime.Serialization.Formatters.Soap;
using System.Runtime.InteropServices;
using System.Threading;

using System.IO;

using Org.LLRP.LTK.LLRPV1;
using LTKD = Org.LLRP.LTK.LLRPV1.DataType;

namespace Org.LLRP.LTK.LLRPV1Test
{

    public partial class MainFrm : Form
    {
        //Create an instance of LLRP reader client.
        LLRPClient reader = new LLRPClient();

        //To accomodate all the all the messages.
        MSG_ERROR_MESSAGE msg_err;

        //
        ulong old_time = 0;

        //

        ArrayList xmlarr = new ArrayList();
        //int xmlarr_index = 0;

        public MainFrm()
        {
            InitializeComponent();
        }

        private void btnOpen_Click(object sender, EventArgs e)
        {
            if (btnOpen.Text == "Open")
            {
                ENUM_ConnectionAttemptStatusType status;
                bool ret = reader.Open(textBox1.Text, 5000, out status);

                if (!ret || status != ENUM_ConnectionAttemptStatusType.Success) return;

                textBox2.Text = status.ToString();

                //subscribe to reader event notification and ro access report
                reader.OnReaderEventNotification += new delegateReaderEventNotification(reader_OnReaderEventNotification);
                reader.OnRoAccessReportReceived += new delegateRoAccessReport(reader_OnRoAccessReportReceived);

                btnOpen.Text = "Close";
            }
            else
            {
                //clean up subscriptions.
                reader.OnReaderEventNotification -= new delegateReaderEventNotification(reader_OnReaderEventNotification);
                reader.OnRoAccessReportReceived -= new delegateRoAccessReport(reader_OnRoAccessReportReceived);

                reader.Close();

                btnOpen.Text = "Open";
            }
        }

        private void UpdateROReport(MSG_RO_ACCESS_REPORT msg)
        {
            if (msg.TagReportData == null) return;

            ulong ms = msg.TagReportData[0].FirstSeenTimestampUTC.Microseconds - old_time;
           
            old_time = msg.TagReportData[0].FirstSeenTimestampUTC.Microseconds;

            if (ms <= 0) ms = 1;

            label1.Text = (60000000 / ms).ToString() + "Tags/Min";

            try
            {
                for (int i = 0; i < msg.TagReportData.Length; i++)
                {
                    if (msg.TagReportData[i].EPCParameter.Count > 0)
                    {
                        string epc;
                        // reports come in two flavors.  Get the right flavor
                        if (msg.TagReportData[i].EPCParameter[0].GetType() == typeof(PARAM_EPC_96))
                        {
                            epc = ((PARAM_EPC_96)(msg.TagReportData[i].EPCParameter[0])).EPC.ToHexString();
                        }
                        else
                        {
                            epc = ((PARAM_EPCData)(msg.TagReportData[i].EPCParameter[0])).EPC.ToHexString();

                        }
                        if (!listBox1.Items.Contains(epc))
                        {
                            try
                            {
                                listBox1.Items.Add(epc);
                                label2.Text = "Tags : " + listBox1.Items.Count.ToString();
                            }
                            catch { }
                        }
                    }
                }
                try
                {
                    textBox2.Text = msg.ToString();
                }
                catch (Exception e)
                {

                    textBox2.Text = e.Message;
                }
            }
            catch (Exception ex)
            {
                textBox2.Text = ex.Message;
            }
        }

        private void UpdateReaderEvent(MSG_READER_EVENT_NOTIFICATION msg)
        {
            try
            {
                textBox2.Text = msg.ToString();
            }
            catch (Exception e)
            {

                textBox2.Text = e.Message;
            }
        }

        void reader_OnRoAccessReportReceived(MSG_RO_ACCESS_REPORT msg)
        {
            delegateRoAccessReport del = new delegateRoAccessReport(UpdateROReport);
            this.Invoke(del, msg);
        }

        void reader_OnReaderEventNotification(MSG_READER_EVENT_NOTIFICATION msg)
        {
            delegateReaderEventNotification del = new delegateReaderEventNotification(UpdateReaderEvent);
            this.Invoke(del, msg);
        }

        private void btnExecute_Click(object sender, EventArgs e)
        {
            listBox1.Items.Clear();
            textBox2.Text = "";

            switch (comboBox1.Text)
            {
                case "ADD_ROSPEC":
                    Add_RoSpec();
                    break;
                case "DELETE_ROSPEC":
                    Delete_RoSpec();
                    break;
                case "ENABLE_ROSPEC":
                    Enable_RoSpec();
                    break;
                case "START_ROSPEC":
                    Start_RoSpec();
                    break;
                case "STOP_ROSPEC":
                    Stop_RoSpec();
                    break;
                case "GET_READER_CONFIG":
                    Get_Reader_Config();
                    break;
                case "GET_READER_CAPABILITY":
                    Get_Reader_Capability();
                    break;
                case "GET_ROSPEC":
                    Get_RoSpec();
                    break;
                case "SET_READER_CONFIG":
                    Set_Reader_Config();
                    break;
                case "ADD_ACCESSSPEC":
                    ADD_ACCESSSPEC();
                    break;
                case "DELETE_ACCESSSPEC":
                    DELETE_ACCESSSPEC();
                    break;
                case "ENABLE_ACCESSSPEC":
                    ENABLE_ACCESSSPEC();
                    break;
                case "DISABLE_ACCESSSPEC":
                    DISABLE_ACCESSPEC();
                    break;
                case "GET_ACCESSSPEC":
                    GET_ACCESSSPEC();
                    break;
                default:
                    break;
            }

        }

        private void ADD_ACCESSSPEC()
        {
            MSG_ADD_ACCESSSPEC msg = new MSG_ADD_ACCESSSPEC();
            msg.AccessSpec = new PARAM_AccessSpec();

            msg.AccessSpec.AccessSpecID = 1001;
            msg.AccessSpec.AntennaID = 1;
            msg.AccessSpec.ProtocolID = ENUM_AirProtocols.EPCGlobalClass1Gen2;
            msg.AccessSpec.CurrentState = ENUM_AccessSpecState.Disabled;
            msg.AccessSpec.ROSpecID = 123;

            //define trigger
            msg.AccessSpec.AccessSpecStopTrigger = new PARAM_AccessSpecStopTrigger();
            msg.AccessSpec.AccessSpecStopTrigger.AccessSpecStopTrigger = ENUM_AccessSpecStopTriggerType.Null;
            msg.AccessSpec.AccessSpecStopTrigger.OperationCountValue = 3;

            //define access command

            //define air protocol spec
            msg.AccessSpec.AccessCommand = new PARAM_AccessCommand();
            msg.AccessSpec.AccessCommand.AirProtocolTagSpec = new UNION_AirProtocolTagSpec();

            PARAM_C1G2TagSpec tagSpec = new PARAM_C1G2TagSpec();
            tagSpec.C1G2TargetTag = new PARAM_C1G2TargetTag[1];
            tagSpec.C1G2TargetTag[0] = new PARAM_C1G2TargetTag();
            tagSpec.C1G2TargetTag[0].Match = true; //change to "true" if you want to the following parameters take effect.
            tagSpec.C1G2TargetTag[0].MB = new LTKD.TwoBits(1);
            tagSpec.C1G2TargetTag[0].Pointer = 0x20;
            tagSpec.C1G2TargetTag[0].TagData = LTKD.LLRPBitArray.FromString("6666");
            tagSpec.C1G2TargetTag[0].TagMask = LTKD.LLRPBitArray.FromBinString("1111111111111111");

            msg.AccessSpec.AccessCommand.AirProtocolTagSpec.Add(tagSpec);

            //define access spec
            msg.AccessSpec.AccessCommand.AccessCommandOpSpec = new UNION_AccessCommandOpSpec();

            PARAM_C1G2Write wr = new PARAM_C1G2Write();
            wr.AccessPassword = 0;
            wr.MB = new LTKD.TwoBits(1);
            wr.OpSpecID = 111;
            wr.WordPointer = 2;
            //Data to be written.
            wr.WriteData = LTKD.UInt16Array.FromString("EEEE11112222333344445555");

            msg.AccessSpec.AccessCommand.AccessCommandOpSpec.Add(wr);

            msg.AccessSpec.AccessReportSpec = new PARAM_AccessReportSpec();
            msg.AccessSpec.AccessReportSpec.AccessReportTrigger = ENUM_AccessReportTriggerType.End_Of_AccessSpec;

            MSG_ADD_ACCESSSPEC_RESPONSE rsp = reader.ADD_ACCESSSPEC(msg, out msg_err, 3000);
            if (rsp != null)
            {
                textBox2.Text = rsp.ToString();
            }
            else if (msg_err != null)
            {
                textBox2.Text = msg_err.ToString();
            }
            else
                textBox2.Text = "Command time out!";
        }

        private void DELETE_ACCESSSPEC()
        {
            MSG_DELETE_ACCESSSPEC msg = new MSG_DELETE_ACCESSSPEC();
            msg.AccessSpecID = 1001;

            MSG_DELETE_ACCESSSPEC_RESPONSE rsp = reader.DELETE_ACCESSSPEC(msg, out msg_err, 3000);
            if (rsp != null)
            {
                textBox2.Text = rsp.ToString();
            }
            else if (msg_err != null)
            {
                textBox2.Text = msg_err.ToString();
            }
            else
                textBox2.Text = "Command time out!";

        }

        private void ENABLE_ACCESSSPEC()
        {
            MSG_ENABLE_ACCESSSPEC msg = new MSG_ENABLE_ACCESSSPEC();
            msg.AccessSpecID = 1001;

            MSG_ENABLE_ACCESSSPEC_RESPONSE rsp = reader.ENABLE_ACCESSSPEC(msg, out msg_err, 3000);

            if (rsp != null)
            {
                textBox2.Text = rsp.ToString();
            }
            else if (msg_err != null)
            {
                textBox2.Text = msg_err.ToString();
            }
            else
                textBox2.Text = "Command time out!";
        }

        private void DISABLE_ACCESSPEC()
        {
            MSG_DISABLE_ACCESSSPEC msg = new MSG_DISABLE_ACCESSSPEC();
            msg.AccessSpecID = 1001;

            MSG_DISABLE_ACCESSSPEC_RESPONSE rsp = reader.DISABLE_ACCESSSPEC(msg, out msg_err, 3000);
            if (rsp != null)
            {
                textBox2.Text = rsp.ToString();
            }
            else if (msg_err != null)
            {
                textBox2.Text = msg_err.ToString();
            }
            else
                textBox2.Text = "Command time out!";
        }

        private void GET_ACCESSSPEC()
        {
            MSG_GET_ACCESSSPECS msg = new MSG_GET_ACCESSSPECS();
            MSG_GET_ACCESSSPECS_RESPONSE rsp = reader.GET_ACCESSSPECS(msg, out msg_err, 3000);

            if (rsp != null)
            {
                textBox2.Text = rsp.ToString();
            }
            else if (msg_err != null)
            {
                textBox2.Text = msg_err.ToString();
            }
            else
                textBox2.Text = "Command time out!";
        }

        private void Get_Reader_Config()
        {
            MSG_GET_READER_CONFIG msg = new MSG_GET_READER_CONFIG();
            msg.AntennaID = 1;
            msg.GPIPortNum = 0;
            MSG_GET_READER_CONFIG_RESPONSE rsp = reader.GET_READER_CONFIG(msg, out msg_err, 3000);

            //rsp.
            if (rsp != null)
            {
                textBox2.Text = rsp.ToString();
            }
            else if (msg_err != null)
            {
                textBox2.Text = msg_err.ToString();
            }
            else
                textBox2.Text = "Command time out!";

        }

        private void Set_Reader_Config()
        {
            MSG_SET_READER_CONFIG msg = new MSG_SET_READER_CONFIG();
            msg.AccessReportSpec = new PARAM_AccessReportSpec();
            msg.AccessReportSpec.AccessReportTrigger = ENUM_AccessReportTriggerType.End_Of_AccessSpec;

            msg.AntennaConfiguration = new PARAM_AntennaConfiguration[1];
            msg.AntennaConfiguration[0] = new PARAM_AntennaConfiguration();
            msg.AntennaConfiguration[0].AirProtocolInventoryCommandSettings = new UNION_AirProtocolInventoryCommandSettings();

            PARAM_C1G2InventoryCommand cmd = new PARAM_C1G2InventoryCommand();
            cmd.C1G2RFControl = new PARAM_C1G2RFControl();
            cmd.C1G2RFControl.ModeIndex = 2;
            cmd.C1G2RFControl.Tari = 0;
            cmd.C1G2SingulationControl = new PARAM_C1G2SingulationControl();
            cmd.C1G2SingulationControl.Session = new LTKD.TwoBits(1);
            cmd.C1G2SingulationControl.TagPopulation = 0;
            cmd.C1G2SingulationControl.TagTransitTime = 1000;
            cmd.TagInventoryStateAware = false;

            msg.AntennaConfiguration[0].AirProtocolInventoryCommandSettings.Add(cmd);
            msg.AntennaConfiguration[0].AntennaID = 0;


            msg.AntennaConfiguration[0].RFReceiver = new PARAM_RFReceiver();
            msg.AntennaConfiguration[0].RFReceiver.ReceiverSensitivity = 12;

            msg.AntennaConfiguration[0].RFTransmitter = new PARAM_RFTransmitter();
            msg.AntennaConfiguration[0].RFTransmitter.ChannelIndex = 1;
            msg.AntennaConfiguration[0].RFTransmitter.HopTableID = 1;
            msg.AntennaConfiguration[0].RFTransmitter.TransmitPower = 61;

            //msg.AntennaProperties = new PARAM_AntennaProperties[1];
            //msg.AntennaProperties[0] = new PARAM_AntennaProperties();
            //msg.AntennaProperties[0].AntennaConnected = true;
            //msg.AntennaProperties[0].AntennaGain = 0;
            //msg.AntennaProperties[0].AntennaID = 1;

            msg.EventsAndReports = new PARAM_EventsAndReports();
            msg.EventsAndReports.HoldEventsAndReportsUponReconnect = false;

            msg.KeepaliveSpec = new PARAM_KeepaliveSpec();
            msg.KeepaliveSpec.KeepaliveTriggerType = ENUM_KeepaliveTriggerType.Null;
            msg.KeepaliveSpec.PeriodicTriggerValue = 0;

            msg.ReaderEventNotificationSpec = new PARAM_ReaderEventNotificationSpec();
            msg.ReaderEventNotificationSpec.EventNotificationState = new PARAM_EventNotificationState[5];
            msg.ReaderEventNotificationSpec.EventNotificationState[0] = new PARAM_EventNotificationState();
            msg.ReaderEventNotificationSpec.EventNotificationState[0].EventType = ENUM_NotificationEventType.AISpec_Event;
            msg.ReaderEventNotificationSpec.EventNotificationState[0].NotificationState = true;

            msg.ReaderEventNotificationSpec.EventNotificationState[1] = new PARAM_EventNotificationState();
            msg.ReaderEventNotificationSpec.EventNotificationState[1].EventType = ENUM_NotificationEventType.Antenna_Event;
            msg.ReaderEventNotificationSpec.EventNotificationState[1].NotificationState = true;

            msg.ReaderEventNotificationSpec.EventNotificationState[2] = new PARAM_EventNotificationState();
            msg.ReaderEventNotificationSpec.EventNotificationState[2].EventType = ENUM_NotificationEventType.GPI_Event;
            msg.ReaderEventNotificationSpec.EventNotificationState[2].NotificationState = true;

            msg.ReaderEventNotificationSpec.EventNotificationState[3] = new PARAM_EventNotificationState();
            msg.ReaderEventNotificationSpec.EventNotificationState[3].EventType = ENUM_NotificationEventType.Reader_Exception_Event;
            msg.ReaderEventNotificationSpec.EventNotificationState[3].NotificationState = true;

            msg.ReaderEventNotificationSpec.EventNotificationState[4] = new PARAM_EventNotificationState();
            msg.ReaderEventNotificationSpec.EventNotificationState[4].EventType = ENUM_NotificationEventType.RFSurvey_Event;
            msg.ReaderEventNotificationSpec.EventNotificationState[4].NotificationState = true;

            msg.ROReportSpec = new PARAM_ROReportSpec();
            msg.ROReportSpec.N = 1;
            msg.ROReportSpec.ROReportTrigger = ENUM_ROReportTriggerType.Upon_N_Tags_Or_End_Of_ROSpec;
            msg.ROReportSpec.TagReportContentSelector = new PARAM_TagReportContentSelector();
            msg.ROReportSpec.TagReportContentSelector.AirProtocolEPCMemorySelector = new UNION_AirProtocolEPCMemorySelector();
            PARAM_C1G2EPCMemorySelector c1g2mem = new PARAM_C1G2EPCMemorySelector();
            c1g2mem.EnableCRC = true;
            c1g2mem.EnablePCBits = true;
            msg.ROReportSpec.TagReportContentSelector.AirProtocolEPCMemorySelector.Add(c1g2mem);

            msg.ROReportSpec.TagReportContentSelector.EnableAccessSpecID = true;
            msg.ROReportSpec.TagReportContentSelector.EnableAntennaID = true;
            msg.ROReportSpec.TagReportContentSelector.EnableChannelIndex = true;
            msg.ROReportSpec.TagReportContentSelector.EnableFirstSeenTimestamp = true;
            msg.ROReportSpec.TagReportContentSelector.EnableInventoryParameterSpecID = true;
            msg.ROReportSpec.TagReportContentSelector.EnableLastSeenTimestamp = true;
            msg.ROReportSpec.TagReportContentSelector.EnablePeakRSSI = true;
            msg.ROReportSpec.TagReportContentSelector.EnableROSpecID = false;
            msg.ROReportSpec.TagReportContentSelector.EnableSpecIndex = true;
            msg.ROReportSpec.TagReportContentSelector.EnableTagSeenCount = true;

            msg.ResetToFactoryDefault = false;

            MSG_SET_READER_CONFIG_RESPONSE rsp = reader.SET_READER_CONFIG(msg, out msg_err, 3000);

            if (rsp != null)
            {
                textBox2.Text = rsp.ToString();
            }
            else if (msg_err != null)
            {
                textBox2.Text = rsp.ToString();
            }
            else
                textBox2.Text = "Commmand time out!";

        }

        private void Get_Reader_Capability()
        {
            MSG_GET_READER_CAPABILITIES msg = new MSG_GET_READER_CAPABILITIES();

            MSG_GET_READER_CAPABILITIES_RESPONSE rsp = reader.GET_READER_CAPABILITIES(msg, out msg_err, 3000);
            if (rsp != null)
            {
                textBox2.Text = rsp.ToString();
            }
            else if (msg_err != null)
            {
                textBox2.Text = msg_err.ToString();
            }
            else
                textBox2.Text = "Command time out!";
        }

        private void Get_RoSpec()
        {
            MSG_GET_ROSPECS msg = new MSG_GET_ROSPECS();
            MSG_GET_ROSPECS_RESPONSE rsp = reader.GET_ROSPECS(msg, out msg_err, 3000);

            
            if (rsp != null)
            {
                textBox2.Text = rsp.ToString();
            }
            else if (msg_err != null)
            {
                textBox2.Text = msg_err.ToString();
            }
            else
                textBox2.Text = "Command time out!";

        }


        private void Add_RoSpec()
        {

            MSG_ADD_ROSPEC msg = new MSG_ADD_ROSPEC();
            msg.ROSpec = new PARAM_ROSpec();
            msg.ROSpec.CurrentState = ENUM_ROSpecState.Disabled;
            msg.ROSpec.Priority = 0x00;
            msg.ROSpec.ROSpecID = 123;


            msg.ROSpec.ROBoundarySpec = new PARAM_ROBoundarySpec();
            msg.ROSpec.ROBoundarySpec.ROSpecStartTrigger = new PARAM_ROSpecStartTrigger();
            msg.ROSpec.ROBoundarySpec.ROSpecStartTrigger.ROSpecStartTriggerType = ENUM_ROSpecStartTriggerType.Null;

            msg.ROSpec.ROBoundarySpec.ROSpecStopTrigger = new PARAM_ROSpecStopTrigger();
            msg.ROSpec.ROBoundarySpec.ROSpecStopTrigger.ROSpecStopTriggerType = ENUM_ROSpecStopTriggerType.Duration;
            msg.ROSpec.ROBoundarySpec.ROSpecStopTrigger.DurationTriggerValue = 1000;

            msg.ROSpec.ROReportSpec = new PARAM_ROReportSpec();
            msg.ROSpec.ROReportSpec.ROReportTrigger = ENUM_ROReportTriggerType.Upon_N_Tags_Or_End_Of_ROSpec;
            msg.ROSpec.ROReportSpec.N = 0;


            msg.ROSpec.ROReportSpec.TagReportContentSelector = new PARAM_TagReportContentSelector();
            msg.ROSpec.ROReportSpec.TagReportContentSelector.EnableAccessSpecID = true;
            msg.ROSpec.ROReportSpec.TagReportContentSelector.EnableAntennaID = true;
            msg.ROSpec.ROReportSpec.TagReportContentSelector.EnableChannelIndex = true;
            msg.ROSpec.ROReportSpec.TagReportContentSelector.EnableFirstSeenTimestamp = true;
            msg.ROSpec.ROReportSpec.TagReportContentSelector.EnableInventoryParameterSpecID = true;
            msg.ROSpec.ROReportSpec.TagReportContentSelector.EnableLastSeenTimestamp = true;
            msg.ROSpec.ROReportSpec.TagReportContentSelector.EnablePeakRSSI = true;
            msg.ROSpec.ROReportSpec.TagReportContentSelector.EnableROSpecID = true;
            msg.ROSpec.ROReportSpec.TagReportContentSelector.EnableSpecIndex = true;
            msg.ROSpec.ROReportSpec.TagReportContentSelector.EnableTagSeenCount = true;

            msg.ROSpec.SpecParameter = new UNION_SpecParameter();
            PARAM_AISpec aiSpec = new PARAM_AISpec();

            aiSpec.AntennaIDs = new LTKD.UInt16Array();
            aiSpec.AntennaIDs.Add(0);       //0 :  applys to all antennae, 
            //aiSpec.AntennaIDs.Add(1);
            //aiSpec.AntennaIDs.Add(2);     ...

            aiSpec.AISpecStopTrigger = new PARAM_AISpecStopTrigger();
            aiSpec.AISpecStopTrigger.AISpecStopTriggerType = ENUM_AISpecStopTriggerType.Duration;
            aiSpec.AISpecStopTrigger.DurationTrigger = 1000;

            aiSpec.InventoryParameterSpec = new PARAM_InventoryParameterSpec[1];
            aiSpec.InventoryParameterSpec[0] = new PARAM_InventoryParameterSpec();
            aiSpec.InventoryParameterSpec[0].InventoryParameterSpecID = 1234;
            aiSpec.InventoryParameterSpec[0].ProtocolID = ENUM_AirProtocols.EPCGlobalClass1Gen2;

            msg.ROSpec.SpecParameter.Add(aiSpec);

            MSG_ADD_ROSPEC_RESPONSE rsp = reader.ADD_ROSPEC(msg, out msg_err, 3000);
            if (rsp != null)
            {
                textBox2.Text = rsp.ToString();
            }
            else if (msg_err != null)
            {
                textBox2.Text = msg_err.ToString();
            }
            else
                textBox2.Text = "Command time out!";

        }

        private void Delete_RoSpec()
        {
            MSG_DELETE_ROSPEC msg = new MSG_DELETE_ROSPEC();
            msg.ROSpecID = 0;

            MSG_DELETE_ROSPEC_RESPONSE rsp = reader.DELETE_ROSPEC(msg, out msg_err, 3000);
            if (rsp != null)
            {
                textBox2.Text = rsp.ToString();
            }
            else if (msg_err != null)
            {
                textBox2.Text = msg_err.ToString();
            }
            else
                textBox2.Text = "Command time out!";
        }

        private void Enable_RoSpec()
        {
            MSG_ENABLE_ROSPEC msg = new MSG_ENABLE_ROSPEC();
            msg.ROSpecID = 123;
            MSG_ENABLE_ROSPEC_RESPONSE rsp = reader.ENABLE_ROSPEC(msg, out msg_err, 3000);
            if (rsp != null)
            {
                textBox2.Text = rsp.ToString();
            }
            else if (msg_err != null)
            {
                textBox2.Text = msg_err.ToString();
            }
            else
                textBox2.Text = "Command time out!";

        }

        private void Start_RoSpec()
        {
            MSG_START_ROSPEC msg = new MSG_START_ROSPEC();
            msg.ROSpecID = 123;
            MSG_START_ROSPEC_RESPONSE rsp = reader.START_ROSPEC(msg, out msg_err, 3000);
            if (rsp != null)
            {
                textBox2.Text = rsp.ToString();
            }
            else if (msg_err != null)
            {
                textBox2.Text = msg_err.ToString();
            }
            else
                textBox2.Text = "Command time out!";

        }

        private void Stop_RoSpec()
        {
            MSG_STOP_ROSPEC msg = new MSG_STOP_ROSPEC();
            msg.ROSpecID = 123;

            MSG_STOP_ROSPEC_RESPONSE rsp = reader.STOP_ROSPEC(msg, out msg_err, 3000);

            if (rsp != null)
            {
                textBox2.Text = rsp.ToString();
            }
            else if (msg_err != null)
            {
                textBox2.Text = msg_err.ToString();
            }
            else
                textBox2.Text = "Command time out!";
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            bool[] sb = new bool[] { true, true, false, false };

            bool[] db = new bool[200];

            sb.CopyTo(db, 0);

        }

        private void button1_Click(object sender, EventArgs e)
        {
            OpenFileDialog ofd = new OpenFileDialog();
            ofd.Filter = "XML File(*.xml)|*.xml|All File(*.*)|*.*";
            ofd.DefaultExt = "xml";
            ofd.RestoreDirectory = true;

              
            if (ofd.ShowDialog() == DialogResult.OK)
            {
                if (ofd.FileName.Contains(".xml"))
                {
                    FileStream fs = new FileStream(ofd.FileName, FileMode.Open);

                    StreamReader sr = new StreamReader(fs);

                    string s = sr.ReadToEnd();
                    fs.Close();

                    LTKD.Message obj;
                    ENUM_LLRP_MSG_TYPE msg_type;

                    try
                    {
                        LLRPXmlParser.ParseXMLToLLRPMessage(s, out obj, out msg_type);
                        if (obj != null) textBox2.Text = obj.ToString();
                        else
                            textBox2.Text = "The input xml is not a valid LLRP message.";
                    }
                    catch { textBox2.Text = "The input xml is not a valid LLRP message."; }
                }
                else
                {


                }
            }
        }

        private void comboBox1_SelectedIndexChanged(object sender, EventArgs e)
        {

        }

        private void btnNext_Click(object sender, EventArgs e)
        {

 

            return;


            //object obj;
            //ENUM_LLRP_MSG_TYPE msg_type;

            //string s = (string)xmlarr[xmlarr_index];

            //try
            //{
            //    LLRPXmlParser.ParseXMLToLLRPMessage(s, out obj, out msg_type);
            //    if (obj != null) textBox2.Text = obj.ToString();
            //    else
            //        MessageBox.Show("The input xml is not a valid LLRP message");
            //}
            //catch { MessageBox.Show("The input xml is not a valid LLRP message"); }


            //xmlarr_index++;

            //Thread.Sleep(200);

            //if (xmlarr_index == xmlarr.Count)
            //{
            //    btnNext.Enabled = false;
            //    xmlarr_index = 0;
            //    xmlarr = null;
            //}

        }
    }

}
