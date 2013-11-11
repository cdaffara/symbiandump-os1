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
// 07/14/02     ED      Lowercase method names
// 10/31/03     CZ      Class descriptions
// 11/04/03     CZ      Augment Documentation 
//
//

/** 
 * @file asn1PerCppTypes.h 
 * PER C++ type and class definitions.
 */

/**
@file
@internalTechnology
*/

#ifndef _ASN1PERCPPTYPES_H_
#define _ASN1PERCPPTYPES_H_

#include "rtpersrc/asn1per.h"
#include "rtsrc/asn1CppTypes.h"

/** @defgroup percppruntime PER C++ Runtime Classes.
 * @{
 */
/**
 * The ASN.1 C++ runtime classes are wrapper classes that provide an
 * object-oriented interface to the ASN.1 C runtime library functions. These
 * classes are derived from the common classes documented in the ASN1C C/C++
 * Common Runtime Functions manual and are specific the Packed Encoding Rules
 * (PER).
*/

/** @defgroup permsgbuffclas PER Message Buffer Classes
 * These classes manage the buffers for encoding and decoding ASN.1 PER
 * messages. @{
*/

// ASN.1 PER message buffer class
/**
 * The ASN1PERMessageBuffer class is derived from the ASN1MessageBuffer base
 * class. It is the base class for the ASN1PEREncodeBuffer and
 * ASN1PERDecodeBuffer derived classes. It contains variables and methods
 * specific to encoding or decoding ASN.1 messages using the Packed Encoding
 * Rules (PER). It is used to manage the buffer into which an ASN.1 message is
 * to be encoded or decoded.
*/
class EXTPERCLASS ASN1PERMessageBuffer : public ASN1MessageBuffer {
 protected:

   /**
    * This constructor does not set a PER input source.  It is used by 
    * the derived encode buffer classes.  Use the getStatus() method to 
    * determine if an error has occured during initialization.
    *
    * @param bufferType   Type of message buffer that is being created (for
    *                       example, PEREncode or PERDecode).
    * @param aligned      Flag indicating if aligned (TRUE) or unaligned
    *                       (FALSE) encoding should be done.
    */
   EXTPERMETHOD ASN1PERMessageBuffer (Type bufferType, OSBOOL aligned);


   /**
    * This constructor allows a memory buffer holding a binary PER message 
    * to be specified. Use the getStatus() method to determine if an error 
    * has occured during initialization.
    *
    * @param bufferType   Type of message buffer that is being created (for
    *                       example, PEREncode or PERDecode).
    * @param pMsgBuf      A pointer to a fixed size message buffer to recieve
    *                       the encoded message.
    * @param msgBufLen    Size of the fixed-size message buffer.
    * @param aligned      Flag indicating if aligned (TRUE) or unaligned
    *                       (FALSE) encoding should be done.
    */
   EXTPERMETHOD ASN1PERMessageBuffer (Type bufferType, OSOCTET* pMsgBuf, 
                         size_t msgBufLen, OSBOOL aligned);

   /**
    * This constructor allows a memory buffer holding a binary PER message 
    * to be specified. It also allows a pre-existing context to be associated
    * with this buffer. Use the getStatus() method to determine if an error
    * has occured during initialization.
    *
    * @param bufferType   Type of message buffer that is being created (for
    *                       example, PEREncode or PERDecode).
    * @param pMsgBuf      A pointer to a fixed size message buffer to recieve
    *                       the encoded message.
    * @param msgBufLen    Size of the fixed-size message buffer.
    * @param aligned      Flag indicating if aligned (TRUE) or unaligned
    *                       (FALSE) encoding should be done.
    * @param pContext     A pointer to an OSRTContext structure.
    */
   EXTPERMETHOD ASN1PERMessageBuffer (Type bufferType, OSOCTET* pMsgBuf, 
                     size_t msgBufLen, OSBOOL aligned, OSRTContext *pContext);
 public:

   /**
    * This method outputs a binary dump of the current buffer contents to
    * stdout.
    *
    * @param varname      char pointer to current buffer
    */
   inline void binDump (const char* varname) { 
      pu_bindump (mpContext->getPtr(), (char*)varname); 
   }

   /** 
    * This method outputs a hexadecimal dump of the current buffer contents to
    * stdout.
    *
    * @param             - none
    */
   inline void hexDump () { 
      pu_hexdump (mpContext->getPtr()); 
   }

   /**
    * This method returns the length of a previously encoded PER message.
    *
    * @param             - none
    */
   inline size_t getMsgLen () { 
      return pu_getMsgLen (mpContext->getPtr());
   }

