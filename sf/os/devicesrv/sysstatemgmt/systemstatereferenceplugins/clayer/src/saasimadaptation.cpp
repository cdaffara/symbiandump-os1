// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "saasimadaptation.h"

#include "startupadaptationadapter.h"
#include "ssmdebug.h"
#include "clayerpanic.h"
/*
 * Creates a new object associated with the passed in CStartupAdaptationAdapter
 * 
 * @internalComponent
 */
CSaaSimAdaptation* CSaaSimAdaptation::NewL(CStartupAdaptationAdapter* aAdapter)
	{
	CSaaSimAdaptation* self = new (ELeave) CSaaSimAdaptation(aAdapter);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

/*
 * Destructor for this object
 * 
 * @internalComponent
 */
CSaaSimAdaptation::~CSaaSimAdaptation()
	{
	iEventQueue.Close();
	}

/*
 * Decrements the reference count for this object, deleting it if necessary
 * 
 * @internalComponent
 */
void CSaaSimAdaptation::Release()
	{
	// This MClass is owned by the singleton CStartupAdaptationAdapter class so
	// release should do nothing.
	}

/*
 * 
 * 
 * @internalComponent
 */
void CSaaSimAdaptation::GetSimOwned(TDes8& aOwnedPckg, TRequestStatus& aStatus)
	{
	// If this adaptation is busy then complete with KErrInUse
	if(Busy())
		{
		TRequestStatus* statusPtr = &aStatus;
		User::RequestComplete(statusPtr, KErrInUse);
		return;
		}
	// Set this request status
	SetRequestStatus(&aStatus);
	aStatus = KRequestPending;
	// No outstand requests so set up command id
	SetCommandId(StartupAdaptation::EGetSimOwned);
	// No parameters to set to pass in
	
	// Remember the package to pass data back into
	iReturnDes = &aOwnedPckg;
	// Pass this to the adapter
	TRAPD(err, iAdapter->QueueDispatchL(this));
	if(err != KErrNone)
		{
		// Failed to queue adaptation, complete with error
		SetRequestStatus(NULL);
		TRequestStatus* statusPtr = &aStatus;
		User::RequestComplete(statusPtr, err);
		}
	}

/*
 * 
 * 
 * @internalComponent
 */
void CSaaSimAdaptation::GetCancel()
	{
	CancelRequest();
	}

/*
 * 
 * 
 * @internalComponent
 */
void CSaaSimAdaptation::NotifySimEvent(TDes8& aTypePckg, TRequestStatus& aStatus)
	{
	if(iEventStatus != NULL)
		{
		DEBUGPRINT1A("SAA - Multiple notify SIM event requests detected, completing with KErrInUse");
		TRequestStatus* statusPtr = &aStatus;
		User::RequestComplete(statusPtr, KErrInUse);
		return;
		}
	if(iEventQueue.Count() != 0)
		{
		// Complete immediately with an event from the queue
		DEBUGPRINT1A("SAA - Completing notify SIM event immediately with queued event");
		// Read value from array
		TPckgBuf<TSsmSimEventType> pckgBuf(iEventQueue[0]);
		// Remove value from array
		iEventQueue.Remove(0);
		// Copy descriptor package across
		aTypePckg.Copy(pckgBuf);
		// Complete status
		TRequestStatus* statusPtr = &aStatus;
		User::RequestComplete(statusPtr, KErrNone);		
		}
	else
		{
		// No events in queue so wait
		aStatus = KRequestPending;
		iEventStatus = &aStatus;
		iEventOutputBuffer = &aTypePckg;
		}
	
	}

/*
 * 
 * 
 * @internalComponent
 */
void CSaaSimAdaptation::NotifyCancel()
	{
	if(iEventStatus != NULL)
		{
		User::RequestComplete(iEventStatus, KErrCancel);
		iEventStatus = NULL;
		}
	// Clear buffer
	iEventOutputBuffer = NULL;
	}
	
/*
 * Constructs a new state adaptation object and associates it with aAdapter
 * 
 * @internalComponent
 */
CSaaSimAdaptation::CSaaSimAdaptation(CStartupAdaptationAdapter* aAdapter)
: CAdaptationBase(aAdapter)
	{

	}

/**
 * Number of events to pre-allocate in the event queue
 * 
 * @internalComponent
 */
const TInt KEventQueueReserveValue = 8;

/*
 * Second phase of construction 
 * 
 * @internalComponent
 */
void CSaaSimAdaptation::ConstructL()
	{
	// Preallocate event queue some memory
	iEventQueue.ReserveL(KEventQueueReserveValue);
	}

/**
 * See CAdaptationBase for description of method.
 *  
 * @internalComponent
 */
void CSaaSimAdaptation::RequestComplete(const StartupAdaptation::TCommand __DEBUG_ONLY(aCommandId), TDesC8& aRetPckg)
	{
	DEBUGPRINT3A("SAA - Response received from adaptation with commandId: %d, expecting %d", aCommandId, CommandId());
	__ASSERT_DEBUG(aCommandId == CommandId(), CLAYER_PANIC(ECLayerUnexpectedCommandResponse));
	// Both Owned and Changed requests have the same return type so no need to switch on CommandId()
	StartupAdaptation::TBooleanResponsePckg booleanResponsePckg;
	booleanResponsePckg.Copy(aRetPckg);
	if(booleanResponsePckg().iErrorCode == KErrNone)
		{
		// Copy the boolean value across to the descriptor passed in
		TPckgBuf<TBool> boolPckg;
		boolPckg() = booleanResponsePckg().iValue;
		iReturnDes->Copy(boolPckg);
		// Null iReturnDes as it has no further use
		iReturnDes = NULL;
		}
	CompleteRequestStatus(booleanResponsePckg().iErrorCode);
	}

/**
 * See CAdaptationBase for description of method.
 *  
 * @internalComponent
 */
TDesC8* CSaaSimAdaptation::ParameterPckg()
	{
	// No methods in this adaptation have any parameters so return an empty descriptor
	return &iNullBuf;
	}

/**
 * Processes the event passed in and distributes to waiting client or queues event.
 * 
 * @internalComponent
 */
void CSaaSimAdaptation::ProcessEventL(TSsmSimEventType aEventType)
	{
	DEBUGPRINT2A("SAA - Sim adaptation processing event with type: %d", aEventType);
	if(iEventStatus == NULL)
		{
		// Need to queue event as nothing is waiting to be notified
		iEventQueue.AppendL(aEventType);
		}
	else
		{
		// Client waiting for event
		__ASSERT_DEBUG(iEventOutputBuffer != NULL, CLAYER_PANIC(ECLayerNullSimEventBuffer));
		TPckgBuf<TSsmSimEventType> pckgBuf(aEventType);
		// Copy event across
		iEventOutputBuffer->Copy(pckgBuf);
		// Complete status
		User::RequestComplete(iEventStatus, KErrNone);
		// Clear event status and buffer
		iEventOutputBuffer = NULL;
		iEventStatus = NULL;
		}
	
	}
