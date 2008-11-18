
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

/*
 * u8v
 */

llrp_u8v_t::llrp_u8v_t (void)
{
    m_pValue = NULL;
    m_nValue = 0;
}

llrp_u8v_t::llrp_u8v_t (
  unsigned int                  nValue)
{
    m_nValue = nValue;
    if(0 < m_nValue)
    {
        m_pValue = new llrp_u8_t[m_nValue];
        memset(m_pValue, 0, sizeof m_pValue[0] * m_nValue);
    }
    else
    {
        m_pValue = NULL;
    }
}

llrp_u8v_t::llrp_u8v_t (
  const llrp_u8v_t &           rOther)
{
    copy(rOther);
}

llrp_u8v_t::~llrp_u8v_t (void)
{
    reset();
}

llrp_u8v_t &
llrp_u8v_t::operator= (
  const llrp_u8v_t &           rOther)
{
    if(this != &rOther)
    {
        reset();
        copy(rOther);
    }

    return *this;
}

void
llrp_u8v_t::reset(void)
{
    if(NULL != m_pValue)
    {
        delete[] m_pValue;
        m_pValue = NULL;
    }
    m_nValue = 0;
}

void
llrp_u8v_t::copy (
  const llrp_u8v_t &           rOther)
{
    m_nValue = rOther.m_nValue;
    if(0 < m_nValue)
    {
        m_pValue = new llrp_u8_t[m_nValue];
        memcpy(m_pValue, rOther.m_pValue, sizeof m_pValue[0] * m_nValue);
    }
    else
    {
        m_pValue = NULL;
    }
}





/*
 * s8v
 */

llrp_s8v_t::llrp_s8v_t (void)
{
    m_pValue = NULL;
    m_nValue = 0;
}

llrp_s8v_t::llrp_s8v_t (
  unsigned int                  nValue)
{
    m_nValue = nValue;
    if(0 < m_nValue)
    {
        m_pValue = new llrp_s8_t[m_nValue];
        memset(m_pValue, 0, sizeof m_pValue[0] * m_nValue);
    }
    else
    {
        m_pValue = NULL;
    }
}

llrp_s8v_t::llrp_s8v_t (
  const llrp_s8v_t &           rOther)
{
    copy(rOther);
}

llrp_s8v_t::~llrp_s8v_t (void)
{
    reset();
}

llrp_s8v_t &
llrp_s8v_t::operator= (
  const llrp_s8v_t &           rOther)
{
    if(this != &rOther)
    {
        reset();
        copy(rOther);
    }

    return *this;
}

void
llrp_s8v_t::reset(void)
{
    if(NULL != m_pValue)
    {
        delete[] m_pValue;
        m_pValue = NULL;
    }
    m_nValue = 0;
}

void
llrp_s8v_t::copy (
  const llrp_s8v_t &           rOther)
{
    m_nValue = rOther.m_nValue;
    if(0 < m_nValue)
    {
        m_pValue = new llrp_s8_t[m_nValue];
        memcpy(m_pValue, rOther.m_pValue, sizeof m_pValue[0] * m_nValue);
    }
    else
    {
        m_pValue = NULL;
    }
}





/*
 * u16v
 */

llrp_u16v_t::llrp_u16v_t (void)
{
    m_pValue = NULL;
    m_nValue = 0;
}

llrp_u16v_t::llrp_u16v_t (
  unsigned int                  nValue)
{
    m_nValue = nValue;
    if(0 < m_nValue)
    {
        m_pValue = new llrp_u16_t[m_nValue];
        memset(m_pValue, 0, sizeof m_pValue[0] * m_nValue);
    }
    else
    {
        m_pValue = NULL;
    }
}

llrp_u16v_t::llrp_u16v_t (
  const llrp_u16v_t &           rOther)
{
    copy(rOther);
}

llrp_u16v_t::~llrp_u16v_t (void)
{
    reset();
}

llrp_u16v_t &
llrp_u16v_t::operator= (
  const llrp_u16v_t &           rOther)
{
    if(this != &rOther)
    {
        reset();
        copy(rOther);
    }

    return *this;
}

void
llrp_u16v_t::reset(void)
{
    if(NULL != m_pValue)
    {
        delete[] m_pValue;
        m_pValue = NULL;
    }
    m_nValue = 0;
}

