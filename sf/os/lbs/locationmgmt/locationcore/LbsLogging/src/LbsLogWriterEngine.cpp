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
//

/**
 @file
*/

#include "LbsLogWriterEngine.h"
#include <logfilterandeventconstants.hrh>

	
CLbsLogWriterEngine* CLbsLogWriterEngine::NewL(TUint aMaxBufferSize)
/**
 2 phase constructor for the CLbsLogWriterEngine
 
 @param aMaxBufferSize The maximum number of events allowed on the queues
 
 @return Pointer to new instance of CLbsLogWriterEngine
*/
	{
	CLbsLogWriterEngine* self = new (ELeave) CLbsLogWriterEngine(aMaxBufferSize);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CLbsLogWriterEngine::~CLbsLogWriterEngine()
/**
 C++ destructor
*/
	{
	__TEST_INVARIANT;

	iLogEvents.ResetAndDestroy();
	iLogEvents.Close();
	iLogEventTypes.Close();

	delete iEventBeingLogged;
	iEventBeingLogged = NULL;	
	delete iClient;
	iFs.Close();
	delete iEventTypeBeingLogged;
	}
	
CLbsLogWriterEngine::CLbsLogWriterEngine(TUint aMaxBufferSize)
	: CActive(EPriorityStandard)
/**
 C++ constructor
*/
	{
	iMaxBufferSize = aMaxBufferSize;
	iLastLogIdAvailable = EFalse;
	iEventBeingLogged = NULL;
	iClient = NULL;
	iLoggingType = EFalse;
	CActiveScheduler::Add(this);
	}

void CLbsLogWriterEngine::ConstructL()
/**
 2nd phase constructor, should be run only once if iFs is connected
*/
	{
	TInt err = iFs.Connect();
	User::LeaveIfError(err);
	CleanupClosePushL(iFs);
	
	iClient = CLogClient::NewL(iFs, CActive::EPriorityStandard);
	CleanupStack::Pop(); // for iFs
	iLogEventTypeInit = EFalse;
	iEventTypeBeingLogged = CLogEventType::NewL();
	}

TInt CLbsLogWriterEngine::StartAddingEventType(CLogEventType& aType)
/**
 Starts adding new event types.
 
 @pre There is no other request to log at the moment
 @pre The pointer to the event that is being logged is NULL
*/
	{
	if(IsActive())
		return KErrInUse;
	iClient->AddEventType(aType, iStatus);
    iStatus = KRequestPending;
	SetActive();
	iLoggingType = ETrue;
	return KErrNone;
	}

TInt CLbsLogWriterEngine::AddEventL(CLbsLogEvent& aLogEvent)
/**
 Handles receiving a new event to log. If there is nothing on the queue and 
 the state of the object is not active then it will start logging it, otherwise 
 it will add the event to the queue
 
 @param aLogEvent The new event to be logged
 
 @return KErrNone if everything fine
 @return KErrNotSupported if the LogEng is not installed
 @return KErrOverflow if the queue already contains the maximum number of items
*/
	{
	__TEST_INVARIANT;
	iLastLogEventType = ELbsLogAddEvent;	

	// This will only add an event type if it has not already been added
	InitialiseLogEventTypeL(aLogEvent);
	if(iLogEventTypeInit)
		return KErrNone;

	if(iLogEvents.Count()==0 && !IsActive())
		{
		// the event needs to be copied 
		// even there is no event in the queue in case
		// the logging event requestor exit before
		// logging request is completed.
		iLastLogIdAvailable = ETrue;
		StartLoggingEventL(aLogEvent);
		}
	else
		{
		if(iLogEvents.Count() < iMaxBufferSize)
			{
			AddLogEventToQueueL(aLogEvent);
			}
		else
			{
			return KErrOverflow;
			}
		}
	__TEST_INVARIANT;
	return KErrNone;
	}

TInt CLbsLogWriterEngine::UpdateEventL(CLbsLogEvent& aLogEvent)
/**
 Handles receiving an update for the last event by either updating it in the log engine 
 (if the engine is not doing anything at the moment and there is nothign queued and there 
 is information available about the last event), or by updating the last element in the 
 queue (or adding it as an element if there is nothing in the queue).
 
 @param aLogEvent The information for updating

 @return KErrNone if everything is fine
 @return KErrNotSupported if the LogEng is not installed
 @return KErrNotFound if there is nothing to update
*/
	{
	__TEST_INVARIANT;
	iLastLogEventType = ELbsLogUpdateEvent;	
	if(iLogEvents.Count()==0 && !IsActive())
		{
		// nothing in the queue and state not active
		if(iLastLogIdAvailable)
			{
			StartLoggingEventL(aLogEvent);
			}
		else
			{
			return KErrNotFound;
			}
		}
	else if(iLogEvents.Count()== 0 && IsActive())
		{ // state is active, but nothing on the queue
		if(iLogEvents.Count() < iMaxBufferSize)
			{
			TInt err = KErrNone;
			CLbsLogEvent* logEvent = NULL;
			TRAP(err, logEvent = aLogEvent.CopyL();)
			if(err != KErrNone)
				return err;
			
			CleanupStack::PushL(logEvent);
			TRAP(err, 
				iLogEvents.AppendL(logEvent);
				iLogEventTypes.AppendL(iLastLogEventType);
				)
			if(err != KErrNone)
				return err;
			CleanupStack::Pop(logEvent);
			}
		else
			{
			return KErrOverflow;
			}
		}
	else if(iLogEvents.Count()>0)
		{ // update the last event in the queue
		CLbsLogEvent* eventToUpdate = iLogEvents[iLogEvents.Count()-1];
		eventToUpdate->UpdateL(aLogEvent);
		}
	__TEST_INVARIANT;
	return KErrNone;
	}

void CLbsLogWriterEngine::StartLoggingEventL(CLbsLogEvent& aLogEvent)
/**
 Starts logging a new event.
 
 @param aLogEvent The new event to log.
 
 @pre There is no other request to log at the moment
 @pre The pointer to the event that is being logged is NULL
*/
	{
	__ASSERT_DEBUG(!IsActive(), User::Panic(KLbsLoggerPanic, KErrInUse));

	iLoggingType = EFalse;
	switch(iLastLogEventType)
		{
		case ELbsLogAddEvent:
			{
			if(iEventBeingLogged != NULL)
				{
				delete iEventBeingLogged;
				iEventBeingLogged = NULL;
				}	
			iEventBeingLogged = aLogEvent.ExternalizeL();
			
			iClient->AddEvent(*iEventBeingLogged, iStatus);
			SetActive();
			break;		
			}
		case ELbsLogUpdateEvent:
			{
			// only update the last logged entry
			// Starts updating the last logged event with tht information given.
			aLogEvent.UpdateRootEvent(iEventBeingLogged);
			iClient->ChangeEvent(*iEventBeingLogged, iStatus);
			SetActive();
			break;
			}
		default:
			User::Panic(KLbsLoggerPanic, KErrNotSupported);
			break;
		}
	}

void CLbsLogWriterEngine::AddLogEventToQueueL(CLbsLogEvent& aLogEvent)
	{
	CLbsLogEvent* logEvent = NULL;
	logEvent = aLogEvent.CopyL();
	CleanupStack::PushL(logEvent);
	iLogEvents.AppendL(logEvent);
	iLogEventTypes.AppendL(iLastLogEventType);
	CleanupStack::Pop(logEvent);
	}

void CLbsLogWriterEngine::InitialiseLogEventTypeL(CLbsLogEvent& aLogEvent)
	{
	iLogEventTypeInit = EFalse;
	iEventTypeBeingLogged->SetLoggingEnabled(ETrue);
	// Check if this is the first time we've seen an event
	// If it is - add event to queue, flag event as seen and start adding the event type
	if((aLogEvent.EventType() == TUid::Uid(KLogLbsSelfLocateEventType)) 
			&& !(ELbsSelfLocateEventType & iInitLogEventTypes))
		{
		AddLogEventToQueueL(aLogEvent);
		iInitLogEventTypes |= ELbsSelfLocateEventType;
		iEventTypeBeingLogged->SetDescription(KLbsLogSelfLocateDes);
		iEventTypeBeingLogged->SetUid(TUid::Uid(KLogLbsSelfLocateEventType));
		StartAddingEventType(*iEventTypeBeingLogged);
		iLogEventTypeInit = ETrue;
		}
	else if((aLogEvent.EventType() == TUid::Uid(KLogLbsExternalLocateEventType)) 
			&& !(ELbsExternalLocateEventType & iInitLogEventTypes))
		{
		AddLogEventToQueueL(aLogEvent);
		iInitLogEventTypes |= ELbsExternalLocateEventType;
		iEventTypeBeingLogged->SetDescription(KLbsLogExternalLocateDes);
		iEventTypeBeingLogged->SetUid(TUid::Uid(KLogLbsExternalLocateEventType));
		StartAddingEventType(*iEventTypeBeingLogged);
		iLogEventTypeInit = ETrue;
		}	
	else if((aLogEvent.EventType() == TUid::Uid(KLogLbsTransmitLocationEventType)) 
			&& !(ELbsTransmitLocationEventType & iInitLogEventTypes))
		{
		AddLogEventToQueueL(aLogEvent);
		iInitLogEventTypes |= ELbsTransmitLocationEventType;
		iEventTypeBeingLogged->SetDescription(KLbsLogX3PLocateDes);
		iEventTypeBeingLogged->SetUid(TUid::Uid(KLogLbsTransmitLocationEventType));
		StartAddingEventType(*iEventTypeBeingLogged);
		iLogEventTypeInit = ETrue;
		}	
	else if((aLogEvent.EventType() == TUid::Uid(KLogLbsNetworkLocateEventType)) 
			&& (!ELbsNetworkLocateEventType & iInitLogEventTypes))
		{
		AddLogEventToQueueL(aLogEvent);
		iInitLogEventTypes |= ELbsNetworkLocateEventType;
		iEventTypeBeingLogged->SetDescription(KLbsLogNetworkLocateDes);
		iEventTypeBeingLogged->SetUid(TUid::Uid(KLogLbsNetworkLocateEventType));
		StartAddingEventType(*iEventTypeBeingLogged);
		iLogEventTypeInit = ETrue;
		}	
	else if((aLogEvent.EventType() == TUid::Uid(KLogLbsAssistanceDataEventType)) 
			&& !(ELbsAssistanceDataEventType & iInitLogEventTypes))
		{
		AddLogEventToQueueL(aLogEvent);
		iInitLogEventTypes |= ELbsAssistanceDataEventType;
		iEventTypeBeingLogged->SetDescription(KLbsLogAssistanceDataDes);
		iEventTypeBeingLogged->SetUid(TUid::Uid(KLogLbsAssistanceDataEventType));
		StartAddingEventType(*iEventTypeBeingLogged);
		iLogEventTypeInit = ETrue;
		}
	}




void CLbsLogWriterEngine::RunL()
/**
 The RunL() method gets called when a logging request finished. So this method will 
 store the ID of the logged event and then if there are more events on the queue 
 start logging them and remove them from the queue
*/
	{
	// If we've just added an event type
	if(iLogEventTypeInit)
		{
		if(!(iStatus.Int() == KErrNone || iStatus.Int() == KErrAlreadyExists || iStatus.Int() == KErrCancel)) // if the event type already been supported
			{
			User::Leave(iStatus.Int());
			}
		iLogEventTypeInit = EFalse;
		iLastLogEventType = ELbsLogNoEvent;
		}
	// if we've just added an event
	else
		{
		if(iStatus.Int() == KErrAlreadyExists)
			{
			iLastLogEventType = ELbsLogNoEvent;
			}
		else if(iStatus.Int() != KErrNone)
			{
			User::Leave(iStatus.Int());
			}
		}
	// only record the last logged event Id if it is adding or updating
	if(iLastLogEventType == ELbsLogAddEvent || iLastLogEventType == ELbsLogUpdateEvent)
		{
		iLastLogId = iEventBeingLogged->Id();	
		iLastLogIdAvailable = ETrue;
		}
	else
		{
		iLastLogId = 0;
		iLastLogIdAvailable = EFalse;
		}
		
	if(iLogEvents.Count() == 0)
		iLastLogEventType = ELbsLogNoEvent;
	else //if(iLogEvents.Count() > 0 )
		{
		iLastLogEventType = iLogEventTypes[0];
		StartLoggingEventL(*iLogEvents[0]);
		delete iLogEvents[0];
		iLogEvents.Remove(0);
		iLogEventTypes.Remove(0);
		}
		
	__TEST_INVARIANT;
	}

void CLbsLogWriterEngine::DoCancel()
/**
 Cancels the current request but waits for it to finish
*/
	{
	__TEST_INVARIANT;

	iClient->Cancel();

	__TEST_INVARIANT;
	}

TInt CLbsLogWriterEngine::RunError(TInt aError)
	{ // Just return the error, donot log the error in case mess up the log entries
	__TEST_INVARIANT;
	return aError;
	}
/**
	Method that tests if the invariant of the class is broken.
	
	@panic User::Invariant() if the invariant is broken.
*/

EXPORT_C void CLbsLogWriterEngine::__DbgTestInvariant() const
	{
#ifdef _DEBUG
	if(iLogEvents.Count() > iMaxBufferSize)
		{
		User::Invariant();
		}

#endif //_DEBUG
	}



