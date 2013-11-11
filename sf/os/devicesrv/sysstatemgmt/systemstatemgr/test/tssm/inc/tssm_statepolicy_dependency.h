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


#ifndef __TSSM_STATEPOLICY_DEPENDENCY_H__
#define __TSSM_STATEPOLICY_DEPENDENCY_H__

#include <e32base.h>
#include <ssm/ssmstatepolicy.h>
#include <ssm/ssmstate.h>


// These are the 2 dependent properties that will be changed when this policy is executed.
const TUint KTestDependentSwp3={0x03A0CACA};
const TUint KTestDependentSwp4={0x04A0CACA};

const TUint16 ESsmTestDependencyState = 0x009F;		//Major state identifier for state policy dll

//Use this known panic reasons / category if the test fails (as TEST macro is not available here)
enum TStateDependencyTestPanicReason
	{
	EStateDependencyTestFailed1 = 777,
	EStateDependencyTestFailed2
	};
_LIT(KStateDependencyTestPanicCategory,"StateDependencyTest");


NONSHARABLE_CLASS(CSsmStatePolicyDependency) : public CBase, public MSsmStatePolicy
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
	CSsmStatePolicyDependency();
	~CSsmStatePolicyDependency();

private:
	void doPrepareCommandListL(TSsmState aState, TInt aReason);

private:
	CSsmCommandList* iCommandList;
	};

#endif
