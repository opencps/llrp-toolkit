
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
 ** @file ltkcpp_base.h
 **
 ** @brief Based type descriptions for the LTKCPP library
 **
 ** Contains the basic types, enumerations, and classes for the LTKCPP
 **
 *****************************************************************************/


/**
 ** @defgroup LTKCoreElement Core LTK Library classes and functions
 **
 **/

#define BOOL int

/**
 ** @\brief The namespace for the LTKCPP library
 **
 ** The namespace for the LTKCPP library
 **/
namespace LLRP
{

/*
 * Forward declarations of structs and classes
 */

//enum EResultCode;
class CErrorDetails;
class CVendorDescriptor;
class CNamespaceDescriptor;
class CTypeDescriptor;
class CFieldDescriptor;
struct SEnumTableEntry;
class CTypeRegistry;
class CElement;
class CMessage;
class CParameter;
class CDecoder;
class CDecoderStream;
class CEncoder;
class CEncoderStream;


/*
 * Simple array (vector) types. There is an array type
 * for each of {u,s}{8,16,32,64}v and u1v and utf8v.
 *
 * The member variables are:
 *      m_nValue            The number of array elements
 *      m_pValue            Pointer to the first element
 *
 * The interfaces:
 *      llrp_XXv_t(void)    Default constructor, just sets the
 *                          member fields to zero.
 *      llrp_XXv_t(llrp_XXv_t & rOther)
 *                          Constructor that clones an existing object.
 *      ~llrp_XXv_t(void)   Destructor, just deletes m_pValue in case
 *                          there is something allocated to it.
 *      operator=           Assignment operator. Used to copy an
 *                          existing instance. It's careful
 *                          to delete m_pValue before overwriting it.
 *
 * Private subroutines
 *      copy()              Makes a copy, supports operator=
 *      reset()             Clears the variable much like the
 *                          destructor does.
 */

/**
 *****************************************************************************
 **
 ** @brief Class for LLRP basic type u8v (List of unsigned 8-bit values)
 **
 *****************************************************************************
 **/
class llrp_u8v_t
{
  public:
    /** @brief The number of arrray elements */
    llrp_u16_t                  m_nValue;
    /** @brief Pointer to the first array element */
    llrp_u8_t *                 m_pValue;

    /** @brief Default Constructer. Sets members fields to zero */
    llrp_u8v_t(void);

    /** @brief Constructor. Allocates spaec for nValue arrays and sets to zero */
    llrp_u8v_t (
      unsigned int              nValue);

    /** @brief Copy Constructor */
    llrp_u8v_t (
      const llrp_u8v_t &        rOther);

    /** @brief Destructor. Deletes m_pValue in cases where it is allocated */
    ~llrp_u8v_t(void);

    /** @brief Assignement operator.  Used to copy an existing instance. */
    llrp_u8v_t &
    operator= (
      const llrp_u8v_t &        rValue);

  private:
    void
    copy (
      const llrp_u8v_t &        rOther);

    void
    reset (void);
};

/**
 *****************************************************************************
 **
 ** @brief Class for LLRP basic type s8v (List of signed 8-bit values)
 **
 *****************************************************************************
 **/
class llrp_s8v_t
{
  public:
    /** @brief The number of arrray elements */
    llrp_u16_t                  m_nValue;
    /** @brief Pointer to the first array element */
    llrp_s8_t *                 m_pValue;

    /** @brief Default Constructer. Sets members fields to zero */
    llrp_s8v_t(void);

    /** @brief Constructor. Allocates spaec for nValue arrays and sets to zero */
    llrp_s8v_t (
      unsigned int              nValue);

    /** @brief Copy Constructor */
    llrp_s8v_t (
      const llrp_s8v_t &        rOther);

    /** @brief Destructor. Deletes m_pValue in cases where it is allocated */
    ~llrp_s8v_t(void);

    /** @brief Assignement operator.  Used to copy an existing instance. */
    llrp_s8v_t &
    operator= (
      const llrp_s8v_t &        rValue);

  private:
    void
    copy (
      const llrp_s8v_t &        rOther);

    void
    reset (void);
};

/**
 *****************************************************************************
 **
 ** @brief Class for LLRP basic type u16v (List of unsigned 16-bit values)
 **
 *****************************************************************************
 **/
class llrp_u16v_t
{
  public:
    /** @brief The number of arrray elements */
    llrp_u16_t                  m_nValue;
    /** @brief Pointer to the first array element */
    llrp_u16_t *                m_pValue;

    /** @brief Default Constructer. Sets members fields to zero */
    llrp_u16v_t(void);

    /** @brief Constructor. Allocates spaec for nValue arrays and sets to zero */
    llrp_u16v_t (
      unsigned int              nValue);

    /** @brief Copy Constructor */
    llrp_u16v_t (
      const llrp_u16v_t &       rOther);

    /** @brief Destructor. Deletes m_pValue in cases where it is allocated */
    ~llrp_u16v_t(void);

    /** @brief Assignement operator.  Used to copy an existing instance. */
    llrp_u16v_t &
    operator= (
      const llrp_u16v_t &       rValue);

  private:
    void
    copy (
      const llrp_u16v_t &       rOther);

    void
    reset (void);
};

/**
 *****************************************************************************
 **
 ** @brief Class for LLRP basic type s16v (List of signed 16-bit values)
 **
 *****************************************************************************
 **/
class llrp_s16v_t
{
  public:
    /** @brief The number of arrray elements */
    llrp_u16_t                  m_nValue;
    /** @brief Pointer to the first array element */
    llrp_s16_t *                m_pValue;

    /** @brief Default Constructer. Sets members fields to zero */
    llrp_s16v_t(void);

    /** @brief Constructor. Allocates spaec for nValue arrays and sets to zero */
    llrp_s16v_t (
      unsigned int              nValue);

    /** @brief Copy Constructor */
    llrp_s16v_t (
      const llrp_s16v_t &       rOther);

    /** @brief Destructor. Deletes m_pValue in cases where it is allocated */
    ~llrp_s16v_t(void);

    /** @brief Assignement operator.  Used to copy an existing instance. */
    llrp_s16v_t &
    operator= (
      const llrp_s16v_t &       rValue);

  private:
    void
    copy (
      const llrp_s16v_t &       rOther);

    void
    reset (void);
};

/**
 *****************************************************************************
 **
 ** @brief Class for LLRP basic type u32v (vector of unsigned 32-bit values)
 **
 *****************************************************************************
 **/
class llrp_u32v_t
{
  public:
    /** @brief The number of arrray elements */
    llrp_u16_t                  m_nValue;
    /** @brief Pointer to the first array element */
    llrp_u32_t *                m_pValue;

