// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Definition of network request channel component.
// 
//

/**
 @file
 @internalTechnology
 @released
*/

#ifndef NETREQUESTCHANNEL_H_
#define NETREQUESTCHANNEL_H_

#include <e32hashtab.h>
#include "lbsnetinternalapi.h"
#include "lbsnrhngmsgs.h"


class CNetworkRequestMessageBuffer;

//
// Network Request Handler Channel
//

/** Observer for Network Request Handler interface
*/
class MNetworkRequestObserver
	{
public:
	virtual void ProcessNetRequestMessage(const TLbsNetInternalMsgBase& aMessage) = 0;
	};

/** Manager for the Network Request Handler interface.

This class handles sending and receiving messages on the
Network Request Handler interface. It is implemented as an active object
to allow it to asnychronously wait for messages to arrive. Sending
is synchronous.
*/
class CNetworkRequestChannel : public CActive, 
							   public MLbsNetChannelObserver
	{
public:
	static CNetworkRequestChannel* NewL(MNetworkRequestObserver& aObserver);
	~CNetworkRequestChannel();
	
	// from CActive
	void RunL();
	void DoCancel();
	TInt RunError(TInt aError);
	
	// Send a message to the NRH.
	void SendNetRequestMessage(const TLbsNetInternalMsgBase& aMessage);
	
	// from MLbsNetChannelObserver : process an incoming message from the NRH
	void ProcessNetChannelMessage(RLbsNetChannel::TLbsNetChannelId aChannelId, const TLbsNetInternalMsgBase& aMessage);
	
private:
	CNetworkRequestChannel(MNetworkRequestObserver& aObserver);
	void ConstructL();
	
	void SendMessageAndNotifyForResponse(const TLbsNetInternalMsgBase& aMessage);
	void SendNextBufferedMessage();
	
private:
	MNetworkRequestObserver& iObserver;
	RLbsNetChannel iNetRequestChannel;
	CNetworkRequestMessageBuffer* iMsgBuffer;
	};

//
// Buffer classes
//

/** Abstract base class for a message buffer.
*/
class CNetworkRequestMessageBuffer : public CBase
	{
public:
	virtual TInt BufferMessage(const TLbsNetInternalMsgBase& aMessage) = 0;
	virtual const TLbsNetInternalMsgBase* PeekNextMessage() = 0;
	virtual void RemoveMessage(const TLbsNetInternalMsgBase* aMessage) = 0;

	static TBool IsMsgTypeEqual(
			const TLbsNetInternalMsgBase::TLbsNetInternalMsgType* aType, 
			const TLbsNetInternalMsgBase& aData);

	static TBool IsMsgEqual(
			const TLbsNetInternalMsgBase& aData1, 
			const TLbsNetInternalMsgBase& aData2);
protected:
	CNetworkRequestMessageBuffer() {}
	CNetworkRequestMessageBuffer(const CNetworkRequestMessageBuffer&);
	};

/** Buffer used when LBS in normal mode.

In normal mode, LBS processes location and privacy requests but
it can only do one at a time. Hence this buffer will remove
messages for previous requests if a newer request comes in.

This buffer doesn't allocate any memory at run-time, making
it safe in a OOM situation.
*/
class CNetworkRequestStandardBuffer : public CNetworkRequestMessageBuffer
	{
public:
	static CNetworkRequestStandardBuffer* NewL();
	~CNetworkRequestStandardBuffer();

	TInt BufferMessage(const TLbsNetInternalMsgBase& aMessage);
	const TLbsNetInternalMsgBase* PeekNextMessage();
	void RemoveMessage(const TLbsNetInternalMsgBase* aMessage);

private:
	CNetworkRequestStandardBuffer();
	CNetworkRequestStandardBuffer(const CNetworkRequestStandardBuffer&);
	
	void ConstructL();

private:
	RPointerArray<TLbsNetInternalMsgBase> iBuffer;
	TLbsNetLocationRequestMsg iBufferedLocationRequestMsg;
	TLbsNetMtLrRequestMsg iBufferedPrivacyRequestMsg;
	TLbsNetSessionCompleteMsg iBufferedSessionCompleteMsg;	
	};

/** Buffer used when LBS in 'standalone privacy' mode.

In this mode, we don't have to be able to work in OOM conditions,
so the buffered messages are allocated off of the heap.

The buffer will hold up to 2 * max num of privacy requests (or until
the heap runs out). It is 2 * max num, because it may need to hold
both the privacy request and session complete message for each request.

In standalone privacy mode we should only get two types of message:
1) Privacy request
2) Session complete

Any other type is a programming error.
*/
class CNetworkRequestPrivacyBuffer : public CNetworkRequestMessageBuffer
	{
public:
	static CNetworkRequestPrivacyBuffer* NewL();
	~CNetworkRequestPrivacyBuffer();

	TInt BufferMessage(const TLbsNetInternalMsgBase& aMessage);
	const TLbsNetInternalMsgBase* PeekNextMessage();
	void RemoveMessage(const TLbsNetInternalMsgBase* aMessage);

private:
	CNetworkRequestPrivacyBuffer();
	CNetworkRequestPrivacyBuffer(const CNetworkRequestPrivacyBuffer&);
	
	void ConstructL();

private:
	RPointerArray<TLbsNetInternalMsgBase> iBuffer;
	};

#endif // NETREQUESTCHANNEL_H_
