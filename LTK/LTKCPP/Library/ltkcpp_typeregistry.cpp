
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

CTypeRegistry::CTypeRegistry (void)
{
    memset(m_apStdMessageTypeDescriptors, 0,
        sizeof m_apStdMessageTypeDescriptors);

    memset(m_apStdParameterTypeDescriptors, 0,
        sizeof m_apStdParameterTypeDescriptors);
}

CTypeRegistry::~CTypeRegistry (void)
{
}

EResultCode
CTypeRegistry::enroll (
  const CTypeDescriptor *       pTypeDescriptor)
{
    if(NULL == pTypeDescriptor->m_pVendorDescriptor)
    {
        if(1023u < pTypeDescriptor->m_TypeNum)
        {
            return RC_EnrollBadTypeNumber;
        }

        /*
         * Standard message or parameter
         */
        if(pTypeDescriptor->m_bIsMessage)
        {
            m_apStdMessageTypeDescriptors[pTypeDescriptor->m_TypeNum] =
                pTypeDescriptor;
        }
        else
        {
            m_apStdParameterTypeDescriptors[pTypeDescriptor->m_TypeNum] =
                pTypeDescriptor;
        }
    }
    else
    {
        /*
         * Custom messages or parameter
         */
        if(pTypeDescriptor->m_bIsMessage)
        {
            m_listCustomMessageTypeDescriptors.push_back(pTypeDescriptor);
        }
        else
        {
            m_listCustomParameterTypeDescriptors.push_back(pTypeDescriptor);
        }
    }

    return RC_OK;
}

const CTypeDescriptor *
CTypeRegistry::lookupMessage (
  unsigned int                  MessageTypeNum) const
{
    if(1023u < MessageTypeNum)
    {
        return NULL;
    }

    return m_apStdMessageTypeDescriptors[MessageTypeNum];
}

const CTypeDescriptor *
CTypeRegistry::lookupParameter (
  unsigned int                  ParameterTypeNum) const
{
    if(1023u < ParameterTypeNum)
    {
        /* throw exception? */
        return NULL;
    }

    return m_apStdParameterTypeDescriptors[ParameterTypeNum];
}

const CTypeDescriptor *
CTypeRegistry::lookupCustomMessage (
  unsigned int                  VendorID,
  unsigned int                  MessageSubTypeNum) const
{
    for (
        std::list<const CTypeDescriptor *>::const_iterator elem =
                            m_listCustomMessageTypeDescriptors.begin();
        elem != m_listCustomMessageTypeDescriptors.end();
        elem++)
    {
        const CTypeDescriptor * pTypeDescriptor;
        pTypeDescriptor = *elem;
        if(VendorID == pTypeDescriptor->m_pVendorDescriptor->m_VendorID &&
           MessageSubTypeNum == pTypeDescriptor->m_TypeNum)
        {
            return pTypeDescriptor;
        }
    }

    return NULL;
}

const CTypeDescriptor *
CTypeRegistry::lookupCustomParameter (
  unsigned int                  VendorID,
  unsigned int                  ParameterSubTypeNum) const
{
    for (
        std::list<const CTypeDescriptor *>::const_iterator elem = 
                            m_listCustomParameterTypeDescriptors.begin();
        elem != m_listCustomParameterTypeDescriptors.end();
        elem++)
    {
        const CTypeDescriptor * pTypeDescriptor;
        pTypeDescriptor = *elem;
        if(VendorID == pTypeDescriptor->m_pVendorDescriptor->m_VendorID &&
           ParameterSubTypeNum == pTypeDescriptor->m_TypeNum)
        {
            return pTypeDescriptor;
        }
    }

    return NULL;
}

/* look up the type descriptor*/
const CTypeDescriptor *
CTypeRegistry::lookupByName (
  char *                    pName) const
{
    unsigned int                i;
    const CTypeDescriptor *     pTypeDescriptor;

    for(i = 0; i < 1024u; i++)
    {
        pTypeDescriptor = m_apStdMessageTypeDescriptors[i];
        if(NULL == pTypeDescriptor)
        {
            continue;
        }

        if(0 == strcmp(pTypeDescriptor->m_pName, pName))
        {
            return pTypeDescriptor;
        }
    }

    for(i = 0; i < 1024u; i++)
    {
        pTypeDescriptor = m_apStdParameterTypeDescriptors[i];
        if(NULL == pTypeDescriptor)
        {
            continue;
        }

        if(0 == strcmp(pTypeDescriptor->m_pName, pName))
        {
            return pTypeDescriptor;
        }
    }

    for (
        std::list<const CTypeDescriptor *>::const_iterator elem = 
                            m_listCustomParameterTypeDescriptors.begin();
        elem != m_listCustomParameterTypeDescriptors.end();
        elem++)
    {
        const CTypeDescriptor * pTypeDescriptor;
        pTypeDescriptor = *elem;
        if(0 == strcmp(pTypeDescriptor->m_pName, pName))
        {
            return pTypeDescriptor;
        }
    }

    for (
        std::list<const CTypeDescriptor *>::const_iterator elem =
                            m_listCustomMessageTypeDescriptors.begin();
        elem != m_listCustomMessageTypeDescriptors.end();
        elem++)
    {
        const CTypeDescriptor * pTypeDescriptor;
        pTypeDescriptor = *elem;
        if(0 == strcmp(pTypeDescriptor->m_pName, pName))
        {
            return pTypeDescriptor;
        }
    }
    return NULL;
}

}; /* namespace LLRP */

