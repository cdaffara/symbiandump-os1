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

#include "MmfBtAudioPolicy.h"
#include "MmfBtAudioPolicySession.h"	
#include "MmfBtAudioPolicyServer.h"
#include "MdaBtHwInfo.h"	
#include "MmfBtAudioPolicyRequest.h"

CAudioPolicy::~CAudioPolicy()
	{
	delete iMdaHwInfo;
	delete iAudioPolicyRequestArray; 		
	}

void CAudioPolicy::ConstructL()
	{
	// Create dynamic array for sessions list
	iAudioPolicyRequestArray = new(ELeave)CArrayFixFlat<CMMFAudioPolicyRequest>(CAudioPolicy::EGranularity);
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
	iAudioPolicyServer(aAudioPolicyServer)
	{
	}

void CAudioPolicy::MakeRequest(CMMFAudioPolicyRequest* aAudioPolicyRequest)
	{
	//Make sure there is sufficient space in the array to avoid expansion in AppendL() 
	__ASSERT_ALWAYS(iAudioPolicyRequestArray->Count() < iAudioPolicyServer->PolicySessionCount(), Panic(EMMFAudioPolicyRequestArrayOverflow));
	TPolicyResponse responseValue; 
	iSessionToAlert = NULL; // Reset

	// Process Request by looking at priorities, preferences, special states...
	responseValue = ProcessRequest(aAudioPolicyRequest);

		// Finally, check current profile settings (only if not denied thus far):
		//	CheckAgainstProfiles(aAudioPolicyRequest, responseValue);
		if (responseValue == EDenied)
			{
			SetSessionToAlert(aAudioPolicyRequest->PolicySessionId(),
							  NULL, TMMFAudioPolicyEvent::EMMFAudioPolicyPriorityTooLow,
							  aAudioPolicyRequest->State());
			if(IsRegisteredNotification(aAudioPolicyRequest->PolicySessionId()))
				{
				TRAPD(err, iAudioPolicyRequestArray->AppendL(*aAudioPolicyRequest) );
				__ASSERT_ALWAYS(err==KErrNone, Panic(EMMFAudioPolicyRequestArrayOverflow) ); // we reserved space, so shouldn't hit this
				}
			}

	// If a client needs to be notified (either a previous session that was booted off or a new
	// one that was denied) then send event for that session to the client:
	// NB KErrInUse, KErrDied OR KErrAccessDenied may be used to indicate this.
	if (iSessionToAlert != NULL)
		{
			//check the session is registered for notification of resource available
			if(!IsRegisteredNotification(iSessionToAlert))
				{
				RemoveFromList(iSessionToAlert);  				
				}
			iAudioPolicyEvent.iErrorCode = KErrInUse;
			iAudioPolicyEvent.iState = aAudioPolicyRequest->State();
			iAudioPolicyServer->SendEventToClient(iSessionToAlert, iSessionToBeLaunched, iAudioPolicyEvent);
		}
		//Resume of a Audio of the notified client ,since it is already in the list 
		//no need to append and send response back to DevSound for request accepted.
		if(responseValue == EResume)
			{
			iAudioPolicyServer->StopNotificationTimer();
			iAudioPolicyEvent.iErrorCode = KErrNone;
			iAudioPolicyEvent.iEventType = TMMFAudioPolicyEvent::EMMFAudioPolicyNoEvent;
			iAudioPolicyEvent.iState = aAudioPolicyRequest->State();
			iAudioPolicyServer->SendEventToClient(aAudioPolicyRequest->PolicySessionId(), NULL, iAudioPolicyEvent);	
			}
		// Add new policy to list, and send response back to DevSound for request accepted
	if (responseValue == EProceed)
		{
		//no possibility of expansion here as sufficient space for the array is reserved in advance during the creation of the policysession
		TRAPD(err, iAudioPolicyRequestArray->AppendL(*aAudioPolicyRequest) );
		__ASSERT_ALWAYS(err==KErrNone,Panic(EMMFAudioPolicyRequestArrayOverflow) ); // we reserved space, so shouldn't hit this
		iAudioPolicyEvent.iErrorCode = KErrNone;
		iAudioPolicyEvent.iEventType = TMMFAudioPolicyEvent::EMMFAudioPolicyNoEvent;
		iAudioPolicyEvent.iState = aAudioPolicyRequest->State();
		iAudioPolicyServer->SendEventToClient(aAudioPolicyRequest->PolicySessionId(), NULL, iAudioPolicyEvent);
		}
	if (responseValue == EStopThenProceed)  // place on list, but dev Sound will launch request
		{
		//no possibility of expansion here as sufficient space for the array is reserved in advance during the creation of the policysession
		TRAPD(err, iAudioPolicyRequestArray->AppendL(*aAudioPolicyRequest) );
		__ASSERT_ALWAYS(err==KErrNone,Panic(EMMFAudioPolicyRequestArrayOverflow) ); // we reserved space, so shouldn't hit this
		}
	}

TPolicyResponse CAudioPolicy::ProcessRequest(CMMFAudioPolicyRequest* aAudioPolicyRequest)
	{
	TPolicyResponse responseValue(EProceed); 
	iSessionToAlert = NULL; // Reset

	// If there is no other item on list, return with proceed
	if (!iAudioPolicyRequestArray->Count()) 
		return EProceed;
	
	// Handle Preferences, if any, otherwise just compare priorities:
	//	HandlePreferences(aAudioPolicyRequest, preference, responseValue);
	if(IsNotified())
		{
		return EResume;
		}
	responseValue = ComparePriorities(aAudioPolicyRequest);

	return responseValue;
	}

void CAudioPolicy::ModifyEntry(TInt aPolicySessionId, CMMFAudioPolicyRequest* aAudioPolicyRequest)
	{
	TMMFAudioPolicyState requestState = aAudioPolicyRequest->State();
	// If state is stopped or paused, remove item from list
	if ((aAudioPolicyRequest->State() == EMMFStatePaused) || (aAudioPolicyRequest->State() == EMMFStateStopped) || (aAudioPolicyRequest->State() == EMMFStateCompleted))
		{
		if(iSessionToBeLaunched == aPolicySessionId)
			{
			iSessionToBeLaunched = 0;
			}
		if((aAudioPolicyRequest->State() == EMMFStateStopped) && (!iSessionToBeLaunched))
			{	
			TInt sessionToNotify = CheckSessionToNotify();
			if(sessionToNotify)
				{
				iAudioPolicyServer->StartNotificationTimer();
				iAudioPolicyEvent.iErrorCode = KErrNone;
				iAudioPolicyEvent.iEventType = TMMFAudioPolicyEvent::EMMFAudioPolicyResourceNotification;
				iAudioPolicyServer->SendEventToClient(sessionToNotify, NULL, iAudioPolicyEvent);
				}	
			}
			if(aAudioPolicyRequest->NotificationEvent() != KMMFEventCategoryAudioResourceAvailable)
				{
				RemoveFromList(aPolicySessionId);	
				return;
				}
		}
	
	for (TInt index = 0; index < iAudioPolicyRequestArray->Count(); index++)
		{
			// Find correct entry to replace state	
			if ( ((*iAudioPolicyRequestArray)[index].PolicySessionId()) == aPolicySessionId)
				{
					(*iAudioPolicyRequestArray)[index].SetState(requestState);
					break;
				}
		}  
	}

void CAudioPolicy::RemoveFromList(TInt aPolicySessionId)
	{
	for (TInt index = 0; index < iAudioPolicyRequestArray->Count(); index++)
		{
			// Find correct entry to remove	
			if ( (*iAudioPolicyRequestArray)[index].PolicySessionId() == aPolicySessionId)
				{
					iAudioPolicyRequestArray->Delete(index);
					break;
				}
		}  
	}
	
TPolicyResponse CAudioPolicy::ComparePriorities(CMMFAudioPolicyRequest* aAudioPolicyRequest/*, TMMFAudioPolicyEvent& aEvent*/)
	{
	TPolicyResponse responseValue(EProceed);
	TInt requestPriority = aAudioPolicyRequest->Priority();

	TBool requestCaps = aAudioPolicyRequest->Capabilities();

	// Iterate through list and compare priorities:
	for (TInt index = 0; index < iAudioPolicyRequestArray->Count(); index++)
		{
			// If there's even one on the list w/ a higher priority deny request and leave:

			if ((*iAudioPolicyRequestArray)[index].Capabilities() > requestCaps)
				{
				responseValue = EDenied;
				break;
				}
			else if((*iAudioPolicyRequestArray)[index].Capabilities() == requestCaps) 
				{
					if((*iAudioPolicyRequestArray)[index].Priority() >= requestPriority)
					{
						responseValue = EDenied;
						break;
					}
				}


			// Otherwise have the one on the list removed:
				SetSessionToAlert((*iAudioPolicyRequestArray)[index].PolicySessionId(), 
					aAudioPolicyRequest->PolicySessionId(), TMMFAudioPolicyEvent::EMMFAudioPolicyPriorityTooLow, aAudioPolicyRequest->State());
				responseValue = EStopThenProceed;
		}    
	return responseValue;
	}

void CAudioPolicy::HandlePreferences(CMMFAudioPolicyRequest* /*aAudioPolicyRequest*/, TInt /*aPref*/, TPolicyResponse& /*aResponse*/)
	{
	}

void CAudioPolicy::LaunchRequest()
	{
	if (iAudioPolicyEventToLaunch.iEventType == TMMFAudioPolicyEvent::EMMFAudioPolicySwitchToIdle)
		iAudioPolicyServer->LaunchRequest(iSessionToBeLaunched, iAudioPolicyEventToLaunch);
	}

// Sets up session information for sending an event on a denied (or killed) request
void CAudioPolicy::SetSessionToAlert(TInt aSessionToAlert, TInt aSessionToBeLaunched, TMMFAudioPolicyEvent::TAudioPolicyEventType aEventType, TMMFAudioPolicyState aState )
	{
	iSessionToAlert = aSessionToAlert;
	iSessionToBeLaunched = aSessionToBeLaunched;
	iAudioPolicyEvent.iEventType = aEventType;

	if (iSessionToBeLaunched != 0)  // When currently playing item needs to be stopped
		{
		iAudioPolicyEventToLaunch.iEventType = TMMFAudioPolicyEvent::EMMFAudioPolicySwitchToIdle;
		iAudioPolicyEventToLaunch.iState = aState;
		}
	}

CArrayFixFlat<CMMFAudioPolicyRequest>* CAudioPolicy::AudioPolicyRequestArray()
	{
	return iAudioPolicyRequestArray;
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
TBool CAudioPolicy::IsRegisteredNotification(TInt aSessionId)
	{
	TUid event;
 	for (TInt index = 0; index < iAudioPolicyRequestArray->Count(); index++)
 		{
 		if((*iAudioPolicyRequestArray)[index].PolicySessionId() == aSessionId)
 			{
 			event = (*iAudioPolicyRequestArray)[index].NotificationEvent();
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

// checks the state,whether any client is already send notification
TBool CAudioPolicy::IsNotified()
	{
 	for (TInt index = 0; index < iAudioPolicyRequestArray->Count(); index++)
 		{
		if((*iAudioPolicyRequestArray)[index].State() == EMMFStateNotified)
			{
			// In a instance only one client should have the state as EMMFStateNotified
			return ETrue;
			}
 		}
 	return EFalse;
	}

// get the next highest priority of the client to notify 	
TInt CAudioPolicy::CheckSessionToNotify()
	{
	TInt nextHighestPriority= -100;
	TInt sessionToNotify = 0;
	TInt notificationIdx = -1;
	if(IsNotified())
		{
		return sessionToNotify;
		}
	// get the max priority and set the Index 		
	for (TInt index = 0; index < iAudioPolicyRequestArray->Count(); index++)
 		{
 		if((nextHighestPriority <= (*iAudioPolicyRequestArray)[index].Priority()) && ((*iAudioPolicyRequestArray)[index].NotificationEvent() != KNullUid) && (!(*iAudioPolicyRequestArray)[index].IsEventNotified()))
 			{
 			nextHighestPriority = (*iAudioPolicyRequestArray)[index].Priority();
 			sessionToNotify = (*iAudioPolicyRequestArray)[index].PolicySessionId();
 			notificationIdx = index;
 			}
 		}
 	// set the state as notified	
 	if(notificationIdx != -1)
 		{
 		(*iAudioPolicyRequestArray)[notificationIdx].SetEventFlag(ETrue);	
 		(*iAudioPolicyRequestArray)[notificationIdx].SetState(EMMFStateNotified);	
 		}
  	return sessionToNotify;	
	}

// send the message to the server 
void CAudioPolicy::DoSendNotification()
	{
	ResetNotification();
	TInt sessionToNotify = CheckSessionToNotify();
	if(sessionToNotify)
		{
		iAudioPolicyEvent.iErrorCode = KErrNone;
		iAudioPolicyEvent.iEventType = TMMFAudioPolicyEvent::EMMFAudioPolicyResourceNotification;
		iAudioPolicyServer->SendEventToClient(sessionToNotify, NULL, iAudioPolicyEvent);
		}
	}

// Set in the AudiopolicyRequestArray the uid registered
TInt CAudioPolicy::SetNotification(TInt aSessionId, TUid aEventType)
	{
	for (TInt index = 0; index < iAudioPolicyRequestArray->Count(); index++)
 		{
 		if((*iAudioPolicyRequestArray)[index].PolicySessionId() == aSessionId)
 			{
 			(*iAudioPolicyRequestArray)[index].SetNotificationEvent(aEventType);
 			return ETrue;	
 			}
 		}
 	return EFalse;
	}

// cancel the state of the message notification to closed 
void 	CAudioPolicy::ResetNotification()
	{
 	for (TInt index = 0; index < iAudioPolicyRequestArray->Count(); index++)
 		{
		if((*iAudioPolicyRequestArray)[index].State() == EMMFStateNotified)
			{
			(*iAudioPolicyRequestArray)[index].SetState(EMMFStateClosed);
			}
 		}
 	}
 
