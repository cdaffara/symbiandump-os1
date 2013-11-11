// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

/**
 @file
 @publishedAll
 @released
*/

#if !defined(__ES_PANIC_H__)
#define __ES_PANIC_H__

#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <es_panic_internal.h>
#include <es_panic_partner.h>
#endif

/** The panic category for panics experienced by ESOCK clients (was "eSock panic" before Symbian OS version 9.1)
@see TESockPanic
*/
_LIT(KESockClientPanic, "ESock_client");

/**	The panic numbers for panics panics experienced by ESOCK clients
@see KESockClientPanic
*/
enum TESockPanic
	{
	/** An invalid RSubSession handle was used. For example a RSocket call before the socket was Open()ed.
	*/
	ESockBadHandle = 0,

	/** (No longer in use)
	*/
	EBadCancel = 1,

	/** RSocket::Connect() used before a previous connect request on the same socket has completed.
	Wait for the first to complete or CancelConnect() it.
	*/
	EConnectingAlready = 2,

	/** RSocket::Read(), Recv(), RecvFrom(), or RecvOneOrMore() used before a previous read request
	on the same socket has completed. Wait for the first to complete or CancelRead()/CancelRecv() it.
	*/
	EReadingAlready = 3,

	/** RSocket::Send(), SendTo(), or Write() used before a previous write request on the same socket
	has completed. Wait for the first to complete or CancelWrite()/CancelSend() it.
	*/
	EWritingAlready = 4,

	/** (No longer in use)
	*/
	ECloseTwice = 5,

	/** RSocket::Shutdown() used before a previous shutdown request on the same socket has completed.
	Wait for the first to complete. This applies also when the shutdown type differs, eg
	Shutdown(EStopInput) must complete before Shutdown(EStopOutput) can be issued.
	*/
	EShutDownTwice = 6,

	/** The socket passed to RSocket::Accept() must be a blank socket opened by the argumentless
	RSocket::Open() overload; a socket opened with a protocol or already Accept()ed is not blank.
	*/
	EAcceptTwice = 7,

	/** RSocket::Ioctl() used before a previous IOCtl request on the same socket has completed. Wait
	for the first to complete or CancelIoctl() it.
	*/
	ETwoIoctls = 8,

	/** RSocket::SendTo() used with a socket of a protocol that is connection-oriented (such as TCP/IP)
	*/
	ECantSendToOnConnection = 9,

	/** RSocket::Listen() used with a socket of a protocol that is not connection-oriented (such as UDP/IP)
	*/
	EListenNeedsConnection = 10,

	/** RSocket::Listen() used on a socket which is already Listen()ing.
	*/
	EAlreadyListening = 11,

	/** RSocket::Accept() used on a socket which has not had Listen() called.
	*/
	ENotListening = 12,

	/** RSocket::Accept() used with a socket which proves invalid when the passive-open completes. For example
	the blank socket passed to Accept() was closed before a connection to the listening server socket was made.
	CancelAccept() must be used before closing the blank socket.
	*/
	EBadAccept = 13,

	/** A descriptor argument was corrupt when used by the socket server, which for asynchronous requests may
	only be when they complete. A common cause of this is allowing a buffer to go out of scope when using data
	transfer functions. For example:
	@code
		void CMailTransferObject::SendText(HBufC8* aData)
			{
			iMailSocket.Write(aData->Des(), iStatus);
			}
	@endcode
	Here the coder has overlooked the fact that HBufC8::Des() returns a TPtr8 object, which being a temporary
	will go out of scope when Write() returns. However because ESOCK runs at high scheduler priority and protocols
	are commonly able to accept data immediately the request will usually have been completed before Write()
	returns. The panic might only occur for this mail application when it sends a large amount of data or the
	network is unusually slow. In this example the code should simply have dereferenced the HBufC8* as all that
	Write() requires is a const TDesC8.
	*/
	EBadDescriptor = 14,

	/** A request was made of a RHostResolver, RServiceResolver, or RNetDatabase when it was still processing a previous
	request. Usually this would be with an explicitly asynchronous request and you must either cancel it or wait for it
	to complete. If occurs with multiple threads issuing synchronous requests then you must implement appropriate
	synchronisation between the threads to avoid this simultaneous use.
	*/
	ETwice = 15,

	/** (No longer in use)
	*/
	EBadQueyComplete = 16,

	/** An improper request was made of a blank socket, ie one created through the argumentless Open() method and not
	yet opened as a passive connection through Accept() on a Listen()ing socket. Improper requests include any which
	require a specific underlying technology, such as Shutdown() - all that need be done to clean-up a blank socket
	is Close()ing it.
	*/
	ENullSocket = 17,

	/** (No longer in use)
	*/
	EMbufManagerNotLoaded = 18,

	/** RHostResolver::Next() was called without a preceding resolution request such as RHostResolver::GetByName()
	*/
	ENoResolveFirst = 19,

	/** Attempted to Connect() on an RSocket in a programmatically invalid state, such as having been used for a
	pending Accept().
	*/
	ECannotConnect = 20,

	/** RHostResolver::QueryGetNext() was called without a preceding RHostResolver::Query().
	*/
	ENoQueryFirst = 21,

	/** (No longer in use)
	*/
	EPermissionDenied = 22,

	/** RSocket::Open() or RHostResolver::Open() used with a RConnection or RSubConnection opened upon a
	different RSocketServ to that supplied to the current Open().
	*/
	EBadRConnection = 23,

	/** RConnection::Open(RSocketServ&, TName&) used with an invalid TName (perhaps never initialised from
	RConnection::Name() or that source connection was subsequently closed)
	*/
	EInvalidName = 24,

    /**  An extension interface has issued a bad request
    */
	EBadRequest = 25,

	/**	The descriptor passed by the client could not be read from by the server.
	@see EBadDescriptor
	*/
	EBadDescriptorRead = 26,

	/**	The descriptor passed by the client could not be written to by the server.
	@see EBadDescriptor
	*/
	EBadDescriptorWrite = 27,

	/**	The descriptor passed by the client had an invalid length when read by the server.
	@see EBadDescriptor
	*/
	EBadDescriptorLength = 28,
	/** Open on an already opened socket
	*/
	ENotNullSocket	= 29

    };


#endif   // __ES_PANIC_H__


