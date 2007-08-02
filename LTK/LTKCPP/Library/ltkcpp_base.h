
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



#define BOOL int

namespace LLRP
{

/*
 * Forward declarations of structs and classes
 */

class CErrorDetails;
struct SEnumTableEntry;
class CFieldDescriptor;
class CTypeDescriptor;
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

class llrp_u8v_t
{
  public:
    llrp_u16_t                  m_nValue;
    llrp_u8_t *                 m_pValue;

    llrp_u8v_t(void);

    llrp_u8v_t (
      unsigned int              nValue);

    llrp_u8v_t (
      const llrp_u8v_t &        rOther);

    ~llrp_u8v_t(void);

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

class llrp_s8v_t
{
  public:
    llrp_u16_t                  m_nValue;
    llrp_s8_t *                 m_pValue;

    llrp_s8v_t(void);

    llrp_s8v_t (
      unsigned int              nValue);

    llrp_s8v_t (
      const llrp_s8v_t &        rOther);

    ~llrp_s8v_t(void);

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

class llrp_u16v_t
{
  public:
    llrp_u16_t                  m_nValue;
    llrp_u16_t *                m_pValue;

    llrp_u16v_t(void);

    llrp_u16v_t (
      unsigned int              nValue);

    llrp_u16v_t (
      const llrp_u16v_t &       rOther);

    ~llrp_u16v_t(void);

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

class llrp_s16v_t
{
  public:
    llrp_u16_t                  m_nValue;
    llrp_s16_t *                m_pValue;

    llrp_s16v_t(void);

    llrp_s16v_t (
      unsigned int              nValue);

    llrp_s16v_t (
      const llrp_s16v_t &       rOther);

    ~llrp_s16v_t(void);

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

class llrp_u32v_t
{
  public:
    llrp_u16_t                  m_nValue;
    llrp_u32_t *                m_pValue;

    llrp_u32v_t(void);

    llrp_u32v_t (
      unsigned int              nValue);

    llrp_u32v_t (
      const llrp_u32v_t &       rOther);

    ~llrp_u32v_t(void);

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

class llrp_s32v_t
{
  public:
    llrp_u16_t                  m_nValue;
    llrp_s32_t *                m_pValue;

    llrp_s32v_t(void);

    llrp_s32v_t (
      unsigned int              nValue);

    llrp_s32v_t (
      const llrp_s32v_t &       rOther);

    ~llrp_s32v_t(void);

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

class llrp_u64v_t
{
  public:
    llrp_u16_t                  m_nValue;
    llrp_u64_t *                m_pValue;

    llrp_u64v_t(void);

    llrp_u64v_t (
      unsigned int              nValue);

    llrp_u64v_t (
      const llrp_u64v_t &       rOther);

    ~llrp_u64v_t(void);

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

class llrp_s64v_t
{
  public:
    llrp_u16_t                  m_nValue;
    llrp_s64_t *                m_pValue;

    llrp_s64v_t(void);

    llrp_s64v_t (
      unsigned int              nValue);

    llrp_s64v_t (
      const llrp_s64v_t &       rOther);

    ~llrp_s64v_t(void);

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

class llrp_u1v_t
{
  public:
    llrp_u16_t                  m_nBit;
    llrp_u8_t *                 m_pValue;

    llrp_u1v_t(void);

    llrp_u1v_t (
      unsigned int              nBit);

    llrp_u1v_t (
      const llrp_u1v_t &        rOther);

    ~llrp_u1v_t(void);

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

class llrp_utf8v_t
{
  public:
    llrp_u16_t                  m_nValue;
    llrp_utf8_t *               m_pValue;

    llrp_utf8v_t(void);

    llrp_utf8v_t (
      unsigned int              nValue);

    llrp_utf8v_t (
      const llrp_utf8v_t &      rOther);

    ~llrp_utf8v_t(void);

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

class llrp_bytesToEnd_t
{
  public:
    llrp_u16_t                  m_nValue;
    llrp_byte_t *               m_pValue;

    llrp_bytesToEnd_t(void);

    llrp_bytesToEnd_t (
      unsigned int              nValue);