    /** @brief Default Constructer. Sets members fields to zero */
    llrp_u32v_t(void);

    /** @brief Constructor. Allocates spaec for nValue arrays and sets to zero */
    llrp_u32v_t (
      unsigned int              nValue);

    /** @brief Copy Constructor */
    llrp_u32v_t (
      const llrp_u32v_t &       rOther);

    /** @brief Destructor. Deletes m_pValue in cases where it is allocated */
    ~llrp_u32v_t(void);

    /** @brief Assignement operator.  Used to copy an existing instance. */
    llrp_u32v_t &
    operator= (
      const llrp_u32v_t &       rValue);

  private:
    void
    copy (
      const llrp_u32v_t &       rOther);

    void
    reset (void);
};

/**
 *****************************************************************************
 **
 ** @brief Class for LLRP basic type s32v (vector of signed 32-bit values)
 **
 *****************************************************************************
 **/
class llrp_s32v_t
{
  public:
    /** @brief The number of arrray elements */
    llrp_u16_t                  m_nValue;
    /** @brief Pointer to the first array element */
    llrp_s32_t *                m_pValue;

    /** @brief Default Constructer. Sets members fields to zero */
    llrp_s32v_t(void);

    /** @brief Constructor. Allocates spaec for nValue arrays and sets to zero */
    llrp_s32v_t (
      unsigned int              nValue);

    /** @brief Copy Constructor */
    llrp_s32v_t (
      const llrp_s32v_t &       rOther);

    /** @brief Destructor. Deletes m_pValue in cases where it is allocated */
    ~llrp_s32v_t(void);

    /** @brief Assignement operator.  Used to copy an existing instance. */
    llrp_s32v_t &
    operator= (
      const llrp_s32v_t &       rValue);

  private:
    void
    copy (
      const llrp_s32v_t &       rOther);

    void
    reset (void);
};

/**
 *****************************************************************************
 **
 ** @brief Class for LLRP basic type u64v (vector of unsigned 64-bit values)
 **
 *****************************************************************************
 **/
class llrp_u64v_t
{
  public:
    /** @brief The number of arrray elements */
    llrp_u16_t                  m_nValue;
    /** @brief Pointer to the first array element */
    llrp_u64_t *                m_pValue;

    /** @brief Default Constructer. Sets members fields to zero */
    llrp_u64v_t(void);

    /** @brief Constructor. Allocates spaec for nValue arrays and sets to zero */
    llrp_u64v_t (
      unsigned int              nValue);

    /** @brief Copy Constructor */
    llrp_u64v_t (
      const llrp_u64v_t &       rOther);

    /** @brief Destructor. Deletes m_pValue in cases where it is allocated */
    ~llrp_u64v_t(void);

    /** @brief Assignement operator.  Used to copy an existing instance. */
    llrp_u64v_t &
    operator= (
      const llrp_u64v_t &       rValue);

  private:
    void
    copy (
      const llrp_u64v_t &       rOther);

    void
    reset (void);
};

/**
 *****************************************************************************
 **
 ** @brief Class for LLRP basic type s64v (vector of signed 64-bit values)
 **
 *****************************************************************************
 **/
class llrp_s64v_t
{
  public:
    /** @brief The number of arrray elements */
    llrp_u16_t                  m_nValue;
    /** @brief Pointer to the first array element */
    llrp_s64_t *                m_pValue;

    /** @brief Default Constructer. Sets members fields to zero */
    llrp_s64v_t(void);

    /** @brief Constructor. Allocates spaec for nValue arrays and sets to zero */
    llrp_s64v_t (
      unsigned int              nValue);

    /** @brief Copy Constructor */
    llrp_s64v_t (
      const llrp_s64v_t &       rOther);

    /** @brief Destructor. Deletes m_pValue in cases where it is allocated */
    ~llrp_s64v_t(void);

    /** @brief Assignement operator.  Used to copy an existing instance. */
    llrp_s64v_t &
    operator= (
      const llrp_s64v_t &       rValue);

  private:
    void
    copy (
      const llrp_s64v_t &       rOther);

    void
    reset (void);
};

/**
 *****************************************************************************
 **
 ** @brief Class for LLRP basic type u1v (vector of unsigned 1-bit values)
 **
 *****************************************************************************
 **/
class llrp_u1v_t
{
  public:
    /** @brief The number of arrray elements */
    llrp_u16_t                  m_nBit;
    /** @brief Pointer to the first array element */
    llrp_u8_t *                 m_pValue;

    /** @brief Default Constructer. Sets members fields to zero */
    llrp_u1v_t(void);

    /** @brief Constructor. Allocates spaec for nValue arrays and sets to zero */
    llrp_u1v_t (
      unsigned int              nBit);

    /** @brief Copy Constructor */
    llrp_u1v_t (
      const llrp_u1v_t &        rOther);

    /** @brief Destructor. Deletes m_pValue in cases where it is allocated */
    ~llrp_u1v_t(void);

    /** @brief Assignement operator.  Used to copy an existing instance. */
    llrp_u1v_t &
    operator= (
      const llrp_u1v_t &        rValue);

  private:
    void
    copy (
      const llrp_u1v_t &        rOther);

    void
    reset (void);
};

/**
 *****************************************************************************
 **
 ** @brief Class for LLRP basic type utf8v (vector of utf-8 encoded characters)
 **
 *****************************************************************************
 **/
class llrp_utf8v_t
{
  public:
    /** @brief The number of arrray elements */
    llrp_u16_t                  m_nValue;
    /** @brief Pointer to the first array element */
    llrp_utf8_t *               m_pValue;

    /** @brief Default Constructer. Sets members fields to zero */
    llrp_utf8v_t(void);

    /** @brief Constructor. Allocates spaec for nValue arrays and sets to zero */
    llrp_utf8v_t (
      unsigned int              nValue);

    /** @brief Copy Constructor */
    llrp_utf8v_t (
      const llrp_utf8v_t &      rOther);

    /** @brief Destructor. Deletes m_pValue in cases where it is allocated */
    ~llrp_utf8v_t(void);

    /** @brief Assignement operator.  Used to copy an existing instance. */
    llrp_utf8v_t &
    operator= (
      const llrp_utf8v_t &      rValue);

  private:
    void
    copy (
      const llrp_utf8v_t &      rOther);

    void
    reset (void);
};

/**
 *****************************************************************************
 **
 ** @brief Class for LLRP basic type bytesToEnd (vector of unsigned bytes)
 **
 *****************************************************************************
 **/
class llrp_bytesToEnd_t
{
  public:
    /** @brief The number of arrray elements */
    llrp_u16_t                  m_nValue;
    /** @brief Pointer to the first array element */
    llrp_byte_t *               m_pValue;

