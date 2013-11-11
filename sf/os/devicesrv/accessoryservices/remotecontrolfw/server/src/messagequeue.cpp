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
 @internalComponent
*/

#include <bluetooth/logger.h>
#include "messagequeue.h"
#include "utils.h"
#include "remconmessage.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_REMCON_SERVER);
#endif

#ifdef _DEBUG
PANICCATEGORY("msgqueue");
#endif

CMessageQueue* CMessageQueue::NewL()
	{
	LOG_STATIC_FUNC
	return new(ELeave) CMessageQueue();
	}

CMessageQueue::CMessageQueue()
:	iQueue(_FOFF(CRemConMessage, iLink)),
	iIter(iQueue)
	{
	LOG_FUNC
	}

CMessageQueue::~CMessageQueue()
	{
	LOG_FUNC;
	LogQueue();

	iIter.SetToFirst(); 
	CRemConMessage* msg;
	while ( ( msg = iIter++ ) != NULL )
		{
		iQueue.Remove(*msg);
		--iCount;
		delete msg;
		}
	
	LogQueue();
	}

TSglQueIter<CRemConMessage>& CMessageQueue::SetToFirst()
	{
	LOG_FUNC;

	iIter.SetToFirst();

	return iIter;
	}

void CMessageQueue::Append(CRemConMessage& aMsg)
	{
	LOG_FUNC;
	LogQueue();

	iQueue.AddLast(aMsg);
	++iCount;

	LogQueue();
	}

void CMessageQueue::RemoveAndDestroy(CRemConMessage& aMsg)
	{
	LOG_FUNC;
	LOG1(_L("\t&aMsg = [0x%08x]"), &aMsg);

	Remove(aMsg);
	delete &aMsg;
	}

void CMessageQueue::RemoveAndDestroy(TUint aSessionId)
	{
	LOG_FUNC;
	LOG1(_L("\taSessionId = %d"), aSessionId);

	TSglQueIter<CRemConMessage> iter(iQueue);
	CRemConMessage* msg;
	while ( ( msg = iter++ ) != NULL )
		{
		if ( msg->SessionId() == aSessionId )
			{
			RemoveAndDestroy(*msg);
			}
		}
	}

void CMessageQueue::Remove(CRemConMessage& aMsg)
	{
	LOG_FUNC;
	LOG1(_L("\t&aMsg = [0x%08x]"), &aMsg);
	LogQueue();

	iQueue.Remove(aMsg);
	--iCount;

	LogQueue();
	}

CRemConMessage& CMessageQueue::First()
	{
	LOG_FUNC;

	CRemConMessage* msg = iQueue.First();
	ASSERT_DEBUG(msg);
	return *msg;
	}

CRemConMessage* CMessageQueue::Message(TUint aTransactionId)
	{
	LOG_FUNC;
	LOG1(_L("\taTransactionId = %d"), aTransactionId);

	TSglQueIter<CRemConMessage> iter(iQueue);
	CRemConMessage* msg;
	CRemConMessage* ret = NULL;
	
	while ( ( msg = iter++ ) != NULL )
		{
		if ( msg->TransactionId() == aTransactionId )
			{
			ret = msg;
			break;
			}
		}
	return ret;
	}

void CMessageQueue::LogQueue() const
	{
#ifdef __FLOG_ACTIVE
	
	LOG2(_L("\tthis = 0x%08x, iCount = %d"), this, iCount);

	// Make our own local iterator so we don't interfere with one the client 
	// may be relying on.
	TSglQueIter<CRemConMessage> iter(const_cast<CMessageQueue*>(this)->iQueue);
	CRemConMessage* msg;
	while ( ( msg = iter++ ) != NULL )
		{
		LOG4(_L("\t\tmsg [0x%08x], Addr.BearerUid = 0x%08x, MsgType = %d, InterfaceUid = 0x%08x,"),
			msg,
			msg->Addr().BearerUid(),
			msg->MsgType(),
			msg->InterfaceUid()
			);
		LOG3(_L("\t\tOperationId = 0x%02x, SessionId = %d, TransactionId = %d"),
			msg->OperationId(),
			msg->SessionId(),
			msg->TransactionId()
			);
		// This logging code left in for maintenance.
		//LOG1(_L("\t\tOperationData = \"%S\""), &msg->OperationData());
		}

#endif // __FLOG_ACTIVE
	}
