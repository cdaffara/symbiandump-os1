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


#ifndef __TSSM_STATEPOLICY_NEXTSTATEB_H__
#define __TSSM_STATEPOLICY_NEXTSTATEB_H__

#include <e32base.h>
#include <ssm/ssmstatepolicy.h>
#include <ssm/ssmstate.h>


const TUint16 ESsmTestNextStateBState = 0x00BB;			//Major state identifier for state policy dll

const TUint16 ESsmTestNextStateB1Substate = 0x00B1;		//sub state identifier for state policy dll
const TUint16 ESsmTestNextStateB2Substate = 0x00B2;		//sub state identifier for state policy dll


//Use this known panic reasons / category if the test fails (as TEST macro is not available here)
enum TStateNextStateBTestPanicReason
	{
	EStateNextStateBTestFailed1 = 3434,
	EStateNextStateBTestFailed2,
	EStateNextStateBTestFailed3
	};
_LIT(KStateNextStateBTestPanicCategory,"StateNextStateBTest");


NONSHARABLE_CLASS(CSsmStatePolicyNextStateB) : public CBase, public MSsmStatePolicy
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
	CSsmStatePolicyNextStateB();
	~CSsmStatePolicyNextStateB();

private:
	void doPrepareCommandListL(TSsmState aState, TInt aReason);

private:
	CSsmCommandList* iCommandList;
	};

#endif
