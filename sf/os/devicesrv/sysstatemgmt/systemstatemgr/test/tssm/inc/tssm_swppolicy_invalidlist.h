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


#ifndef __TSSM_SWPPOLICY_INVALIDLIST_H__
#define __TSSM_SWPPOLICY_INVALIDLIST_H__

#include <ssm/ssmswppolicy.h>
#include "../../testapps/inc/ssmtestapps.h"
#include "../../testapps/inc/ssmtestapprverror.h"


//Use this known panic reasons / category if the test fails (as TEST macro is not available here)
enum TSwpInvalidListTestPanicReason
	{
	ESwpInvalidListTestFailed1 = 4171,
	ESwpInvalidListTestFailed2
	};
_LIT(KSwpInvalidListTestPanicCategory,"SwpCmdListTest");
	

//Dll policy file name to use for registering a key
_LIT(KTestSwpPolicyInvalidListFile, "ssm.swp.policy.test.invalidlist.dll");

//Available test cases provided by this swp policy plugin
enum TSwpCmdPanicTestCases
	{
	ESwpCmdWithoutPublishSwp=112233,
	ESwpCmdTwoPublishSwp,
	ESwpCmdPublishState,
	ESwpCmdNoMultipleWait,
	ESwpCmdTooManyMultipleWaitErr1,
	ESwpCmdTooManyMultipleWaitErr2,
	ESwpCmdPrepareCmdListFailed
	};


NONSHARABLE_CLASS(CSsmSwpPolicyInvalidList) : public CBase, public MSsmSwpPolicy
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
	CSsmSwpPolicyInvalidList();
	~CSsmSwpPolicyInvalidList();

private:
	void doPrepareCommandListL(TSsmSwp aSwp);

private:
	CSsmCommandList* iCommandList;
	};

#endif
