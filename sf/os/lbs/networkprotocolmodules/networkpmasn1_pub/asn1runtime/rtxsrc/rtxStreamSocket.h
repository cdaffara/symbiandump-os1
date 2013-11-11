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
 * @file rtxStreamSocket.h 
 */

/**
@file
@internalTechnology
*/

#ifndef _RTXSTREAMSOCKET_H_
#define _RTXSTREAMSOCKET_H_

#include "rtxsrc/rtxStream.h"
#include "rtxsrc/rtxSocket.h"

#ifdef __cplusplus
extern "C" {
#endif

/** 
 * @defgroup rtxStreamSocket Socket stream functions.
 * Socket stream functions are used for socket stream operations.
 * @{
 */
/**
 * Attaches the existing socket handle to the stream. The socket should be
 * already opened and connected. The 'flags' parameter specifies the access
 * mode for the stream - input or output.
 *
 * @param pctxt        Pointer to a context structure variable that has 
 *                       been initialized for stream operations.
 * @param socket       The socket handle created by \c rtxSocketCreate.
 * @param flags        Specifies the access mode for the stream:
 *                       - OSRTSTRMF_INPUT = input (reading) stream;
 *                       - OSRTSTRMF_OUTPUT = output (writing) stream.
 * @return             Completion status of operation: 0 = success,
 *                       negative return value is error.
 */
EXTERNRT int rtxStreamSocketAttach (OSCTXT* pctxt, 
                                     OSRTSOCKET socket, OSUINT16 flags);

/**
 * This function closes a socket stream.
 *
 * @param pctxt        Pointer to a context structure variable that has 
 *                       been initialized for stream operations.
 * @return             Completion status of operation: 0 = success,
 *                       negative return value is error.
 */
EXTERNRT int rtxStreamSocketClose (OSCTXT* pctxt);

/**
 * This function opens a socket stream for writing.
 *
 * @param pctxt        Pointer to a context structure variable that has 
 *                       been initialized for stream operations.
 * @param host         Name of host or IP address to which to connect.
 * @param port         Port number to which to connect.
 * @return             Completion status of operation: 0 = success,
 *                       negative return value is error.
 */
EXTERNRT int rtxStreamSocketCreateWriter (OSCTXT* pctxt, 
                                           const char* host, int port);

/**
 * This function transfers ownership of the socket to or from the 
 * stream instance. The socket will be closed and deleted when the stream 
 * is closed or goes out of scope. By default stream socket owns the socket.
 *
 * @param pctxt        Pointer to a context structure variable that has 
 *                       been initialized for stream operations.
 * @param ownSocket    Boolean value.
 */
EXTERNRT int rtxStreamSocketSetOwnership (OSCTXT* pctxt, OSBOOL ownSocket);

#ifdef __cplusplus
}
#endif
/**
 * @} rtxStreamSocket
 */

#endif /* _RTXSTREAMSOCKET_H_ */
