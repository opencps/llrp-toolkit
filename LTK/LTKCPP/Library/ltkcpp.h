
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
/**
 *****************************************************************************
 **
 ** @file ltkcpp.h 
 **
 ** @brief  File that includes all LLRP classes and types
 **
 ** This file includes all the LLRP types and classes that are generated
 ** by the LLRP automated build process. 
 **
 *****************************************************************************/


#ifndef _LTKCPP_H
#define _LTKCPP_H

#include "ltkcpp_platform.h"
#include "ltkcpp_base.h"
#include "ltkcpp_frame.h"
#include "ltkcpp_xmltext.h"
#include "ltkcpp_connection.h"

/* for passing version information as a define */
#define STRINGIFY(x) XSTRINGIFY(x)
#define XSTRINGIFY(x) #x

#include "version.inc"

/*
** @brief The Namespace for the LLRP LTK Library */
namespace LLRP
{
#include "out_ltkcpp.h"

/* @brief Gets a new type registry and enrolls the core LLRP types
**
** Convenience function to get a new type registry, and enroll the
** core LLRP types
**
** @return CTypeRegistry The type registry created 
**
** @ingroup LTKCoreElement
*/
extern CTypeRegistry *
getTheTypeRegistry (void);

}; /* namespace LLRP */

#endif /* !_LTKCPP_H */