void
llrp_u16v_t::copy (
  const llrp_u16v_t &           rOther)
{
    m_nValue = rOther.m_nValue;
    if(0 < m_nValue)
    {
        m_pValue = new llrp_u16_t[m_nValue];
        memcpy(m_pValue, rOther.m_pValue, sizeof m_pValue[0] * m_nValue);
    }
    else
    {
        m_pValue = NULL;
    }
}





/*
 * s16v
 */

llrp_s16v_t::llrp_s16v_t (void)
{
    m_pValue = NULL;
    m_nValue = 0;
}

llrp_s16v_t::llrp_s16v_t (
  unsigned int                  nValue)
{
    m_nValue = nValue;
    if(0 < m_nValue)
    {
        m_pValue = new llrp_s16_t[m_nValue];
        memset(m_pValue, 0, sizeof m_pValue[0] * m_nValue);
    }
    else
    {
        m_pValue = NULL;
    }
}

llrp_s16v_t::llrp_s16v_t (
  const llrp_s16v_t &           rOther)
{
    copy(rOther);
}

llrp_s16v_t::~llrp_s16v_t (void)
{
    reset();
}

llrp_s16v_t &
llrp_s16v_t::operator= (
  const llrp_s16v_t &           rOther)
{
    if(this != &rOther)
    {
        reset();
        copy(rOther);
    }

    return *this;
}

void
llrp_s16v_t::reset(void)
{
    if(NULL != m_pValue)
    {
        delete[] m_pValue;
        m_pValue = NULL;
    }
    m_nValue = 0;
}

void
llrp_s16v_t::copy (
  const llrp_s16v_t &           rOther)
{
    m_nValue = rOther.m_nValue;
    if(0 < m_nValue)
    {
        m_pValue = new llrp_s16_t[m_nValue];
        memcpy(m_pValue, rOther.m_pValue, sizeof m_pValue[0] * m_nValue);
    }
    else
    {
        m_pValue = NULL;
    }
}





/*
 * u32v
 */

llrp_u32v_t::llrp_u32v_t (void)
{
    m_pValue = NULL;
    m_nValue = 0;
}

llrp_u32v_t::llrp_u32v_t (
  unsigned int                  nValue)
{
    m_nValue = nValue;
    if(0 < m_nValue)
    {
        m_pValue = new llrp_u32_t[m_nValue];
        memset(m_pValue, 0, sizeof m_pValue[0] * m_nValue);
    }
    else
    {
        m_pValue = NULL;
    }
}

llrp_u32v_t::llrp_u32v_t (
  const llrp_u32v_t &           rOther)
{
    copy(rOther);
}

llrp_u32v_t::~llrp_u32v_t (void)
{
    reset();
}

llrp_u32v_t &
llrp_u32v_t::operator= (
  const llrp_u32v_t &           rOther)
{
    if(this != &rOther)
    {
        reset();
        copy(rOther);
    }

    return *this;
}

void
llrp_u32v_t::reset(void)
{
    if(NULL != m_pValue)
    {
        delete[] m_pValue;
        m_pValue = NULL;
    }
    m_nValue = 0;
}

void
llrp_u32v_t::copy (
  const llrp_u32v_t &           rOther)
{
    m_nValue = rOther.m_nValue;
    if(0 < m_nValue)
    {
        m_pValue = new llrp_u32_t[m_nValue];
        memcpy(m_pValue, rOther.m_pValue, sizeof m_pValue[0] * m_nValue);
    }
    else
    {
        m_pValue = NULL;
    }
}





/*
 * s32v
 */

llrp_s32v_t::llrp_s32v_t (void)
{
    m_pValue = NULL;
    m_nValue = 0;
}

llrp_s32v_t::llrp_s32v_t (
  unsigned int                  nValue)
{
    m_nValue = nValue;
    if(0 < m_nValue)
    {
        m_pValue = new llrp_s32_t[m_nValue];
        memset(m_pValue, 0, sizeof m_pValue[0] * m_nValue);
    }
    else
    {
        m_pValue = NULL;
    }
}

llrp_s32v_t::llrp_s32v_t (
  const llrp_s32v_t &           rOther)
{
    copy(rOther);
}

llrp_s32v_t::~llrp_s32v_t (void)
{
    reset();
}

llrp_s32v_t &
llrp_s32v_t::operator= (
  const llrp_s32v_t &           rOther)
{
    if(this != &rOther)
    {
        reset();
        copy(rOther);
    }

    return *this;
}

