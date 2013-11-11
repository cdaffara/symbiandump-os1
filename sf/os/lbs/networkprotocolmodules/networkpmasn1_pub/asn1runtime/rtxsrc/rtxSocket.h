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
 * @file rtxSocket.h 
 */

/**
@file
@internalTechnology
*/

#ifndef _RTXSOCKET_H_
#define _RTXSOCKET_H_

#include "rtxsrc/osSysTypes.h"
#include "rtxsrc/rtxExternDefs.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined (_WIN64) && !defined(__SYMBIAN32__)
typedef unsigned __int64 OSRTSOCKET; /**< socket handle */
#elif defined (_WIN32) && !defined(__SYMBIAN32__)
typedef unsigned int OSRTSOCKET; /**< socket handle */
#else
typedef int OSRTSOCKET;          /**< socket handle */
#endif

#define OSRTSOCKET_INVALID ((OSRTSOCKET)-1)

/**
 * @defgroup ccfSocket TCP/IP or UDP socket utility functions
 * @{
 */ 
/** 
 * The IP address represented as unsigned long value. The most significant 8
 * bits in this unsigned long value represent the first number of the IP
 * address. The least significant 8 bits represent the last number of the IP
 * address.
 */
typedef unsigned long OSIPADDR;

#define OSIPADDR_ANY   ((OSIPADDR)0)
#define OSIPADDR_LOCAL ((OSIPADDR)0x7f000001UL) /* 127.0.0.1 */

struct in_addr;

/**
 * This function permits an incoming connection attempt on a socket. It
 * extracts the first connection on the queue of pending connections on socket.
 * It then creates a new socket and returns a handle to the new socket. The
 * newly created socket is the socket that will handle the actual connection
 * and has the same properties as original socket. See description of 'accept'
 * socket function for further details.
 *
 * @param socket       The socket handle created by call to ::rtxSocketCreate
 *                       function.
 * @param pNewSocket   The pointer to variable to receive the new socket
 *                       handle.
 * @param destAddr     Optional pointer to a buffer that receives the IP
 *                       address of the connecting entity. It may be NULL.
 * @param destPort     Optional pointer to a buffer that receives the port of
 *                       the connecting entity. It may be NULL.
 * @return             Completion status of operation: 0 (0) = success,
 *                       negative return value is error.
 */
EXTERNRT int rtxSocketAccept (OSRTSOCKET socket, OSRTSOCKET *pNewSocket, 
                               OSIPADDR* destAddr, int* destPort);

/**
 * This function converts an IP address to its string representation.
 *
 * @param ipAddr       The IP address to be converted.
 * @param pbuf         Pointer to the buffer to receive a string with the IP
 *                       address.
 * @param bufsize      Size of the buffer.
 * @return             Completion status of operation: 0 (0) = success,
 *                       negative return value is error.
 */
EXTERNRT int rtxSocketAddrToStr (OSIPADDR ipAddr, char* pbuf, size_t bufsize);

/**
 * This function associates a local address with a socket. It is used on an
 * unconnected socket before subsequent calls to the ::rtxSocketConnect or
 * ::rtxSocketListen functions. See description of 'bind' socket function for
 * further details.
 *
 * @param socket       The socket handle created by call to ::rtxSocketCreate
 *                       function.
 * @param addr         The local IP address to assign to the socket.
 * @param port         The local port number to assign to the socket.
 * @return             Completion status of operation: 0 (0) = success,
 *                       negative return value is error.
 */
EXTERNRT int rtxSocketBind (OSRTSOCKET socket, OSIPADDR addr, int port);

/**
 * This function closes an existing socket.
 *
 * @param socket       The socket handle created by call to ::rtxSocketCreate
 *                       or ::rtxSocketAccept function.
 * @return             Completion status of operation: 0 (0) = success,
 *                       negative return value is error.
 */
EXTERNRT int rtxSocketClose (OSRTSOCKET socket);

/**
 * This function establishes a connection to a specified socket. It is used to
 * create a connection to the specified destination. When the socket call
 * completes successfully, the socket is ready to send and receive data. See
 * description of 'connect' socket function for further details.
 *
 * @param socket       The socket handle created by call to ::rtxSocketCreate
 *                       function.
 * @param host         The null-terminated string with the IP address in the
 *                       following format: "NNN.NNN.NNN.NNN", where NNN is a
 *                       number in the range (0..255).
 * @param port         The destination port to connect.
 * @return             Completion status of operation: 0 (0) = success,
 *                       negative return value is error.
 */
EXTERNRT int rtxSocketConnect (OSRTSOCKET socket, const char* host, int port);

