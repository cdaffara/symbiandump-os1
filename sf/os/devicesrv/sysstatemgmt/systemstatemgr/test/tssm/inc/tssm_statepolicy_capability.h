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


#ifndef __TSSM_STATEPOLICY_CAPABILITY_H__
#define __TSSM_STATEPOLICY_CAPABILITY_H__

#include <e32base.h>
#include <ssm/ssmstatepolicy.h>
#include <ssm/ssmstate.h>


const TUint16 ESsmTestCapabilityState = 0x0090;			//Major state identifier for state policy dll


// define few test reasons that will correspond to different test cases
const TUint16 ESsmTestCapabilityReasonCustomCmd = 1977;
const TUint16 ESsmTestCapabilityReasonPowerMgmt = 1978;
const TUint16 ESsmTestCapabilityReasonLeave = 1979;

//Use this known panic reasons / category if the test fails (as TEST macro is not available here)
enum TStateCapabilityTestPanicReason
	{
	EStateCapabilityTestFailed1 = 4343,
	EStateCapabilityTestFailed2,
	EStateCapabilityTestFailed3
	};
_LIT(KStateCapabilityTestPanicCategory,"StateCapabilityTest");


NONSHARABLE_CLASS(CSsmStatePolicyCapability) : public CBase, public MSsmStatePolicy
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
	CSsmStatePolicyCapability();
	~CSsmStatePolicyCapability();

private:
	void doPrepareCommandListL(TSsmState aState, TInt aReason);

private:
	CSsmCommandList* iCommandList;
	};

#endif
