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
 * @file OSRTFileInputStream.h 
 * C++ base class definitions for operations with input file streams.
 */

/**
@file
@internalTechnology
*/

#ifndef _OSRTFILEINPUTSTREAM_H_
#define _OSRTFILEINPUTSTREAM_H_

#include "rtxsrc/OSRTInputStream.h"

/**
 * Generic file input stream. This class opens an existing file for input in
 * binary mode and reads data from it.
 */
class EXTRTCLASS OSRTFileInputStream : public OSRTInputStream {
 public:
   /**
    * Creates and initializes a file input stream using the name of file.
    *
    * @param pFilename                Name of file.
    * @see                           ::rtxStreamFileOpen
    */
   EXTRTMETHOD OSRTFileInputStream (const char* pFilename);

   /**
    * Creates and initializes a file input stream using the name of file.
    *
    * @param pContext                 Pointer to a context to use.
    * @param pFilename                Name of file.
    * @see                           ::rtxStreamFileOpen
    */
   EXTRTMETHOD OSRTFileInputStream (OSRTContext* pContext, const char* pFilename);

   /**
    * Initializes the file input stream using the opened FILE structure
    * descriptor.
    *
    * @param file                     Pointer to FILE structure.
    * @see                           ::rtxStreamFileAttach
    */
   EXTRTMETHOD OSRTFileInputStream (FILE* file);

   /**
    * Initializes the file input stream using the opened FILE structure
    * descriptor.
    *
    * @param pContext                 Pointer to a context to use.
    * @param file                     Pointer to FILE structure.
    * @see                           ::rtxStreamFileAttach
    */
   EXTRTMETHOD OSRTFileInputStream (OSRTContext* pContext, FILE* file);

} ;

#endif /* _OSRTFILEINPUTSTREAM_H_ */
