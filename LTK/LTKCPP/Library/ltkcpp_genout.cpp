
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


#include "ltkcpp.h"


namespace LLRP
{
#include "out_ltkcpp.inc"


static char     ident[] = {
    "$" "Id: libltkcpp "
    STRINGIFY(LTKCPP_VERSION_STR)
    "  Built: "
    __DATE__
    " "
    __TIME__
    " $"
};


/**
 ****************************************************************************
 ** @brief Gets the type registry for the LTKCPP
 ** 
 ** @return CTypeRegistry The Type registry.
 **/
CTypeRegistry *
getTheTypeRegistry (void)
{
    CTypeRegistry *         pTypeRegistry = new CTypeRegistry();

    enrollCoreTypesIntoRegistry (pTypeRegistry);

    return pTypeRegistry;
}


}; /* namespace LLRP */

