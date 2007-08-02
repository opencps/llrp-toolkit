
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


#include "ltkcpp_platform.h"
#include "ltkcpp_base.h"

namespace LLRP
{

CElement::CElement(void)
{
    m_pType = NULL;
    m_pParent = NULL;
}

CElement::~CElement (void)
{

    for (
        tListOfParameters::iterator elem = m_listAllSubParameters.begin();
        elem != m_listAllSubParameters.end();
        elem++)
    {
        delete *elem;
    }
}


void
CElement::addSubParameterToAllList (
  CParameter *                  pParameter)
{
    if(NULL != pParameter)
    {
        m_listAllSubParameters.push_back(pParameter);
    }
}


void
CElement::removeSubParameterFromAllList (
  CParameter *                  pParameter)
{
    if(NULL != pParameter)
    {
        m_listAllSubParameters.remove(pParameter);
    }
}

void
CElement::clearSubParameterList (
  tListOfParameters *           pParameterList)
{
    for(
        tListOfParameters::iterator elem = pParameterList->begin();
        elem != pParameterList->end();
        elem++)
    {
        removeSubParameterFromAllList(*elem);
        delete *elem;
    }

    pParameterList->clear();
}

}; /* namespace LLRP */
