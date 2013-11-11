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
 * @file OSRTSocketInputStream.h 
 * C++ base class definitions for operations with input socket streams.
 */

/**
@file
@internalTechnology
*/

#ifndef _OSRTSOCKETINPUTSTREAM_H_
#define _OSRTSOCKETINPUTSTREAM_H_

#include "rtxsrc/OSRTSocket.h"
#include "rtxsrc/OSRTInputStream.h"

/**
 * Generic socket input stream. This class opens an existing socket for input in
 * binary mode and reads data from it.
 */
class EXTRTCLASS OSRTSocketInputStream : public OSRTInputStream {
 protected:
   OSRTSocket mSocket; //< a socket 
 public:
   /**
    * Creates and initializes a socket input stream using the OSRTSocket 
    * instance of socket.
    *
    * @param socket                    Reference to OSRTSocket instance.
    * @see                           ::rtxStreamSocketOpen
    */
   EXTRTMETHOD OSRTSocketInputStream (OSRTSocket& socket);

   /**
    * Creates and initializes a socket input stream using the OSRTSocket 
    * instance of socket.
    *
    * @param pContext                 Pointer to a context to use.
    * @param socket                   Reference to OSRTSocket instance.
    * @see                           ::rtxStreamSocketOpen
    */
   EXTRTMETHOD OSRTSocketInputStream (OSRTContext* pContext, OSRTSocket& socket);

   /**
    * Creates and initializes the socket input stream using the socket
    * handle.
    *
    * @param socket                    Handle of the socket.
    * @param ownership                 Indicates ownership of the socket. 
    *                                  Set to TRUE to pass ownership to 
    *                                  this object instance.  The socket 
    *                                  will be closed when this object 
    *                                  instance is deleted or goes out 
    *                                  of scope.
    * @see                           ::rtxStreamSocketAttach
    */
   EXTRTMETHOD OSRTSocketInputStream (OSRTSOCKET socket, OSBOOL ownership = FALSE);

   /**
    * Creates and initializes the socket input stream using the socket
    * handle.
    *
    * @param pContext                  Pointer to a context to use.
    * @param socket                    Handle of the socket.
    * @param ownership                 Indicates ownership of the socket. 
    *                                  Set to TRUE to pass ownership to 
    *                                  this object instance.  The socket 
    *                                  will be closed when this object 
    *                                  instance is deleted or goes out 
    *                                  of scope.
    * @see                           ::rtxStreamSocketAttach
    */
   OSRTSocketInputStream (OSRTContext* pContext, 
                          OSRTSOCKET socket, 
                          OSBOOL ownership = FALSE);

} ;

#endif /* _OSRTSOCKETINPUTSTREAM_H_ */