   /**
    * This method turns PER diagnostic tracing on or off.
    *
    * This enables the collection of the bit statistics inside the PER library
    * functions that can be displayed using the binDump method.
    *
    * @param value        Boolean value indicating whether tracing should be on
    *                       (true) or off (false).
    */
   inline void setTrace (OSBOOL value) {
      pu_setTrace (mpContext->getPtr(), value);
   }

   /**
    * This method sets a buffer to receive the encoded message.
    *
    * @param pMsgBuf      A pointer to a memory buffer to use to encode a
    *                       message. The buffer should be declared as an array
    *                       of unsigned characters (OSOCTETs). This parameter
    *                       can be set to NULL to specify dynamic encoding
    *                       (i.e., the encode functions will dynamically
    *                       allocate a buffer for the message).
    * @param msgBufLen    The length of the memory buffer in bytes. If pMsgBuf
    *                       is NULL, this parameter specifies the initial size
    *                       of the dynamic buffer; if 0 - the default size will
    *                       be used.
    * @return             Completion status of operation:
    *                       - 0 (0) = success,
    *                       - negative return value is error.
    */
   EXTPERMETHOD int setBuffer (const OSOCTET* pMsgBuf, size_t msgBufLen);

   // deprecated methods
   inline void BinDump (const char* varname) { binDump (varname); }
   inline void HexDump () { hexDump (); }
   inline int GetMsgLen () { return (int)getMsgLen (); }
   inline void SetTrace (OSBOOL value) { setTrace (value); }
} ;

// ASN.1 PER encode message buffer class
/**
 * The ASN1PEREncodeBuffer class is derived from the ASN1PERMessageBuffer base
 * class. It contains variables and methods specific to encoding ASN.1
 * messages. It is used to manage the buffer into which an ASN.1 PER message
 * is to be encoded.
 */
class EXTPERCLASS ASN1PEREncodeBuffer : public ASN1PERMessageBuffer {
 public:
   /**
    * The ASN1PEREncodeBuffer class has three overloaded constructors: This
    * version that takes one argument, aligned flag (dynamic encoding version).
    * Use getStatus() method to determine has error occured during the
    * initialization or not.
    *
    * @param aligned      Flag indicating if aligned (TRUE) or unaligned
    *                       (FALSE) encoding should be done.
    */
   ASN1PEREncodeBuffer (OSBOOL aligned) : 
      ASN1PERMessageBuffer (PEREncode, aligned) {}

   /**
    * The ASN1PEREncodeBuffer class has three overloaded constructors: This
    * version that takes a message buffer and size argument and an aligned flag
    * argument (static encoding version). Use getStatus() method to determine
    * has error occured during the initialization or not.
    *
    * @param pMsgBuf      A pointer to a fixed-size message buffer to receive
    *                       the encoded message.
    * @param msgBufLen    Size of the fixed-size message buffer.
    * @param aligned      Flag indicating if aligned (TRUE) or unaligned
    *                       (FALSE) encoding should be done.
    */
   ASN1PEREncodeBuffer (OSOCTET* pMsgBuf, size_t msgBufLen, OSBOOL aligned) 
      : ASN1PERMessageBuffer (PEREncode, pMsgBuf, msgBufLen, aligned) {}


   /**
    * The ASN1PEREncodeBuffer class has three overloaded constructors: This
    * version that takes a message buffer and size argument and an aligned flag
    * argument (static encoding version). Use getStatus() method to determine
    * has error occured during the initialization or not.
    *
    * @param pMsgBuf      A pointer to a fixed-size message buffer to receive
    *                       the encoded message.
    * @param msgBufLen    Size of the fixed-size message buffer.
    * @param aligned      Flag indicating if aligned (TRUE) or unaligned
    *                       (FALSE) encoding should be done.
    * @param pContext     A pointer to an OSRTContext structure created by
    *                         the user.
    */
   ASN1PEREncodeBuffer (OSOCTET *pMsgBuf, size_t msgBufLen, OSBOOL aligned,
         OSRTContext *pContext) : ASN1PERMessageBuffer (PEREncode, pMsgBuf,
            msgBufLen, aligned, pContext) { }

   /**
    * This method returns the length (in bits) of the encoded message.
    *
    * @return             Length(in bits)of encoded message
    */
   inline size_t getMsgBitCnt () { 
      return pe_GetMsgBitCnt (mpContext->getPtr());
   }

   /**
    * This method returns a copy of the current encoded message. Memory is
    * allocated for the message using the 'new' operation. It is the user's
    * responsibility to free the memory using 'delete'.
    *
    * @return             Pointer to copy of encoded message. It is the user's
    *                       responsibility to release the memory using the
    *                       'delete' operator (i.e., delete [] ptr;)
    */
   virtual EXTPERMETHOD OSOCTET* getMsgCopy ();

