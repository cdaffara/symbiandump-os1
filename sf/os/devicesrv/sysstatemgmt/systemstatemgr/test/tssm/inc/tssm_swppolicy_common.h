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


#ifndef __TSSM_SWPPOLICY_COMMON_H__
#define __TSSM_SWPPOLICY_COMMON_H__

#include <ssm/ssmswppolicy.h>


_LIT(KTestSwpPolicyCommonFile, "ssm.swp.policy.test.common.dll");

//Use this known panic reasons / category if the test fails (as TEST macro is not available here)
enum TSwpCommonTestPanicReason
	{
	ESwpCommonTestFailed1 = 1010,
	ESwpCommonTestFailed2,
	ESwpCommonTestFailed3
	};
_LIT(KSwpCommonTestPanicCategory,"SwpCommonTest");

const TInt KTestRSsmSwpPolicySessionErr = -1234;	//Random no. used as error code

NONSHARABLE_CLASS(CSsmSwpPolicyCommon) : public CBase, public MSsmSwpPolicy
	{
public:
	IMPORT_C static MSsmSwpPolicy* NewL(); //lint !e1511
	
	//from MSsmSwpPolicy
	void ConstructL();
	void Initialize(TRequestStatus& aStatus);
	void InitializeCancel();
	void Release();
	TResponse TransitionAllowed(const TSsmSwp& aSwp, const RMessagePtr2& aMessage);
	void PrepareCommandList(const TSsmSwp& aSwp, TRequestStatus& aStatus);
	void PrepareCommandListCancel();
	CSsmCommandList* CommandList();
	void HandleCleReturnValue(const TSsmSwp& aSwp, TInt aError, TInt aSeverity, TRequestStatus& aStatus);
	void HandleCleReturnValueCancel();
	
private:
	//from CBase
	CSsmSwpPolicyCommon();
	~CSsmSwpPolicyCommon();

private:
	void doPrepareCommandListL(TSsmSwp aSwp);

private:
	CSsmCommandList* iCommandList;
	};

#endif
