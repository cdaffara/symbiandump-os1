// Copyright (c) 2001-2009 Objective Systems, Inc. (http://www.obj-sys.com) 
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available 
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Objective Systems, Inc - initial contribution.
//
// Contributors:
//
// Description:
// 

//
// CHANGE LOG
// Date         Init    Description
// 07/14/04     AB      Exceptions have been removed
// 04/19/02     ED      Added XEREncode, XERDecode message buffer types
// 04/19/02     ED      Removed StringOverflow and added RTLError excp
// 07/14/02     ED      Changed all methods to being with lowercase 
//                      char for consistency.  Added deprecated inline 
//                      methods for those that did not to maintain 
//                      backward compatibility.
// 07/16/02     AB      added 'const' to RTLError::getStatus
// 10/29/02     CZ      Added/modified doxygen code for groups.
//
//

/** 
 * @file rtsrc/asn1CppTypes.h 
 * Common C++ type and class definitions.
 */

/**
@file
@internalTechnology
*/

#ifndef _ASN1CPPTYPES_H_
#define _ASN1CPPTYPES_H_

#include "rtxsrc/rtxMemory.h"
#include "rtxsrc/rtxDiag.h"
#include "rtxsrc/rtxError.h"
#include "rtxsrc/rtxMemBuf.h"
#include "rtsrc/asn1CppEvtHndlr.h"
#include "rtsrc/ASN1Context.h"
#include "rtxsrc/OSRTMsgBuf.h"

// for compatibilty only 
#define ASN1TRY try
#define ASN1RTLTHROW(stat) exit (-1)
#define ASN1THROW(ex) exit (-1)
#define ASN1CATCH(exType,ex,body) if (0) { body; }

/** 
 * @defgroup cppruntime C++ Run-Time Classes 
 * The <B>OSRT C++ run-time classes</B> are wrapper classes that provide an 
 * object-oriented interface to the common C run-time library functions. The 
 * categories of classes provided are as follows:
 * <UL>
 * <LI>Context management classes manage the context structure (OSCTXT) 
 * used to keep track of the working variables required to encode or 
 * decode XML messages.</LI>
 * <LI>Message buffer classes are used to manage message buffers for 
 * encoding or decoding XML messages.</LI>
 * <LI>XSD type base classes are used as the base for compiler-
 * generated C++ data structures. </LI>
 * <LI>Stream classes are used to read and write messages to and from 
 * files, sockets, and memory buffers.</LI> 
 * </UL>
 * @{
 */
/** 
 * @defgroup osrtmsgbuf OSRT Message Buffer Classes
 * @{
 */
/** 
 * Abstract ASN.1 message buffer base class. This class is used to manage a
 * message buffer containing an ASN.1 message. For encoding, this is the buffer
 * the message is being built in. For decoding, it is a message that was read
 * into memory to be decoded. Further classes are derived from this to handle
 * encoding and decoding of messages for different encoding rules types.
 */
class EXTRTCLASS ASN1MessageBuffer : public OSRTMessageBuffer
{
 protected:

   /**
    * The protected constructor creates a new context and sets the buffer class
    * type.
    *
    * @param bufferType   Type of message buffer that is being created (for
    *                       example, BEREncode).
    */
   EXTRTMETHOD ASN1MessageBuffer (Type bufferType);

   /** 
    * This constructor sets the buffer class type and also a pointer to a 
    * context created by the user.
    *
    * @param bufferType    Type of message buffer that is being created (for
    *                         example, BEREncode).
    * @param pContext      A pointer to an OSRTContext structure previously
    *                         created by the user. */
   EXTRTMETHOD ASN1MessageBuffer (Type bufferType, OSRTContext* pContext);

   /**
    * This method sets error status to the context.
    *
    * @return     Error status value being set.
    */
   virtual int setStatus (int stat) { 
      mpContext->setStatus (stat);
      return stat;
   }

 public:
   /**
    * The virtual destructor does nothing. It is overridden by derived versions
    * of this class.
    */
   virtual ~ASN1MessageBuffer () {}

