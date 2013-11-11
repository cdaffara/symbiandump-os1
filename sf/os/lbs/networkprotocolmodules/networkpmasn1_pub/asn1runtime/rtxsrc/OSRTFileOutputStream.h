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
 * @file OSRTFileOutputStream.h 
 * C++ base class definitions for operations with output file streams.
 */

/**
@file
@internalTechnology
*/

#ifndef _OSRTFILEOUTPUTSTREAM_H_
#define _OSRTFILEOUTPUTSTREAM_H_

#include "rtxsrc/OSRTOutputStream.h"

/**
 * Generic file output stream. This class opens an existing file for output in
 * binary mode and reads data from it.
 */
class EXTRTCLASS OSRTFileOutputStream : public OSRTOutputStream {
 public:
   /**
    * Creates and initializes a file output stream using the name of file.
    *
    * @param pFilename                Name of file.
    * @exception OSStreamException    Stream create or initialize failed.
    * @see                           ::rtxStreamFileOpen
    */
   EXTRTMETHOD OSRTFileOutputStream (const char* pFilename);

   /**
    * Creates and initializes a file output stream using the name of file.
    *
    * @param pContext                 Pointer to a context to use.
    * @param pFilename                Name of file.
    * @exception OSStreamException    Stream create or initialize failed.
    * @see                           ::rtxStreamFileOpen
    */
   EXTRTMETHOD OSRTFileOutputStream (OSRTContext* pContext, const char* pFilename);

   /**
    * Initializes the file output stream using the opened FILE structure
    * descriptor.
    *
    * @param file                     Pointer to FILE structure.
    * @exception OSStreamException    Stream create or initialize failed.
    * @see                           ::rtxStreamFileAttach
    */
   EXTRTMETHOD OSRTFileOutputStream (FILE* file);

   /**
    * Initializes the file output stream using the opened FILE structure
    * descriptor.
    *
    * @param pContext                 Pointer to a context to use.
    * @param file                     Pointer to FILE structure.
    * @exception OSStreamException    Stream create or initialize failed.
    * @see                           ::rtxStreamFileAttach
    */
   EXTRTMETHOD OSRTFileOutputStream (OSRTContext* pContext, FILE* file);

} ;

#endif /* _OSRTFILEOUTPUTSTREAM_H_ */
