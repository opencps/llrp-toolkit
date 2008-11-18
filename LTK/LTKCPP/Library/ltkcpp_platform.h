
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
 ** @file ltkcpp_platform.h
 **
 ** @brief Based types for the LKTCPP library
 ** 
 ** Contains the platform dependent basic types for the LTKCPP library
 **
 *****************************************************************************/

/*
 * Include file to establish context
 * for the LLRP Tool Kit (LTK) C++ platform.
 */

#include <list>
#include <string.h>         /* memcpy() */

#define FALSE       0
#define TRUE        1

namespace LLRP
{

/*
 * Typedefs of simple types.
 * The LTK/C++ uses these types extensively.
 * To retarget to another C++ platform change
 * these typedefs. Everything else should be
 * good to go.
 */

#ifdef linux
#include <stdint.h>
/** Basic type for LLRP 8-bit unsigned data values*/
typedef uint8_t                 llrp_u8_t;
/** Basic type for LLRP 8-bit signed data values*/
typedef int8_t                  llrp_s8_t;
/** Basic type for LLRP 16-bit unsigned data values*/
typedef uint16_t                llrp_u16_t;
/** Basic type for LLRP 16-bit signed data values*/
typedef int16_t                 llrp_s16_t;
/** Basic type for LLRP 32-bit unsigned data values*/
typedef uint32_t                llrp_u32_t;
/** Basic type for LLRP 32-bit signed data values*/
typedef int32_t                 llrp_s32_t;
/** Basic type for LLRP 64-bit unsigned data values*/
typedef uint64_t                llrp_u64_t;
/** Basic type for LLRP 64-bit signed data values*/
typedef int64_t                 llrp_s64_t;
/** Basic type for LLRP 1-bit unsigned data values*/
typedef uint8_t                 llrp_u1_t;
/** Basic type for LLRP 2-bit unsigned data values*/
typedef uint8_t                 llrp_u2_t;
/** Basic type for LLRP utf8 character values*/
typedef uint8_t                 llrp_utf8_t;
/** Basic type for LLRP boolean values*/
typedef bool                    llrp_bool_t;
/** Basic type for LLRP byte encoded values */
typedef uint8_t                 llrp_byte_t;
#endif /* linux */

#ifdef WIN32
typedef unsigned char           llrp_u8_t;
typedef signed char             llrp_s8_t;
typedef unsigned short          llrp_u16_t;
typedef signed short            llrp_s16_t;
typedef unsigned int            llrp_u32_t;
typedef signed int              llrp_s32_t;
typedef unsigned long long      llrp_u64_t;
typedef signed long long        llrp_s64_t;
typedef unsigned char           llrp_u1_t;
typedef unsigned char           llrp_u2_t;
typedef unsigned char           llrp_utf8_t;
typedef bool                    llrp_bool_t;
typedef unsigned char           llrp_byte_t;
#endif /* WIN32 */

};  /* namespace LLRP */