   /**
    * The addEventHandler method is used to register a used-defined named event
    * handler. Methods from within this handler will be invoked when this
    * message buffer is used in the decoding of a message.
    *
    * @param pEventHandler    - Pointer to named event handler object to
    *                           register.
    */
   virtual void addEventHandler (Asn1NamedEventHandler* pEventHandler) {
      Asn1NamedEventHandler::addEventHandler 
         (getCtxtPtr(), pEventHandler);
   }

#ifndef __SYMBIAN32__
   /**
    * The CStringToBMPString method is a utility function for converting a
    * null-terminated Ascii string into a BMP string. A BMP string contains
    * 16-bit Unicode characters.
    *
    * @param cstring      - Null-terminated character string to convert
    * @param pBMPString   - Pointer to BMP string target variable
    * @param pCharSet     - Pointer to permitted alphabet character set. If
    *                       provided, index to character within this set is
    *                       returned.
    */
   virtual ASN1BMPString* CStringToBMPString 
      (const char* cstring, ASN1BMPString* pBMPString, 
       Asn116BitCharSet* pCharSet = 0);
#endif

   /**
    * Returns a pointer to application-specific information block 
    */
   virtual void* getAppInfo () { return 0; }

   /**
    * This version of the overloaded initBuffer method initializes the message
    * buffer to point at the memory contained within the referenced OSRTMEMBUF
    * object.
    *
    * @param membuf      OSRTMEMBUF memory buffer class object reference.
    * @return            Completion status of operation:
    *                      - 0 (0) = success,
    *                      - negative return value is error.
    */
   virtual EXTRTMETHOD int initBuffer (OSRTMEMBUF& membuf);

   /**
    * This version of the overloaded initBuffer method initializes the message
    * buffer to point at the given Unicode string. This is used mainly for XER
    * (XML) message decoding.
    *
    * @param unistr      Pointer to a Unicode character string.
    * @return            Completion status of operation:
    *                      - 0 (0) = success,
    *                      - negative return value is error.
    */
   virtual EXTRTMETHOD int initBuffer (OSUNICHAR* unistr);

   /**
    * This version of the overloaded initBuffer method initializes the message
    * buffer to point at the given null-terminated UTF-8 character string.
    *
    * @param str         Pointer to a null-terminated ASCII character
    *                       string.
    * @return            Completion status of operation:
    *                      - 0 (0) = success,
    *                      - negative return value is error.
    */
   virtual int initBuffer (const OSUTF8CHAR* /*str*/) { return RTERR_NOTSUPP; }

   /**
    * This method checks the type of the message buffer.
    *
    * @param bufferType    Enumerated identifier specifying a derived class.
    *                         Possible values 
    *                         are: BEREncode, BERDecode, PEREncode, PERDecode, 
    *                         XEREncode, XERDecode, XMLEncode, XMLDecode, 
    *                         Stream.
    * @return              Boolean result of the match operation. True if this
    *                         is the class corresponding to the identifier
    *                         argument.
    */ 
   virtual OSBOOL isA (Type /*bufferType*/) { return FALSE; }

   /**
    * The removeEventHandler method is used to de-register a used-defined named
    * event handler.
    *
    * @param pEventHandler    - Pointer to named event handler object to
    *                           de-register.
    */
   virtual void removeEventHandler (Asn1NamedEventHandler* pEventHandler) {
      Asn1NamedEventHandler::removeEventHandler 
         (getCtxtPtr(), pEventHandler);
   }

   /**
    * The resetErrorInfo method resets information on errors contained within
    * the context.
    */
   virtual void resetErrorInfo () {
      OSRTMessageBuffer::resetErrorInfo ();
   }

   /**
    * Sets the application-specific information block.
    */
   virtual void setAppInfo (void* /*pAppInfo*/) { }