/**
 * This function creates a TCP socket.
 *
 * @param psocket      The pointer to the socket handle variable to receive
 *                       the handle of new socket.
 * @return             Completion status of operation: 0 (0) = success,
 *                       negative return value is error.
 */
EXTERNRT int rtxSocketCreate (OSRTSOCKET* psocket);

/*
 * This function creates a UDP socket.
 *
 * @param psocket    The Pointer to the socket handle variable to receive
 *                   the handle of the new socket.
 *
 * @return           0 upon success, < 0 upon failure. */
EXTERNRT int rtxSocketCreateUDP (OSRTSOCKET *psocket);

/**
 * This function resolves the given host name to an IP address.  The 
 * resulting address is stored in the given socket address structure.
 *
 * @param host          Host name to resolve
 * @param inaddr        Socket address structure to receive resolved IP address
 * @return              Completion status of operation: 0 (0) = success,
 *                        negative return value is error.
 */
EXTERNRT int rtxSocketGetHost (const char* host, struct in_addr *inaddr);

/**
 * This function initiates use of sockets by an application. This function must
 * be called first before use sockets.
 *
 * @return             Completion status of operation: 0 (0) = success,
 *                       negative return value is error.
 */
EXTERNRT int rtxSocketsInit ();

/**
 * This function places a socket a state where it is listening for an incoming
 * connection. To accept connections, a socket is first created with the
 * ::rtxSocketCreate function and bound to a local address with the
 * ::rtxSocketBind function, a maxConnection for incoming connections is
 * specified with ::rtxSocketListen, and then the connections are accepted with
 * the ::rtxSocketAccept function. See description of 'listen' socket function
 * for further details.
 *
 * @param socket           The socket handle created by call to
 *                           ::rtxSocketCreate function.
 * @param maxConnection    Maximum length of the queue of pending connections.
 * @return                 Completion status of operation: 0 (0) =
 *                           success, negative return value is error.
 */
EXTERNRT int rtxSocketListen (OSRTSOCKET socket, int maxConnection);

/**
 * This function parses a simple URL of the form 
 * \<protocol\>://\<address\>:\<port\>
 * into its individual components.  It is assumed that the buffer the 
 * URL is provided in is modifiable.  Null-terminators are inserted in the 
 * buffer to delimit the individual components.  If the user needs to use 
 * the URL in unparsed form for any other purpose, they will need to make 
 * a copy of it before calling this function.
 *
 * @param url           URL to be parsed.  Buffer will be altered.
 * @param protocol      Protocol string parsed from the URL.
 * @param address       IP address or domain name parsed from URL.
 * @param port          Optional port number.  Zero if no port provided.
 * @return              Zero if parse successful or negative error code.
 */
EXTERNRT int rtxSocketParseURL 
(char* url, char** protocol, char** address, int* port);

/**
 * This function receives data from a connected socket. It is used to read
 * incoming data on sockets. The socket must be connected before calling this
 * function. See description of 'recv' socket function for further details.
 *
 * @param socket       The socket handle created by call to ::rtxSocketCreate
 *                       or ::rtxSocketAccept function.
 * @param pbuf         Pointer to the buffer for the incoming data.
 * @param bufsize      Length of the buffer.
 * @return             If no error occurs, returns the number of bytes
 *                       received. Otherwise, the negative value is error code.
 */
EXTERNRT int rtxSocketRecv (OSRTSOCKET socket, OSOCTET* pbuf, int bufsize);

/**
 * This function sends data on a connected socket. It is used to write outgoing
 * data on a connected socket. See description of 'send' socket function for
 * further details.
 *
 * @param socket       The socket handle created by call to ::rtxSocketCreate
 *                       or ::rtxSocketAccept function.
 * @param pdata        Buffer containing the data to be transmitted.
 * @param size         Length of the data in pdata.
 * @return             Completion status of operation: 0 (0) = success,
 *                       negative return value is error.
 */
EXTERNRT int rtxSocketSend (OSRTSOCKET socket, const OSOCTET* pdata, int size);

/**
 * This function converts the string with IP address to a double word
 * representation. The converted address may be used with the ::rtxSocketBind
 * function.
 *
 * @param pIPAddrStr   The null-terminated string with the IP address in the
 *                       following format: "NNN.NNN.NNN.NNN", where NNN is a
 *                       number in the range (0..255).
 * @param pIPAddr      Pointer to the converted IP address.
 * @return             Completion status of operation: 0 (0) = success,
 *                       negative return value is error.
 */
EXTERNRT int rtxSocketStrToAddr (const char* pIPAddrStr, OSIPADDR* pIPAddr);

/**
 * @}
 */
#ifdef __cplusplus
}
#endif

#endif /* _RTXSOCKET_H_ */