    /** @brief Default Constructer. Sets members fields to zero */
    llrp_bytesToEnd_t(void);

    /** @brief Constructor. Allocates spaec for nValue arrays and sets to zero */
    llrp_bytesToEnd_t (
      unsigned int              nValue);

    /** @brief Copy Constructor */
    llrp_bytesToEnd_t (
      const llrp_bytesToEnd_t & rOther);

    /** @brief Destructor. Deletes m_pValue in cases where it is allocated */
    ~llrp_bytesToEnd_t(void);

    /** @brief Assignement operator.  Used to copy an existing instance. */
    llrp_bytesToEnd_t &
    operator= (
      const llrp_bytesToEnd_t & rValue);

  private:
    void
    copy (
      const llrp_bytesToEnd_t & rOther);

    void
    reset (void);
};

/**
 *****************************************************************************
 **
 ** @brief Class for LLRP basic type u96  (96-bit value)
 **
 *****************************************************************************
 **/
class llrp_u96_t
{
  public:
    /** @brief Simple array of basic type llrp_u8_t */
    llrp_u8_t                   m_aValue[12];
};


/** @enum EResultCode
 *****************************************************************************
 **
 ** @brief Error result codes for LTK operations
 **
 ** These error codes are common across many LTKCPP functions.
 **
 *****************************************************************************
 **/
enum EResultCode
{
    RC_OK                       = 0,   /**< Success */
    RC_MiscError                = 100, /**< Error */
    RC_Botch,                          /**< Error */
    RC_SendIOError,                    /**< Error */
    RC_RecvIOError,                    /**< Error */
    RC_RecvEOF,                        /**< Error */
    RC_RecvTimeout,                    /**< Error */
    RC_RecvFramingError,               /**< Error */
    RC_RecvBufferOverflow,             /**< Error */
    RC_BadVersion,                     /**< Error */
    RC_MissingResponseType,            /**< Error */
    RC_UnknownMessageType,             /**< Error */
    RC_UnknownParameterType,           /**< Error */
    RC_ExcessiveLength,                /**< Error */
    RC_InvalidLength,                  /**< Error */
    RC_FieldUnderrun,                  /**< Error */
    RC_ReservedBitsUnderrun,           /**< Error */
    RC_FieldOverrun,                   /**< Error */
    RC_ReservedBitsOverrun,            /**< Error */
    RC_UnalignedBitField,              /**< Error */
    RC_UnalignedReservedBits,          /**< Error */
    RC_MessageAllocationFailed,        /**< Error */
    RC_ParameterAllocationFailed,      /**< Error */
    RC_FieldAllocationFailed,          /**< Error */
    RC_ExtraBytes,                     /**< Error */
    RC_MissingParameter,               /**< Error */
    RC_UnexpectedParameter,            /**< Error */
    RC_InvalidChoiceMember,            /**< Error */
    RC_EnrollBadTypeNumber,            /**< Error */
    RC_NotAllowedAtExtensionPoint,     /**< Error */
    RC_XMLInvalidNodeType,             /**< Error */
    RC_XMLMissingField,                /**< Error */
    RC_XMLExtraNode,                   /**< Error */
    RC_XMLInvalidFieldCharacters,      /**< Error */
    RC_XMLOutOfRange,                  /**< Error */
};

/**
 *****************************************************************************
 **
 ** @brief Class to return error details in LTKCPP operations
 **
 ** @ingroup LTKCoreElement
 **
 *****************************************************************************
 **/
class CErrorDetails
{
  public:
    /** @brief Result code from operation */
    EResultCode                 m_eResultCode;
    /** @brief Descriptive printable error string */
    const char *                m_pWhatStr;
    /** @brief If non-NULL this is the type descriptors for the errored type */
    const CTypeDescriptor *     m_pRefType;
    /** @brief If non-NULL this is the field descriptors for the errored field */
    const CFieldDescriptor *    m_pRefField;

    int                         m_OtherDetail;

    /** @brief Default Constructor */
    CErrorDetails (void);

    /** @brief Clears the contents of a CErrorDetails */
    void
    clear (void);

    /**
     ** @brief Convenience function used by the toolkit to set CErrorDetails
     **        when it encounters an missing parameters
     **/
    void
    missingParameter (
      const CTypeDescriptor *   pRefType);

    /**
     ** @brief Convenience function used by the toolkit to set CErrorDetails
     **        when it encounters an unexpected parameters
     **/
    void
    unexpectedParameter (
      const CParameter *        pParameter);

    /**
     ** @brief Convenience function used by the toolkit to set CErrorDetails
     **        result code and pWhatStr
     **/
    void
    resultCodeAndWhatStr (
      EResultCode               eResultCode,
      const char *              pWhatStr);
};

/**
 *****************************************************************************
 **
 ** @brief Describes the vendor information for vendor extension classes
 **
 *****************************************************************************
 **/
class CVendorDescriptor
{
  public:
    /** @brief Short name for the vendor, e.g. "Acme" */
    const char *                m_pName;

    /** @brief Vendor PEN of a custom message or parameter */
    llrp_u32_t                  m_VendorID;
};

/**
 *******************************************************************************
 ** @brief Describes the namespae information for converting LLRP messages to LTK-XML
 *****************************************************************************
 **/
class CNamespaceDescriptor
{
  public:
    /** @brief Short name for the namespace, e.g. "acmeNS" */
    const char *                m_pPrefix;

    /** @brief URI for the namespace, this is the true namespace name */
    const char *                m_pURI;

    /** @brief URI for the XSD (schema) for custom parameters and messages
     ** defined within the namespace */
    const char *                m_pSchemaLocation;
};



/**
 *****************************************************************************
 ** CTypeDescriptor
 **
 ** @brief Describes a message or parameter type.
 **
 **
 **  +-----------------------+
 **  |                       |
 **  | CTypeDescriptor       --------+   m_ppFieldDescriptorTable
 **  |                       |       |
 **  +-----------------------+       |
 **                                  |
 **              +-------------------+
 **              V
 **  +-----------------------+
 **  |                       |           [field number]
 **  ~ CFieldDescriptor *[]  --------+
 **  |                       |       |
 **  +-----------------------+       |
 **                                  |
 **              +-------------------+
 **              V
 **  +-----------------------+
 **  |                       |           Optional m_pEnumTable
 **  | CFieldDescriptor      --------+
 **  |                       |       |
 **  +-----------------------+       |
 **                                  |
 **              +-------------------+
 **              V
 **  +-----------------------+
 **  |                       |
 **  | SEnumTableEntry[]     |
 **  |                       |
 **  +-----------------------+
 **
 *****************************************************************************
 */
class CTypeDescriptor
{
  public:
    /** @brief TRUE for a message type, FALSE for a parameter type */
    llrp_bool_t                 m_bIsMessage;

