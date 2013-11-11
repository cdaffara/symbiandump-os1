// Copyright (c) 2004-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// Header for etel driver strategy class
// 
//

/**
 @file 
 @internalComponent
*/

#ifndef METELDRIVERSTRATEGY_H
#define METELDRIVERSTRATEGY_H

// forward declarations
class CEtelDriverContext;

/** base class for spud etel driver strategy */
class MEtelDriverStrategy
	{
public:
	/** union of all elementary steps to be performed during execution of strategies */
	enum TSequenceStep
		{
		EStartStep,
		EInitPhoneStep,
		
		EOpenNewQoSStep,
		EGetConfigStep,
		ESetConfigStep,
		EInitialiseContextStep,
		
		ESetProfileParamsStep,
		EGetProfileParamsStep,
		
		ECreateNewTFTStep,
		EAddFirstTftStep,
		EDeleteTftStep,
		ERemoveFirstTftStep,
		ERemoveTftStep,
		EAddTftStep,
		
		EActivateStep,
		EModifyActiveStep,
		EDeactivateStep,
		
		EDeleteContextStep,
		EChangeTftFinished,
		EModifyActiveContext,
		// add execution steps here
		
		EMbmsSetSession,
		EUpdateMbmsSessionList,
		EPrepareSessionList,
		ENotifyStatusChange,
			
		EFinishStep
		};

public:
	/** Executes next asynchronous step in a strategy. 

	@param aContext - pdp context
	@param aStatus - request status of this step
	*/
	virtual void Next (CEtelDriverContext& aContext, TRequestStatus* aStatus) = 0;
	
	/** 
	Notifies SPUD FSM about execution results.

	@param aContext - pdp context
	@param aCompletionStatus - completion request status
	*/	
	virtual void NotifyFsm (CEtelDriverContext& aContext, TRequestStatus& aCompletionStatus) = 0;
	
	/** Cancels last asynchronous request to eTel
	
	@param  aContext - pdp context
	*/
	virtual void CancelAsyncRequest(CEtelDriverContext& aContext) = 0;
	};

#endif // METELDRIVERSTRATEGY_H
