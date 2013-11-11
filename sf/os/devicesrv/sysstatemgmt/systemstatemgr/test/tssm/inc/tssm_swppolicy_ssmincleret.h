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


#ifndef __TSSM_SWPPOLICY_SSMINCLERET_H__
#define __TSSM_SWPPOLICY_SSMINCLERET_H__

#include <ssm/ssmswppolicy.h>
#include "tssm_swppolicy_common.h"

/********************* utility class ******************/
class CRequestRunInThread : public CActive
	{
public:
	CRequestRunInThread();
	CRequestRunInThread(TRequestStatus& aClientStatus);
	~CRequestRunInThread();
	void StartL(const TSsmStateTransition& aState);
		 
private:
	void RunL();
	void DoCancel();

private:
	TRequestStatus* iClientStatus;
	RSsmStateManagerTestClient iClient;
	};

/******************** ssmincleret class ***************/

// This swp is the one allowed to test for state transition request in the HandleCleReturnValue method
const TUint KTestSwpHandleCleRetStateReq = {0x00201200};

_LIT(KTestSwpPolicySsmInCleRetFile, "ssm.swp.policy.test.ssmincleret.dll");

enum TSsmTransitionRequest
	{
	ERequestStateTranSync = 10,
	ERequestStateTranAsync = 20,
	};

//Use this known panic reasons / category if the test fails (as TEST macro is not available here)
enum TSwpSsmInCleRetTestPanicReason
	{
	ESwpSsmInCleRetTestFailed1 = 7070,
	ESwpSsmInCleRetTestFailed2
	};
_LIT(KSwpSsmInCleRetTestPanicCategory,"SwpSsmInCleRetTest");


NONSHARABLE_CLASS(CSsmSwpPolicySsmInCleRet) : public CBase, public MSsmSwpPolicy
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
	CSsmSwpPolicySsmInCleRet();
	~CSsmSwpPolicySsmInCleRet();

private:
	void doPrepareCommandListL(TSsmSwp aSwp);

private:
	CSsmCommandList* iCommandList;
	};

#endif
