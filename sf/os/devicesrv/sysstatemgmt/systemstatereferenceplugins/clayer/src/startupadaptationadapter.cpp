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

#include "startupadaptationadapter.h"

#include <ssm/startupadaptation.h>

#include <ecom/ecom.h>

#include "saastateadaptation.h"
#include "saasimadaptation.h"
#include "saartcadaptation.h"
#include "saaemergencycallrfadaptation.h"
#include "saamiscadaptation.h"

#include "clayerpanic.h"

#include "ssmdebug.h"



/**
 * Returns the singleton @c CStartupAdaptationAdapter.
 * 
 * @return The singleton CStartupAdaptationAdapter
 * @leave KErrPermissionDenied - the caller does not have permission to perform this operation
 * 
 * @internalComponent
 */
CStartupAdaptationAdapter* CStartupAdaptationAdapter::GetL()
	{
	DEBUGVERBOSE1A("Checking TLS for CStartupAdaptationAdapter");
	CStartupAdaptationAdapter* self =
		static_cast<CStartupAdaptationAdapter*>(Dll::Tls());
	if(self == 0)
		{
		// TLS uninitialised, need to create
		DEBUGPRINT1A("Constructing CStartupAdaptationAdapter singleton");
		self = CStartupAdaptationAdapter::NewLC();
		DEBUGPRINT1A("Storing CStartupAdaptationAdapter singleton in TLS");
		TInt err = Dll::SetTls(self);
		SSMLOGLEAVEIFERROR(err);
		CleanupStack::Pop(self);
		}
	DEBUGVERBOSE1A("Returning CStartupAdaptationAdapter singleton");
	return self;
	}

/**
 * Destroys the singleton @c CStartupAdaptationAdapter.
 * 
 * @internalComponent
 */
void CStartupAdaptationAdapter::Destroy()
	{
	CStartupAdaptationAdapter* self =
			static_cast<CStartupAdaptationAdapter*>(Dll::Tls());
	if(self != 0)
		{
		DEBUGPRINT1A("Destroying CStartupAdaptationAdapter singleton");
		delete self;
		TInt err = Dll::SetTls(0);
		if(err != KErrNone)
			{
			DEBUGPRINT2A("Error clearing TLS: %d", err);
			return;
			}
		}
	}

/**
 * Creates a new CStartupAdaptationAdapter.
 * 
 * The CStartupAdaptationAdapter class should be accessed via a singleton pattern.
 * GetL() is provided to manage a thread level singleton for this class.
 * 
 * @internalComponent
 */