    llrp_bytesToEnd_t (
      const llrp_bytesToEnd_t & rOther);

    ~llrp_bytesToEnd_t(void);

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

class llrp_u96_t
{
  public:
    llrp_u8_t                   m_aValue[12];
};


enum EResultCode
{
    RC_OK                       = 0,
    RC_MiscError                = 100,
    RC_Botch,
    RC_SendIOError,
    RC_RecvIOError,
    RC_RecvEOF,
    RC_RecvTimeout,
    RC_RecvFramingError,
    RC_BadVersion,
    RC_UnknownMessageType,
    RC_UnknownParameterType,
    RC_ExcessiveLength,
    RC_InvalidLength,
    RC_FieldUnderrun,
    RC_ReservedBitsUnderrun,
    RC_FieldOverrun,
    RC_ReservedBitsOverrun,
    RC_UnalignedBitField,
    RC_UnalignedReservedBits,
    RC_MessageAllocationFailed,
    RC_ParameterAllocationFailed,
    RC_FieldAllocationFailed,
    RC_ExtraBytes,
    RC_MissingParameter,
    RC_UnexpectedParameter,
    RC_InvalidChoiceMember,
    RC_EnrollBadTypeNumber,
};

class CErrorDetails
{
  public:
    EResultCode                 m_eResultCode;
    const char *                m_pWhatStr;
    const CTypeDescriptor *     m_pRefType;
    const CFieldDescriptor *    m_pRefField;
    int                         m_OtherDetail;

    CErrorDetails (void);

    void
    clear (void);

    void
    missingParameter (
      const CTypeDescriptor *   pRefType);

    void
    unexpectedParameter (
      const CParameter *        pParameter);

    void
    resultCodeAndWhatStr (
      EResultCode               eResultCode,
      const char *              pWhatStr);
};


/*
 *
 *  +-----------------------+
 *  |                       |
 *  | CTypeDescriptor       --------+   m_ppFieldDescriptorTable
 *  |                       |       |
 *  +-----------------------+       |
 *                                  |
 *              +-------------------+
 *              V
 *  +-----------------------+
 *  |                       |           [field number]
 *  ~ CFieldDescriptor *[]  --------+
 *  |                       |       |
 *  +-----------------------+       |
 *                                  |
 *              +-------------------+
 *              V
 *  +-----------------------+
 *  |                       |           Optional m_pEnumTable
 *  | CFieldDescriptor      --------+
 *  |                       |       |
 *  +-----------------------+       |
 *                                  |
 *              +-------------------+
 *              V
 *  +-----------------------+
 *  |                       |
 *  | SEnumTableEntry[]     |
 *  |                       |
 *  +-----------------------+
 *
 */




/*
 * CTypeDescriptor
 *
 * Describes a message or parameter type.
 */
class CTypeDescriptor
{
  public:
    /* TRUE for a message type, FALSE for a parameter type */
    llrp_bool_t                 m_bIsMessage;
    /* String name of parameter/message type (e.g. "ROSpec") */
    char *                      m_pName;
    /* 0=>standard LLRP, !0=>Vendor PEN of custom message or parameter */
    llrp_u32_t                  m_VendorID;
    /* Type number or, for custom, subtype number */
    llrp_u32_t                  m_TypeNum;

    /* Table of pointers to the field descriptors */
    const CFieldDescriptor * const * const m_ppFieldDescriptorTable;

    /* Function to make an instance of the parameter or message */
    CElement *
    (*m_pfConstruct)(void);

    /* Decoder, sometimes used when we want to decode w/o an instance */
    void
    (*m_pfDecodeFields) (
      CDecoderStream *          pDecoderStream,
      CElement *                pTargetElement);

  public:
    /* Convenience function */
    inline CElement *
    constructElement(void) const
    {
       return (*m_pfConstruct)();
    }
};

/*
 * CFieldDescriptor
 *
 * Describes a single field.
 */
class CFieldDescriptor
{
  public:
    enum FieldType {
        FT_U8,  FT_S8,  FT_U8V,  FT_S8V,
        FT_U16, FT_S16, FT_U16V, FT_S16V,
        FT_U32, FT_S32, FT_U32V, FT_S32V,
        FT_U64, FT_S64, FT_U64V, FT_S64V,

