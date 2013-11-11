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
//

#include "adaptationbase.h"
#include "clayerpanic.h"

#include "ssmdebug.h"
#include "startupadaptationadapter.h" 

/**
 * Frees any resources allocated to this class
 * 
 * @internalComponent
 */
CAdaptationBase::~CAdaptationBase()
	{
	
	}

/**
 * Constructor for CAdaptationBase
 * 
 * Does not take ownership of the CStartupAdaptationAdapter passed in
 * 
 * @internalComponent
 */
CAdaptationBase::CAdaptationBase(CStartupAdaptationAdapter* aAdapter)
: 	iAdapter(aAdapter),
	iRequestStatus(NULL),
	iDispatched(EFalse),
	iCommandId(static_cast<StartupAdaptation::TCommand>(0))
	{
	
	
	}


/**
 * Sets the dispatch status of this adaptation.
 * 
 * An adaptation is dispatched if it has had a command issued to
 * the startup adaptation but is waiting for a response.
 * 
 * @internalComponent
 */ 
void CAdaptationBase::SetDispatched(TBool aDispatched)
	{
	iDispatched = aDispatched;
	}

/**
 * Returns the dispatch status of this adaptation.
 * 
 * An adaptation is dispatched if it has had a command issued to
 * the startup adaptation but is waiting for a response.
 * 
 * @internalComponent
 */
TBool CAdaptationBase::Dispatched()
	{
	return iDispatched;
	}

/**
 * Sets the command ID for the next command that this adaptation wishes to issue.
 * 
 * @internalComponent
 */
void CAdaptationBase::SetCommandId(StartupAdaptation::TCommand aCommandId)
	{
	iCommandId = aCommandId;
	}


/**
 * Returns the command ID for the next command that this adaptation wishes to issue.
 * 
 * @internalComponent
 */
StartupAdaptation::TCommand CAdaptationBase::CommandId()
	{
	return iCommandId;
	}

/**
 * Returns true if this adaptation is busy.
 * 
 * An adaptation is busy if it has a command request queued or
 * has dispatched a command request but is waiting for the reply. 
 * 
 * @internalComponent
 */
TBool CAdaptationBase::Busy()
	{
	return iRequestStatus != NULL;
	}
	
/**
 * Sets the request status for this adaptation object to the
 * supplied parameter
 * 
 * @internalComponent
 */
void CAdaptationBase::SetRequestStatus(TRequestStatus* aStatus)
	{
	__ASSERT_DEBUG(iRequestStatus == NULL, CLAYER_PANIC(EClayerOutstandingRequest));
	iRequestStatus = aStatus;
	}

/**
 * Completes the outstanding request for this adaptation with the given value
 * and sets this adaptation to not be busy and not dispatched.
 * 
 * @internalComponent
 */
void CAdaptationBase::CompleteRequestStatus(TInt aValue)
	{
	__ASSERT_DEBUG(iRequestStatus != NULL, CLAYER_PANIC(ECLayerCompletingNull));
	if(iRequestStatus != NULL)
		{
		User::RequestComplete(iRequestStatus, aValue);
		iRequestStatus = NULL;
		iDispatched = EFalse;
		}
	}

/**
 * Cancels any outstanding request and completes the status with KErrCancel
 * 
 * @internalComponent
 */
void CAdaptationBase::CancelRequest()
	{
	if(Busy())
		{
		if (Dispatched())
			{
			// Cancel and remove dispatched
			TRAPD(err, iAdapter->CancelDispatchedL(this));
			if(err != KErrNone)
				{
				DEBUGPRINT2A("SAA - failed to cancel dispatch with error: %d", err);
				// return immediately and don't attempt to complete it, it should be completed later
				return;
				}
			}
		else
			{
			// Remove from queue
			iAdapter->RemoveFromDispatchQueue(this);
			}
		CompleteRequestStatus(KErrCancel);
		}
	// Nothing to do if not busy
	}
