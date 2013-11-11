// Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32math.h>
#include "MmfBtAudioPolicyServer.h"
#include "MmfBtAudioPolicyStart.h"
#include "MmfBtAudioPolicySession.h"

const TInt KTimeOutInterval = 1000000;

CMMFAudioPolicyServer* CMMFAudioPolicyServer::NewL()
	{
	CMMFAudioPolicyServer* s = new(ELeave) CMMFAudioPolicyServer();
	CleanupStack::PushL(s);
	s->ConstructL();
	CleanupStack::Pop();
	return s;
	}

CMMFAudioPolicyServer::CMMFAudioPolicyServer() :
	CMmfIpcServer(EPriorityStandard)
	{
	}

void CMMFAudioPolicyServer::ConstructL()
	{
	// Create AudioPolicy singleton
	iAudioPolicy = CAudioPolicy::NewL(this);
	TCallBack callBack(SendNotification,this);
	iNotificationTimer = CNotificationTimer::NewL(callBack);
	// Call base class to Start server
	StartL(KNullDesC);
	}

CMMFAudioPolicyServer::~CMMFAudioPolicyServer()
	{
	if(iNotificationTimer != NULL)
		{
		StopNotificationTimer();
		}
	delete iAudioPolicy;
	delete iNotificationTimer;
	}

CMmfIpcSession* CMMFAudioPolicyServer::NewSessionL(const TVersion& aVersion) const
	{
	TVersion v(KMMFAudioPolicyVersion,KMMFAudioPolicyMinorVersionNumber,KMMFAudioPolicyBuildVersionNumber);
	if(!User::QueryVersionSupported(v, aVersion))
		User::Leave(KErrNotSupported);
	
	CMMFAudioPolicySession* aAudioPolicySession = CMMFAudioPolicySession::NewL();
	return aAudioPolicySession;
	}

void CMMFAudioPolicyServer::IncrementSessionId()
	{
	iPolicySessionId++;
	}

void CMMFAudioPolicyServer::SendEventToClient(TInt aSessionToAlert, TInt /*aSessionToBeLaunched*/, TMMFAudioPolicyEvent& aEvent)
	{
	TMMFAudioPolicyEvent eventForClient;
	eventForClient.iEventType = aEvent.iEventType;
	eventForClient.iErrorCode = aEvent.iErrorCode;
	eventForClient.iState = aEvent.iState;

	// For the session requested, send event to client
	iSessionIter.SetToFirst();
	CMMFAudioPolicySession* session = STATIC_CAST(CMMFAudioPolicySession*, iSessionIter++);
	while (session != NULL)
		{
		if (session->PolicySessionId() == aSessionToAlert)
			{
			session->SendEventToClient(eventForClient);
			break;  // Finished
			}
		session = STATIC_CAST(CMMFAudioPolicySession*, iSessionIter++);
		}
	}
	
void CMMFAudioPolicyServer::LaunchRequest(TInt aSessionId, TMMFAudioPolicyEvent& aEvent)
	{
	iSessionIter.SetToFirst();
	CMMFAudioPolicySession* session = STATIC_CAST(CMMFAudioPolicySession*, iSessionIter++);
	while (session != NULL)
		{
		if (session->PolicySessionId() == aSessionId)
			{
			session->SendEventToClient(aEvent);
			break;  // Finished
			}
		session = STATIC_CAST(CMMFAudioPolicySession*, iSessionIter++);
		}
	}

void CMMFAudioPolicyServer::IncrementSessionCount()
	{
	iPolicySessionCount++;
	}

CMMFAudioPolicyServer::CNotificationTimer *CMMFAudioPolicyServer::CNotificationTimer::NewL(TCallBack aCallBack)	
	{
	CNotificationTimer* self = new(ELeave) CNotificationTimer(aCallBack);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(); // self
	return self;
	}

CMMFAudioPolicyServer::CNotificationTimer::CNotificationTimer(TCallBack aCallBack)
: CTimer(EPriorityHigh),iCallBack(aCallBack)
	{
	CActiveScheduler::Add(this);
	}

void CMMFAudioPolicyServer::CNotificationTimer::RunL()
	{
	iCallBack.CallBack();
	}

void CMMFAudioPolicyServer::DecrementSessionCount()
	{
	iPolicySessionCount--;
	}

TInt CMMFAudioPolicyServer::PolicySessionCount() 
	{
	return iPolicySessionCount;
	}

void CMMFAudioPolicyServer::StartNotificationTimer()
	{
	iNotificationTimer->After(KTimeOutInterval);
	}
	
void CMMFAudioPolicyServer::StopNotificationTimer()
	{
	iNotificationTimer->Cancel();			
	}

TInt CMMFAudioPolicyServer::SendNotification(TAny* aAny)
	{
	CMMFAudioPolicyServer* policyServer = reinterpret_cast<CMMFAudioPolicyServer*>(aAny);
	policyServer->iAudioPolicy->DoSendNotification();
	return KErrNone;
	}

TBool CMMFAudioPolicyServer::IsTimerActive() const 
	{
	return iNotificationTimer->IsActive();
	}
