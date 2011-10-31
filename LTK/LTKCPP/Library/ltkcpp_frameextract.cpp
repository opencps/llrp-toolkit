
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



#include <assert.h>
#include <stdio.h>

#include "ltkcpp_platform.h"
#include "ltkcpp_base.h"
#include "ltkcpp_frame.h"


namespace LLRP
{

CFrameExtract::CFrameExtract (void)
{
    memset(this, 0, sizeof *this);
}

CFrameExtract::CFrameExtract (
  const unsigned char *         pBuffer,
  unsigned int                  nBuffer)
{
    memset(this, 0, sizeof *this);

    if(10u > nBuffer)
    {
        m_MessageLength = 10u;
        m_nBytesNeeded = m_MessageLength - nBuffer;
        m_eStatus = NEED_MORE;
    }
    else
    {
        llrp_u16_t              VersType;

        VersType = pBuffer[0];
        VersType <<= 8u;
        VersType |= pBuffer[1];

        m_MessageLength = pBuffer[2];
        m_MessageLength <<= 8u;
        m_MessageLength |= pBuffer[3];
        m_MessageLength <<= 8u;
        m_MessageLength |= pBuffer[4];
        m_MessageLength <<= 8u;
        m_MessageLength |= pBuffer[5];

        /*
         * Should we be picky about reserved bits?
         */

        m_MessageType = VersType & 0x3FFu;
        m_ProtocolVersion = (VersType >> 10u) & 0x7u;

        m_MessageID = pBuffer[6];
        m_MessageID <<= 8u;
        m_MessageID |= pBuffer[7];
        m_MessageID <<= 8u;
        m_MessageID |= pBuffer[8];
        m_MessageID <<= 8u;
        m_MessageID |= pBuffer[9];

        if(10u > m_MessageLength)
        {
            m_nBytesNeeded = 0;
            m_eStatus = FRAME_ERROR;
        }
        else if(nBuffer >= m_MessageLength)
        {
            m_nBytesNeeded = 0;
            m_eStatus = READY;
        }
        else
        {
            m_nBytesNeeded = m_MessageLength - nBuffer;
            m_eStatus = NEED_MORE;
        }
    }
}


}; /* namespace LLRP */