    /** @brief String name of parameter/message type (e.g. "ROSpec") */
    const char *                      m_pName;

    /** @brief NULL=>standard LLRP, !NULL=>Vendor (PEN) of custom
     * message or parameter */
    const CVendorDescriptor *   m_pVendorDescriptor;

    /** @brief Namespace of message or parameter, for XML */
    const CNamespaceDescriptor *m_pNamespaceDescriptor;

    /** @brief Type number or, for custom, subtype number */
    llrp_u32_t                  m_TypeNum;

    /** @brief For messages (bIsMessage==TRUE), this is the type descriptor for
     * the corresponding response. NULL for a request or notification. */
    const CTypeDescriptor *     m_pResponseType;

    /** @brief Table of pointers to the field descriptors */
    const CFieldDescriptor * const * const m_ppFieldDescriptorTable;

    /** @brief Function to make an instance of the parameter or message */
    CElement *
    (*m_pfConstruct)(void);

    /** @brief Decoder, sometimes used when we want to decode w/o an instance */
    void
    (*m_pfDecodeFields) (
      CDecoderStream *          pDecoderStream,
      CElement *                pTargetElement);

  public:
    /** @brief Convenience function */
    inline CElement *
    constructElement(void) const
    {
       return (*m_pfConstruct)();
    }
};

/**
 *****************************************************************************
 * CFieldDescriptor
 *
 * @brief Describes a single field inside an LTKCPP message or parameters
 *****************************************************************************
 **/
class CFieldDescriptor
{
  public:
      /** @brief Describes the basic field type */
    enum FieldType {
        FT_U8,  FT_S8,  FT_U8V,  FT_S8V,
        FT_U16, FT_S16, FT_U16V, FT_S16V,
        FT_U32, FT_S32, FT_U32V, FT_S32V,
        FT_U64, FT_S64, FT_U64V, FT_S64V,

        FT_U1,  FT_U1V, FT_U2,   FT_U96,
        FT_UTF8V,

        FT_E1, FT_E2, FT_E8, FT_E16, FT_E32,
        FT_E8V,

        FT_BYTESTOEND,
    };

    /** @brief Describes the fields display format */
    enum FieldFormat {
        FMT_NORMAL,
        FMT_DEC,
        FMT_HEX,
        FMT_UTF8,
        FMT_DATETIME,
    };

    /** @brief A code for the field type */
    FieldType                   m_eFieldType;
    /** @brief A code for how the field should be formatted */
    FieldFormat                 m_eFieldFormat;
    /** @brief String name of field (e.g. "ROSpecID") */
    const char *                      m_pName;
    /** @brief NULL or ptr to table base for enumerated fields */
    const SEnumTableEntry *     m_pEnumTable;
};

/*
 * ltkcpp_hdrfd.c
 */
extern CFieldDescriptor     g_fdMessageHeader_Type;
extern CFieldDescriptor     g_fdMessageHeader_Length;
extern CFieldDescriptor     g_fdMessageHeader_MessageID;
extern CFieldDescriptor     g_fdMessageHeader_VendorPEN;
extern CFieldDescriptor     g_fdMessageHeader_Subtype;
extern CFieldDescriptor     g_fdParameterHeader_TVType;
extern CFieldDescriptor     g_fdParameterHeader_TLVType;
extern CFieldDescriptor     g_fdParameterHeader_TLVLength;
extern CFieldDescriptor     g_fdParameterHeader_VendorPEN;
extern CFieldDescriptor     g_fdParameterHeader_Subtype;


/*
 * SEnumTableEntry
 *
 * @brief Simple table of enumerations. Table is terminated
 * by an entry with pName==NULL.
 */
struct SEnumTableEntry
{
    /* String name, (e.g. "Immediate") */
    const char *                      pName;
    int                         Value;
};

/**
 *****************************************************************************
 ** CTypeRegistry
 **
 ** @brief A collection of pointers to CTypeDescriptors.
 **
 ** During decode operations types can be looked up
 ** by code (vendor and typenum) or by name.
 **
 ** @ingroup LTKCoreElement
 *****************************************************************************
 */
class CTypeRegistry
{
  public:
    /** @brief Standard messages subscripted by type number */
    const CTypeDescriptor *     m_apStdMessageTypeDescriptors[1024u];
    /** @brief Standard parameters subscripted by type number */
    const CTypeDescriptor *     m_apStdParameterTypeDescriptors[1024u];
    /** @brief List of custom message types */
    std::list<const CTypeDescriptor *> m_listCustomMessageTypeDescriptors;
    /** @brief List of custom parameters types */
    std::list<const CTypeDescriptor *> m_listCustomParameterTypeDescriptors;

    CTypeRegistry(void);
    ~CTypeRegistry(void);

    /** @brief Add a type descriptor to the registry */
    EResultCode
    enroll (
      const CTypeDescriptor *   pTypeDescriptor);

    /** @brief Lookup a standard message type descriptor. NULL=>not found */
    const CTypeDescriptor *
    lookupMessage (
      unsigned int              MessageTypeNum) const;

    /** @brief Lookup a standard parameter type descriptor. NULL=>not found */
    const CTypeDescriptor *
    lookupParameter (
      unsigned int              ParameterTypeNum) const;

    /** @brief Lookup a custom message type descriptor. NULL=>not found */
    const CTypeDescriptor *
    lookupCustomMessage (
      unsigned int              VendorID,
      unsigned int              MessageSubTypeNum) const;

    /** @brief Lookup a custom parameter type descriptor. NULL=>not found */
    const CTypeDescriptor *
    lookupCustomParameter (
      unsigned int              VendorID,
      unsigned int              ParameterSubTypeNum) const;

   /* look up the type descriptor*/
    const CTypeDescriptor *
    lookupByName (
      char *                    pName) const;

};



typedef std::list<CParameter *> tListOfParameters;

/**
 *****************************************************************************
 ** CElement
 **
 ** @brief This is the base class for all parameter and message types.
 **
 ** During decode, all subparameters found are entered
 ** on m_listAllSubParameters. Then the element's
 ** assimilateSubParameters() member function is called
 ** to iterate through the list and attach the parameters
 ** to specific fields.
 **
 ** The m_listAllSubParameters is a secondary reference to
 ** all the subparameters. When the element is destructed
 ** all parameters referenced by m_listAllSubParameters
 ** are deleted. The C++ intrinsic destructors take care
 ** of deleting the list itself.
 **
 ** During destruct the specific fields are not processed.
 ** The fields that are lists are automatically desctructed.
 ** So are the fields that are array types (i.e. utf8v) are
 ** also automatically destructed. The fields that are simple
 ** pointers are simply ignored.
 **
 ** This works because every parameter referenced by specific
 ** fields is also referenced by m_listAllSubParameters.
 **
 ** @ingroup LTKCoreElement
 *****************************************************************************
 */
class CElement
{
  public:
    /** @brief The type descriptor desribing this element */
    const CTypeDescriptor *     m_pType;