        FT_U1,  FT_U1V, FT_U2,   FT_U96,
        FT_UTF8V,

        FT_E1, FT_E2, FT_E8, FT_E16, FT_E32,
        FT_BYTESTOEND,
    };

    /* A code for the field type */
    FieldType                   m_eFieldType;
    /* String name of field (e.g. "ROSpecID") */
    char *                      m_pName;
    /* NULL or ptr to table base for enumerated fields */
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
 * Simple table of enumerations. Table is terminated
 * by an entry with pName==NULL.
 */
struct SEnumTableEntry
{
    /* String name, (e.g. "Immediate") */
    char *                      pName;
    int                         Value;
};

/*
 * CTypeRegistry
 *
 * A collection of pointers to CTypeDescriptors.
 * During decode operations types can be looked up
 * by code (vendor and typenum) or by name.
 */
class CTypeRegistry
{
  public:
    /* Standard messages subscripted by type number */
    const CTypeDescriptor *     m_apStdMessageTypeDescriptors[1024u];
    /* Standard parameters subscripted by type number */
    const CTypeDescriptor *     m_apStdParameterTypeDescriptors[1024u];
    /* List of custom message types */
    std::list<const CTypeDescriptor *> m_listCustomMessageTypeDescriptors;
    /* List of custom parameters types */
    std::list<const CTypeDescriptor *> m_listCustomParameterTypeDescriptors;

    CTypeRegistry(void);
    ~CTypeRegistry(void);

    /* Add a type descriptor to the registry */
    EResultCode
    enroll (
      const CTypeDescriptor *   pTypeDescriptor);

    /* Lookup a standard message type descriptor. NULL=>not found */
    const CTypeDescriptor *
    lookupMessage (
      unsigned int              MessageTypeNum) const;

    /* Lookup a standard parameter type descriptor. NULL=>not found */
    const CTypeDescriptor *
    lookupParameter (
      unsigned int              ParameterTypeNum) const;

    /* Lookup a custom message type descriptor. NULL=>not found */
    const CTypeDescriptor *
    lookupCustomMessage (
      unsigned int              VendorID,
      unsigned int              MessageSubTypeNum) const;

    /* Lookup a custom parameter type descriptor. NULL=>not found */
    const CTypeDescriptor *
    lookupCustomParameter (
      unsigned int              VendorID,
      unsigned int              ParameterSubTypeNum) const;
};


typedef std::list<CParameter *> tListOfParameters;

/*
 * CElement
 *
 * This is the base class for all parameter and message types.
 *
 * During decode, all subparameters found are entered
 * on m_listAllSubParameters. Then the element's
 * assimilateSubParameters() member function is called
 * to iterate through the list and attach the parameters
 * to specific fields.
 *
 * The m_listAllSubParameters is a secondary reference to
 * all the subparameters. When the element is destructed
 * all parameters referenced by m_listAllSubParameters
 * are deleted. The C++ intrinsic destructors take care
 * of deleting the list itself.
 *
 * During destruct the specific fields are not processed.
 * The fields that are lists are automatically desctructed.
 * So are the fields that are array types (i.e. utf8v) are
 * also automatically destructed. The fields that are simple
 * pointers are simply ignored.
 *
 * This works because every parameter referenced by specific
 * fields is also referenced by m_listAllSubParameters.
 */

class CElement
{
  public:
    /* The type descriptor desribing this element */
    const CTypeDescriptor *     m_pType;

    /* Element that encloses this one, NULL if this is top-level element */
    CElement *                  m_pParent;

    /* List of all sub elements */
    tListOfParameters           m_listAllSubParameters;

  public:
    CElement(void);

    virtual
    ~CElement(void);

    /* Add a subparameter to m_listAllSubParameters. Called
     * by the accessor functions setXXX and addXXX. */
    void
    addSubParameterToAllList (
      CParameter *              pParameter);

