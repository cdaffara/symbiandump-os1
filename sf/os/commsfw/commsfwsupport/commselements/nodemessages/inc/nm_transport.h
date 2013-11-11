// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalTechnology
*/

#ifndef SYMBIAN_NM_TRANSPORT_H
#define SYMBIAN_NM_TRANSPORT_H

#include <elements/nm_address.h>
#include <elements/metadata.h>

namespace Messages
{
class MTransportSender
/** Defines the interface used for sending messages to a peer worker thread. Various functions are present,
supporting different message sizes and different policies for coping with the inability to send. Behaviours
common to all Send...() operations are finding a transport to the peer based upon its cookie and panicking
if no transport is available (the sender has corrupted the peer cookie or is using it after termination of
the transport service - either case is serious misbehaviour)
*/
	{
public:
	/** Blocking send functions. If no outbound queue space is available they block until
	either space becomes available or a GENEROUS time limit is exceeded, in which case it panics diagnostically.
	This relieves the caller of the burden of supporting an error case which will only be seen in a grossly
	malfunctioning system and so will be hard to test and difficult to recover from. To avoid deadlock due
	to the peer being in the same state the inbound queue is drained while waiting for space. The receipt order
	is always preserved.
	*/
	virtual void PostMessage(const TRuntimeCtxId& aPostFrom, const TRuntimeCtxId& aPostTo, const Meta::SMetaData& aMessage) = 0;
	};

//[EC120 TODO] RZ it seems a little bit bizzare that a receiver's abstraction
//has nothing to do with message reception. It only deals with address registration.
//rename to something like MTransportReceiversRegister? 

//[MZ] Not only registration. VerifyDestination and perhaps more in the future. MTransportReceiver is good.
class MTransportReceiver
	{
public:
	virtual TInt RegisteredCount() const = 0;
	virtual void RegisterAddress(Messages::TRuntimeCtxId& aNodeId) = 0;

	/** Relinquish an address.
	Note that this is also required for addresses other than object handles
	for	logging, debug support, and code sanity reasons.
	*/
	virtual void DeregisterAddress(Messages::TRuntimeCtxId& aNodeId) = 0;
	//[RZ] VerifyDestination not called by NodeMessages, only called by the transport impl.
	//[MZ] It does not matter who the caller is but what the API is.
	//VerifyDestination is part of this API is it not?
	virtual TInt VerifyDestination(const Messages::TRuntimeCtxId& aDestination) = 0;
	};

} //namespace Messages


#endif
//SYMBIAN_NM_TRANSPORT_H

