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

using LLRP;
using LLRP.DataType;

namespace LLRPTest
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
                MSG_CLOSE_CONNECTION msg = new MSG_CLOSE_CONNECTION();
                MSG_CLOSE_CONNECTION_RESPONSE rsp = reader.CLOSE_CONNECTION(msg, out msg_err, 3000);
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
                

                //clean up subscrptions.
                reader.OnReaderEventNotification -= new delegateReaderEventNotification(reader_OnReaderEventNotification);
                reader.OnRoAccessReportReceived -= new delegateRoAccessReport(reader_OnRoAccessReportReceived);

                reader.Close();

                btnOpen.Text = "Open";
            }
        }

        private void UpdateROReport(MSG_RO_ACCESS_REPORT msg)
        {
            if (msg.__TagReportData == null) return;

            ulong ms = msg.__TagReportData[0].__FirstSeenTimestampUTC.__Microseconds - old_time;
           
            old_time = msg.__TagReportData[0].__FirstSeenTimestampUTC.__Microseconds;

            if (ms <= 0) ms = 1;

            label1.Text = (60000000 / ms).ToString() + "Tags/Min";

            string epc = ((PARAM_EPC_96)(msg.__TagReportData[0].__EPCParameter[0])).__EPC.ToHexString();
            if (!listBox1.Items.Contains(epc))
            {
                listBox1.Items.Add(epc);
                label2.Text = "Tags : " + listBox1.Items.Count.ToString();
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
            msg.__AccessSpec = new PARAM_AccessSpec();

            msg.__AccessSpec.__AccessSpecID = 1001;
            msg.__AccessSpec.__AntennaID = 1;
            msg.__AccessSpec.__ProtocolID = ENUM_AirProtocols.EPCGlobalClass1Gen2;
            msg.__AccessSpec.__CurrentState = ENUM_AccessSpecState.Disabled;
            msg.__AccessSpec.__ROSpecID = 123;

            //define trigger
            msg.__AccessSpec.__AccessSpecStopTrigger = new PARAM_AccessSpecStopTrigger();
            msg.__AccessSpec.__AccessSpecStopTrigger.__AccessSpecStopTrigger = ENUM_AccessSpecStopTriggerType.Null;
            msg.__AccessSpec.__AccessSpecStopTrigger.__OperationCountValue = 3;

            //define access command

            //define air protocol spec
            msg.__AccessSpec.__AccessCommand = new PARAM_AccessCommand();
            msg.__AccessSpec.__AccessCommand.__AirProtocolTagSpec = new UNION_AirProtocolTagSpec();

            PARAM_C1G2TagSpec tagSpec = new PARAM_C1G2TagSpec();
            tagSpec.__C1G2TargetTag = new PARAM_C1G2TargetTag[1];
            tagSpec.__C1G2TargetTag[0] = new PARAM_C1G2TargetTag();
            tagSpec.__C1G2TargetTag[0].__Match = true; //change to "true" if you want to the following parameters take effect.
            tagSpec.__C1G2TargetTag[0].__MB = new TwoBits(1);
            tagSpec.__C1G2TargetTag[0].__Pointer = 0x20;
            tagSpec.__C1G2TargetTag[0].__TagData = LLRPBitArray.FromString("6666");
            tagSpec.__C1G2TargetTag[0].__TagMask = LLRPBitArray.FromBinString("1111111111111111");

            msg.__AccessSpec.__AccessCommand.__AirProtocolTagSpec.Add(tagSpec);

            //define access spec
            msg.__AccessSpec.__AccessCommand.__AccessCommandOpSpec = new UNION_AccessCommandOpSpec();

            PARAM_C1G2Write wr = new PARAM_C1G2Write();
            wr.__AccessPassword = 0;
            wr.__MB = new TwoBits(1);
            wr.__OpSpecID = 111;
            wr.__WordPointer = 2;
            //Data to be written.
            wr.__WriteData = UInt16Array.FromString("EEEE11112222333344445555");

            msg.__AccessSpec.__AccessCommand.__AccessCommandOpSpec.Add(wr);

            msg.__AccessSpec.__AccessReportSpec = new PARAM_AccessReportSpec();
            msg.__AccessSpec.__AccessReportSpec.__AccessReportTrigger = ENUM_AccessReportTriggerType.End_Of_AccessSpec;

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
            msg.__AccessSpecID = 1001;

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
            msg.__AccessSpecID = 1001;

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
            msg.__AccessSpecID = 1001;

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
            msg.__AntennaID = 1;
            msg.__GPIPortNum = 0;
            MSG_GET_READER_CONFIG_RESPONSE rsp = reader.GET_READER_CONFIG(msg, out msg_err, 3000);

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
            msg.__AccessReportSpec = new PARAM_AccessReportSpec();
            msg.__AccessReportSpec.__AccessReportTrigger = ENUM_AccessReportTriggerType.End_Of_AccessSpec;

            msg.__AntennaConfiguration = new PARAM_AntennaConfiguration[1];
            msg.__AntennaConfiguration[0] = new PARAM_AntennaConfiguration();
            msg.__AntennaConfiguration[0].__AirProtocolInventoryCommandSettings = new UNION_AirProtocolInventoryCommandSettings();

            PARAM_C1G2InventoryCommand cmd = new PARAM_C1G2InventoryCommand();
            cmd.__C1G2RFControl = new PARAM_C1G2RFControl();
            cmd.__C1G2RFControl.__ModeIndex = 2;
            cmd.__C1G2RFControl.__Tari = 0;
            cmd.__C1G2SingulationControl = new PARAM_C1G2SingulationControl();
            cmd.__C1G2SingulationControl.__Session = new TwoBits(2);
            cmd.__C1G2SingulationControl.__TagPopulation = 0;
            cmd.__C1G2SingulationControl.__TagTransitTime = 1000;
            cmd.__TagInventoryStateAware = false;

            msg.__AntennaConfiguration[0].__AirProtocolInventoryCommandSettings.Add(cmd);
            msg.__AntennaConfiguration[0].__AntennaID = 1;


            msg.__AntennaConfiguration[0].__RFReceiver = new PARAM_RFReceiver();
            msg.__AntennaConfiguration[0].__RFReceiver.__ReceiverSensitivity = 1;

            msg.__AntennaConfiguration[0].__RFTransmitter = new PARAM_RFTransmitter();
            msg.__AntennaConfiguration[0].__RFTransmitter.__ChannelIndex = 0;
            msg.__AntennaConfiguration[0].__RFTransmitter.__HopTableID = 1;
            msg.__AntennaConfiguration[0].__RFTransmitter.__TransmitPower = 30;

            //msg.__AntennaProperties = new PARAM_AntennaProperties[1];
            //msg.__AntennaProperties[0] = new PARAM_AntennaProperties();
            //msg.__AntennaProperties[0].__AntennaConnected = true;
            //msg.__AntennaProperties[0].__AntennaGain = 0;
            //msg.__AntennaProperties[0].__AntennaID = 1;

            msg.__EventsAndReports = new PARAM_EventsAndReports();
            msg.__EventsAndReports.__HoldEventsAndReportsUponReconnect = false;

            msg.__KeepaliveSpec = new PARAM_KeepaliveSpec();
            msg.__KeepaliveSpec.__KeepaliveTriggerType = ENUM_KeepaliveTriggerType.Null;
            msg.__KeepaliveSpec.__PeriodicTriggerValue = 0;

            msg.__ReaderEventNotificationSpec = new PARAM_ReaderEventNotificationSpec();
            msg.__ReaderEventNotificationSpec.__EventNotificationState = new PARAM_EventNotificationState[5];
            msg.__ReaderEventNotificationSpec.__EventNotificationState[0] = new PARAM_EventNotificationState();
            msg.__ReaderEventNotificationSpec.__EventNotificationState[0].__EventType = ENUM_NotificationEventType.AISpec_Event;
            msg.__ReaderEventNotificationSpec.__EventNotificationState[0].__NotificationState = true;

            msg.__ReaderEventNotificationSpec.__EventNotificationState[1] = new PARAM_EventNotificationState();
            msg.__ReaderEventNotificationSpec.__EventNotificationState[1].__EventType = ENUM_NotificationEventType.Antenna_Event;
            msg.__ReaderEventNotificationSpec.__EventNotificationState[1].__NotificationState = true;

            msg.__ReaderEventNotificationSpec.__EventNotificationState[2] = new PARAM_EventNotificationState();
            msg.__ReaderEventNotificationSpec.__EventNotificationState[2].__EventType = ENUM_NotificationEventType.GPI_Event;
            msg.__ReaderEventNotificationSpec.__EventNotificationState[2].__NotificationState = true;

            msg.__ReaderEventNotificationSpec.__EventNotificationState[3] = new PARAM_EventNotificationState();
            msg.__ReaderEventNotificationSpec.__EventNotificationState[3].__EventType = ENUM_NotificationEventType.Reader_Exception_Event;
            msg.__ReaderEventNotificationSpec.__EventNotificationState[3].__NotificationState = true;

            msg.__ReaderEventNotificationSpec.__EventNotificationState[4] = new PARAM_EventNotificationState();
            msg.__ReaderEventNotificationSpec.__EventNotificationState[4].__EventType = ENUM_NotificationEventType.RFSurvey_Event;
            msg.__ReaderEventNotificationSpec.__EventNotificationState[4].__NotificationState = true;

            msg.__ROReportSpec = new PARAM_ROReportSpec();
            msg.__ROReportSpec.__N = 1;
            msg.__ROReportSpec.__ROReportTrigger = ENUM_ROReportTriggerType.Upon_N_Tags_Or_End_Of_ROSpec;
            msg.__ROReportSpec.__TagReportContentSelector = new PARAM_TagReportContentSelector();
            msg.__ROReportSpec.__TagReportContentSelector.__AirProtocolEPCMemorySelector = new UNION_AirProtocolEPCMemorySelector();
            PARAM_C1G2EPCMemorySelector c1g2mem = new PARAM_C1G2EPCMemorySelector();
            c1g2mem.__EnableCRC = true;
            c1g2mem.__EnablePCBits = true;
            msg.__ROReportSpec.__TagReportContentSelector.__AirProtocolEPCMemorySelector.Add(c1g2mem);

            msg.__ROReportSpec.__TagReportContentSelector.__EnableAccessSpecID = true;
            msg.__ROReportSpec.__TagReportContentSelector.__EnableAntennaID = true;
            msg.__ROReportSpec.__TagReportContentSelector.__EnableChannelIndex = true;
            msg.__ROReportSpec.__TagReportContentSelector.__EnableFirstSeenTimestamp = true;
            msg.__ROReportSpec.__TagReportContentSelector.__EnableInventoryParameterSpecID = true;
            msg.__ROReportSpec.__TagReportContentSelector.__EnableLastSeenTimestamp = true;
            msg.__ROReportSpec.__TagReportContentSelector.__EnablePeakRSSI = true;
            msg.__ROReportSpec.__TagReportContentSelector.__EnableROSpecID = false;
            msg.__ROReportSpec.__TagReportContentSelector.__EnableSpecIndex = true;
            msg.__ROReportSpec.__TagReportContentSelector.__EnableTagSeenCount = true;

            msg.__ResetToFactoryDefault = false;

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
            msg.__ROSpec = new PARAM_ROSpec();
            msg.__ROSpec.__CurrentState = ENUM_ROSpecState.Disabled;
            msg.__ROSpec.__Priority = 0x00;
            msg.__ROSpec.__ROSpecID = 123;


            msg.__ROSpec.__ROBoundarySpec = new PARAM_ROBoundarySpec();
            msg.__ROSpec.__ROBoundarySpec.__ROSpecStartTrigger = new PARAM_ROSpecStartTrigger();
            msg.__ROSpec.__ROBoundarySpec.__ROSpecStartTrigger.__ROSpecStartTriggerType = ENUM_ROSpecStartTriggerType.Null;

            msg.__ROSpec.__ROBoundarySpec.__ROSpecStopTrigger = new PARAM_ROSpecStopTrigger();
            msg.__ROSpec.__ROBoundarySpec.__ROSpecStopTrigger.__ROSpecStopTriggerType = ENUM_ROSpecStopTriggerType.Null;
            msg.__ROSpec.__ROBoundarySpec.__ROSpecStopTrigger.__DurationTriggerValue = 0;

            msg.__ROSpec.__ROReportSpec = new PARAM_ROReportSpec();
            msg.__ROSpec.__ROReportSpec.__ROReportTrigger = ENUM_ROReportTriggerType.Upon_N_Tags_Or_End_Of_ROSpec;
            msg.__ROSpec.__ROReportSpec.__N = 1;


            msg.__ROSpec.__ROReportSpec.__TagReportContentSelector = new PARAM_TagReportContentSelector();
            msg.__ROSpec.__ROReportSpec.__TagReportContentSelector.__EnableAccessSpecID = true;
            msg.__ROSpec.__ROReportSpec.__TagReportContentSelector.__EnableAntennaID = true;
            msg.__ROSpec.__ROReportSpec.__TagReportContentSelector.__EnableChannelIndex = true;
            msg.__ROSpec.__ROReportSpec.__TagReportContentSelector.__EnableFirstSeenTimestamp = true;
            msg.__ROSpec.__ROReportSpec.__TagReportContentSelector.__EnableInventoryParameterSpecID = true;
            msg.__ROSpec.__ROReportSpec.__TagReportContentSelector.__EnableLastSeenTimestamp = true;
            msg.__ROSpec.__ROReportSpec.__TagReportContentSelector.__EnablePeakRSSI = true;
            msg.__ROSpec.__ROReportSpec.__TagReportContentSelector.__EnableROSpecID = true;
            msg.__ROSpec.__ROReportSpec.__TagReportContentSelector.__EnableSpecIndex = true;
            msg.__ROSpec.__ROReportSpec.__TagReportContentSelector.__EnableTagSeenCount = true;

            msg.__ROSpec.__SpecParameter = new UNION_SpecParameter();
            PARAM_AISpec aiSpec = new PARAM_AISpec();

            aiSpec.__AntennaIDs = new UInt16Array();
            aiSpec.__AntennaIDs.Add(1);
            //aiSpec.__AntennaIDs.Add(2);
            //aiSpec.__AntennaIDs.Add(3);

            aiSpec.__AISpecStopTrigger = new PARAM_AISpecStopTrigger();
            aiSpec.__AISpecStopTrigger.__AISpecStopTriggerType = ENUM_AISpecStopTriggerType.Duration;
            aiSpec.__AISpecStopTrigger.__DurationTrigger = 50000;

            aiSpec.__InventoryParameterSpec = new PARAM_InventoryParameterSpec[1];
            aiSpec.__InventoryParameterSpec[0] = new PARAM_InventoryParameterSpec();
            aiSpec.__InventoryParameterSpec[0].__InventoryParameterSpecID = 1234;
            aiSpec.__InventoryParameterSpec[0].__ProtocolID = ENUM_AirProtocols.EPCGlobalClass1Gen2;

            msg.__ROSpec.__SpecParameter.Add(aiSpec);

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
            msg.__ROSpecID = 0;

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
            msg.__ROSpecID = 123;
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
            msg.__ROSpecID = 123;
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
            msg.__ROSpecID = 123;

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

                    object obj;
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