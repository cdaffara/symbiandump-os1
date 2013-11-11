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
 * @file OSRTStreamIF.h 
 * C++ interface class definitions for operations with I/O streams.
 */

/**
@file
@internalTechnology
*/

#ifndef _OSRTSTREAMIF_H_
#define _OSRTSTREAMIF_H_

#include "rtxsrc/rtxCommon.h"
#include "rtxsrc/OSRTCtxtHolderIF.h"

/** @defgroup osrtstream ASN.1 Stream Classes
 *
 *  Classes that read or write ASN.1 messages to files, sockets, memory buffers,
 *  et c., are derived from this class.
 *
 *  @{
 */
class EXTRTCLASS OSRTStreamIF : public OSRTCtxtHolderIF {
 public:

   /**
    * Closes the input or output stream and releases any system resources
    * associated with the stream. For output streams this function also flushes
    * all internal buffers to the stream.
    *
    * @return            Completion status of operation:
    *                      - 0 = success,
    *                      - negative return value is error.
    * @see       ::rtxStreamClose
    */
   virtual int close () = 0;

   /**
    * Flushes the buffered data to the stream.
    *
    * @return            Completion status of operation:
    *                      - 0 = success,
    *                      - negative return value is error.
    * @see       ::rtxStreamFlush
    */
   virtual int flush () = 0;

   /**
    * Checks, is the stream opened or not.
    * @returns TRUE, if the stream is opened, FALSE otherwise. 
    * @see       ::rtxStreamIsOpened
    */
   virtual OSBOOL isOpened () = 0;

} ;

/** @} */

#endif /* _OSRTSTREAMIF_H_ */
