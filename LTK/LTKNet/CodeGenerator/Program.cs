using System;
using System.Collections.Generic;
using System.Text;
using System.IO;
using System.Xml;
using System.Xml.Xsl;
using System.Xml.XPath;
using Microsoft.Build.Framework;
using Microsoft.Build.Utilities;

namespace CodeGenerator
{
    public class LLRPCodeGenerator : Task
    {
        private string source_file;
        private string stylesheet;
        private string output_file;

        public override bool Execute()
        {
            try
            {
                Log.LogMessage(string.Format("Start generate {0}...", output_file ));
                if (Transform(source_file, stylesheet, output_file))
                {
                    Log.LogMessage(string.Format("{0} is generated.", output_file));
                    return true;
                }
                else
                    return true;
            }
            catch(Exception ex)
            {
                Log.LogError(ex.Message);
                return false;
            }
        }

        public string LLRP_XML_FILE
        {
            get { return source_file; }
            set { source_file = value; }
        }

        public string XSLT_FILE
        {
            get { return stylesheet; }
            set { stylesheet = value; }
        }

        public string OUTPUT_CS_FILE
        {
            get { return output_file; }
            set { output_file = value; }
        }

        bool Transform(string source_file, string stylesheet, string output_file)
        {
            FileStream fs = null;
            string path = Environment.CurrentDirectory;

            try
            {
                //Load Xslt style sheet
                XslCompiledTransform trans = new XslCompiledTransform();
                fs = new System.IO.FileStream(path + "\\" + stylesheet, FileMode.Open);
                XmlTextReader xr = new XmlTextReader(fs);

                trans.Load(xr);

                //Load XML file
                XPathDocument doc = new XPathDocument(path + "\\" + source_file);

                //Tranform
                XmlTextWriter xw = new XmlTextWriter(path + "\\" + output_file, Encoding.ASCII);
                trans.Transform(doc, null, xw);

                xr.Close();
                xw.Close();

                return true;
            }
            catch (Exception ex)
            {
                try { if (fs != null)fs.Close(); }
                catch { }

                Console.WriteLine(ex.Message);

                return false;
            }
        }

    }
}
