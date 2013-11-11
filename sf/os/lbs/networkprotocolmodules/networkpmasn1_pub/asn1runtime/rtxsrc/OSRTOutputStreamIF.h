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
 * @file OSRTOutputStreamIF.h 
 * C++ interface class definitions for operations with output streams.
 */

/**
@file
@internalTechnology
*/

#ifndef _OSRTOUTPUTSTREAMIF_H_
#define _OSRTOUTPUTSTREAMIF_H_

#include "rtxsrc/OSRTStreamIF.h"

/**
 * @defgroup ostrmclas  Generic Output Stream Classes
 * @ingroup cppruntime
 *
 * The interface class definition for operations with output streams.  Classes
 * that implement this interface are used for writing data to the various 
 * stream types, not to encode ASN.1 messages.
 *
 * @{
 */
class EXTRTCLASS OSRTOutputStreamIF : public OSRTStreamIF {
 public:
   /**
    * Virtual destructor. Closes the stream if it was opened.
    */
   virtual EXTRTMETHOD ~OSRTOutputStreamIF ();

   /**
    * Write data to the stream.  This method writes the given number of 
    * octets from the given array to the output stream.
    *
    * @param pdata       Pointer to the data to be written.
    * @param size        The number of octets to write.
    * @return            The total number of octets written into the stream, or
    *                       negative value with error code if any error is
    *                       occurred.
    * @see               ::rtxStreamWrite
    */
   virtual long write (const OSOCTET* pdata, size_t size) = 0;

} ;

class EXTRTCLASS OSRTOutputStreamPtr {
   OSRTOutputStreamIF* mPtr; 
 public:
   inline OSRTOutputStreamPtr (OSRTOutputStreamIF* ptr) : mPtr (ptr) {}
   inline ~OSRTOutputStreamPtr () { delete mPtr; }

   inline operator OSRTOutputStreamIF* ()   { return mPtr; }
   inline OSRTOutputStreamIF* operator-> () { return mPtr; }
} ;

/** @} */

#endif /* _OSRTOUTPUTSTREAMIF_H_ */