   /**
    * This method returns the internal pointer to the current encoded message.
    *
    * @return             Pointer to encoded message.
    */
   virtual EXTPERMETHOD const OSOCTET* getMsgPtr ();

   /**
    * This method reinitializes the encode buffer pointer to allow a new
    * message to be encoded. This makes it possible to reuse one message buffer
    * object in a loop to encode multiple messages. After this method is
    * called, any previously encoded message in the buffer will be overwritten
    * on the next encode call.
    *
    * @return             Completion status of operation:
    *                       - 0 (0) = success,
    *                       - negative return value is error.
    */
   EXTPERMETHOD int init ();

   /**
    * This method checks the type of the message buffer.
    *
    * @param bufferType   Enumerated identifier specifying a derived class. The
    *                       only possible value for this class is PEREncode.
    * @return             Boolean result of the match operation. True if this
    *                       is the class corresponding to the identifier
    *                       argument.
    */ 
   virtual OSBOOL isA (int bufferType) {
      return bufferType == PEREncode;
   }

   // deprecated methods
   inline int GetMsgBitCnt () { return (int)getMsgBitCnt(); }

} ;

// ASN.1 PER decode message buffer class
/**
 * The ASN1PERDecodeBuffer class is derived from the ASN1PERMessageBuffer base
 * class. It contains variables and methods specific to decoding ASN.1 PER
 * messages. It is used to manage the input buffer containing the ASN.1 message
 * to be decoded. This class has 3 overloaded constructors.
 *

 */
class EXTPERCLASS ASN1PERDecodeBuffer : public ASN1PERMessageBuffer {
 public:
   /**
    * This is a default constructor. Use getStatus() method to determine has
    * error occured during the initialization or not.
    *
    * @param aligned      Flag indicating if the message was encoded using
    *                       aligned (TRUE)* or unaligned (FALSE) encoding.
    */
   ASN1PERDecodeBuffer (OSBOOL aligned) :
      ASN1PERMessageBuffer (PERDecode, aligned)
      {}

   /**
    * This constructor is used to describe the message to be decoded. Use
    * getStatus() method to determine has error occured during the
    * initialization or not.
    *
    * @param pMsgBuf      A pointer to the message to be decoded.
    * @param msgBufLen    Length of the message buffer.
    * @param aligned      Flag indicating if the message was encoded using
    *                       aligned (TRUE) * or unaligned (FALSE) encoding.
    */
   ASN1PERDecodeBuffer (const OSOCTET* pMsgBuf, 
                        size_t msgBufLen, OSBOOL aligned) :
      ASN1PERMessageBuffer (PERDecode, (OSOCTET*)pMsgBuf, msgBufLen, aligned)
      {}

   /**
    * This constructor is used to describe the message to be decoded. Use
    * getStatus() method to determine has error occured during the
    * initialization or not.
    *
    * @param pMsgBuf      A pointer to the message to be decoded.
    * @param msgBufLen    Length of the message buffer.
    * @param aligned      Flag indicating if the message was encoded using
    *                       aligned (TRUE) * or unaligned (FALSE) encoding.
    * @param pContext     A pointer to an OSRTContext structure created by
    *                       the user.
    */
   ASN1PERDecodeBuffer (const OSOCTET* pMsgBuf, 
                        size_t msgBufLen, OSBOOL aligned, 
                        OSRTContext *pContext) :
      ASN1PERMessageBuffer (PERDecode, (OSOCTET*)pMsgBuf, msgBufLen, aligned,
            pContext) {}

    /**
     * This constructor takes a pointer to the path of a file containing 
     * a binary PER message to be decoded.
     *
     * @param filePath     Complete file path and name of file to read.
     * @param aligned      Flag indicating if the message was encoded using
     *                       aligned (TRUE) * or unaligned (FALSE) encoding.
     */
    EXTPERMETHOD ASN1PERDecodeBuffer (const char* filePath, OSBOOL aligned);

   /**
    * This method checks the type of the message buffer.
    *
    * @param bufferType   Enumerated identifier specifying a derived class. The
    *                       only possible value for this class is PERDecode.
    * @return             Boolean result of the match operation. True if this
    *                       is the class corresponding to the identifier
    *                       argument.
    */ 
   virtual OSBOOL isA (int bufferType) {
      return bufferType == PERDecode;
   }

   /**
    * This method reads the file into the buffer to decode.
    *
    * @param filePath     The zero-terminated string containing the path to the
    *                       file.
    * @return             Completion status of operation:
    *                       - 0 (0) = success,
    *                       - negative return value is error.
    */
   EXTPERMETHOD int readBinaryFile (const char* filePath);
} ;


/** @}permsgbuffclas

*/
/** @}percppruntime

 */
#endif
