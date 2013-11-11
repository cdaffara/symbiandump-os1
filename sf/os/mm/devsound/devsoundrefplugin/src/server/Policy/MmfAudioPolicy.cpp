// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "MmfAudioPolicy.h"
#include "MmfAudioPolicySession.h"	
#include "MmfAudioPolicyServer.h"
#include "MdaHwInfo.h"	
#include "MmfAudioPolicyRequest.h"

/**
*@internalTechnology
*@return if a client owns or wish to own audio resource
*/
inline TBool IsActiveState(TMMFAudioPolicyState aState)
	{
	return (aState < EMMFStateStopped || aState==EMMFStateNotified || aState==EMMFStatePlayDualTone);
	}

CAudioPolicy::~CAudioPolicy()
	{
	delete iMdaHwInfo;
	delete iAudioPolicyRequestArray;
	}

void CAudioPolicy::ConstructL()
	{
	// Create dynamic array for sessions list
	iAudioPolicyRequestArray = new(ELeave) CPolicyReqPtrArray(CAudioPolicy::EGranularity);
	iMdaHwInfo = CMdaHwInfo::NewL();
	}

EXPORT_C CAudioPolicy* CAudioPolicy::NewL(CMMFAudioPolicyServer* aAudioPolicyServer)
	{	

	CAudioPolicy* self = new(ELeave)CAudioPolicy(aAudioPolicyServer);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return(self);
	}

CAudioPolicy::CAudioPolicy(CMMFAudioPolicyServer* aAudioPolicyServer) :
	iAudioPolicyServer(aAudioPolicyServer),
	iNotifiedSessionId(KErrNotFound),
	iSessionIdAwaitingForDevsound(KErrNotFound),
	iStopHandledFromSessId(KErrNotFound)
	{
	}

void CAudioPolicy::MakeRequest(CMMFAudioPolicyRequest* aAudioPolicyRequest)
	{
// since we have a FIFO q, then remove request and re-add it.
	RemoveFromList(aAudioPolicyRequest->PolicySessionId(), EFalse); 
	if (iStopHandledFromSessId==aAudioPolicyRequest->PolicySessionId())
		{
		iStopHandledFromSessId=KErrNotFound;
		}
// Process Request by looking at priorities, preferences, special states...
	TPolicyResponse responseValue = ProcessRequest(aAudioPolicyRequest);
#if defined(ALLOW_POLICY_DEBUG)
	RDebug::Print(_L("Sess ID=%d, Priority=%d"),aAudioPolicyRequest->PolicySessionId(),aAudioPolicyRequest->Priority());
#endif	
	switch (responseValue)
		{
		case EDenied:
			{
			TMMFAudioPolicyEvent responseEvent(TMMFAudioPolicyEvent::EMMFAudioPolicyPriorityTooLow, KErrInUse,EMMFStateWaitingForResource);
			// the client won't be notified until he has request so, so we can set its state anyway
			aAudioPolicyRequest->SetState( EMMFStateWaitingForResource ); 
			iAudioPolicyServer->SendEventToClient(aAudioPolicyRequest->PolicySessionId(), responseEvent);
			}
			break;
		case EProceed:
			{
			iAudioPolicyServer->StopNotificationTimer();
			TMMFAudioPolicyEvent responseEvent(TMMFAudioPolicyEvent::EMMFAudioPolicyNoEvent, KErrNone, aAudioPolicyRequest->State());
			iAudioPolicyServer->SendEventToClient(aAudioPolicyRequest->PolicySessionId(), responseEvent);
			}
			break;
		case EStopThenProceed:
			{
			iAudioPolicyServer->StopNotificationTimer(); 
			iSessionIdAwaitingForDevsound=aAudioPolicyRequest->PolicySessionId();
			// we have to wait for devsound to stop client(s), then notify that one
			}
			break;
		case EResume:
		case EMix:
		default:
			ASSERT(EFalse);
		}
	TRAPD(err, iAudioPolicyRequestArray->AppendL(aAudioPolicyRequest) );
	__ASSERT_ALWAYS(err==KErrNone, Panic(EMMFAudioPolicyRequestArrayOverflow) ); // we reserved space, so shouldn't hit this
	}