CStartupAdaptationAdapter* CStartupAdaptationAdapter::NewLC()
	{
	CStartupAdaptationAdapter* self = new (ELeave) CStartupAdaptationAdapter();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

/**
 * First phase constructor
 * 
 * @internalComponent
 */

CStartupAdaptationAdapter::CStartupAdaptationAdapter()
: CActive(EPriorityStandard)
	{
	
	}

/**
 * Second phase constructor
 * 
 * @internalComponent
 */
void CStartupAdaptationAdapter::ConstructL()
	{ 
	iStateAdaptation = CSaaStateAdaptation::NewL(this);
	iSimAdaptation = CSaaSimAdaptation::NewL(this);
	iRtcAdaptation = CSaaRtcAdaptation::NewL(this);
	iEmergencyCallRfAdaptation = CSaaEmergencyCallRfAdaptation::NewL(this);
	iMiscAdaptation = CSaaMiscAdaptation::NewL(this);
	CActiveScheduler::Add(this);
	}

/**
 * Destructor for CStartupAdaptationAdapters
 * 
 * @internalComponent
 */
CStartupAdaptationAdapter::~CStartupAdaptationAdapter()
	{
	Cancel();
	
	// destroy the adaptation, which is an ECOM plugin
	delete iStartupAdaptation;
	
	// Signal to ECOM that it should cleanup
	REComSession::FinalClose();
	
	// Cleanup other members
	delete iStateAdaptation;
	delete iSimAdaptation;
	delete iRtcAdaptation;
	delete iEmergencyCallRfAdaptation;
	delete iMiscAdaptation;
	}

/**
 * Returns an object conforming to the MStateAdaptation interface.
 * 
 * The objects returned from this method may be reference counted.
 * This may mean that consecutive calls to this method will return the same
 * address.
 * 
 * @internalComponent
 */
EXPORT_C MStateAdaptation* CStartupAdaptationAdapter::NewStateAdaptationL()
	{
	CStartupAdaptationAdapter* adapter = GetL();
	return static_cast<MStateAdaptation*>(adapter->iStateAdaptation);
	}

/**
 * Returns an object conforming to the MSimAdaptation interface.
 * 
 * The objects returned from this method may be reference counted.
 * This may mean that consecutive calls to this method will return the same
 * address.
 * 
 * @internalComponent
 */
EXPORT_C MSimAdaptation* CStartupAdaptationAdapter::NewSimAdaptationL()
	{
	CStartupAdaptationAdapter* adapter = GetL();
	return static_cast<MSimAdaptation*>(adapter->iSimAdaptation);
	}

/**
 * Returns an object conforming to the MRtcAdaptation interface.
 * 
 * The objects returned from this method may be reference counted.
 * This may mean that consecutive calls to this method will return the same
 * address.
 * 
 * @internalComponent
 */
EXPORT_C MRtcAdaptation* CStartupAdaptationAdapter::NewRtcAdaptationL()
	{
	CStartupAdaptationAdapter* adapter = GetL();
	return static_cast<MRtcAdaptation*>(adapter->iRtcAdaptation);
	}

/**
 * Returns an object conforming to the MEmergencyCallRfAdaptation interface.
 * 
 * The objects returned from this method may be reference counted.
 * This may mean that consecutive calls to this method will return the same
 * address.
 * 
 * @internalComponent
 */
EXPORT_C MEmergencyCallRfAdaptation* CStartupAdaptationAdapter::NewEmergencyCallRfAdaptationL()
	{
	CStartupAdaptationAdapter* adapter = GetL();
	return static_cast<MEmergencyCallRfAdaptation*>(adapter->iEmergencyCallRfAdaptation);
	}

/**
 * Returns an object conforming to the MMiscAdaptation interface.
 * 
 * The objects returned from this method may be reference counted.
 * This may mean that consecutive calls to this method will return the same
 * address.
 * 
 * @internalComponent
 */
EXPORT_C MMiscAdaptation* CStartupAdaptationAdapter::NewMiscAdaptationL()
	{
	CStartupAdaptationAdapter* adapter = GetL();
	return static_cast<MMiscAdaptation*>(adapter->iMiscAdaptation);
	}

/**
 * Returns a pointer to the adaptation class, creating and loading it if necessary 
 * 
 * @internalComponent 
 */
CStartupAdaptation* CStartupAdaptationAdapter::GetAdaptationL()
	{
	if(iStartupAdaptation == NULL)
		{
		iStartupAdaptation = CStartupAdaptation::NewL(*this);
		}
	return iStartupAdaptation;
	}

/**
 * Dispatches or queues for dispatch the provided adaptation base class 
 * 
 * @internalComponent 
 */
void CStartupAdaptationAdapter::QueueDispatchL(CAdaptationBase* aAdaptation)
	{
	
	if(iDispatchedAdaptation == NULL)
		{
		DEBUGPRINT2A("SAA - Dispatch request immediately commandId: %d", aAdaptation->CommandId());
		// Nothing is waiting for a response, so dispatch immediately
		iDispatchedAdaptation = aAdaptation;
		GetAdaptationL()->CommandL(iDispatchedAdaptation->CommandId(), *iDispatchedAdaptation->ParameterPckg());
		iDispatchedAdaptation->SetDispatched(ETrue);
		}
	else
		{
		DEBUGPRINT3A("SAA - Queueing request for commandId: %d queue size:", aAdaptation->CommandId(), iDispatchQueueSize);
		// Check queue size
		if (iDispatchQueueSize == KMaxAdaptationClasses)
			{
			CLAYER_PANIC(ECLayerMaximumQueueSize);
			}
		// Add to queue
		iDispatchQueue[iDispatchQueueSize] = aAdaptation;
		// Update queue size
		++iDispatchQueueSize;
		}
	}

/**
 * Handles responses from the adaptation.
 * 
 * Defined in MStartupAdaptationObserver.
 * 
 * @internalComponent
 */
void CStartupAdaptationAdapter::ResponseL(const StartupAdaptation::TCommand aCommandId, TDesC8& aData)
	{
	DEBUGPRINT2A("SAA - Response received from adaptation with commandId: %d", aCommandId);
	// Check we were expecting a response
	if(iDispatchedAdaptation == NULL)
		{
		CLAYER_PANIC(ECLayerUnexpectedResponse);
		}
	// Indicate that it is complete
	iDispatchedAdaptation->RequestComplete(aCommandId, aData);
	
	// clear the dispatched adaptation so it can be reused
	iDispatchedAdaptation = NULL;
	
	// self complete to dispatch next queued request if any
	SelfComplete(KErrNone);
	}

/**
 * Handles events from the adaptation.
 * 
 * Defined in MStartupAdaptationObserver.
 * 
 * @internalComponent
 */
void CStartupAdaptationAdapter::EventL(const StartupAdaptation::TEvent aEventId, TDesC8& aData)
	{
	using namespace StartupAdaptation;
	// Switch on event type
	switch(aEventId)
		{
		case EFatalError:
			{
			TFatalErrorTypePckg fatalErrorPckg;
			fatalErrorPckg.Copy(aData);
			if(fatalErrorPckg() == ESimRemoved)
				{
				// SIM event
				iSimAdaptation->ProcessEventL(ESsmSimRemoved);
				}
			else
				{
				// State event
				iStateAdaptation->ProcessEventL(ESsmFatalCoopSysError);
				}
			break;
			}
		case EDOSOriginatedReset:
			{
			iStateAdaptation->ProcessEventL(ESsmRestartDevice);
			break;
			}
		case EDOSOriginatedShutdown:
			{
			iStateAdaptation->ProcessEventL(ESsmShutdownDevice);
			break;
			}
		case ESimEvent:
			{
			TSimEventTypePckg simEventPckg;
			simEventPckg.Copy(aData);
			switch(simEventPckg())
			{
				case ESimUsable:
					iSimAdaptation->ProcessEventL(ESsmSimUsable);
					break;
				case ESimReadable:
					iSimAdaptation->ProcessEventL(ESsmSimReadable);
					break;
				case ESimNotReady:
					iSimAdaptation->ProcessEventL(ESsmSimNotReady);
					break;
				default:
					DEBUGPRINT2A("SAA - Received unknown SIM event with SIM event id: %d - ignoring", simEventPckg());
					break;
			}
			break;
			}
		default:
			DEBUGPRINT2A("SAA - Received unknown event with event id: %d - ignoring", aEventId);
			break;
		}
	}

/**
 * Cancels an already dispatched adaptation request
 * 
 * This should not be called if the response for this request has already been received.
 * 
 * @internalComponent
 */
void CStartupAdaptationAdapter::CancelDispatchedL(CAdaptationBase* aAdaptation)
	{
	DEBUGPRINT3A("SAA - Cancelling dispatched request with command id: %d dispatched command id: %d", aAdaptation->CommandId(), iDispatchedAdaptation->CommandId());
	if(aAdaptation != iDispatchedAdaptation)
		{
		CLAYER_PANIC(ECLayerInCorrectDispatchedCancel);
		}
	// Request the startup adaptation cancel the command
	GetAdaptationL()->CancelCommandL(iDispatchedAdaptation->CommandId());
	iDispatchedAdaptation = NULL;
	// Dispatch any items in the queue if present
	DispatchQueued();
	}

/**
 * Removes a queued adaptation request from the queue
 * 
 * This should not be called if the response for this request has already been dispatched.
 * 
 * @internalComponent
 */
void CStartupAdaptationAdapter::RemoveFromDispatchQueue(CAdaptationBase* aAdaptation)
	{
	DEBUGPRINT2A("SAA - Cancelling queued request with command id: %d ", aAdaptation->CommandId());
	// Search for offset
	TInt index;
	for(index = 0; index < iDispatchQueueSize && iDispatchQueue[index] != aAdaptation; ++index)
		{
		// Do nothing
		}
	// remove from queue
	if (index != iDispatchQueueSize)
		{
		// Found it at index so remove from array and shift ones above down
		for(TInt j = index + 1; j < iDispatchQueueSize; ++j)
			{
			iDispatchQueue[j-1] = iDispatchQueue[j];
			}
		// Decrease queue size
		--iDispatchQueueSize;
		}
	else
		{
		DEBUGPRINT2A("SAA - Couldn't find request to de-queue with command id: %d ", aAdaptation->CommandId());
		}
	}


/**
 * Dispatches the next adaptation request in the queue and removes it from the queue
 * 
 * @internalComponent
 */
void CStartupAdaptationAdapter::DispatchQueued()
	{
	// Dispatch the head of the queue if any items there
	// retry as long as there are items in the queue
	while(iDispatchQueueSize > 0 && iDispatchedAdaptation == NULL)
		{
		// Update the current pointer
		iDispatchedAdaptation = iDispatchQueue[0];
		// shift the queue down
		for(TInt i = 1; i < iDispatchQueueSize; ++i)
			{
			iDispatchQueue[i - 1] = iDispatchQueue[i]; 
			}
		--iDispatchQueueSize;
		// dispatch the new request
		DEBUGPRINT2A("SAA - Dispatch queue non empty, dispatchign command with commandId: %d", iDispatchedAdaptation->CommandId());
		TRAPD(err, GetAdaptationL()->CommandL(iDispatchedAdaptation->CommandId(), *iDispatchedAdaptation->ParameterPckg()));
		if(err != KErrNone)
			{
			// failed to dispatch
			// return error to client
			iDispatchedAdaptation->CompleteRequestStatus(err);
			// clear iDispatchedAdaptation so that the while loop tries again
			iDispatchedAdaptation = NULL;
			}
		}
	}

void CStartupAdaptationAdapter::SelfComplete(TInt aError)
	{
	TRequestStatus* trs = &iStatus;
	iStatus = KRequestPending;
	User::RequestComplete(trs, aError);
	SetActive();
	}

// From CActive

void CStartupAdaptationAdapter::RunL()
	{
	// Dispatch any queued request
	DispatchQueued();
	}

TInt CStartupAdaptationAdapter::RunError(TInt aError)
	{
	DEBUGPRINT2(_L("CStartupAdaptationAdapter::RunError() called with error %d"), aError);
	// Return the error to the active scheduler to panic as this should never occur
	return aError;
	}

void CStartupAdaptationAdapter::DoCancel()
	{
	// Nothing to cancel as iStatus should never be KRequestPending
	}