    /** @brief Element that encloses this one, NULL if this is top-level element */
    CElement *                  m_pParent;

    /** @brief List of all sub elements */
    tListOfParameters           m_listAllSubParameters;

  public:
    CElement(void);

    virtual
    ~CElement(void);

    /** @brief Add a subparameter to m_listAllSubParameters. Called
     ** by the accessor functions setXXX and addXXX.
     **
     ** @param[in] pParameter The list of sub-parameteters to add
     **
     ** @return void
     **/
    void
    addSubParameterToAllList (
      CParameter *              pParameter);

    /** @brief Delete a subparameter from m_listAllSubParameters. Called
     ** by the accessor functions setXXX (to remove prior reference)
     ** clearXXX().
     **
     ** @param[in] *pParameter  The parameter to delete
     **
     ** @return void
     **/
    void
    removeSubParameterFromAllList (
      CParameter *              pParameter);

    /** @brief Clear a list of subparameters. For each entry apply
     ** removeSubParameter().
     **
     ** @param[in] pParameterList  The list of parameters to clear
     **
     ** @return void
     **/
    void
    clearSubParameterList (
      tListOfParameters *       pParameterList);

    /** @brief Recursive tree walk. The callback is invoked for each element. */
    int
    walk (
      int                       (*pFunc)(
                                  const CElement *  pElement,
                                  void *            pArg),
      void *                    pArg,
      int                       iDepth,
      int                       nMaxDepth) const;


    /**
     ** @brief A wrapper around LLRP::toXMLString()
     ** @param[in] pBuffer Pointer to the buffer to store the result
     ** @param[in] nBuffer size of the result buffer
     ** @return EResultCode Result code from the operation
     **/
    EResultCode
    toXMLString (
      char *                    pBuffer,
      int                       nBuffer);


    /**
     ** @brief Virtual function provided by each specific element type
     ** to decode fields (simple values). Leaves pDecoderStream
     ** at first subparameter.
     **
     ** @param[in] pDecoderStream The stream from which to decode fields
     ** @return void
     **/
    virtual void
    decodeFields (
      CDecoderStream *          pDecoderStream) = 0;

    /** @brief Assimilates the decoded sub-parameters into m_listAllSubParameters
     **
     ** After fields are decoded, the CDecoder itself takes care
     ** of gathering the subparameters into m_listAllSubParameters.
     ** Once the end of the enclosing TLV (or message) is reached
     ** this assimilateSubParameters() function is called to
     ** create parameter refrences from the primary member variables.
     **
     ** @param[out] pError Error details for encoded stream
     **/
    virtual void
    assimilateSubParameters (
      CErrorDetails *           pError) = 0;

    /** @brief Encode fields and subparameters
     **
     ** @param[in] pEncoderStream Stream to encode
     ** @return void
     **/
    virtual void
    encode (
      CEncoderStream *          pEncoderStream) const = 0;
};


/**
 *****************************************************************************
 **
 ** @brief  Base Class for All LLRP LTK Messages
 **
 ** @ingroup LTKCoreElement
 *****************************************************************************
 **/
class CMessage : public CElement
{
  public:
    CMessage(void);

    /**
     ** @brief Sets the LLRP Message ID for the Message
     **/
    inline void
    setMessageID (
      llrp_u32_t                MessageID)
    {
        m_MessageID = MessageID;
    }

    /**
     ** @brief Gets the current LLRP Message ID for the Message
     **/
    inline llrp_u32_t
    getMessageID (void) const
    {
        return m_MessageID;
    }

  protected:
    llrp_u32_t                  m_MessageID;

};

/**
 *****************************************************************************
 **
 ** @brief  Base Class for All LLRP LTK Parameters
 **
 ** @ingroup LTKCoreElement
 *****************************************************************************
 **/
class CParameter : public CElement
{
  public:
    /** @brief Validate whether the element is allowed as a base type in the enclosing element
     **
     ** @param[in] pEnclosingTypeDescriptor A pointer to the enclosing type
     **
     ** @return TRUE if the element is allowed to be encoded within the EnclosingElement
     ** @return FALSE if the element is not allowed to be encoded within the EnclosingElement
     **/
    virtual llrp_bool_t
    isAllowedIn (
      const CTypeDescriptor *   pEnclosingTypeDescriptor) const;

    /** @brief Validate whether the element is allowed as an extension in the enclosing element
     **
     ** @param[in] pEnclosingTypeDescriptor A pointer to the enclosing type
     **
     ** @return TRUE if the element is allowed to be encoded within the EnclosingElement
     ** @return FALSE if the element is not allowed to be encoded within the EnclosingElement
     **/
    llrp_bool_t
    isAllowedExtension (
      const CTypeDescriptor *   pEnclosingTypeDescriptor);
};

/**
 **
 ** @brief LTK LLRP Decoder class
 **
 ** By way of example, this is how the CDecoder and CDecoderStream
 ** classes work. This example is for decoding a binary frame.
 **
 **      +-------------------+               +---------------+
 **      |                   |               |               |
 **      |   CDecoder        --------------->| CTypeRegistry |
 **      |                   |               |               |
 **      +--|----------------+               +---------------+
 **         |    ^
 ** pointer |    |
 ** to next |    |   +-------------------+
 ** byte    |    |   |                   |           pointer to msg end
 **         |    ^----  CDecoderStream   ----------------+
 **         |    |   |                   |               |
 **         |    |   +-------------------+               |
 **         |    |             ^                         |
 **         |    |             |                         |
 **         |    |   +-------------------+   ptr to      |
 **         |    |   |                   |   TLV end     |
 **         |    ^----  CDecoderStream   ------------+   |
 **         |    |   |                   |           |   |
 **         |    |   +-------------------+           |   |
 **         |    |             ^                     |   |
 **         |    |             |                     |   |
 **         |    |   +-------------------+           |   |
 **         |    |   |                   |           |   |
 **         |    ^----  CDecoderStream   --------+   |   |
 **         |        |                   |       |   |   |
 **         |        +-------------------+       |   |   |
 **         |                                    |   |   |
 **         +-------------------+                |   |   |
 **                             |                |   |   |
 **                             v                v   v   v
 **  +---------------------------------------------------------------+
 **  |                   Binary Frame Buffer                         |
 **  +---------------------------------------------------------------+
 **
 **                            |_________________|          Nested TLVs
 **        |________________||___________________________|  Nested TLVs
 **    |_________________________________________________|  Message
 **
 **
 ** In the case of binary frame the references are to
 ** bytes within the buffer. Lookups are by type number.
 **
 ** In the case of an XML DOM tree, the references are
 ** to nodes in the DOM tre. Lookups are by string name.

 ** @ingroup LTKCoreElement
 **/
class CDecoder
{
  public:
    /** @brief The error details from the last decode operation */
    CErrorDetails               m_ErrorDetails;

