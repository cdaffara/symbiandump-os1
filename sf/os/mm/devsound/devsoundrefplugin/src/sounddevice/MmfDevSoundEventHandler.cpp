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

#include "MmfDevSoundEventHandler.h"
#include "mmfAudioPolicyProxy.h"

#include "MmfDevSoundSessionXtnd.h"

CMMFDevSoundEventHandler* CMMFDevSoundEventHandler::NewL(RMMFAudioPolicyProxy* aAudioPolicyProxy)
	{
	CMMFDevSoundEventHandler* self = new(ELeave) CMMFDevSoundEventHandler(aAudioPolicyProxy);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

CMMFDevSoundEventHandler::CMMFDevSoundEventHandler(RMMFAudioPolicyProxy* aAudioPolicyProxy) :
	CActive(EPriorityLow), iAudioPolicyProxy(aAudioPolicyProxy)
	{
	CActiveScheduler::Add(this);
	}

void CMMFDevSoundEventHandler::ConstructL()
	{
	}

void CMMFDevSoundEventHandler::SetDevSoundInfo(CMMFDevSoundSessionXtnd* aDevSound)
	{
	iDevSound = aDevSound;
	iDevSoundId++;
	aDevSound->SetDevSoundId(iDevSoundId);
	}

CMMFDevSoundEventHandler::~CMMFDevSoundEventHandler()
	{
	Cancel();
	}

void CMMFDevSoundEventHandler::ReceiveEvents()
	{
	iAudioPolicyProxy->ReceiveEvents(iAudioPolicyEventPckg, iStatus);
	SetActive();
	}

void CMMFDevSoundEventHandler::CancelReceiveEvents()
	{
	Cancel();
	}

void CMMFDevSoundEventHandler::RunL()
	{
	TMMFAudioPolicyEvent aEvent;
	aEvent.iEventType = iAudioPolicyEventPckg().iEventType;
	aEvent.iErrorCode = iAudioPolicyEventPckg().iErrorCode;
	aEvent.iState = iAudioPolicyEventPckg().iState;

	// Determine whether or not to call on client to send event or call on other
	// SoundDevice's to start play/record functions:
	if ( (aEvent.iEventType == TMMFAudioPolicyEvent::EMMFAudioPolicySwitchToIdle)
		 || (aEvent.iEventType == TMMFAudioPolicyEvent::EMMFAudioPolicyNoEvent) )
		{
		// Determine what to do, based on state:
		switch (aEvent.iState)
			{
		case EMMFStatePlayData:
			iDevSound->StartPlayDataL();
			break;
		case EMMFStateRecordData:
			iDevSound->StartRecordDataL();
			break;
		case EMMFStatePlayTone:
			iDevSound->StartPlayToneL();
			break;
		case EMMFStatePlayDualTone:
			iDevSound->StartPlayDualToneL();
			break;
		case EMMFStatePlayDTMFString:
			iDevSound->StartPlayDTMFStringL();
			break;
		case EMMFStatePlayToneSequence:
			iDevSound->StartPlayToneSequenceL();
			break;
		default:
			ASSERT(EFalse);
			break;
			}  // End switch
		}
	else if(aEvent.iEventType == TMMFAudioPolicyEvent::EMMFAudioPolicyResourceNotification)	
		{
		TMMFEvent event;
		event.iErrorCode = aEvent.iErrorCode;
		event.iEventType = KMMFEventCategoryAudioResourceAvailable;
		iDevSound->SendEvent(event);
		}
	else
		{
		ASSERT(aEvent.iEventType == TMMFAudioPolicyEvent::EMMFAudioPolicyPriorityTooLow);
		//Create a TMMFEvent element for client:
		//NB KErrInUse, KErrDied OR KErrAccessDenied may be returned by the 
		//policy server to indicate that the sound device is in use by another 
		//higher priority client.
		TMMFEvent event;
		event.iErrorCode = aEvent.iErrorCode;
		iDevSound->SendEvent(event);
		}
	// Set self up to receive more events:
	iAudioPolicyProxy->ReceiveEvents(iAudioPolicyEventPckg, iStatus);
	SetActive();
	}

void CMMFDevSoundEventHandler::DoCancel()
	{
	iAudioPolicyProxy->CancelReceiveEvents();
	}

TInt CMMFDevSoundEventHandler::RunError(TInt aError)
	{
	TMMFEvent event;
	event.iErrorCode = aError;//should probably have an appropriate event type here too?
	iDevSound->SendEventToClient(event);
	return KErrNone;
	}
