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

/** 
 * @file OSRTMsgBuf.h 
 * C++ run-time message buffer class definition.
 */

/**
@file
@internalTechnology
*/

#ifndef _OSRTMSGBUF_H_
#define _OSRTMSGBUF_H_

#include "rtxsrc/OSRTCtxtHolder.h"
#include "rtxsrc/OSRTMsgBufIF.h"

/** 
 * @addtogroup osrtmsgbuf
 * @{
 */
/** 
 * Abstract message buffer base class. This class is used to manage an
 * encode or decode message buffer. For encoding, this is the buffer
 * into which the message is being built. For decoding, it describes a 
 * message that was read into memory to be decoded. Further classes are 
 * derived from this to handle encoding and decoding of messages for 
 * different encoding rules types.
 */
class EXTRTCLASS OSRTMessageBuffer : 
public OSRTCtxtHolder, public OSRTMessageBufferIF 
{
 protected:

   /**
    * The mBufferType member variable holds information on the derived 
    * message buffer class type (for example, XMLEncode).
    */
   Type mBufferType;

   /**
    * The protected constructor creates a new context and sets the buffer class
    * type.
    *
    * @param bufferType   Type of message buffer that is being created (for
    *                       example, XMLEncode).
    * @param pContext     Pointer to a context to use. If NULL, new context 
    *                       will be allocated. 
    */
   EXTRTMETHOD OSRTMessageBuffer (Type bufferType, OSRTContext* pContext = 0);

 public:
   /**
    * The virtual destructor does nothing. It is overridden by derived versions
    * of this class.
    */
   virtual ~OSRTMessageBuffer () {}

   /**
    * Returns a pointer to application-specific information block 
    */
   virtual void* getAppInfo () { return 0; }

   /**
    * The getByteIndex method is used to fetch the current byte offset within
    * the current working buffer. For encoding, this is the next location that
    * will be written to. For decoding, this is the next byte the parser will
    * read.
    */
   virtual size_t getByteIndex () {
      return getCtxtPtr()->buffer.byteIndex;
   }

   /**
    * The getContext method returns the underlying context smart-pointer
    * object.
    */
   virtual OSRTCtxtPtr getContext () {
      return OSRTCtxtHolder::getContext ();
   }

   /**
    * The getCtxtPtr method returns the underlying C runtime context. This
    * context can be used in calls to C runtime functions.
    */
   virtual OSCTXT* getCtxtPtr () {
      return OSRTCtxtHolder::getCtxtPtr ();
   }

   /**
    * Returns error text in a dynamic memory buffer. The buffer is allocated 
    * using 'operator new []'. The calling routine is responsible to free the 
    * memory by using 'operator delete []'. 
    *
    * @return  A pointer to a newly allocated buffer with error text.
    */
   virtual char* getErrorInfo () {
      return OSRTCtxtHolder::getErrorInfo ();
   }

   /**
    * Returns error text in a memory buffer. If buffer pointer is specified in 
    * parameters (not NULL) then error text will be copied in the passed 
    * buffer. Otherwise, this method allocates memory using the 
    * 'operator new []' function. The calling routine is responsible to free 
    * the memory by using 'operator delete []'. 
    *
    * @param pBuf         A pointer to a destination buffer to obtain the error
    *                       text. If NULL, dynamic buffer will be allocated.
    * @param bufSize      A reference to buffer size. If pBuf is NULL it will 
    *                       receive the size of allocated dynamic buffer.
    * @return             A pointer to a buffer with error text. If pBuf is not
    *                       NULL, the return pointer will be equal to it. 
    *                       Otherwise, returns newly allocated buffer with 
    *                       error text. NULL, if error occurred.
    */
   virtual char* getErrorInfo (char* pBuf, size_t& bufSize) {
      return OSRTCtxtHolder::getErrorInfo (pBuf, bufSize);
   }

   /**
    * The getMsgCopy method will return a copy of the encoded message
    * managed by the object.
    */
   virtual OSOCTET* getMsgCopy () {
      return 0;
   }

   /**
    * The getMsgPtr method will return a const pointer to the encoded 
    * message managed by the object.
    */
   virtual const OSOCTET* getMsgPtr () {
      return getCtxtPtr()->buffer.data;
   }

   /**
    * This method returns the completion status of previous operation.
    * It can be used to check completion status of constructors or methods,
    * which do not return completion status.
    *
    * @return            Runtime status code:
    *                      - 0 = success,
    *                      - negative return value is error.
    */ 
   inline int getStatus () const { 
      return OSRTCtxtHolder::getStatus(); 
   }

   /**
    * Initializes message buffer.
    *
    * @return            Completion status of operation:
    *                      - 0 (0) = success,
    *                      - negative return value is error.
    */
   virtual int init () { return 0; }

   /**
    * This version of the overloaded initBuffer method initializes the message
    * buffer to point at the given null-terminated character string.
    *
    * @param pMsgBuf    Pointer to message buffer.
    * @param msgBufLen  Length of message buffer in bytes.
    *
    * @return            Completion status of operation:
    *                      - 0 (0) = success,
    *                      - negative return value is error.
    */
   virtual EXTRTMETHOD int initBuffer (OSOCTET* pMsgBuf, size_t msgBufLen);

   /**
    * The printErrorInfo method prints information on errors contained within
    * the context.
    */
   virtual void printErrorInfo () {
      OSRTCtxtHolder::printErrorInfo ();
   }

   /**
    * The resetErrorInfo method resets information on errors contained within
    * the context.
    */
   virtual void resetErrorInfo () {
      OSRTCtxtHolder::resetErrorInfo ();
   }

   /**
    * Sets the application-specific information block.
    */
   virtual void setAppInfo (void*) {}

   /**
    * The setDiag method will turn diagnostic tracing on or off.
    * 
    * @param value - Boolean value (default = TRUE = on)
    */
   virtual EXTRTMETHOD void setDiag (OSBOOL value=TRUE);

} ;

/** @} */

#endif
