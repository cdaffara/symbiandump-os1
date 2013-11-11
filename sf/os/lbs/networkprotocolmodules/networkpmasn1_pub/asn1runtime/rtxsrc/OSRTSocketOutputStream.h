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
 * @file OSRTSocketOutputStream.h 
 * C++ base class definitions for operations with output socket streams.
 */

/**
@file
@internalTechnology
*/

#ifndef _OSRTSOCKETOUTPUTSTREAM_H_
#define _OSRTSOCKETOUTPUTSTREAM_H_

#include "rtxsrc/OSRTSocket.h"
#include "rtxsrc/OSRTOutputStream.h"

/**
 * Generic socket output stream. This class opens an existing socket for output in
 * binary mode and reads data from it.
 */
class EXTRTCLASS OSRTSocketOutputStream : public OSRTOutputStream {
 protected:
   OSRTSocket mSocket; //< a socket 
 public:
   /**
    * Creates and initializes a socket output stream using the OSRTSocket 
    * instance of socket.
    *
    * @param socket                  Reference to OSRTSocket instance.
    * @see                           ::rtxStreamSocketOpen
    */
   EXTRTMETHOD OSRTSocketOutputStream (OSRTSocket& socket);

   /**
    * Creates and initializes a socket output stream using the OSRTSocket 
    * instance of socket.
    *
    * @param pContext                Pointer to a context to use.
    * @param socket                  Reference to OSRTSocket instance.
    * @see                           ::rtxStreamSocketOpen
    */
   EXTRTMETHOD OSRTSocketOutputStream (OSRTContext* pContext, OSRTSocket& socket);

   /**
    * Initializes the socket output stream using the socket handle.
    *
    * @param socket                  Handle of the socket.
    * @param ownership               Indicates ownership of the socket. 
    *                                  Set to TRUE to pass ownership to 
    *                                  this object instance.  The socket 
    *                                  will be closed when this object 
    *                                  instance is deleted or goes out 
    *                                  of scope.
    * @see                           ::rtxStreamSocketAttach
    */
   EXTRTMETHOD OSRTSocketOutputStream (OSRTSOCKET socket, OSBOOL ownership = FALSE);

   /**
    * Initializes the socket output stream using the socket handle.
    *
    * @param pContext                Pointer to a context to use.
    * @param socket                  Handle of the socket.
    * @param ownership               Indicates ownership of the socket. 
    *                                  Set to TRUE to pass ownership to 
    *                                  this object instance.  The socket 
    *                                  will be closed when this object 
    *                                  instance is deleted or goes out 
    *                                  of scope.
    * @see                           ::rtxStreamSocketAttach
    */
   OSRTSocketOutputStream (OSRTContext* pContext, 
                           OSRTSOCKET socket, 
                           OSBOOL ownership = FALSE);

} ;

#endif /* _OSRTSOCKETOUTPUTSTREAM_H_ */
