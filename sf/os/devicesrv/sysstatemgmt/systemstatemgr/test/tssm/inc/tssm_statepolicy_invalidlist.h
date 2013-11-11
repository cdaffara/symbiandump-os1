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

/**
 @file
 @test
 @internalComponent - Internal Symbian test code 
*/


#ifndef __TSSM_STATEPOLICY_INVALIDLIST_H__
#define __TSSM_STATEPOLICY_INVALIDLIST_H__

#include <ssm/ssmstatepolicy.h>
#include "../../testapps/inc/ssmtestapps.h"
#include "../../testapps/inc/ssmtestapprverror.h"


//Use this known panic reasons / category if the test fails (as TEST macro is not available here)
enum TStateInvalidListTestPanicReason
	{
	EStateInvalidListTestFailed1 = 119,
	EStateInvalidListTestFailed2
	};
_LIT(KStateInvalidListTestPanicCategory,"StateCmdListTest");
	

//Major state identifier for state policy dll
const TUint16 ESsmTestInvalidListState = 0x009D;

//Available test cases provided by this state policy plugin
enum TStateCmdPanicTestCases
	{
	EStateCmdWithoutPublishState=332211,
	EStateCmdTwoPublishState,
	EStateCmdPublishSwp,
	EStateCmdNoMultipleWait,
	EStateCmdTooManyMultipleWaitErr1,
	EStateCmdTooManyMultipleWaitErr2,
	EStateCmdPrepareCmdListFailed
	};


NONSHARABLE_CLASS(CSsmStatePolicyInvalidListTest) : public CBase, public MSsmStatePolicy
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
	CSsmStatePolicyInvalidListTest();
	~CSsmStatePolicyInvalidListTest();

private:
	void doPrepareCommandListL(TSsmState aState, TInt aReason);

private:
	CSsmCommandList* iCommandList;
	};

#endif
