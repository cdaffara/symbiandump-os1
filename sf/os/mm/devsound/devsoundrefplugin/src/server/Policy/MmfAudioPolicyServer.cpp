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
#include "MmfAudioPolicyServer.h"
#include "MmfAudioPolicyStart.h"
#include "MmfAudioPolicySession.h"


#if defined(_DEBUG) && defined(__WINS__)
	const TInt KTimeOutInterval =2*1000000;  // 2 seconds
#else
	const TInt KTimeOutInterval =1000000;	// 1 second
#endif

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

void CMMFAudioPolicyServer::SendEventToClient(TInt aSessionToAlert, const TMMFAudioPolicyEvent& aEvent)
	{
// For the session requested, send event to client
	iSessionIter.SetToFirst();
	CMMFAudioPolicySession* session = static_cast<CMMFAudioPolicySession*>(iSessionIter++);
	ASSERT(session != NULL);
	do
		{
		if (session->PolicySessionId() == aSessionToAlert)
			{
			session->SendEventToClient(aEvent);
			return;  // Finished
			}
		session = static_cast<CMMFAudioPolicySession*>(iSessionIter++);
		} while (session != NULL);
	ASSERT(EFalse); // invalid session Id
	}
	
void CMMFAudioPolicyServer::LaunchRequest(TInt aSessionId,const TMMFAudioPolicyEvent& aEvent)
	{
	SendEventToClient(aSessionId, aEvent);
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

void CMMFAudioPolicyServer::StartNotificationTimer(TBool aZeroDelay)
	{
	iNotificationTimer->After(aZeroDelay? 0 : KTimeOutInterval);
	}
	
void CMMFAudioPolicyServer::StopNotificationTimer()
	{
	iNotificationTimer->Cancel();			
	}

TInt CMMFAudioPolicyServer::SendNotification(TAny* aAny)
	{
	CMMFAudioPolicyServer* policyServer = reinterpret_cast<CMMFAudioPolicyServer*>(aAny);
	policyServer->iAudioPolicy->NotifyNextClient();
	return KErrNone;
	}

TBool CMMFAudioPolicyServer::IsTimerActive() const 
	{
	return iNotificationTimer->IsActive();
	}
