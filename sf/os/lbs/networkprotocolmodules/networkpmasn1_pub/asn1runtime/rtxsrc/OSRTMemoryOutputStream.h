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
 * @file OSRTMemoryOutputStream.h 
 * C++ base class definitions for operations with output memory streams.
 */

/**
@file
@internalTechnology
*/

#ifndef _OSRTMEMORYOUTPUTSTREAM_H_
#define _OSRTMEMORYOUTPUTSTREAM_H_

#include "rtxsrc/OSRTOutputStream.h"

/**
 * Generic memory output stream. This class opens an existing file for output in
 * binary mode and reads data from it.
 */
class EXTRTCLASS OSRTMemoryOutputStream : public OSRTOutputStream {
 public:
   /**
    * Initializes the memory output stream using the specified memory buffer.
    *
    * @param pMemBuf                   The pointer to the buffer.
    * @param bufSize                   The size of the buffer.
    * @exception OSCStreamException    stream can't be created or initialized.
    * @see                            ::rtxStreamMemoryAttach
    */
   EXTRTMETHOD OSRTMemoryOutputStream (OSOCTET* pMemBuf, size_t bufSize);

   /**
    * Initializes the memory output stream using the specified memory buffer.
    *
    * @param pContext                  Pointer to a context to use.
    * @param pMemBuf                   The pointer to the buffer.
    * @param bufSize                   The size of the buffer.
    * @exception OSCStreamException    stream can't be created or initialized.
    * @see                            ::rtxStreamMemoryAttach
    */
   EXTRTMETHOD OSRTMemoryOutputStream (OSRTContext* pContext, OSOCTET* pMemBuf, size_t bufSize);

} ;

#endif /* _OSRTMEMORYOUTPUTSTREAM_H_ */
