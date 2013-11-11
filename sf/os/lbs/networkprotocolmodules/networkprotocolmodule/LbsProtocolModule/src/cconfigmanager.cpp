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
// This file provides the implementation of the class that manages
// configuration of Test Protocol Module operation.
// 
//

#include <centralrepository.h>
#include "cconfigmanager.h"

/** Static constructor.
@return A new instance of the CConfigManager class
*/  
CConfigManager* CConfigManager::NewL()
	{
	CConfigManager* self = new (ELeave) CConfigManager();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}


/** Standard constructor.
*/  
CConfigManager::CConfigManager()
	{
	}


/** Standard destructor.
*/  
CConfigManager::~CConfigManager()
	{
	iDecisionTable.Close();
	}
	
	
/** Private second-stage constructor.
*/  
void CConfigManager::ConstructL()
	{
	LoadDecisionDataL();
	}
	
	
/** Load external decision table data.
This retrieves decision table entries from the external data source which
has been installed as part of the system configuration.

This data is stored in the internal decision table in rows and columns
that represent the current and new operations, respectively.
*/  
void CConfigManager::LoadDecisionDataL()
	{
	// Pre-allocate table space
	iDecisionTable.ReserveL(KMaxOperationTypes * KMaxOperationTypes);

	// Open central repository
	CRepository* repository = CRepository::NewLC(TUid::Uid(KMyRepositoryUid));

	// For each row - current operation
	for (TInt row = 0; row < KMaxOperationTypes; row++)
		{
		// For each column - new operation
		for (TInt col = 0; col < KMaxOperationTypes; col++)
			{
			// Retrieve an item from central repository using a UID
			// which is derived from the row and column numbers
			TInt dataValue = -1;
			TUint32 uid = (row * KMaxOperationTypes) + col + 1;
			TConflictResult value = KDefaultDecision; // initialise to default
			if (KErrNone == repository->Get(uid, dataValue))
				{
				// Convert entry value to decision value
				switch (dataValue)
					{
				case 0:
					value = EConflictRejectNew;
					break;
				case 1:
#ifdef PM_HACK_FOR_INC103104				
					value = EConflictRejectNew;
#else
					value = EConflictQueueNew;
#endif //PM_HACK_FOR_INC103104
					break;
				case 2:
					value = EConflictCancelCurrent;
					break;
				case 3:
					value = EConflictQueueCurrent;
					break;
				default:
					value = EConflictNone;
					break;
					};
				}

			iDecisionTable.AppendL(value);
			}
		}

	// Close central repository
	CleanupStack::PopAndDestroy(repository);
	}
	
	
/** Get conflict decision.
Generates a conflict decision based on the nature of a current operation,
and the nature of a new operation.

@param aCurrentOp The current operation
@param aNewOp The new operation for which a decision is required
@return TConflictDecision The conflict decision:
	EConflictNone			- This implies there is no current operation
							  and the new operation is simply started
	EConflictRejectNew		- The new operation is rejected and any
							  current operation proceeds unaffected
	EConflictQueueNew		- The new operation is queued awaiting
							  completion of any current operation
	EConflictCancelCurrent	- The current operation is cancelled and
							  the new operation is started
	EConflictQueueCurrent	- The current operation is queued an awaits
							  the completion of the new operation
*/  
CConfigManager::TConflictResult CConfigManager::ConflictDecision(const TConflictOperation& aCurrentOp, const TConflictOperation& aNewOp) const
	{
	TConflictResult result = KDefaultDecision; // initialise to default decision
	TInt currentOpIndex;
	TInt newOpIndex;

	// Get table row index - current operation
	currentOpIndex = DecisionTableIndex(aCurrentOp);
	if (0 <= currentOpIndex)
		{
		// Get table column index - new operation
		newOpIndex = DecisionTableIndex(aNewOp);
		if (0 <= newOpIndex)
			{
			// Retrieve entry using row and column index values
			result = iDecisionTable[(currentOpIndex*KMaxOperationTypes) + newOpIndex];
			}
		}

	return result;
	}


/** Get an index for the specified operation.
*/
TInt CConfigManager::DecisionTableIndex(const TConflictOperation& aOperation) const
	{
	TInt index = -1; // initialised to an invalid index

	// What type of operation is specified?
	switch (aOperation.iOperation)
		{

	case EOpMoLr:
	case EOpNbLr:
		index = KMoLr;
		break;

	case EOpMtLr:
	case EOpNiLr:
		if (EPriorityEmergency == aOperation.iPriority)
			{
			index = KEmergency;
			}
		else
			{
			index = KMtLr;
			}
		break;

	case EOpX3p:
		// What priority of X3P is specified?
		switch (aOperation.iPriority)
			{
		case EPriorityLow:
			index = KX3pLow;
			break;

		case EPriorityMedium:
			index = KX3pMid;
			break;

		case EPriorityHigh:
			index = KX3pHi;
			break;

		default:
			break;
			};
		break;

	default:
		break;
		};

	return index;
	}

