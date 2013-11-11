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

#include "saaemergencycallrfadaptation.h"

#include "startupadaptationadapter.h"
#include "ssmdebug.h"
#include "clayerpanic.h"
/*
 * Creates a new object associated with the passed in CStartupAdaptationAdapter
 * 
 * @internalComponent
 */
CSaaEmergencyCallRfAdaptation* CSaaEmergencyCallRfAdaptation::NewL(CStartupAdaptationAdapter* aAdapter)
	{
	CSaaEmergencyCallRfAdaptation* self = new (ELeave) CSaaEmergencyCallRfAdaptation(aAdapter);
	return self;
	}

/*
 * Destructor for this object
 * 
 * @internalComponent
 */
CSaaEmergencyCallRfAdaptation::~CSaaEmergencyCallRfAdaptation()
	{
	
	}
	
/*
 * Decrements the reference count for this object, deleting it if necessary
 * 
 * @internalComponent
 */
void CSaaEmergencyCallRfAdaptation::Release()
	{
	// This MClass is owned by the singleton CStartupAdaptationAdapter class so
	// release should do nothing.
	}

/*
 * 
 * 
 * @internalComponent
 */
void CSaaEmergencyCallRfAdaptation::ActivateRfForEmergencyCall(TRequestStatus& aStatus)
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
	SetCommandId(StartupAdaptation::EActivateRfForEmergencyCall);
	// No parameters to set to pass in
	
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
void CSaaEmergencyCallRfAdaptation::DeactivateRfForEmergencyCall(TRequestStatus& aStatus)
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
	SetCommandId(StartupAdaptation::EDeactivateRfAfterEmergencyCall);
	// No parameters to set to pass in
	
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
void CSaaEmergencyCallRfAdaptation::Cancel()
	{
	CancelRequest();
	}

	
/*
 * Constructs a new state adaptation object and associates it with aAdapter
 * 
 * @internalComponent
 */
CSaaEmergencyCallRfAdaptation::CSaaEmergencyCallRfAdaptation(CStartupAdaptationAdapter* aAdapter)
: CAdaptationBase(aAdapter)
	{
	
	}

/**
 * See CAdaptationBase for description of method.
 *  
 * @internalComponent
 */
void CSaaEmergencyCallRfAdaptation::RequestComplete(const StartupAdaptation::TCommand __DEBUG_ONLY(aCommandId), TDesC8& aRetPckg)
	{
	DEBUGPRINT3A("SAA - Response received from adaptation with commandId: %d, expecting %d", aCommandId, CommandId());
	__ASSERT_DEBUG(aCommandId == CommandId(), CLAYER_PANIC(ECLayerUnexpectedCommandResponse));
	switch(CommandId())
		{
		case StartupAdaptation::EActivateRfForEmergencyCall: // Fall-through
		case StartupAdaptation::EDeactivateRfAfterEmergencyCall:
			{
			StartupAdaptation::TResponsePckg responsePckg;
			responsePckg.Copy(aRetPckg);
			CompleteRequestStatus(responsePckg());
			break;
			}
		default:
			CLAYER_PANIC(ECLayerUnknownCommandResponse);
			break;
		}
	}

/**
 * See CAdaptationBase for description of method.
 *  
 * @internalComponent
 */
TDesC8* CSaaEmergencyCallRfAdaptation::ParameterPckg()
	{
	// No methods in this adaptation have any parameters so return an empty descriptor
	return &iNullBuf; //lint !e1536 Suppress exposing low access member
	}
