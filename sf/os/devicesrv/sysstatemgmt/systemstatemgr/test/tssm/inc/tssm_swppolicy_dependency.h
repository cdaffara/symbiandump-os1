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


#ifndef __TSSM_SWPPOLICY_DEPENDENCY_H__
#define __TSSM_SWPPOLICY_DEPENDENCY_H__

#include <ssm/ssmswppolicy.h>


// These are the 2 dependent properties that will be changed when this policy is executed.
const TUint KTestDependentSwp1={0x01A0CACA};
const TUint KTestDependentSwp2={0x02A0CACA};

_LIT(KTestSwpPolicyDependencyFile, "ssm.swp.policy.test.dependency.dll");

//Use this known panic reasons / category if the test fails (as TEST macro is not available here)
enum TSwpDependencyTestPanicReason
	{
	ESwpDependencyTestFailed1 = 333,
	ESwpDependencyTestFailed2
	};
_LIT(KSwpDependencyTestPanicCategory,"SwpDependencyTest");


NONSHARABLE_CLASS(CSsmSwpPolicyDependency) : public CBase, public MSsmSwpPolicy
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
	CSsmSwpPolicyDependency();
	~CSsmSwpPolicyDependency();

private:
	void doPrepareCommandListL(TSsmSwp aSwp);

private:
	CSsmCommandList* iCommandList;
	};

#endif
