
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


#include "ltkcpp_platform.h"
#include "ltkcpp_base.h"


namespace LLRP
{

CErrorDetails::CErrorDetails (void)
{
    clear();
}

void
CErrorDetails::clear (void)
{
    memset(this, 0, sizeof *this);
}

void
CErrorDetails::missingParameter (
  const CTypeDescriptor *       pRefType)
{
    if(RC_OK == m_eResultCode)
    {
        clear();
        m_eResultCode = RC_MissingParameter;
        m_pWhatStr    = "missing parameter";
        m_pRefType    = pRefType;
    }
}

void
CErrorDetails::unexpectedParameter (
  const CParameter *            pParameter)
{
    if(RC_OK == m_eResultCode)
    {
        clear();
        m_eResultCode = RC_UnexpectedParameter;
        m_pWhatStr    = "unexpected parameter";
        m_pRefType    = pParameter->m_pType;
    }
}

void
CErrorDetails::resultCodeAndWhatStr (
  EResultCode                   eResultCode,
  const char *                  pWhatStr)
{
    if(RC_OK == m_eResultCode)
    {
        clear();
        m_eResultCode = eResultCode;
        m_pWhatStr    = pWhatStr;
    }
}


}; /* namespace LLRP */
