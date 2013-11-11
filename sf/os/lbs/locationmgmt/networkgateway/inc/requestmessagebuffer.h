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

#ifndef REQUESTMESSGAEBUFFER_H_
#define REQUESTMESSGAEBUFFER_H_

#include <e32hashtab.h>
#include "lbsnrhngmsgs.h"
#include "lbsnetinternalapi.h"



/** Buffer used by CNetworkRequestChannel and CPsyRequestChannel for non-emergency requests.

LBS processes location and privacy requests and buffers them dynamically
when an outstanding request is being processed by the NRH.

This buffer allocates memory at run-time, making it unsafe in an OOM
situation.
*/
class CRequestMessageBuffer : public CBase
	{
public:
	static CRequestMessageBuffer* NewL();
	~CRequestMessageBuffer();

	TInt BufferMessage(const TLbsNetInternalMsgBase& aMessage);
	const TLbsNetInternalMsgBase* PeekNextMessage();
	void RemoveMessage(const TLbsNetInternalMsgBase* aMessage);

	static TBool IsMsgTypeEqual(
			const TLbsNetInternalMsgBase::TLbsNetInternalMsgType* aType, 
			const TLbsNetInternalMsgBase& aData);

	static TBool IsMsgEqual(
			const TLbsNetInternalMsgBase& aData1, 
			const TLbsNetInternalMsgBase& aData2);

private:
	CRequestMessageBuffer() {}
	CRequestMessageBuffer(const CRequestMessageBuffer&);
	
	void ConstructL();
	
private:
	RPointerArray<TLbsNetInternalMsgBase> iBuffer;
	};

#endif // REQUESTMESSGAEBUFFER_H_
