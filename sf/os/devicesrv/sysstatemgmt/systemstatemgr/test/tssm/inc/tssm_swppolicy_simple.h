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


#ifndef __TSSM_SWPPOLICY_SIMPLE_H__
#define __TSSM_SWPPOLICY_SIMPLE_H__

#include <ssm/ssmswppolicy.h>

const TInt KTestNotAllowedValue = 0xCACA;
_LIT(KTestSwpPolicySimpleFile, "ssm.swp.policy.test.simple.dll");
_LIT(KRequestCompleteSem, "requestcompletesemphore");

//Use this known panic reasons / category if the test fails (as TEST macro is not available here)
enum TSwpSimpleTestPanicReason
	{
	ESwpSimpleTestFailed1 = 2020,
	ESwpSimpleTestFailed2
	};
_LIT(KSwpSimpleTestPanicCategory,"SwpSimpleTest");


NONSHARABLE_CLASS(CSsmSwpPolicySimple) : public CBase, public MSsmSwpPolicy
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
	CSsmSwpPolicySimple();
	~CSsmSwpPolicySimple();

private:
	CSsmCommandList* iCommandList;
	};

#endif
