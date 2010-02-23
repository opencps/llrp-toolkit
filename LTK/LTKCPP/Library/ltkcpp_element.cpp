
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

int
CElement::walk (
  int                           (*pFunc)(
                                  const CElement *  pElement,
                                  void *            pArg),
  void *                        pArg,
  int                           iDepth,
  int                           nMaxDepth) const
{
    int                         rc;

    rc = (*pFunc)(this, pArg);
    if(0 != rc)
    {
        return rc;
    }

    if(iDepth >= nMaxDepth)
    {
        return 0;
    }

    for (
        tListOfParameters::const_iterator elem =
                                    m_listAllSubParameters.begin();
        elem != m_listAllSubParameters.end();
        elem++)
    {
        (*elem)->walk(pFunc, pArg, iDepth+1, nMaxDepth);
    }

    return 0;
}

CMessage::CMessage(void)
{
    m_MessageID = 0;
}


llrp_bool_t
CParameter::isAllowedIn (
  const CTypeDescriptor *       pEnclosingTypeDescriptor) const
{
    return FALSE;
}

llrp_bool_t
CParameter::isAllowedExtension (
  const CTypeDescriptor *       pEnclosingTypeDescriptor)
{
    /*
     * If it is a generic Custom parameter allow it.
     */
    if(!m_pType->m_bIsMessage && NULL == m_pType->m_pVendorDescriptor &&
       1023u == m_pType->m_TypeNum)
    {
        return TRUE;
    }

    /*
     * At this point checking specifically if it is allowed
     * is perfunctory.
     */
    return isAllowedIn(pEnclosingTypeDescriptor);
}


}; /* namespace LLRP */
