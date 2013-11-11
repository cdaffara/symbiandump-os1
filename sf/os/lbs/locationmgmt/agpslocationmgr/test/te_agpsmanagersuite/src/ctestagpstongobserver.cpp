// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// GPS Integration Module Observer for AGPS Manager UNIT testing
// 
//

#include "ctestagpstongobserver.h"
#include "lbsagpsngmsgs.h"
#include "lbsnrhngmsgs.h"


CTestAgpsToNgObserver* CTestAgpsToNgObserver::NewL()
	{
	CTestAgpsToNgObserver* self = new(ELeave) CTestAgpsToNgObserver();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

void CTestAgpsToNgObserver::ConstructL()
	{
	CTestObserverBase::ConstructL();
	iSendWaiter = CTestSendWaiter::NewL(*this);
	}

CTestAgpsToNgObserver::~CTestAgpsToNgObserver()
	{
	delete iSendWaiter;
	}

CTestAgpsToNgObserver::CTestAgpsToNgObserver()
	{
		
	}

void CTestAgpsToNgObserver::SendSessionCompleteToAgpsManager(TInt aReason)
	{
	TLbsNetSessionCompleteAgpsMsg sessionCompleteMessage(iSessionId, aReason);
	iSendWaiter->SendMessageToAgpsManager(sessionCompleteMessage);
	}

void CTestAgpsToNgObserver::ProcessNetChannelMessage(RLbsNetChannel::TLbsNetChannelId aChannelId, const TLbsNetInternalMsgBase& aMessage)
	{
	ASSERT(aChannelId == RLbsNetChannel::EChannelNG2AGPS);
	
	// to avoid urel build warning:
	aChannelId = RLbsNetChannel::EChannelNG2AGPS;
	
	// No need to store the message, just look at the type and
	// set the corresponding flag
	switch(aMessage.Type())
		{
		case TLbsNetInternalMsgBase::ESelfLocationRequest :
			{
			iObservedNgEvents |= ESelfLocRequestCall;
			// Remember the session ID
			const TLbsNetSelfLocationRequestMsg& msgLocReq = static_cast<const TLbsNetSelfLocationRequestMsg&>(aMessage);
			iSessionId = msgLocReq.SessionId();
			}
		break;
		
		case TLbsNetInternalMsgBase::ESelfLocationCancel :
			iObservedNgEvents |= ESelfLocCancelCall;
		break;
		
		case TLbsNetInternalMsgBase::EAssistanceDataRequest :
			iObservedNgEvents |= EAssistDataReqCall;
		break;
		
		case TLbsNetInternalMsgBase::ESystemStatusAdvice :
			iObservedNgEvents |= ESystemStatusAdviceCall;
		break;
		
		default:
			ASSERT(EFalse);
		break;
		}

	SignalObserverCallBack(ECallBackErrorNone);
	}

TBool CTestAgpsToNgObserver::WasSelfLocationRequested()
	{
	TBool ret = EFalse;
	if (CheckCallbackObserved(ESelfLocRequestCall))
		{
		ret = ETrue;
		}
	return ret;	
	}
	
TBool CTestAgpsToNgObserver::WasSelfLocationCancelled()
	{
	TBool ret = EFalse;
	if (CheckCallbackObserved(ESelfLocCancelCall))
		{
		ret = ETrue;
		}
	return ret;		
	}

TBool CTestAgpsToNgObserver::WasAssistanceDataRequested()
	{
	TBool ret = EFalse;
	if (CheckCallbackObserved(EAssistDataReqCall))
		{
		ret = ETrue;
		}
	return ret;					
	}
	
TBool CTestAgpsToNgObserver::WasSystemStatusAdvised()
	{
	TBool ret = EFalse;
	if (CheckCallbackObserved(ESystemStatusAdviceCall))
		{
		ret = ETrue;
		}
	return ret;				
	}

TBool CTestAgpsToNgObserver::CheckCallbackObserved(const TObservedNetworkGatewayCalls aCall)
	{
	TBool ret = EFalse;
	if (iObservedNgEvents & aCall)
		{	
		// Clear this call's flag from the list of received calls
	 	iObservedNgEvents = iObservedNgEvents & ~aCall;
	 	CallbackHandled();
	 	ret = ETrue;
		}
	return ret;	
	}


CTestSendWaiter::CTestSendWaiter() : CActive(EPriorityStandard)
	{
	}

// aChannel observer will receive the messages from the AGPS Manager
CTestSendWaiter* CTestSendWaiter::NewL(MLbsNetChannelObserver& aChannelObserver)
	{
	CTestSendWaiter* self = new(ELeave) CTestSendWaiter();
	CleanupStack::PushL(self);
	self->ConstructL(aChannelObserver);
	CleanupStack::Pop(self);
	return self;
	}

void CTestSendWaiter::ConstructL(MLbsNetChannelObserver& aChannelObserver)
	{
	iNg2AgpsManager.OpenL(RLbsNetChannel::EChannelNG2AGPS, aChannelObserver);	
	}
	
CTestSendWaiter::~CTestSendWaiter()
	{
	iNg2AgpsManager.Close();
	}

void CTestSendWaiter::SendMessageToAgpsManager(const TLbsNetInternalMsgBase& aMsg)
	{
	//Check that no other message is being sent
	if (IsActive())
		{
		// A teststep is not allowed to send a message on this channel
		// until the AGPS Manager has read the previous one. If
		// that functionality is required, consider adding to this
		// class a queue using RLbsNetMsgFifo.
		ASSERT(EFalse);
		}
	else
		{
		iNg2AgpsManager.SendMessage(aMsg, iStatus);
		}		
	}

void CTestSendWaiter::RunL()
	{
	// Called when the send operation has completed.
	// Just check that there have been
	// no errors sending a message from
	// the simulated NG to the AGPS Manager
	ASSERT(KErrNone == iStatus.Int());
	}

TInt CTestSendWaiter::RunError(TInt aError)
	{
	// Just panic
	ASSERT(EFalse);
	return aError;
	}

void CTestSendWaiter::DoCancel()
	{
	//Nothing to do
	}