    CDecoder (void);

    /** @brief Constructor to build a decoder for the given type registry */
    CDecoder (
      const CTypeRegistry *     pTypeRegistry);

    virtual
    ~CDecoder (void) = 0;

    /** @brief Decodes a message into the return value CMessage */
    virtual CMessage *
    decodeMessage (void) = 0;

  protected:
    const CTypeRegistry *       m_pRegistry;
};

/**
 *****************************************************************************
 **
 ** @brief  Class that decodes messages into internal LTK CElements
 **
 *****************************************************************************
 **/
class CDecoderStream
{
  public:
    virtual
    ~CDecoderStream(void) = 0;

    /*
     * 8-bit types
     */

    /** @brief Decodes an llrp_u8_t (unsigned 8-bit number) into the specified field descriptor */
    virtual llrp_u8_t
    get_u8 (
      const CFieldDescriptor *  pFieldDesc) = 0;

    /** @brief Decodes a llrp_s8_t (signed 8-bit number) into the specified field descriptor */
    virtual llrp_s8_t
    get_s8 (
      const CFieldDescriptor *  pFieldDesc) = 0;

    /** @brief Decodes an llrp_u8v_t into the specified field descriptor */
    virtual llrp_u8v_t
    get_u8v (
      const CFieldDescriptor *  pFieldDesc) = 0;

    /** @brief Decodes an llrp_s8v_t into the specified field descriptor */
    virtual llrp_s8v_t
    get_s8v (
      const CFieldDescriptor *  pFieldDesc) = 0;

    /*
     * 16-bit types
     */

    /** @brief Decodes an llrp_u16_t (unsigned 16-bit number) into the specified field descriptor */
    virtual llrp_u16_t
    get_u16 (
      const CFieldDescriptor *  pFieldDesc) = 0;

    /** @brief Decodes a llrp_s16_t (signed 16-bit number) into the specified field descriptor */
    virtual llrp_s16_t
    get_s16 (
      const CFieldDescriptor *  pFieldDesc) = 0;

    /** @brief Decodes a llrp_u16v_t into the specified field descriptor */
    virtual llrp_u16v_t
    get_u16v (
      const CFieldDescriptor *  pFieldDesc) = 0;

    /** @brief Decodes an llrp_u16v_t into the specified field descriptor */
    virtual llrp_s16v_t
    get_s16v (
      const CFieldDescriptor *  pFieldDesc) = 0;

    /*
     * 32-bit types
     */

    /** @brief Decodes an llrp_u32_t (unsigned 32-bit number) into the specified field descriptor */
    virtual llrp_u32_t
    get_u32 (
      const CFieldDescriptor *  pFieldDesc) = 0;

    /** @brief Decodes an llrp_s32_t (signed 32-bit number) into the specified field descriptor */
    virtual llrp_s32_t
    get_s32 (
      const CFieldDescriptor *  pFieldDesc) = 0;

    /** @brief Decodes an llrp_u32v_t into the specified field descriptor */
    virtual llrp_u32v_t
    get_u32v (
      const CFieldDescriptor *  pFieldDesc) = 0;

    /** @brief Decodes an llrp_s32v_t into the specified field descriptor */
    virtual llrp_s32v_t
    get_s32v (
      const CFieldDescriptor *  pFieldDesc) = 0;

    /*
     * 64-bit types
     */

    /** @brief Decodes an llrp_u64_t (unsigned 64-bit number) into the specified field descriptor */
    virtual llrp_u64_t
    get_u64 (
      const CFieldDescriptor *  pFieldDesc) = 0;

    /** @brief Decodes an llrp_s64_t (signed 64-bit number) into the specified field descriptor */
    virtual llrp_s64_t
    get_s64 (
      const CFieldDescriptor *  pFieldDesc) = 0;

    /** @brief Decodes an llrp_u64v_t into the specified field descriptor */
    virtual llrp_u64v_t
    get_u64v (
      const CFieldDescriptor *  pFieldDesc) = 0;

    /** @brief Decodes an llrp_s64v_t into the specified field descriptor */
    virtual llrp_s64v_t
    get_s64v (
      const CFieldDescriptor *  pFieldDesc) = 0;

    /*
     * Special types
     */

    /** @brief Decodes an llrp_u1_t (unsigned 1 bit number) into the specified field descriptor */
    virtual llrp_u1_t
    get_u1 (
      const CFieldDescriptor *  pFieldDesc) = 0;

    /** @brief Decodes an llrp_u1v_t into the specified field descriptor */
    virtual llrp_u1v_t
    get_u1v (
      const CFieldDescriptor *  pFieldDesc) = 0;

    /** @brief Decodes an llrp_u2_t into the specified field descriptor */
    virtual llrp_u2_t
    get_u2 (
      const CFieldDescriptor *  pFieldDesc) = 0;

    /** @brief Decodes an llrp_u96_t into the specified field descriptor */
    virtual llrp_u96_t
    get_u96 (
      const CFieldDescriptor *  pFieldDesc) = 0;

    /** @brief Decodes an llrp_utf8v_t into the specified field descriptor */
    virtual llrp_utf8v_t
    get_utf8v (
      const CFieldDescriptor *  pFieldDesc) = 0;

    /** @brief Decodes an llrp_bytesToEnd_t into the specified field descriptor */
    virtual llrp_bytesToEnd_t
    get_bytesToEnd (
      const CFieldDescriptor *  pFieldDesc) = 0;

    /*
     * Enumerated types of various sizes
     */

    /** @brief Decodes a 1 bit enumerated field into the specified field descriptor */
    virtual int
    get_e1 (
      const CFieldDescriptor *  pFieldDesc) = 0;

    /** @brief Decodes a 2 bit enumerated field into the specified field descriptor */
    virtual int
    get_e2 (
      const CFieldDescriptor *  pFieldDesc) = 0;

    /** @brief Decodes a 8 bit enumerated field into the specified field descriptor */
    virtual int
    get_e8 (
      const CFieldDescriptor *  pFieldDesc) = 0;