    /* Delete a subparameter from m_listAllSubParameters. Called
     * by the accessor functions setXXX (to remove prior reference)
     * clearXXX(). */
    void
    removeSubParameterFromAllList (
      CParameter *              pParameter);

    /* Clear a list of subparameters. For each entry apply
     * removeSubParameter(). */
    void
    clearSubParameterList (
      tListOfParameters *       pParameterList);

    /* Virtual function provided by each specific element type
     * to decode fields (simple values). Leaves pDecoderStream
     * at first subparameter. */
    virtual void
    decodeFields (
      CDecoderStream *          pDecoderStream) = 0;

    /* After fields are decoded, the CDecoder itself takes care
     * of gathering the subparameters into m_listAllSubParameters.
     * Once the end of the enclosing TLV (or message) is reached
     * this assimilateSubParameters() function is called to
     * create parameter refrences from the primary member variables.
     */
    virtual void
    assimilateSubParameters (
      CErrorDetails *           pError) = 0;

    /* Encode fields and subparameters */
    virtual void
    encode (
      CEncoderStream *          pEncoderStream) const = 0;
};

class CMessage : public CElement
{
  public:
    inline void
    setMessageID (
      llrp_u32_t                MessageID)
    {
        m_MessageID = MessageID;
    }

    inline llrp_u32_t
    getMessageID (void) const
    {
        return m_MessageID;
    }

  protected:
    llrp_u32_t                  m_MessageID;

};

class CParameter : public CElement
{
};

/*
 *
 * By way of example, this is how the CDecoder and CDecoderStream
 * classes work. This example is for decoding a binary frame.
 *
 *      +-------------------+               +---------------+
 *      |                   |               |               |
 *      |   CDecoder        --------------->| CTypeRegistry |
 *      |                   |               |               |
 *      +--|----------------+               +---------------+
 *         |    ^
 * pointer |    |
 * to next |    |   +-------------------+
 * byte    |    |   |                   |           pointer to msg end
 *         |    ^----  CDecoderStream   ----------------+
 *         |    |   |                   |               |
 *         |    |   +-------------------+               |
 *         |    |             ^                         |
 *         |    |             |                         |
 *         |    |   +-------------------+   ptr to      |
 *         |    |   |                   |   TLV end     |
 *         |    ^----  CDecoderStream   ------------+   |
 *         |    |   |                   |           |   |
 *         |    |   +-------------------+           |   |
 *         |    |             ^                     |   |
 *         |    |             |                     |   |
 *         |    |   +-------------------+           |   |
 *         |    |   |                   |           |   |
 *         |    ^----  CDecoderStream   --------+   |   |
 *         |        |                   |       |   |   |
 *         |        +-------------------+       |   |   |
 *         |                                    |   |   |
 *         +-------------------+                |   |   |
 *                             |                |   |   |
 *                             v                v   v   v
 *  +---------------------------------------------------------------+
 *  |                   Binary Frame Buffer                         |
 *  +---------------------------------------------------------------+
 *
 *                            \_________________/          Nestec TLVs
 *        \________________/\___________________________/  Nested TLVs
 *    \_________________________________________________/  Message
 *
 *
 * In the case of binary frame the references are to
 * bytes within the buffer. Lookups are by type number.
 *
 * In the case of an XML DOM tree, the references are
 * to nodes in the DOM tre. Lookups are by string name.
 */

class CDecoder
{
  public:
    CErrorDetails               m_ErrorDetails;

    CDecoder (void);

    CDecoder (
      const CTypeRegistry *     pTypeRegistry);

    virtual
    ~CDecoder (void) = 0;

    virtual CMessage *
    decodeMessage (void) = 0;

  protected:
    const CTypeRegistry *       m_pRegistry;
};

class CDecoderStream
{
  public:
    virtual
    ~CDecoderStream(void) = 0;

    /*
     * 8-bit types
     */

    virtual llrp_u8_t
    get_u8 (
      const CFieldDescriptor *  pFieldDesc) = 0;

    virtual llrp_s8_t
    get_s8 (
      const CFieldDescriptor *  pFieldDesc) = 0;

    virtual llrp_u8v_t
    get_u8v (
      const CFieldDescriptor *  pFieldDesc) = 0;

