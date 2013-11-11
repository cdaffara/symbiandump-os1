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
 * @file OSRTMemoryInputStream.h 
 * C++ base class definitions for operations with input memory streams.
 */

/**
@file
@internalTechnology
*/

#ifndef _OSRTMEMORYINPUTSTREAM_H_
#define _OSRTMEMORYINPUTSTREAM_H_

#include "rtxsrc/OSRTInputStream.h"

/**
 * Generic memory input stream. This class opens an existing file for input in
 * binary mode and reads data from it.
 */
class EXTRTCLASS OSRTMemoryInputStream : public OSRTInputStream {
 public:
   /**
    * Initializes the memory input stream using the specified memory buffer.
    *
    * @param pMemBuf                   The pointer to the buffer.
    * @param bufSize                   The size of the buffer.
    * @see                            ::rtxStreamMemoryAttach
    */
   EXTRTMETHOD OSRTMemoryInputStream (const OSOCTET* pMemBuf, size_t bufSize);

   /**
    * Initializes the memory input stream using the specified memory buffer.
    *
    * @param pContext                  Pointer to a context to use.
    * @param pMemBuf                   The pointer to the buffer.
    * @param bufSize                   The size of the buffer.
    * @see                            ::rtxStreamMemoryAttach
    */
   OSRTMemoryInputStream 
      (OSRTContext* pContext, const OSOCTET* pMemBuf, size_t bufSize);

} ;

#endif /* _OSRTMEMORYINPUTSTREAM_H_ */