TPolicyResponse CAudioPolicy::ProcessRequest(CMMFAudioPolicyRequest* aAudioPolicyRequest)
	{
	// If there is no other item on list, return with proceed
	if (iAudioPolicyRequestArray->Count()==0)
		{
		return EProceed;
		}
		
	TPolicyResponse responseValue(EProceed);
	TInt requestPriority = aAudioPolicyRequest->Priority();
	
	TBool requestCaps = aAudioPolicyRequest->Capabilities();

	// Iterate through list and compare priorities:
	// QUEST: state checking shall be done as well?
	for (TInt index = 0; index < iAudioPolicyRequestArray->Count(); index++)
		{
		CMMFAudioPolicyRequest& currentReq=*(*iAudioPolicyRequestArray)[index];
		if (!IsActiveState(currentReq.State()) ) // this request is inactive
			{
			continue;
			}
			// If there's even one on the list w/ a higher priority deny request and leave:

		if (currentReq.Capabilities() > requestCaps)
			{
			responseValue = EDenied;
			break;
			}
		else if(currentReq.Capabilities() == requestCaps) 
			{
			if(currentReq.Priority() >= requestPriority)
				{
				responseValue = EDenied;
				break;
				}
			}
		if (currentReq.State()==EMMFStateWaitingForResource || currentReq.State()==EMMFStatePreempted)
			{
			continue;
			}
		// we need to stop active client since new request is of higher priority
		TMMFAudioPolicyEvent freezeEvent(TMMFAudioPolicyEvent::EMMFAudioPolicyPriorityTooLow, KErrInUse, EMMFStatePaused);
#if defined(ALLOW_POLICY_DEBUG)	
		RDebug::Print(_L("Sess ID=%d, State=%d Has been preempted"),currentReq.PolicySessionId(),currentReq.State());
#endif			
		currentReq.SetState( EMMFStatePreempted );
		currentReq.SetEventFlag(EFalse);
		iAudioPolicyServer->SendEventToClient(currentReq.PolicySessionId(), freezeEvent);
		responseValue = EStopThenProceed;
		}

	return responseValue;
	}

void CAudioPolicy::ModifyEntry(TInt aPolicySessionId,const TMMFAudioPolicyState aNewState)
	{
	CMMFAudioPolicyRequest* sessionEntry=FindPolicyRequestById(aPolicySessionId);
#if defined(ALLOW_POLICY_DEBUG)	
	RDebug::Print(_L("Sess ID=%d, Old State=%d New State=%d"),aPolicySessionId,sessionEntry?sessionEntry->State():-1, aNewState);
#endif	
	// some client took over resource, so update its state and cancel timer
	if (IsActiveState(aNewState))
		{
		if (sessionEntry)
			{
			sessionEntry->SetState(aNewState);	
			sessionEntry->SetEventFlag(EFalse);
			}
		iAudioPolicyServer->StopNotificationTimer();
		iNotifiedSessionId	=KErrNotFound;
		ASSERT(iSessionIdAwaitingForDevsound==KErrNotFound); // we shouldn't have a client waiting to be notified
		if (iStopHandledFromSessId==aPolicySessionId)
			{
			iStopHandledFromSessId=KErrNotFound;
			}
		return;
		}
	if (iNotifiedSessionId==aPolicySessionId) // if client that was notified, then stop timer.
		{
		iAudioPolicyServer->StopNotificationTimer();
		}
	if (sessionEntry==NULL) // to cope with erroneous behaviour of devsound
		{
		return;
		}
	// we have update from the client, if we have other clients waiting we should notify them
	if ( (aNewState == EMMFStatePaused || (aNewState == EMMFStateStopped && iStopHandledFromSessId!=aPolicySessionId) )
			&& sessionEntry->State()!=EMMFStateStopped && sessionEntry->State()!=EMMFStateWaitingForResource)
		{
		if (aNewState == EMMFStateStopped) // to eliminate duplicate stop events
			{
			iStopHandledFromSessId=aPolicySessionId;
			}
		if (sessionEntry->State()==EMMFStatePreempted)
			{
			sessionEntry->SetState(EMMFStateWaitingForResource);
			}
			
		if (iSessionIdAwaitingForDevsound==aPolicySessionId)
			{
			iSessionIdAwaitingForDevsound=KErrNotFound;
			}
			
		if (aNewState == EMMFStatePaused || aNewState == EMMFStateStopped) // devsound should free, so notify waiting client
			{
			if (iSessionIdAwaitingForDevsound!=KErrNotFound) // we have a client waiting for Devsound, so notify it
				{
				NotifySessionWithTimeout(iSessionIdAwaitingForDevsound, TMMFAudioPolicyEvent::EMMFAudioPolicyNoEvent);
				iSessionIdAwaitingForDevsound=KErrNotFound;
				}
			else if (!iAudioPolicyServer->IsTimerActive()) // do not try to notify if we're still waiting for response
				{
				const TInt sessionIdToNotify = CheckSessionToNotify();
				if(sessionIdToNotify != KErrNotFound)
					{
					// set the state as notified
					NotifySessionWithTimeout(sessionIdToNotify, TMMFAudioPolicyEvent::EMMFAudioPolicyResourceNotification);
					}				
				}
			}
		}
	// we update state to passive only if the client hasn't been stopped by us, so as not loose its waiting indication
	if (sessionEntry->State()!=EMMFStateWaitingForResource)
		{
		sessionEntry->SetState(aNewState);
		}
	}
	