    /** @brief Decodes a 16 bit enumerated field into the specified field descriptor */
    virtual int
    get_e16 (
      const CFieldDescriptor *  pFieldDesc) = 0;

    /** @brief Decodes a 32 bit enumerated field into the specified field descriptor */
    virtual int
    get_e32 (
      const CFieldDescriptor *  pFieldDesc) = 0;

    /** @brief Decodes an enumerated u8v field into the specified field descriptor */
    virtual llrp_u8v_t
    get_e8v (
      const CFieldDescriptor *  pFieldDesc) = 0;

    /*
     * Reserved means some number of bits
     */
    /* @brief skips a number of reserved bits and discards during the encode/decode process */
    virtual void
    get_reserved (
      unsigned int          nBits) = 0;
};

/**
 *****************************************************************************
 ** @brief LTK LLRP Encoder class
 **
 ** @ingroup LTKCoreElement
 *****************************************************************************
 **/
class CEncoder
{
  public:
    /** @brief internal storage of encoding errors */
    CErrorDetails               m_ErrorDetails;

    /** @brief Constructor */
    CEncoder (void);

    /** @brief destructor */
    virtual
    ~CEncoder (void) = 0;

    /** @brief encodes the tree formed by pElement */
    virtual void
    encodeElement (
      const CElement *          pElement) = 0;
};

/**
 *****************************************************************************
 **
 ** @brief  Class that encodes internal LTK CElements into messages
 **
 *****************************************************************************
 **/
class CEncoderStream
{
  public:
    virtual
    ~CEncoderStream(void) = 0;

    /** @brief Encodes the requiredSubParameter into the encode stream
     ** @param[in] pParameter   The parameter instance to encode
     ** @param[in] pRefType     The type of the parameter:w
     **/
    virtual void
    putRequiredSubParameter (
      const CParameter *        pParameter,
      const CTypeDescriptor *   pRefType) = 0;

    /** @brief Encodes the optionalSubParameter into the encode stream
     ** @param[in] pParameter   The parameter instance to encode
     ** @param[in] pRefType     The type of the parameter:w
     **/
    virtual void
    putOptionalSubParameter (
      const CParameter *        pParameter,
      const CTypeDescriptor *   pRefType) = 0;

    /** @brief Encodes the requiredSubParameterList into the encode stream
     ** @param[in] pParameterList   The parameter instance to encode
     ** @param[in] pRefType     The type of the parameter:w
     **/
    virtual void
    putRequiredSubParameterList (
      const tListOfParameters * pParameterList,
      const CTypeDescriptor *   pRefType) = 0;

    /** @brief Encodes the optionalSubParameterList into the encode stream
     ** @param[in] pParameterList   The parameter instance to encode
     ** @param[in] pRefType     The type of the parameter:w
     **/
    virtual void
    putOptionalSubParameterList (
      const tListOfParameters * pParameterList,
      const CTypeDescriptor *   pRefType) = 0;

    /*
     * 8-bit types
     */

    /** @brief Encodes an llrp_u8_t (unsigned 8-bit number) into the encode stream
     ** @param[in] Value      The value to encode
     ** @param[in] pFieldDesc The field descriptor that describes the encoding
     **/
    virtual void
    put_u8 (
      llrp_u8_t                 Value,
      const CFieldDescriptor *  pFieldDesc) = 0;

    /** @brief Encodes an llrp_s8_t (signed 8-bit number) into the encode stream
     ** @param[in] Value      The value to encode
     ** @param[in] pFieldDesc The field descriptor that describes the encoding
     **/
    virtual void
    put_s8 (
      llrp_s8_t                 Value,
      const CFieldDescriptor *  pFieldDesc) = 0;

    /** @brief Encodes an llrp_u8v_t into the encode stream
     ** @param[in] Value      The value to encode
     ** @param[in] pFieldDesc The field descriptor that describes the encoding
     **/
    virtual void
    put_u8v (
      llrp_u8v_t                Value,
      const CFieldDescriptor *  pFieldDesc) = 0;

    /** @brief Encodes an llrp_u8_t (unsigned 8-bit number) into the encode stream
     ** @param[in] Value      The value to encode
     ** @param[in] pFieldDesc The field descriptor that describes the encoding
     **/
    virtual void
    put_s8v (
      llrp_s8v_t                Value,
      const CFieldDescriptor *  pFieldDesc) = 0;

    /*
     * 16-bit types
     */

    /** @brief Encodes an llrp_u16_t (unsigned 16-bit number) into the encode stream
     ** @param[in] Value      The value to encode
     ** @param[in] pFieldDesc The field descriptor that describes the encoding
     **/
    virtual void
    put_u16 (
      llrp_u16_t                Value,
      const CFieldDescriptor *  pFieldDesc) = 0;

    /** @brief Encodes an llrp_s16_t (signed 16-bit number) into the encode stream
     ** @param[in] Value      The value to encode
     ** @param[in] pFieldDesc The field descriptor that describes the encoding
     **/
    virtual void
    put_s16 (
      llrp_s16_t                Value,
      const CFieldDescriptor *  pFieldDesc) = 0;

    /** @brief Encodes an llrp_u16v_t into the encode stream
     ** @param[in] Value      The value to encode
     ** @param[in] pFieldDesc The field descriptor that describes the encoding
     **/
    virtual void
    put_u16v (
      llrp_u16v_t               Value,
      const CFieldDescriptor *  pFieldDesc) = 0;

    /** @brief Encodes an put_s16v into the encode stream
     ** @param[in] Value      The value to encode
     ** @param[in] pFieldDesc The field descriptor that describes the encoding
     **/
    virtual void
    put_s16v (
      llrp_s16v_t               Value,
      const CFieldDescriptor *  pFieldDesc) = 0;

    /*
     * 32-bit types
     */

    /** @brief Encodes an llrp_u32_t (unsigned 32-bit number) into the encode stream
     ** @param[in] Value      The value to encode
     ** @param[in] pFieldDesc The field descriptor that describes the encoding
     **/
    virtual void
    put_u32 (
      llrp_u32_t                Value,
      const CFieldDescriptor *  pFieldDesc) = 0;

    /** @brief Encodes an llrp_s32_t (signed 32-bit number) into the encode stream
     ** @param[in] Value      The value to encode
     ** @param[in] pFieldDesc The field descriptor that describes the encoding
     **/
    virtual void
    put_s32 (
      llrp_s32_t                Value,
      const CFieldDescriptor *  pFieldDesc) = 0;

    /** @brief Encodes an llrp_u32v_t into the encode stream
     ** @param[in] Value      The value to encode
     ** @param[in] pFieldDesc The field descriptor that describes the encoding
     **/
    virtual void
    put_u32v (
      llrp_u32v_t               Value,
      const CFieldDescriptor *  pFieldDesc) = 0;

