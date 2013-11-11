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
// This file provides the implementation of the class that manages
// conflict control of SUPL Protocol Module.
// 
//

/**
 @file
 @internalTechnology
 @deprecated
*/


#include <centralrepository.h>
#include "suplconflictmanager.h"
#include "supldevloggermacros.h"

/** Static constructor.
@return A new instance of the CSuplConflictManager class
*/  
CSuplConflictManager* CSuplConflictManager::NewL()
	{
	SUPLLOG(ELogP1, "CSuplConflictManager::NewL() Begin\n");
	CSuplConflictManager* self = new (ELeave) CSuplConflictManager();
	CleanupStack::PushL(self);
	self->ConstructL();
	SUPLLOG(ELogP1, "CSuplConflictManager::NewL() End\n");
	CleanupStack::Pop(self);
	return self;
	}


/** Standard constructor.
*/  
CSuplConflictManager::CSuplConflictManager()
	{
	SUPLLOG(ELogP1, "CSuplConflictManager::CSuplConflictManager() Begin\n");
	SUPLLOG(ELogP1, "CSuplConflictManager::CSuplConflictManager() End\n");
	}


/** Standard destructor.
*/  
CSuplConflictManager::~CSuplConflictManager()
	{
	SUPLLOG(ELogP1, "CSuplConflictManager::~CSuplConflictManager() Begin\n");
	iDecisionTable.Close();
	SUPLLOG(ELogP1, "CSuplConflictManager::~CSuplConflictManager() End\n");
	}
	
	
/** Private second-stage constructor.
*/  
void CSuplConflictManager::ConstructL()
	{
	SUPLLOG(ELogP1, "CSuplConflictManager::ConstructL() Begin\n");
	SUPLLOG(ELogP1, "CSuplConflictManager::ConstructL() End\n");
	}

	
/** Get conflict decision.
Generates a conflict decision based on the nature of a current operation,
and the nature of a new operation.

The conflict rules are as follow:

- Only procedure, either MO-LR or MT-LR is allowed to be active at a time.
- A new SUPL INIT is ignored and discarded if either MO-LR or MT-LR are in progress.
- SelfLocateRequest when MT-LR is in progress terminates MT-LR and starts MO-LR.
- SelfLocateRequest(sessId2) when MO-LR (sessId1) is in progress, the second request is rejected

@param aActiveServiceMask Services currently in progress in the Protocol Module (MoLr, MtLr,...)
@param aNewOperation The new service for which a decision is required
@param aNewOperationOptions Options about aNewOperation that affect the conflict decision (mode, new client,...)
@param aIsAssistanceDataNeeded Boolean that is 'true' when the new service needs assistance data from the network.
@return TConflictDecision The conflict decision:

	EConflictNone				- This implies there is no current operation
								  and the new operation is simply started
	EConflictRejectNew			- The new operation is rejected and any
								  current operation proceeds unaffected
	EConflictCancelCurrent		- The current operation is cancelled and
								  the new operation is started
	EConflictAdoptNewSessionId	- The current self location operation proceeds
								  but it adopts the session Id of the new operation.
*/  
CSuplConflictManager::TConflictResult CSuplConflictManager::ConflictDecision(const MLbsNetworkProtocolObserver::TLbsNetProtocolServiceMask& aActiveServiceMask,
									 const MLbsNetworkProtocolObserver::TLbsNetProtocolService& aNewOperation,
									 const TLbsNetPosRequestOptionsBase& /*aNewOperationOptions*/) const
	{
	SUPLLOG(ELogP1, "CSuplConflictManager::ConflictDecision() Begin\n");

	TConflictResult result = EConflictNone;
	
	switch (aNewOperation)
		{
		case MLbsNetworkProtocolObserver::EServiceMobileTerminated:
			// reject new MTLR request if any other procedure is in progress
			if (MLbsNetworkProtocolObserver::EServiceNone != aActiveServiceMask)
				result =  EConflictRejectNew;
			break;
			
		case MLbsNetworkProtocolObserver::EServiceNetworkLocation:
		case MLbsNetworkProtocolObserver::EServiceSelfLocation:
			if (aNewOperation & aActiveServiceMask)
				{
				// Reject the new session.
				result = EConflictRejectNew;
				}
				else if (MLbsNetworkProtocolObserver::EServiceMobileTerminated & aActiveServiceMask)
				{
				// terminate the MTLR before starting the new procedure
				result = EConflictCancelCurrent;
				}
		}
	
	SUPLLOG(ELogP1, "CSuplConflictManager::ConflictDecision() End\n");
	return result;
	}