void CAudioPolicy::NotifySessionWithTimeout(TInt aPolicySessionId, TMMFAudioPolicyEvent::TAudioPolicyEventType aEvent)
	{
	CMMFAudioPolicyRequest* sessionEntry=FindPolicyRequestById(aPolicySessionId);
	ASSERT(sessionEntry);
#if defined(ALLOW_POLICY_DEBUG)	
	RDebug::Print(_L("Sending timed not. ID=%d, State=%d Event=%d"),aPolicySessionId,sessionEntry->State(), aEvent);
#endif
	const TMMFAudioPolicyEvent eventToSend(aEvent, KErrNone, sessionEntry->State());
	sessionEntry->SetEventFlag(ETrue);
	iAudioPolicyServer->StartNotificationTimer();
	iNotifiedSessionId 	= aPolicySessionId;
	iAudioPolicyServer->SendEventToClient(aPolicySessionId, eventToSend);
	}

void CAudioPolicy::RemoveFromList(TInt aPolicySessionId, TBool aAllowTimerRestart)
	{
	if (aPolicySessionId==KErrNotFound)
		{
		return;
		}
	for (TInt index = iAudioPolicyRequestArray->Count(); index-- ;)
		{
		// Find correct entry to remove	
		if ( (*iAudioPolicyRequestArray)[index]->PolicySessionId() == aPolicySessionId)
			{
			if (iSessionIdAwaitingForDevsound==aPolicySessionId)
				{
				iSessionIdAwaitingForDevsound=KErrNotFound;
				}			
			if (iNotifiedSessionId==aPolicySessionId && iAudioPolicyServer->IsTimerActive()) 
				{
				iNotifiedSessionId=KErrNotFound;
				// the session we were waiting for disconnected so try to immediately notify another one
				iAudioPolicyServer->StopNotificationTimer();
				if (iAudioPolicyRequestArray->Count() > 1 && aAllowTimerRestart)
					{
					iAudioPolicyServer->StartNotificationTimer(ETrue);
					}
				}			
			else if(!iAudioPolicyServer->IsTimerActive())
				{
				if (iAudioPolicyRequestArray->Count() > 1 && aAllowTimerRestart)
					{
					iAudioPolicyServer->StartNotificationTimer(ETrue);
					}
				}
			iAudioPolicyRequestArray->Delete(index);
			return;
			}
		}
	}
	
void CAudioPolicy::HandlePreferences(CMMFAudioPolicyRequest* /*aAudioPolicyRequest*/, TInt /*aPref*/, TPolicyResponse& /*aResponse*/)
	{
	}

// this is weird, but devsound 
// does Stop() if a client is denied access to resource
// then calls this routine to indicate that resource became available
void CAudioPolicy::LaunchRequest(TInt aSessionId)
	{
	ASSERT(iSessionIdAwaitingForDevsound!=aSessionId);
	ModifyEntry(aSessionId, EMMFStateStopped);
	}

void CAudioPolicy::ReserveClientNumL(TInt aNum)
	{
	iAudioPolicyRequestArray->SetReserveL(aNum);
	}

/**
@internalTechnology

This function raises a panic

@param	aError
		one of the several panics codes that may be raised by this dll

@panic	EMMFAudioPolicyRequestArrayOverflow is raised when policyrequest array is full
*/
GLDEF_C void Panic(TMMFAudioPolicyPanicCodes aPanicCode)
	{
	User::Panic(KMMFAudioPolicyPanicCategory, aPanicCode);
	}