void
llrp_s32v_t::reset(void)
{
    if(NULL != m_pValue)
    {
        delete[] m_pValue;
        m_pValue = NULL;
    }
    m_nValue = 0;
}

void
llrp_s32v_t::copy (
  const llrp_s32v_t &           rOther)
{
    m_nValue = rOther.m_nValue;
    if(0 < m_nValue)
    {
        m_pValue = new llrp_s32_t[m_nValue];
        memcpy(m_pValue, rOther.m_pValue, sizeof m_pValue[0] * m_nValue);
    }
    else
    {
        m_pValue = NULL;
    }
}





/*
 * u64v
 */

llrp_u64v_t::llrp_u64v_t (void)
{
    m_pValue = NULL;
    m_nValue = 0;
}

llrp_u64v_t::llrp_u64v_t (
  unsigned int                  nValue)
{
    m_nValue = nValue;
    if(0 < m_nValue)
    {
        m_pValue = new llrp_u64_t[m_nValue];
        memset(m_pValue, 0, sizeof m_pValue[0] * m_nValue);
    }
    else
    {
        m_pValue = NULL;
    }
}

llrp_u64v_t::llrp_u64v_t (
  const llrp_u64v_t &           rOther)
{
    copy(rOther);
}

llrp_u64v_t::~llrp_u64v_t (void)
{
    reset();
}

llrp_u64v_t &
llrp_u64v_t::operator= (
  const llrp_u64v_t &           rOther)
{
    if(this != &rOther)
    {
        reset();
        copy(rOther);
    }

    return *this;
}

void
llrp_u64v_t::reset(void)
{
    if(NULL != m_pValue)
    {
        delete[] m_pValue;
        m_pValue = NULL;
    }
    m_nValue = 0;
}

void
llrp_u64v_t::copy (
  const llrp_u64v_t &           rOther)
{
    m_nValue = rOther.m_nValue;
    if(0 < m_nValue)
    {
        m_pValue = new llrp_u64_t[m_nValue];
        memcpy(m_pValue, rOther.m_pValue, sizeof m_pValue[0] * m_nValue);
    }
    else
    {
        m_pValue = NULL;
    }
}





/*
 * s64v
 */

llrp_s64v_t::llrp_s64v_t (void)
{
    m_pValue = NULL;
    m_nValue = 0;
}

llrp_s64v_t::llrp_s64v_t (
  unsigned int                  nValue)
{
    m_nValue = nValue;
    if(0 < m_nValue)
    {
        m_pValue = new llrp_s64_t[m_nValue];
        memset(m_pValue, 0, sizeof m_pValue[0] * m_nValue);
    }
    else
    {
        m_pValue = NULL;
    }
}

llrp_s64v_t::llrp_s64v_t (
  const llrp_s64v_t &           rOther)
{
    copy(rOther);
}

llrp_s64v_t::~llrp_s64v_t (void)
{
    reset();
}

llrp_s64v_t &
llrp_s64v_t::operator= (
  const llrp_s64v_t &           rOther)
{
    if(this != &rOther)
    {
        reset();
        copy(rOther);
    }

    return *this;
}

void
llrp_s64v_t::reset(void)
{
    if(NULL != m_pValue)
    {
        delete[] m_pValue;
        m_pValue = NULL;
    }
    m_nValue = 0;
}

void
llrp_s64v_t::copy (
  const llrp_s64v_t &           rOther)
{
    m_nValue = rOther.m_nValue;
    if(0 < m_nValue)
    {
        m_pValue = new llrp_s64_t[m_nValue];
        memcpy(m_pValue, rOther.m_pValue, sizeof m_pValue[0] * m_nValue);
    }
    else
    {
        m_pValue = NULL;
    }
}





/*
 * u1v
 */

llrp_u1v_t::llrp_u1v_t (void)
{
    m_pValue = NULL;
    m_nBit = 0;
}

llrp_u1v_t::llrp_u1v_t (
  unsigned int                  nBit)
{
    m_nBit = nBit;
    if(0 < m_nBit)
    {
        unsigned int            nByte = (m_nBit + 7u) / 8u;

        m_pValue = new llrp_byte_t[nByte];
        memset(m_pValue, 0, nByte);
    }
    else
    {
        m_pValue = NULL;
    }
}