    virtual llrp_s8v_t
    get_s8v (
      const CFieldDescriptor *  pFieldDesc) = 0;

    /*
     * 16-bit types
     */

    virtual llrp_u16_t
    get_u16 (
      const CFieldDescriptor *  pFieldDesc) = 0;

    virtual llrp_s16_t
    get_s16 (
      const CFieldDescriptor *  pFieldDesc) = 0;

    virtual llrp_u16v_t
    get_u16v (
      const CFieldDescriptor *  pFieldDesc) = 0;

    virtual llrp_s16v_t
    get_s16v (
      const CFieldDescriptor *  pFieldDesc) = 0;

    /*
     * 32-bit types
     */

    virtual llrp_u32_t
    get_u32 (
      const CFieldDescriptor *  pFieldDesc) = 0;

    virtual llrp_s32_t
    get_s32 (
      const CFieldDescriptor *  pFieldDesc) = 0;

    virtual llrp_u32v_t
    get_u32v (
      const CFieldDescriptor *  pFieldDesc) = 0;

    virtual llrp_s32v_t
    get_s32v (
      const CFieldDescriptor *  pFieldDesc) = 0;

    /*
     * 64-bit types
     */

    virtual llrp_u64_t
    get_u64 (
      const CFieldDescriptor *  pFieldDesc) = 0;

    virtual llrp_s64_t
    get_s64 (
      const CFieldDescriptor *  pFieldDesc) = 0;

    virtual llrp_u64v_t
    get_u64v (
      const CFieldDescriptor *  pFieldDesc) = 0;

    virtual llrp_s64v_t
    get_s64v (
      const CFieldDescriptor *  pFieldDesc) = 0;

    /*
     * Special types
     */

    virtual llrp_u1_t
    get_u1 (
      const CFieldDescriptor *  pFieldDesc) = 0;

    virtual llrp_u1v_t
    get_u1v (
      const CFieldDescriptor *  pFieldDesc) = 0;

    virtual llrp_u2_t
    get_u2 (
      const CFieldDescriptor *  pFieldDesc) = 0;

    virtual llrp_u96_t
    get_u96 (
      const CFieldDescriptor *  pFieldDesc) = 0;

    virtual llrp_utf8v_t
    get_utf8v (
      const CFieldDescriptor *  pFieldDesc) = 0;

    virtual llrp_bytesToEnd_t
    get_bytesToEnd (
      const CFieldDescriptor *  pFieldDesc) = 0;

    /*
     * Enumerated types of various sizes
     */

    virtual int
    get_e1 (
      const CFieldDescriptor *  pFieldDesc) = 0;

    virtual int
    get_e2 (
      const CFieldDescriptor *  pFieldDesc) = 0;

    virtual int
    get_e8 (
      const CFieldDescriptor *  pFieldDesc) = 0;

    virtual int
    get_e16 (
      const CFieldDescriptor *  pFieldDesc) = 0;

    virtual int
    get_e32 (
      const CFieldDescriptor *  pFieldDesc) = 0;

    /*
     * Reserved means some number of bits
     */

    virtual void
    get_reserved (
      unsigned int          nBits) = 0;
};


class CEncoder
{
  public:
    CErrorDetails               m_ErrorDetails;

    CEncoder (void);

    virtual
    ~CEncoder (void) = 0;

    virtual void
    encodeElement (
      const CElement *          pElement) = 0;
};

class CEncoderStream
{
  public:
    virtual
    ~CEncoderStream(void) = 0;

    virtual void
    putRequiredSubParameter (
      const CParameter *        pParameter,
      const CTypeDescriptor *   pRefType) = 0;

    virtual void
    putOptionalSubParameter (
      const CParameter *        pParameter,
      const CTypeDescriptor *   pRefType) = 0;

    virtual void
    putRequiredSubParameterList (
      const tListOfParameters * pParameterList,
      const CTypeDescriptor *   pRefType) = 0;

    virtual void
    putOptionalSubParameterList (
      const tListOfParameters * pParameterList,
      const CTypeDescriptor *   pRefType) = 0;

