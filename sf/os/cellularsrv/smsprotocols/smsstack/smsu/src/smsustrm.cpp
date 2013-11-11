// Copyright (c) 1999-2010 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// This file contains the implementation of the RSmsSocket class
// 
//

/**
 @file
*/


#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "smsustrmTraces.h"
#endif

#include "smsustrm.h"
#include "smsumain.h"
#include <es_sock.h>
#include "smsstacklog.h"


/**
 *  Constructor
 *  @param aSocket representing the socket the buffer reads and writes to.
 */
RSmsSocketBuf::RSmsSocketBuf(RSocket& aSocket)
	:iSocket(aSocket)
	{
	SetBuf(ERead|EWrite,iBuffer,iBuffer);
	} // RSmsSocketBuf::RSmsSocketBuf


/**
 *  Fills the buffer's read area from the socket.
 *  
 *  @leave The function panics if the socket is not readable.
 *  @param aMaxLength Unused
 *  @return The length of the buffer
 */
TInt RSmsSocketBuf::UnderflowL(TInt)
//Leaves if the Request Status (s) is in error
//Panics if ERead==0
//
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, RSMSSOCKETBUF_UNDERFLOWL_1, "RSmsSocketBuf::UnderflowL()");

	__ASSERT_ALWAYS(Avail(ERead)==0,SmsuPanic(KSsmuPanicStreamReadUnavailable));
	SocketWriteL();
	SetBuf(EWrite,iBuffer,iBuffer);
//
	TRequestStatus s;
	TSockXfrLength l;
	TPtr8 ptr(iBuffer,sizeof(iBuffer));
//	iSocket.RecvOneOrMore(ptr,0,s,l);
	iSocket.Recv(ptr,0,s,l);
	User::WaitForRequest(s); 
	User::LeaveIfError(s.Int());
	TInt len=ptr.Length();
	SetBuf(ERead,iBuffer,iBuffer+len);
	return len;
	} // RSmsSocketBuf::UnderflowL


/**
 *  Empties the buffer and sets up the buffer's write area.
 *  
 *  The function panics if the socket is not writable. 
 */
void RSmsSocketBuf::OverflowL()
//Panics if EWrite == 0
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, RSMSSOCKETBUF_OVERFLOWL_1, "RSmsSocketBuf::OverflowL()");

	__ASSERT_ALWAYS(Avail(EWrite)==0,SmsuPanic(KSsmuPanicStreamWriteUnavailable));
	SetBuf(ERead,iBuffer,iBuffer);
//
	SocketWriteL();
	SetBuf(EWrite,iBuffer,iBuffer+sizeof(iBuffer));
	} // RSmsSocketBuf::OverflowL


/**
 *  Synchronises the stream buffer with the stream, leaving if any error occurs.
 */
void RSmsSocketBuf::DoSynchL()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, RSMSSOCKETBUF_DOSYNCHL_1, "RSmsSocketBuf::DoSynchL()");

	SocketWriteL();
	SetBuf(ERead|EWrite,iBuffer,iBuffer);
	} // RSmsSocketBuf::DoSynchL


/**
 *  Writes the buffered data to the socket.
 */
void RSmsSocketBuf::SocketWriteL()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, RSMSSOCKETBUF_SOCKETWRITEL_1, "RSmsSocketBuf::SocketWriteL()");

	TInt length=Lag(EWrite);
	if (length==0)
		return;
//
	TRequestStatus s;
	iSocket.Write(TPtrC8(iBuffer,length),s);
	User::WaitForRequest(s);  
	User::LeaveIfError(s.Int());
	} // RSmsSocketBuf::SocketWriteL


/**
 *  Constructor.
 *  
 *  @param aSocket RSmsSocketBuf through which to read data
 *  @capability None
 */
EXPORT_C RSmsSocketReadStream::RSmsSocketReadStream(RSocket& aSocket)
	:RReadStream(&iBuf),
	iBuf(aSocket)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, RSMSSOCKETREADSTREAM_CTOR_1, "RSmsSocketReadStream::RSmsSocketReadStream()");

	} // RSmsSocketReadStream::RSmsSocketReadStream


/**
 *  Constructor.
 *  
 *  @param aSocket RSmsSocketBuf through which to write data
 *  @capability None
 */
EXPORT_C RSmsSocketWriteStream::RSmsSocketWriteStream(RSocket& aSocket)
	:RWriteStream(&iBuf),
	iBuf(aSocket)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, RSMSSOCKETWRITESTREAM_CTOR_1, "RSmsSocketWriteStream::RSmsSocketWriteStream()");

	} // RSmsSocketWriteStream::RSmsSocketWriteStream

