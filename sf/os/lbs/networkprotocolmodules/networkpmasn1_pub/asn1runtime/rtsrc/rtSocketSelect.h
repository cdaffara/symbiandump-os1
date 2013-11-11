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
 * @file rtSocketSelect.h 
 * Support for socket select operation.
 */


/**
@file
@internalTechnology
*/

#ifndef _RTSOCKETSELECT_H_
#define _RTSOCKETSELECT_H_

#ifdef _WIN32_WCE
#include <winsock.h>
#elif (defined(_WIN32) || defined(_WIN64)) && !defined(__SYMBIAN32__)
#include <sys/types.h>
#ifdef INCL_WINSOCK_API_TYPEDEFS
#undef INCL_WINSOCK_API_TYPEDEFS
#endif
#ifdef INCL_WINSOCK_API_PROTOTYPES
#undef INCL_WINSOCK_API_PROTOTYPES
#endif
#define INCL_WINSOCK_API_TYPEDEFS   1
#define INCL_WINSOCK_API_PROTOTYPES 0
#include <winsock2.h>
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#endif

#include "asn1type.h"
#ifdef __cplusplus
extern "C" {
#endif


/** 
 * @addtogroup sockets 
 * @ingroup cruntime 
 * @{
 */

#if !defined(_AIX) && !defined (__SYMBIAN32__) /* AIX 4.3 & Symbian doesn't support select */
/**
 * This function is used for synchronous monitoring of multiple sockets.
 * For more information refer to documnetation of "select" system call. 
 *
 * @param nfds         The highest numbered descriptor to be monitored 
 *                       plus one.
 * @param readfds      The descriptors listed in readfds will be watched for 
 *                       whether read would block on them.
 * @param writefds     The descriptors listed in writefds will be watched for
 *                       whether write would block on them.
 * @param exceptfds    The descriptors listed in exceptfds will be watched for
 *                       exceptions.
 * @param timeout      Upper bound on amount of time elapsed before select 
 *                       returns. 
 *
 * @return             Completion status of operation: 0 (ASN_OK) = success,
 *                       negative return value is error.
 */                                                       
EXTERNRT int rtSocketSelect(int nfds, fd_set *readfds, fd_set *writefds, 
                            fd_set *exceptfds, struct timeval * timeout);
#endif /* !defined(_AIX or SYMBIAN) */

/** @} */
#ifdef __cplusplus
}
#endif

#endif /* _RTSOCKETSELECT_H_ */