    /*
     * 8-bit types
     */

    virtual void
    put_u8 (
      llrp_u8_t                 Value,
      const CFieldDescriptor *  pFieldDesc) = 0;

    virtual void
    put_s8 (
      llrp_s8_t                 Value,
      const CFieldDescriptor *  pFieldDesc) = 0;

    virtual void
    put_u8v (
      llrp_u8v_t                Value,
      const CFieldDescriptor *  pFieldDesc) = 0;

    virtual void
    put_s8v (
      llrp_s8v_t                Value,
      const CFieldDescriptor *  pFieldDesc) = 0;

    /*
     * 16-bit types
     */

    virtual void
    put_u16 (
      llrp_u16_t                Value,
      const CFieldDescriptor *  pFieldDesc) = 0;

    virtual void
    put_s16 (
      llrp_s16_t                Value,
      const CFieldDescriptor *  pFieldDesc) = 0;

    virtual void
    put_u16v (
      llrp_u16v_t               Value,
      const CFieldDescriptor *  pFieldDesc) = 0;

    virtual void
    put_s16v (
      llrp_s16v_t               Value,
      const CFieldDescriptor *  pFieldDesc) = 0;

    /*
     * 32-bit types
     */

    virtual void
    put_u32 (
      llrp_u32_t                Value,
      const CFieldDescriptor *  pFieldDesc) = 0;

    virtual void
    put_s32 (
      llrp_s32_t                Value,
      const CFieldDescriptor *  pFieldDesc) = 0;

    virtual void
    put_u32v (
      llrp_u32v_t               Value,
      const CFieldDescriptor *  pFieldDesc) = 0;

    virtual void
    put_s32v (
      llrp_s32v_t               Value,
      const CFieldDescriptor *  pFieldDesc) = 0;

    /*
     * 64-bit types
     */

    virtual void
    put_u64 (
      llrp_u64_t                Value,
      const CFieldDescriptor *  pFieldDesc) = 0;

    virtual void
    put_s64 (
      llrp_s64_t                Value,
      const CFieldDescriptor *  pFieldDesc) = 0;

    virtual void
    put_u64v (
      llrp_u64v_t               Value,
      const CFieldDescriptor *  pFieldDesc) = 0;

    virtual void
    put_s64v (
      llrp_s64v_t               Value,
      const CFieldDescriptor *  pFieldDesc) = 0;

    /*
     * Special types
     */

    virtual void
    put_u1 (
      llrp_u1_t                 Value,
      const CFieldDescriptor *  pFieldDesc) = 0;

    virtual void
    put_u1v (
      llrp_u1v_t                Value,
      const CFieldDescriptor *  pFieldDesc) = 0;

    virtual void
    put_u2 (
      llrp_u2_t                 Value,
      const CFieldDescriptor *  pFieldDesc) = 0;

    virtual void
    put_u96 (
      llrp_u96_t                Value,
      const CFieldDescriptor *  pFieldDesc) = 0;

    virtual void
    put_utf8v (
      llrp_utf8v_t              Value,
      const CFieldDescriptor *  pFieldDesc) = 0;

    virtual void
    put_bytesToEnd (
      llrp_bytesToEnd_t         Value,
      const CFieldDescriptor *  pFieldDesc) = 0;

    /*
     * Enumerated types of various sizes
     */

    virtual void
    put_e1 (
      int                       eValue,
      const CFieldDescriptor *  pFieldDesc) = 0;

    virtual void
    put_e2 (
      int                       eValue,
      const CFieldDescriptor *  pFieldDesc) = 0;

    virtual void
    put_e8 (
      int                       eValue,
      const CFieldDescriptor *  pFieldDesc) = 0;

    virtual void
    put_e16 (
      int                       eValue,
      const CFieldDescriptor *  pFieldDesc) = 0;

    virtual void
    put_e32 (
      int                       eValue,
      const CFieldDescriptor *  pFieldDesc) = 0;

    /*
     * Reserved types are some number of bits
     */

    virtual void
    put_reserved (
      unsigned int              nBits) = 0;
};


}; /* namespace LLRP */
