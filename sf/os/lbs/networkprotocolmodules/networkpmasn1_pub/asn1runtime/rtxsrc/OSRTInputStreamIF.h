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
 * @file OSRTInputStreamIF.h
 * C++ interface class definitions for operations with input streams.
 */

/**
@file
@internalTechnology
*/

#ifndef _OSRTINPUTSTREAMIF_H_
#define _OSRTINPUTSTREAMIF_H_

#include "rtxsrc/OSRTStreamIF.h"

/**
 * @defgroup istrmclas Generic Input Stream Classes
 * @ingroup cppruntime
 *
 * The C++ interface class definitions for operations with input streams.  
 * Classes that implement this interface are used to input data from the
 * various stream types, not to decode ASN.1 messages.
 *
 * @{
 */
class EXTRTCLASS OSRTInputStreamIF : public OSRTStreamIF {
 public:
   /**
    * Virtual destructor. Closes the stream if it was opened.
    */
   virtual EXTRTMETHOD ~OSRTInputStreamIF ();

   /**
    * This method returns the current position in the stream (in octets).
    *
    * @return             The number of octets already read from the stream.
    */
   virtual size_t currentPos () = 0;

   /**
    * Tests if this input stream supports the mark and reset methods. Whether
    * or not mark and reset are supported is an invariant property of a
    * particular input stream instance. By default, it returns FALSE.
    *
    * @return             TRUE if this stream instance supports the mark and
    *                       reset methods; FALSE otherwise.
    * @see               ::rtxStreamIsMarkSupported
    */
   virtual OSBOOL markSupported () = 0;

   /**
    * This method marks the current position in this input stream. A subsequent
    * call to the reset method repositions this stream at the last marked
    * position so that subsequent reads re-read the same bytes. The
    * readAheadLimit argument tells this input stream to allow that many bytes
    * to be read before the mark position gets invalidated.
    *
    * @param readAheadLimit           the maximum limit of bytes that can be
    *                                   read before the mark position becomes
    *                                   invalid.
    * @return            Completion status of operation:
    *                      - 0 = success,
    *                      - negative return value is error.
    * @see                           ::rtxStreamMark, ::rtxStreamReset
    */
   virtual int mark (size_t readAheadLimit) = 0;

   /**
    * Read data from the stream. This method reads up to \c maxToRead bytes
    * from the stream. It may return a value less then this if the mamimum
    * number of bytes is not available.
    *
    * @param pDestBuf                 Pointer to a buffer to receive a data.
    * @param maxToRead                Size of the buffer.
    * @return             The total number of octets read into the buffer, or
    *                       negative value with error code if any error is
    *                       occurred.
    * @see                           ::rtxStreamRead
    */
   virtual long read (OSOCTET* pDestBuf, size_t maxToRead) = 0;

   /**
    * Read data from the stream. This method reads up to \c maxToRead bytes
    * from the stream. It may return a value less then this if the mamimum
    * number of bytes is not available.
    *
    * @param pDestBuf                 Pointer to a buffer to receive a data.
    * @param toReadBytes              Number of bytes to be read.
    * @return             The total number of octets read into the buffer, or
    *                       negative value with error code if any error is
    *                       occurred.
    * @see                           ::rtxStreamRead
    */
   virtual long readBlocking (OSOCTET* pDestBuf, size_t toReadBytes) = 0;

   /**
    * Repositions this stream to the position at the time the mark method was
    * last called on this input stream.
    *
    * @return            Completion status of operation:
    *                      - 0 = success,
    *                      - negative return value is error.
    * @see                           ::rtxStreamMark, ::rtxStreamReset
    */
   virtual int reset () = 0;

   /**
    * Skips over and discards the specified amount of data octets from this
    * input stream.
    *
    * @param n           The number of octets to be skipped.
    * @return            Completion status of operation:
    *                      - 0 = success,
    *                      - negative return value is error.
    * @see                           ::rtxStreamSkip
    */
   virtual int skip (size_t n) = 0;

} ;

class EXTRTCLASS OSRTInputStreamPtr {
   OSRTInputStreamIF* mPtr; 
 public:
   inline OSRTInputStreamPtr (OSRTInputStreamIF* ptr) : mPtr (ptr) {}
   inline ~OSRTInputStreamPtr () { delete mPtr; }

   inline operator OSRTInputStreamIF* ()   { return mPtr; }
   inline OSRTInputStreamIF* operator-> () { return mPtr; }
} ;

/** @} */

#endif /* _OSRTINPUTSTREAMIF_H_ */