   /**
    * The setErrorHandler method is used to register a used-defined error
    * handler. Methods from within this handler will be invoked when an error
    * occurs in decoding a message using this message buffer object.
    *
    * @param pErrorHandler    - Pointer to error handler object to register.
    */
   virtual void setErrorHandler (Asn1ErrorHandler* pErrorHandler) {
      Asn1ErrorHandler::setErrorHandler 
         (getCtxtPtr(), pErrorHandler);
   }

   /**
    * This method sets run-time key to the context. This method does nothing
    * for unlimited redistribution libraries.
    *
    * @param key - array of octets with the key
    * @param keylen - number of octets in key array.
    * @return            Completion status of operation:
    *                      - 0 (ASN_OK) = success,
    *                      - negative return value is error.
    */
   EXTRTMETHOD int setRunTimeKey (const OSOCTET* key, size_t keylen);

   // The following are deprecated method definitions maintained for 
   // backward compatibility only..

   inline OSOCTET* GetMsgCopy () { return getMsgCopy(); }
   inline const OSOCTET* GetMsgPtr() { return getMsgPtr(); }
   inline void PrintErrorInfo() { printErrorInfo(); }

} ;
/** @} */

/**
 * @defgroup asn1ctrl Control (ASN1C_) Base Classes 
 * The ASN1C Control Base Classes are used as the base for compiler-generated
 * ASN1C_ classes. These are wrapper classes that can be used to encode/decode
 * PDU types and as helper classes for performing operations on complex data
 * types. @{
 */
/** 
 * ASN1C control class base class. This is the main base class for all
 * generated ASN1C_\<name\> control classes. It holds a variable of a generated
 * data type as well as the associated message buffer or stream class to which
 * a message will be encoded or from which a message will be decoded.
 */
class EXTRTCLASS ASN1CType {
 protected:
   /**
    * The mpContext member variable holds a reference-counted C runtime
    * variable. This context is used in calls to all C run-time functions. The
    * context pointed at by this smart-pointer object is shared with the
    * message buffer object contained within this class.
    */
   OSRTCtxtPtr mpContext;

   /**
    * The mpMsgBuf member variable is a pointer to a derived message buffer or
    * stream class that will manage the ASN.1 message being encoded or decoded.
    */
   OSRTMessageBufferIF* mpMsgBuf;

   /**
    * The default constructor sets the message pointer member variable to NULL
    * and creates a new context object.
    */
   EXTRTMETHOD ASN1CType ();

   /**
    * This constructor sets the message pointer member variable to NULL and
    * initializes the context object to point at the given context value.
    *
    * @param ctxt         - Reference to a context object.
    */
   EXTRTMETHOD ASN1CType (OSRTContext& ctxt);

   EXTRTMETHOD int setMsgBuf (OSRTMessageBufferIF& msgBuf, OSBOOL initBuf=FALSE);

   /**
    * This method sets run-time key to the context. This method does nothing
    * for unlimited redistribution libraries.
    *
    * @param key - array of octets with the key
    * @param keylen - number of octets in key array.
    * @return            Completion status of operation:
    *                      - 0 (ASN_OK) = success,
    *                      - negative return value is error.
    */
   EXTRTMETHOD int setRunTimeKey (const OSOCTET* key, size_t keylen);

 public:
   /**
    * This constructor sets the internal message buffer pointer to point at the
    * given message buffer or stream object. The context is set to point at the
    * context contained within the message buffer object. Thus, the message
    * buffer and control class object share the context. It will not be
    * released until both objects are destroyed.
    *
    * @param msgBuf       - Reference to a message buffer or stream object.
    */
   EXTRTMETHOD ASN1CType (OSRTMessageBufferIF& msgBuf);

   /**
    * The copy constructor sets the internal message buffer pointer and context
    * to point at the message buffer and context from the original ASN1CType
    * object.
    *
    * @param orig         - Reference to a message buffer or stream object.
    */
   EXTRTMETHOD ASN1CType (const ASN1CType& orig);

   /**
    * The virtual destructor does nothing. It is overridden by derived versions
    * of this class.
    */
   virtual ~ASN1CType () {}

