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

/**
 @file
 @test
 @internalComponent - Internal Symbian test code 
*/


#ifndef __TSSM_STATEPOLICY_NEXTSTATEC_H__
#define __TSSM_STATEPOLICY_NEXTSTATEC_H__

#include <e32base.h>
#include <ssm/ssmstatepolicy.h>
#include <ssm/ssmstate.h>


const TUint16 ESsmTestNextStateCState = 0x00CC;			//Major state identifier for state policy dll

const TUint16 ESsmTestNextStateC1Substate = 0x00C1;		//sub state identifier for state policy dll

const TUint16 ESsmTestNextStateCNoPanicReason = 0xBEEB;	//Reason that won't cause a panic in TransitionAllowed() for state policy dll


//Use this known panic reasons / category if the test fails (as TEST macro is not available here)
enum TStateNextStateCTestPanicReason
	{
	EStateNextStateCTestFailed1 = 3535,
	EStateNextStateCTestFailed2,
	EStateNextStateCTestFailed3
	};
_LIT(KStateNextStateCTestPanicCategory,"StateNextStateCTest");
_LIT(KStateTransitionComplete, "StateTransitionComplete");

NONSHARABLE_CLASS(CSsmStatePolicyNextStateC) : public CBase, public MSsmStatePolicy
	{
public:
	IMPORT_C static MSsmStatePolicy* NewL(); //lint !e1511
	
	//from CSsmStatePolicy
	void ConstructL();
	void Initialize(TRequestStatus& aStatus);
	void InitializeCancel();
	void Release();
	TResponse TransitionAllowed(const TSsmStateTransition& aRequest, TSsmStateTransition const* aCurrent, TSsmStateTransition const* aQueued, const RMessagePtr2& aMessage);
	void PrepareCommandList(TSsmState aState, TInt aReason, TRequestStatus& aStatus);
	void PrepareCommandListCancel();
	CSsmCommandList* CommandList();
	TBool GetNextState(TSsmState aCurrentTransition, TInt aReason, TInt aError, TInt aSeverity, TSsmState& aNextState);
	
private:
	//from CBase
	CSsmStatePolicyNextStateC();
	~CSsmStatePolicyNextStateC();

private:
	void doPrepareCommandListL(TSsmState aState, TInt aReason);

private:
	CSsmCommandList* iCommandList;
	};

#endif