// checks based on the session ,Is the session is registered for Notification 
TBool CAudioPolicy::IsRegisteredNotification(TInt aSessionId) const
	{
	TUid event;
 	for (TInt index = 0; index < iAudioPolicyRequestArray->Count(); index++)
 		{
 		if((*iAudioPolicyRequestArray)[index]->PolicySessionId() == aSessionId)
 			{
 			event = (*iAudioPolicyRequestArray)[index]->NotificationEvent();
 			if (event  == KMMFEventCategoryAudioResourceAvailable)
 				{
 				// only when the client is registered for KMMFEventCategoryAudioResourceAvailable event
 				return ETrue; 
 				}
 			break;	
 			} 
 		} 
 	return EFalse;
	}

// get the next highest priority of the client to notify 	
TInt CAudioPolicy::CheckSessionToNotify()
	{
	TInt nextHighestPriority= -100;
	TInt sessionToNotify = KErrNotFound;

	// get the max priority and set the Index
	for (TInt attempt=2; attempt-- && sessionToNotify==KErrNotFound;)
		{
		
		for (TInt index = 0; index < iAudioPolicyRequestArray->Count(); ++index)
	 		{
	 		CMMFAudioPolicyRequest& currentReq=*(*iAudioPolicyRequestArray)[index];
	 		if((nextHighestPriority <= currentReq.Priority())
	 					&& (currentReq.NotificationEvent() == KMMFEventCategoryAudioResourceAvailable) 
	 					&& (!currentReq.IsEventNotified())
	 					&& currentReq.State()==EMMFStateWaitingForResource)
	 			{
	 			nextHighestPriority = currentReq.Priority();
	 			sessionToNotify = currentReq.PolicySessionId();
	 			}
	 		}
	 	// we tried to notify every session once, so reset flag and try again.
	 	if (sessionToNotify==KErrNotFound)
	 		{
	 		for (TInt i=iAudioPolicyRequestArray->Count(); i--;)
	 			{
	 			(*iAudioPolicyRequestArray)[i]->SetEventFlag(EFalse);
	 			}
	 		}
 		}
  	return sessionToNotify;	
	}

// send the message to the server 
void CAudioPolicy::NotifyNextClient()
	{
	const TInt sessionIdToNotify = CheckSessionToNotify();
#if defined(ALLOW_POLICY_DEBUG)	
	RDebug::Print(_L("Sess ID %d didn't continue within timeout, Next ID=%d"), iNotifiedSessionId, sessionIdToNotify);
#endif	
	iNotifiedSessionId = KErrNotFound;
	if(sessionIdToNotify != KErrNotFound)
		{
		NotifySessionWithTimeout(sessionIdToNotify, TMMFAudioPolicyEvent::EMMFAudioPolicyResourceNotification);
		}	
	}

// Set in the AudiopolicyRequestArray the uid registered
TInt CAudioPolicy::SetNotification(TInt aSessionId, TUid aEventType)
	{
	if (KMMFEventCategoryAudioResourceAvailable!=aEventType)
		{
		return EFalse;
		}
	for (TInt index = iAudioPolicyRequestArray->Count(); index-- ; )
 		{
 		CMMFAudioPolicyRequest& currentReq=*(*iAudioPolicyRequestArray)[index];
 		if(currentReq.PolicySessionId() == aSessionId)
 			{
 			currentReq.SetNotificationEvent(aEventType);
#if defined(ALLOW_POLICY_DEBUG)
			RDebug::Print(_L("Sess ID %d state=%d requested resource notification"), aSessionId, currentReq.State());
#endif			 			 			
 			if (!IsActiveState(currentReq.State()))
 				{
 				currentReq.SetState(EMMFStateWaitingForResource);
 				}
 			return ETrue;	
 			}
 		}
 	return EFalse;
	}

CMMFAudioPolicyRequest* CAudioPolicy::FindPolicyRequestById(TInt aSessionId) const
	{
	for (TInt index = iAudioPolicyRequestArray->Count(); index-- ; )
 		{
 		if((*iAudioPolicyRequestArray)[index]->PolicySessionId() == aSessionId)
 			{
 			return (*iAudioPolicyRequestArray)[index];
 			}
 		}
	return NULL;
	}