   /**
    * The append method can be used to append an element to any 
    * linked list structure contained within the generated type.
    *
    * @param llist Linked list structure.
    * @param pdata Data record to be appended.  Note that the pointer 
    *    value is appended.  The data is not copied.
    */
   inline void append (OSRTDList& llist, void* pdata) {
      rtxDListAppend (getCtxtPtr(), &llist, pdata);
   }

   /**
    * The getContext method returns the underlying context smart-pointer
    * object.
    *
    * @return Context smart pointer object.
    */
   inline OSRTCtxtPtr getContext () {
      return mpContext;
   }

   /**
    * The getCtxtPtr method returns the underlying C runtime context. This
    * context can be used in calls to C runtime functions.
    */
   inline OSCTXT* getCtxtPtr () {
      return (!mpContext.isNull ()) ? mpContext->getPtr() : 0;
   }

   /**
    * This method returns the completion status of previous operation.
    * It can be used to check completion status of constructors or methods,
    * which do not return completion status. If error occurs, use 
    * printErrorInfo method to print out the error's description and 
    * stack trace.
    * Method resetError can be used to reset error to continue operations 
    * after recovering from the error.
    *
    * @return            Runtime status code:
    *                      - 0 (0) = success,
    *                      - negative return value is error.
    */ 
   inline int getStatus () const { 
      return (!mpContext.isNull()) ? mpContext->getStatus() : 0; 
   }

   /**
    * The memAlloc method allocates memory using the C runtime memory
    * management functions. The memory is tracked in the underlying context
    * structure. When both this ASN1CType derived control class object and the
    * message buffer object are destroyed, this memory will be freed.
    *
    * @param numocts      Number of bytes of memory to allocate
    * @return             Void pointer to allocated memory or NULL if 
    *   insufficient memory was available to fulfill the request.
    */
   inline void* memAlloc (size_t numocts) {
      if (mpContext.isNull ()) return 0;
      return mpContext->memAlloc (numocts);
   }

   /**
    * The \c memFreeAll method will free all memory currently tracked within
    * the context. This includes all memory allocated with the memAlloc method
    * as well as any memory allocated using the C \c rtxMemAlloc function with
    * the context returned by the \c getCtxtPtr method.
    */
   inline void memFreeAll () {
      if (!mpContext.isNull ())
         mpContext->memFreeAll ();
   }

   /**
    * The memRealloc method reallocates memory using the C runtime memory
    * management functions.
    *
    * @param ptr          Original pointer containing dynamic memory to be
    *                       resized.
    * @param numocts      Number of bytes of memory to allocate
    * @return             Reallocated memory pointer
    */
   inline void* memRealloc (void* ptr, size_t numocts) {
      if (mpContext.isNull ()) return 0;
      return mpContext->memRealloc (ptr, numocts);
   }

   /**
    * The memReset method resets dynamic memory using the C runtime memory
    * management functions.
    *
    */
   inline void memReset () {
      if (!mpContext.isNull ())
         mpContext->memReset ();
   }

   /**
    * The memFreePtr method frees the memory at a specific location. This
    * memory must have been allocated using the memAlloc method described
    * earlier.
    *
    * @param ptr          - Pointer to a block of memory allocated with \c
    *                       memAlloc
    */
   inline void memFreePtr (void* ptr) {
      if (!mpContext.isNull ())
         mpContext->memFreePtr (ptr);
   }

   /**
    * The PrintErrorInfo method prints information on errors contained within
    * the context.
    */
   inline void printErrorInfo () {
      if (!mpContext.isNull ())
         mpContext->printErrorInfo ();
   }

   /**
    * This method resets error status and stack trace. This method should be
    * used to continue operations after recovering from the error.
    */
   inline void resetError () {
      if (!mpContext.isNull ())
         mpContext->resetErrorInfo ();
   }

   /**
    * This method turns diagnostic tracing on or off.
    *
    * @param value Boolean value; TRUE = turn tracing on.
    * @return      Previous state.
    */
   inline OSBOOL setDiag (OSBOOL value) {
      return getContext()->setDiag (value);
   }