llrp_u1v_t::llrp_u1v_t (
  const llrp_u1v_t &           rOther)
{
    copy(rOther);
}

llrp_u1v_t::~llrp_u1v_t (void)
{
    reset();
}

llrp_u1v_t &
llrp_u1v_t::operator= (
  const llrp_u1v_t &           rOther)
{
    if(this != &rOther)
    {
        reset();
        copy(rOther);
    }

    return *this;
}

void
llrp_u1v_t::reset(void)
{
    if(NULL != m_pValue)
    {
        delete[] m_pValue;
        m_pValue = NULL;
    }
    m_nBit = 0;
}

void
llrp_u1v_t::copy (
  const llrp_u1v_t &           rOther)
{
    m_nBit = rOther.m_nBit;
    if(0 < m_nBit)
    {
        unsigned int            nByte = (m_nBit + 7u) / 8u;

        m_pValue = new llrp_byte_t[nByte];
        memcpy(m_pValue, rOther.m_pValue, nByte);
    }
    else
    {
        m_pValue = NULL;
    }
}





/*
 * utf8v
 */

llrp_utf8v_t::llrp_utf8v_t (void)
{
    m_pValue = NULL;
    m_nValue = 0;
}

llrp_utf8v_t::llrp_utf8v_t (
  unsigned int                  nValue)
{
    m_nValue = nValue;
    if(0 < m_nValue)
    {
        m_pValue = new llrp_utf8_t[m_nValue];
        memset(m_pValue, 0, sizeof m_pValue[0] * m_nValue);
    }
    else
    {
        m_pValue = NULL;
    }
}

llrp_utf8v_t::llrp_utf8v_t (
  const llrp_utf8v_t &           rOther)
{
    copy(rOther);
}

llrp_utf8v_t::~llrp_utf8v_t (void)
{
    reset();
}

llrp_utf8v_t &
llrp_utf8v_t::operator= (
  const llrp_utf8v_t &           rOther)
{
    if(this != &rOther)
    {
        reset();
        copy(rOther);
    }

    return *this;
}

void
llrp_utf8v_t::reset(void)
{
    if(NULL != m_pValue)
    {
        delete[] m_pValue;
        m_pValue = NULL;
    }
    m_nValue = 0;
}

void
llrp_utf8v_t::copy (
  const llrp_utf8v_t &           rOther)
{
    m_nValue = rOther.m_nValue;
    if(0 < m_nValue)
    {
        m_pValue = new llrp_utf8_t[m_nValue];
        memcpy(m_pValue, rOther.m_pValue, sizeof m_pValue[0] * m_nValue);
    }
    else
    {
        m_pValue = NULL;
    }
}





/*
 * bytesToEnd
 */

llrp_bytesToEnd_t::llrp_bytesToEnd_t (void)
{
    m_pValue = NULL;
    m_nValue = 0;
}

llrp_bytesToEnd_t::llrp_bytesToEnd_t (
  unsigned int                  nValue)
{
    m_nValue = nValue;
    if(0 < m_nValue)
    {
        m_pValue = new llrp_byte_t[m_nValue];
        memset(m_pValue, 0, sizeof m_pValue[0] * m_nValue);
    }
    else
    {
        m_pValue = NULL;
    }
}

llrp_bytesToEnd_t::llrp_bytesToEnd_t (
  const llrp_bytesToEnd_t &           rOther)
{
    copy(rOther);
}

llrp_bytesToEnd_t::~llrp_bytesToEnd_t (void)
{
    reset();
}

llrp_bytesToEnd_t &
llrp_bytesToEnd_t::operator= (
  const llrp_bytesToEnd_t &           rOther)
{
    if(this != &rOther)
    {
        reset();
        copy(rOther);
    }

    return *this;
}

void
llrp_bytesToEnd_t::reset(void)
{
    if(NULL != m_pValue)
    {
        delete[] m_pValue;
        m_pValue = NULL;
    }
    m_nValue = 0;
}

void
llrp_bytesToEnd_t::copy (
  const llrp_bytesToEnd_t &           rOther)
{
    m_nValue = rOther.m_nValue;
    if(0 < m_nValue)
    {
        m_pValue = new llrp_byte_t[m_nValue];
        memcpy(m_pValue, rOther.m_pValue, sizeof m_pValue[0] * m_nValue);
    }
    else
    {
        m_pValue = NULL;
    }
}



}; /* namespace LLRP */

