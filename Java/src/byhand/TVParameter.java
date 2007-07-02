//   Copyright (c) 2007 by the Board of Trustees of the University of Arkansas.
//
//   Licensed under the Apache License, Version 2.0 (the "License");
//   you may not use this file except in compliance with the License.
//   You may obtain a copy of the License at
//
//       http://www.apache.org/licenses/LICENSE-2.0
//
//   Unless required by applicable law or agreed to in writing, software
//   distributed under the License is distributed on an "AS IS" BASIS,
//   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied, 
//   including, without limitation, any warranties or conditions of TITLE, 
//   NON-INFRINGEMENT, MERCHANTABILITY, or FITNESS FOR A PARTICULAR PURPOSE.
//   See the License for the specific language governing permissions and
//   limitations under the License.

/*
 * TVParameter.java
 *
 * Created on May 26, 2007, 12:39 PM
 */

package edu.uark.csce.llrp;
import java.io.*;

/**
 *
 * @author Joe Hoag
 */
public abstract class TVParameter extends Parameter {
    
    /** Creates a new instance of TLVParameter */
    public TVParameter() {
    }
    
    public void serialize(DataOutputStream out) throws IOException
    {
	out.writeByte((byte) (getParamType() | 0x80) );
	serializeBody(out);
    }
    
    //public void show()
    //{
    //    System.out.println("TVParameter type: " + getParamType());
    //    showBody();
    //}
    
}