   /**
    * The \c Encode method encodes an ASN.1 message using the encoding rules
    * specified by the derived message buffer object.
    *
    * @return            Completion status of operation:
    *                      - 0 (0) = success,
    *                      - negative return value is error.
    */
   virtual EXTRTMETHOD int Encode ();

   /**
    * The \c Decode method decodes the ASN.1 message described by the
    * encapsulated message buffer object.
    *
    * @return            Completion status of operation:
    *                      - 0 (0) = success,
    *                      - negative return value is error.
    */
   virtual EXTRTMETHOD int Decode ();

   /**
    * The \c EncodeTo method encodes an ASN.1 message into the given message
    * buffer or stream argument.
    *
    * @param msgBuf      Message buffer or stream to which the message is to
    *                       be encoded.
    * @return            Completion status of operation:
    *                      - 0 (0) = success,
    *                      - negative return value is error.
    */
   virtual int EncodeTo (OSRTMessageBufferIF& /*msgBuf*/) 
      { return ASN_E_NOTPDU; }

   /**
    * The \c DecodeFrom method decodes an ASN.1 message from the given message
    * buffer or stream argument.
    *
    * @param msgBuf      Message buffer or stream containing message to
    *                       decode.
    * @return            Completion status of operation:
    *                      - 0 (0) = success,
    *                      - negative return value is error.
    */
   virtual int DecodeFrom (OSRTMessageBufferIF& /*msgBuf*/) 
      { return ASN_E_NOTPDU; }
} ;
/** @} */

/** 
 * @defgroup asn1data ASN.1 Type (ASN1T_) Base Classes 
 * These classes are used as the base for compiler-generated ASN1T_ C++ data
 * structures. These are enhanced versions of the C structures used for mapping
 * ASN.1 types. The main difference is that constructors and operators have
 * been added to the derived classes. @{
 */
/** 
 * Dynamic bit string. This is the base class for generated C++ data type
 * classes for unsized BIT STRING's.
 */
struct EXTRTCLASS ASN1TDynBitStr : public ASN1DynBitStr {
   /**
    * The default constructor creates an empty bit string.
    */
   ASN1TDynBitStr () { numbits = 0; }

   /**
    * This constructor initializes the bit string to contain the given data
    * values.
    *
    * @param _numbits     - Number of bits in the bit string.
    * @param _data        - The binary bit data values.
    */
   ASN1TDynBitStr (OSUINT32 _numbits, const OSOCTET* _data) { 
      numbits = _numbits; 
      data = _data;
   }

   /**
    * This constructor initializes the bit string to contain the given data
    * values.
    *
    * @param _bs          - C bit string structure.
    */
   ASN1TDynBitStr (ASN1DynBitStr& _bs) { 
      numbits = _bs.numbits; 
      data = _bs.data;
   }
} ;

/* Dynamic octet string */
#include "rtsrc/ASN1TOctStr.h"

/** 
 * BMPString. This is the base class for generated C++ data type classes for
 * BMPString values.
 */
struct EXTRTCLASS ASN1TBMPString : public ASN1BMPString {
   /**
    * The default constructor creates an empty BMPString value.
    */
   inline ASN1TBMPString () { 
      nchars = 0; 
      data = 0;
   }
} ;

/** 
 * UniversalString. This is the base class for generated C++ data type classes for
 * UniversalString values.
 */
struct EXTRTCLASS ASN1TUniversalString : public ASN1UniversalString {
   /**
    * The default constructor creates an empty UniversalString value.
    */
   inline ASN1TUniversalString () { 
      nchars = 0; 
      data = 0;
   }
} ;

/** 
 * Open type. This is the base class for generated C++ data type classes for
 * open type values.
 */
struct EXTRTCLASS ASN1TOpenType : public ASN1OpenType {
   /**
    * The default constructor creates an empty open type value.
    */
   inline ASN1TOpenType () { 
      numocts = 0; 
      data = 0;
   }
} ;

/** 
 * Open type with table constraint. This is the base class for generated C++
 * data type classes for open type values with table constraints. It is only
 * used when the -tables compiler command line option is specified.
 */