    /** @brief Encodes an llrp_s32v_t into the encode stream
     ** @param[in] Value      The value to encode
     ** @param[in] pFieldDesc The field descriptor that describes the encoding
     **/
    virtual void
    put_s32v (
      llrp_s32v_t               Value,
      const CFieldDescriptor *  pFieldDesc) = 0;

    /*
     * 64-bit types
     */

    /** @brief Encodes an llrp_u64_t (unsigned 64-bit number) into the encode stream
     ** @param[in] Value      The value to encode
     ** @param[in] pFieldDesc The field descriptor that describes the encoding
     **/
    virtual void
    put_u64 (
      llrp_u64_t                Value,
      const CFieldDescriptor *  pFieldDesc) = 0;

    /** @brief Encodes an llrp_s64_t (signed 64-bit number) into the encode stream
     ** @param[in] Value      The value to encode
     ** @param[in] pFieldDesc The field descriptor that describes the encoding
     **/
    virtual void
    put_s64 (
      llrp_s64_t                Value,
      const CFieldDescriptor *  pFieldDesc) = 0;

    /** @brief Encodes an llrp_u64v_t into the encode stream
     ** @param[in] Value      The value to encode
     ** @param[in] pFieldDesc The field descriptor that describes the encoding
     **/
    virtual void
    put_u64v (
      llrp_u64v_t               Value,
      const CFieldDescriptor *  pFieldDesc) = 0;

    /** @brief Encodes an llrp_s64v_t into the encode stream
     ** @param[in] Value      The value to encode
     ** @param[in] pFieldDesc The field descriptor that describes the encoding
     **/
    virtual void
    put_s64v (
      llrp_s64v_t               Value,
      const CFieldDescriptor *  pFieldDesc) = 0;

    /*
     * Special types
     */

    /** @brief Encodes an llrp_u64_t (unsigned 64-bit number) into the encode stream
     ** @param[in] Value      The value to encode
     ** @param[in] pFieldDesc The field descriptor that describes the encoding
     **/
    virtual void
    put_u1 (
      llrp_u1_t                 Value,
      const CFieldDescriptor *  pFieldDesc) = 0;

    /** @brief Encodes an llrp_u1v_t (unsigned 1-bit number) into the encode stream
     ** @param[in] Value      The value to encode
     ** @param[in] pFieldDesc The field descriptor that describes the encoding
     **/
    virtual void
    put_u1v (
      llrp_u1v_t                Value,
      const CFieldDescriptor *  pFieldDesc) = 0;

    /** @brief Encodes an llrp_u2_t (unsigned 2-bit number) into the encode stream
     ** @param[in] Value      The value to encode
     ** @param[in] pFieldDesc The field descriptor that describes the encoding
     **/
    virtual void
    put_u2 (
      llrp_u2_t                 Value,
      const CFieldDescriptor *  pFieldDesc) = 0;

    /** @brief Encodes an llrp_u96_t (unsigned 96-bit number) into the encode stream
     ** @param[in] Value      The value to encode
     ** @param[in] pFieldDesc The field descriptor that describes the encoding
     **/
    virtual void
    put_u96 (
      llrp_u96_t                Value,
      const CFieldDescriptor *  pFieldDesc) = 0;

    /** @brief Encodes an put_utf8v (utf8 character string) into the encode stream
     ** @param[in] Value      The value to encode
     ** @param[in] pFieldDesc The field descriptor that describes the encoding
     **/
    virtual void
    put_utf8v (
      llrp_utf8v_t              Value,
      const CFieldDescriptor *  pFieldDesc) = 0;

    /** @brief Encodes an put_bytesToEnd into the encode stream
     ** @param[in] Value      The value to encode
     ** @param[in] pFieldDesc The field descriptor that describes the encoding
     **/
    virtual void
    put_bytesToEnd (
      llrp_bytesToEnd_t         Value,
      const CFieldDescriptor *  pFieldDesc) = 0;

    /*
     * Enumerated types of various sizes
     */

    /** @brief Encodes a 1-bit enumerated type into the encode stream
     ** @param[in] eValue      The value to encode
     ** @param[in] pFieldDesc The field descriptor that describes the encoding
     **/
    virtual void
    put_e1 (
      int                       eValue,
      const CFieldDescriptor *  pFieldDesc) = 0;

    /** @brief Encodes a 2-bit enumerated type into the encode stream
     ** @param[in] eValue      The value to encode
     ** @param[in] pFieldDesc The field descriptor that describes the encoding
     **/
    virtual void
    put_e2 (
      int                       eValue,
      const CFieldDescriptor *  pFieldDesc) = 0;

    /** @brief Encodes an 8-bit enumerated type into the encode stream
     ** @param[in] eValue      The value to encode
     ** @param[in] pFieldDesc The field descriptor that describes the encoding
     **/
    virtual void
    put_e8 (
      int                       eValue,
      const CFieldDescriptor *  pFieldDesc) = 0;

    /** @brief Encodes a 16-bit enumerated type into the encode stream
     ** @param[in] eValue      The value to encode
     ** @param[in] pFieldDesc The field descriptor that describes the encoding
     **/
    virtual void
    put_e16 (
      int                       eValue,
      const CFieldDescriptor *  pFieldDesc) = 0;

    /** @brief Encodes a32-bit enumerated type into the encode stream
     ** @param[in] eValue      The value to encode
     ** @param[in] pFieldDesc The field descriptor that describes the encoding
     **/
    virtual void
    put_e32 (
      int                       eValue,
      const CFieldDescriptor *  pFieldDesc) = 0;

    /** @brief Encodes an enumerated u8v type into the encode stream
     ** @param[in] Value      The value to encode
     ** @param[in] pFieldDesc The field descriptor that describes the encoding
     **/
    virtual void
    put_e8v (
      llrp_u8v_t                Value,
      const CFieldDescriptor *  pFieldDesc) = 0;

    /*
     * Reserved types are some number of bits
     */

    /* @brief skips a number of reserved bits and discards during the encode/decode process */
    virtual void
    put_reserved (
      unsigned int              nBits) = 0;
};


/*
** @brief Encodes a CElement into an XML string
**
** @param[in]  pElement      The CElement to encode to XML
** @param[out] pBuffer       The pointer to the buffer in which to place the XML result
** @param[in]  nBuffer       The size of the buffer available to hold the XML
**
** @return EResultCode Result code from the operation
*/
extern EResultCode
toXMLString (
  const CElement *              pElement,
  char *                        pBuffer,
  int                           nBuffer);

}; /* namespace LLRP */
