
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
#include "ltkcpp_frame.h"



namespace LLRP
{


CFieldDescriptor
g_fdMessageHeader_Type =
{
    CFieldDescriptor::FT_U16,
    "MessageHeader.Type",
    NULL
};

CFieldDescriptor
g_fdMessageHeader_Length =
{
    CFieldDescriptor::FT_U32,
    "MessageHeader.Length",
    NULL
};

CFieldDescriptor
g_fdMessageHeader_MessageID =
{
    CFieldDescriptor::FT_U32,
    "MessageHeader.MessageID",
    NULL
};

CFieldDescriptor
g_fdMessageHeader_VendorPEN =
{
    CFieldDescriptor::FT_U32,
    "MessageHeader.CustomVendorPEN",
    NULL
};

CFieldDescriptor
g_fdMessageHeader_Subtype =
{
    CFieldDescriptor::FT_U8,
    "MessageHeader.CustomSubtype",
    NULL
};

CFieldDescriptor
g_fdParameterHeader_TVType =
{
    CFieldDescriptor::FT_U8,
    "ParameterHeader.TVType",
    NULL
};

CFieldDescriptor
g_fdParameterHeader_TLVType =
{
    CFieldDescriptor::FT_U16,
    "ParameterHeader.TLVType",
    NULL
};

CFieldDescriptor
g_fdParameterHeader_TLVLength =
{
    CFieldDescriptor::FT_U16,
    "ParameterHeader.TLVLength",
    NULL
};

CFieldDescriptor
g_fdParameterHeader_VendorPEN =
{
    CFieldDescriptor::FT_U32,
    "ParameterHeader.CustomVendorPEN",
    NULL
};

CFieldDescriptor
g_fdParameterHeader_Subtype =
{
    CFieldDescriptor::FT_U32,
    "ParameterHeader.CustomSubtype",
    NULL
};


};