struct EXTRTCLASS Asn1TObject : public Asn1Object {
   /**
    * The default constructor creates an empty object value.
    */
   inline Asn1TObject () { 
      encoded.numocts = 0; 
      encoded.data = 0;
      decoded = 0;
      index =0;
   }
} ;

typedef Asn1TObject ASN1TObject;

/* Object identifier */
#include "rtsrc/ASN1TObjId.h"

/**
 * Object identifier with 64-bit arcs. This class is identical to the
 * ASN1TObjId class except it allows 64-bit integers to be used for the arc
 * (i.e. subidentifier) values.
 */
struct EXTRTCLASS ASN1TObjId64 : public ASN1OID64 {
   inline ASN1TObjId64 () { numids = 0; }
   EXTRTMETHOD ASN1TObjId64 (OSOCTET _numids, const OSINT64* _subids);
   EXTRTMETHOD ASN1TObjId64 (const ASN1OID64& oid);
   EXTRTMETHOD ASN1TObjId64 (const ASN1TObjId64& oid);
   EXTRTMETHOD void operator= (const ASN1OID64& rhs);
   EXTRTMETHOD void operator= (const ASN1TObjId64& rhs);
} ;

/** 
 * SEQUENCE or SET extension element holder. This is used for the /c extElem1
 * open extension element in extensible SEQUENCE or SET constructs.
 */
struct EXTRTCLASS ASN1TSeqExt : public OSRTDList {
   /**
    * The default constructor creates an empty open extension element.
    */
   inline ASN1TSeqExt () { 
      rtxDListInit (this);
   }
} ;

/** 
 * Base class for PDU types. This class is used as the base class for all
 * compiler-generated PDU types.
 */
struct EXTRTCLASS ASN1TPDU {
 protected:
   /** 
    * The mpContext member variable holds a smart-pointer to the current
    * context variable. This ensures an instance of this PDU type will persist
    * if the control class and message buffer classes used to decode or copy
    * the message are destroyed.
    */
   OSRTCtxtPtr mpContext;

 public:
   /** 
    * The setContext method allows the context member variable to be set. It is
    * invoked in compiler-generated control class decode and copy methods.
    * This method is invoked to prevent memory freeing of decoded or copied 
    * data after a control class or message buffer object goes out of scope. 
    * Also, if context is set to ASN1TPDU then generated destuctor of 
    * inherited ASN1T_<type> class will invoke generated free routines. 
    * Note, it is not obligatory to call generated free routines unless a 
    * series of messages is being decoded or control class and message buffer 
    * objects go out of scope somewhere. The destructor of the control class 
    * or message buffer class will free all dynamicly allocated memory. 
    * Thus, if performance is a main issue, "setContext (NULL)" may be called 
    * after Decode method call. In this case destructor of ASN1T_<type> will 
    * do nothing.
    *
    * @param ctxt  A pointer to reference counted ASN.1 context class instance.
    */
   inline void setContext (OSRTContext* ctxt) {
      if (mpContext.isNull()) mpContext = ctxt;
      else if (ctxt == 0) mpContext = 0;
   }
} ;

/** 
 * SEQUENCE OF element holder. This class is used as the base class for
 * compiler-generated SEQUENCE OF linked-list types.
 */
struct EXTRTCLASS ASN1TSeqOfList : public OSRTDList {
   /**
    * The default constructor creates an empty list.
    */
   inline ASN1TSeqOfList () { 
      rtxDListInit (this);
   }
} ;

/** 
 * SEQUENCE OF element holder (PDU). This class is used as the base class for
 * compiler-generated SEQUENCE OF linked-list types. In this case, the type has
 * also been determined to be a PDU.
 */
struct EXTRTCLASS ASN1TPDUSeqOfList : public ASN1TSeqOfList, public ASN1TPDU {
   /**
    * The default constructor creates an empty list.
    */
   ASN1TPDUSeqOfList () {} 
} ;

/** @} */

/** @} */

#endif
