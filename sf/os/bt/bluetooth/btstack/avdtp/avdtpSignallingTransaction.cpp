// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Implements the avdtp signalling transaction
// 
//

/**
 @file
 @internalComponent
*/

#include <bluetooth/logger.h>
#include "btsockettimer.h"
#include "avdtpSignallingTransaction.h"
#include "avdtpSignallingChannel.h"
#include "avdtpServiceInterface.h"
#include "avdtputil.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_AVDTP);
#endif

#ifdef _DEBUG
PANICCATEGORY("avdtpsigtr");
#endif

CSignallingTransaction::CSignallingTransaction(CSignallingChannel& aSignallingChannel,
												TAvdtpMessage aSignal)
: iSignal(aSignal), iSigCh(aSignallingChannel),
  iTimerSet(EFalse), iPostSendAction(EDiscard)
	{
	LOG_FUNC
	}
	
/*static*/ CSignallingTransaction* CSignallingTransaction::New(CSignallingChannel& aSignallingChannel,
												TAvdtpMessage aSignal,
												TAvdtpMessageType aMessageType)
	{
	LOG_STATIC_FUNC
	CSignallingTransaction* self = new CSignallingTransaction(aSignallingChannel, aSignal);
	if (self)
		{
		TInt res = self->Construct(aMessageType);
		if (res!=KErrNone)
			{
			delete self;
			self = NULL;
			}
		}
	return self;
	}
	
TInt CSignallingTransaction::Construct(TAvdtpMessageType aMessageType)
	{
	LOG_FUNC
	iMessage = new CAvdtpOutboundSignallingMessage();
	if (iMessage)
		{
		iMessage->SetType(aMessageType, iSignal);
		}
	return iMessage ? KErrNone : KErrNoMemory;
	}

CSignallingTransaction::~CSignallingTransaction()
	{
	LOG_FUNC
	// might be on queue, so dequeue (safe for double queues)
	iLink.Deque();
	CancelTimer();
	delete iMessage;
	}

void CSignallingTransaction::StartTimer()
	{
	LOG_FUNC
	CancelTimer();
	
	TCallBack cb(TimerExpired, this);
	iTimerEntry.Set(cb);
	BTSocketTimer::Queue(KAvdtpSigRTXTimeout, iTimerEntry);
	iTimerSet=ETrue;
	}
	
void CSignallingTransaction::CancelTimer()
	{
	LOG_FUNC
	if (iTimerSet)
		{
		BTSocketTimer::Remove(iTimerEntry);
		iTimerSet = EFalse;
		}
	}

/*static*/ TInt CSignallingTransaction::TimerExpired(TAny* aTransaction)
	{
	LOG_STATIC_FUNC
	CSignallingTransaction* transaction = reinterpret_cast<CSignallingTransaction*>(aTransaction);
	ASSERT_DEBUG(transaction->SentAction()==EKeepSetRTX);
	transaction->Error(KErrAvdtpRequestTimeout);
	return EFalse;
	}

// error the transaction => tell user
// will be due to timeout or signalling channel error etc	
void CSignallingTransaction::Error(TInt aError)
	{
	// remove this transaction from the pending queue
	iLink.Deque();
	
	// get acp seid from cookie - all cookies are the same at the moment
	TSEID seid(reinterpret_cast<TUint>(Cookie()));
	
	switch (Signal())
		{
		case EAvdtpDiscover:
			{
			// no timeout specified in avdtp for Discover
			__ASSERT_DEBUG(aError!=KErrAvdtpRequestTimeout, Panic(EAvdtpInvalidTimeout));
			User()->DiscoverConfirm(aError, NULL);			
			break;			
			}
		case EAvdtpGetCapabilities:
			{
			// no timeout specified in avdtp for GetCaps
			__ASSERT_DEBUG(aError!=KErrAvdtpRequestTimeout, Panic(EAvdtpInvalidTimeout));
			User()->GetCapsConfirm(aError, seid, NULL);			
			break;			
			}
		case EAvdtpSecurityControl:
			{
			// no timeout specified in avdtp for SecurityControl
			__ASSERT_DEBUG(aError!=KErrAvdtpRequestTimeout, Panic(EAvdtpInvalidTimeout));
			User()->SecurityControlConfirm(aError, seid, KNullDesC8);			
			break;			
			}
		case EAvdtpAbort:
			{
			// Abort is special as the spec doesn't allow errors in the Confirm,
			// so signal anyway = assume it worked as semantics of Abort allow this
			User()->AbortConfirm(seid);
			break;
			}
		case EAvdtpSetConfiguration:
			{			
			User()->SetConfigConfirm(aError, seid, EServiceCategoryNull);
			break;
			}
		case EAvdtpOpen:
			{
			User()->OpenConfirm(aError, seid);
			break;
			}
		case EAvdtpReconfigure:
			{
			User()->ReconfigConfirm(aError, seid, EServiceCategoryNull);
			break;
			}
		case EAvdtpSuspend:
			{
			User()->SuspendConfirm(aError, seid);
			break;
			}
		case EAvdtpStart:
			{
			User()->StartConfirm(aError, seid);
			break;
			}
		case EAvdtpRelease:
			{
			User()->ReleaseConfirm(aError, seid);
			break;
			}
		default:
			{
			// note we never send GetConfiguration
			__DEBUGGER();
			}
		}
	// will delete us:
	SignallingChannel().RemoveTransaction(*this);
	}
	
 void CSignallingTransaction::SetSentAction()
	{
	iPostSendAction = Message().PostSendAction();
	}
